#ifndef CIA405_H
#define CIA405_H

#pragma once

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include<vector>
#include<map>
#include<list>

#include "interfaces.h"
#include "driver_factory.h"

#include "CO_driver_target.h"
#include "OD.h"

#include "CO_error.h"
#include "CO_epoll_interface.h"
#include "CO_storageLinux.h"

#include "QCANopenNode/CANopen.h"
#include "QCANopenNode/301/CO_ODinterface.h"
#include "QCANopenNode/301/CO_NMT_Heartbeat.h"
#include "QCANopenNode/301/CO_Emergency.h"
#include "QCANopenNode/301/CO_SDOserver.h"
#include "QCANopenNode/301/CO_SDOclient.h"

using namespace std;

//-------------------------------------------------------
//
//-------------------------------------------------------
/* Include optional external application functions */
#ifdef CO_USE_APPLICATION
#include "CO_application.h"
#endif

/* Add trace functionality for recording variables over time */
#if (CO_CONFIG_TRACE) & CO_CONFIG_TRACE_ENABLE
#include "CO_time_trace.h"
#endif

/* Interval of mainline and real-time thread in microseconds */
#ifndef MAIN_THREAD_INTERVAL_US
#define MAIN_THREAD_INTERVAL_US 100000
#endif
#ifndef TMR_THREAD_INTERVAL_US
#define TMR_THREAD_INTERVAL_US 1000
#endif

/* default values for CO_CANopenInit() */
#ifndef NMT_CONTROL
#define NMT_CONTROL                                                                                                \
CO_NMT_STARTUP_TO_OPERATIONAL                                                                                      \
    /*| CO_NMT_ERR_ON_ERR_REG*/ | CO_ERR_REG_GENERIC_ERR | CO_ERR_REG_COMMUNICATION
#endif
#ifndef FIRST_HB_TIME
#define FIRST_HB_TIME 500
#endif
#ifndef SDO_SRV_TIMEOUT_TIME
#define SDO_SRV_TIMEOUT_TIME 1000
#endif
#ifndef SDO_CLI_TIMEOUT_TIME
#define SDO_CLI_TIMEOUT_TIME 500
#endif
#ifndef SDO_CLI_BLOCK
#define SDO_CLI_BLOCK false
#endif
#ifndef OD_STATUS_BITS
#define OD_STATUS_BITS NULL
#endif
/* CANopen gateway enable switch for CO_epoll_processMain() */
#ifndef GATEWAY_ENABLE
#define GATEWAY_ENABLE true
#endif
/* Interval for time stamp message in milliseconds */
#ifndef TIME_STAMP_INTERVAL_MS
#define TIME_STAMP_INTERVAL_MS 10000
#endif

/* Definitions for application specific data storage objects */
#ifndef CO_STORAGE_APPLICATION
#define CO_STORAGE_APPLICATION
#endif
/* Interval for automatic data storage in microseconds */
#ifndef CO_STORAGE_AUTO_INTERVAL
#define CO_STORAGE_AUTO_INTERVAL 60000000
#endif

//-------------------------------------------------------
//
//-------------------------------------------------------
/* Data block for mainline data, which can be stored to non-volatile memory */
typedef struct {
    char _CAN_device[100];
    char _storage_path[255];
    /* Pending CAN bit rate, can be set by argument or LSS slave. */
    uint16_t _bitrate;
    /* Pending CANopen NodeId, can be set by argument or LSS slave. */
    uint8_t _node_id;
}T_Storage_t;

//-------------------------------------------------------
//
//-------------------------------------------------------
typedef union T_1F80_NMT_START_UP
{
    struct {
        uint32_t     _NMT_Master_0                       :1;
        uint32_t     _Run_Optional_Nodes_1               :1;
        uint32_t     _Auto_Run_2                         :1;
        uint32_t     _Run_All_Nodes_3                    :1;
        uint32_t     _On_Error_Control_Reset_All_Nodes_4 :1;
        uint32_t     _Survey_Optional_Nodes_5            :1;
        uint32_t     _On_Error_Control_Stop_All_Nodes_6  :1;
        uint32_t     _reserved                           :25;
    }_part;
    uint32_t _data;
}T_1F80_NMT_START_UP;

//-------------------------------------------------------
//
//-------------------------------------------------------
typedef union T_1F81_SLAVE_ASSIGNMENT
{
    // bool        _check_version_number;      // проверка номера версии для СЛЕЙВа
    struct {
        uint32_t     _is_Slave                               :1;
        uint32_t     _check_manufacturer_number              :1;
        uint32_t     _On_Error_Control_Event_Start_Node      :1;
        uint32_t     _Mandatory_Node                         :1;
        uint32_t     _Operational_Not_Reset_Communication    :1;
        uint32_t     _App_Version_Verification               :1;
        uint32_t     _App_Version_Update                     :1;
        uint32_t     _check_product_number                   :1;
        uint32_t     _Retry_Factor                           :8;
        uint32_t     _Guard_Time                             :16;
    }_part;
    uint32_t _data;
}T_1F81_SLAVE_ASSIGNMENT;
//-------------------------------------------------------
//
//-------------------------------------------------------
typedef enum
{
    CANOPEN_KERNEL_NO_ERROR = 0,
    CANOPEN_KERNEL_OTHER_ERROR,
    CANOPEN_KERNEL_DATA_OVERFLOW,
    CANOPEN_KERNEL_TIMEOUT,
    CANOPEN_KERNEL_CANBUS_OFF,
    CANOPEN_KERNEL_CAN_ERROR_PASSIVE,
    CANOPEN_INTERNAL_FB_ERROR,
    CANOPEN_NO_MORE_MEMORY,
    CANOPEN_WRONG_PARAMETER,
    CANOPEN_NODEID_UNKNOWN,
    CANOPEN_NETID_UNKNOWN,
    CANOPEN_REQUEST_ERROR,
    CANOPEN_SDOCHANNEL_UNKNOWN,
    CANOPEN_NO_DRIVER,
    CANOPEN_CONFIGURATION_ERROR,
    CANOPEN_GUARDING_ERROR,
}CANOPEN_KERNEL_ERROR;

//-------------------------------------------------------
//
//-------------------------------------------------------
typedef enum
{
    AUTO = 0,
    EXPEDITED,
    SEGMENTED,
    BLOCK,
}SDO_MODE;

//-------------------------------------------------------
//
//-------------------------------------------------------
typedef enum {
    NMT_NO_COMMAND = 0,              /**< 0, No command */
    NMT_ENTER_OPERATIONAL = 1,       /**< 1, Start device */
    NMT_ENTER_STOPPED = 2,           /**< 2, Stop device */
    NMT_ENTER_PRE_OPERATIONAL = 128, /**< 128, Put device into pre-operational */
    NMT_RESET_NODE = 129,            /**< 129, Reset device */
    NMT_RESET_COMMUNICATION = 130    /**< 130, Reset CANopen communication on device */
}CIA405_NMT_COMMAND;

typedef enum {
    NMT_STATE_UNKNOWN             = -1,   /**< -1, Device state is unknown (for heartbeat consumer) */
    NMT_STATE_INITIALIZING        = 0,    /**< 0, Device is initializing */
    NMT_STATE_RESET_COMM          = 1,    /**< 1, Application must provide communication reset. */
    NMT_STATE_RESET_APP           = 2,    /**< 2, Application must provide complete device reset */
    NMT_STATE_RESET_QUIT          = 3,    /**< 3, Application must quit, no reset of microcontroller (command is not requested by the*/
    NMT_STATE_STOPPED             = 4,    /**< 4, Device is stopped */
    NMT_STATE_OPERATIONAL         = 5,    /**< 5, Device is in operational state */
    NMT_STATE_PRE_OPERATIONAL     = 127,  /**< 127, Device is in pre-operational state */
}CIA405_NMT_STATE;
//-------------------------------------------------------
//
//-------------------------------------------------------
typedef struct {
    CO_NMT_t        *NMT;
    CO_EM_t         *EM;
    CO_SDOserver_t  *SDO_S;
    CO_SDOclient_t  *SDO_C;
}T_NETWORK;

//-------------------------------------------------------
//
//-------------------------------------------------------
typedef struct {
    uint8_t      ERROR_NODE_ID;
    uint16_t     ERROR_CODE;
    uint8_t      ERROR_REGISTER;
    uint8_t      ERROR_BIT;
    uint32_t     INFO_CODE;
//    uint8_t ERROR_FIELD[5];
}T_CIA405_EMCY_ERROR;

//-------------------------------------------------------
//
//-------------------------------------------------------
typedef struct {
    bool_t                  CONFIRM;
    CANOPEN_KERNEL_ERROR    ERROR;
    uint32_t                 STATE;
    uint8_t                 *_buf;
    size_t                  _bufSize;
    size_t                  _readSize;
}T_CIA405_STATE;

//-------------------------------------------------------
//
//-------------------------------------------------------
typedef enum {
    CIA405_ST_IDLE                          = 0x00U,    /**< Gateway is idle, no command is processing. This state is starting point for new
                                                        commands, which are parsed here. */

    CIA405_NMT                              = 0x01U,    /**< NMT commands */

    CIA405_ST_SDO_READ                      = 0x10U,    /**< SDO 'read' (upload) */
    CIA405_ST_SDO_WRITE                     = 0x11U,    /**< SDO 'write' (download) */
    CIA405_ST_SDO_WRITE_ABORTED             = 0x12U,    /**< SDO 'write' (download) - aborted, purging remaining data */

    CIA405_ST_LSS_SWITCH_GLOB               = 0x20U,    /**< LSS 'lss_switch_glob' */
    CIA405_ST_LSS_SWITCH_SEL                = 0x21U,    /**< LSS 'lss_switch_sel' */
    CIA405_ST_LSS_SET_NODE                  = 0x22U,    /**< LSS 'lss_set_node' */
    CIA405_ST_LSS_CONF_BITRATE              = 0x23U,    /**< LSS 'lss_conf_bitrate' */
    CIA405_ST_LSS_STORE                     = 0x24U,    /**< LSS 'lss_store' */
    CIA405_ST_LSS_INQUIRE                   = 0x25U,    /**< LSS 'lss_inquire_addr' or 'lss_get_node' */
    CIA405_ST_LSS_INQUIRE_ADDR_ALL          = 0x26U,    /**< LSS 'lss_inquire_addr', all parameters */

    CIA405_ST_LSS_FASTSCAN                  = 0x30U,    /**< LSS '_lss_fastscan' */
    CIA405_ST_LSS_ALLNODES                  = 0x31U,    /**< LSS 'lss_allnodes' */

    // CIA405_ST_LOG                        = 0x80U,    /**< print message 'log' */
    // CIA405_ST_HELP                       = 0x81U,    /**< print 'help' text */
    // CIA405_ST_LED                        = 0x82U     /**< print 'status' of the node */
}TE_CIA405_STATE;

//-------------------------------------------------------
//
//-------------------------------------------------------
typedef struct {
    bool_t              _first_step;
    TE_CIA405_STATE     _command;
    uint8_t              _sub_state_cmd;
    uint8_t              _node_id;
    uint16_t             _index;
    uint8_t              _sub_index;
    uint8_t              *_data;
    uint64_t             _data_length;
    uint64_t             _timeout;
    SDO_MODE            _sdo_mode;
    uint64_t             _n_writen;
    bool                _bufferPartial;
    uint64_t             _stateTimeoutTmr;

    CO_LSS_address_t    _lss_address;
    uint16_t             _boudrate;
    uint8_t              _new_node_id;
    uint8_t              _lss_inquire;

    pthread_cond_t      _wait;
    pthread_mutex_t     _wait_mutex;
    pthread_mutex_t     _cmd_mutex;
    pthread_mutex_t     _func_mutex;
}T_CIA405_PROCESS_COMMAND;

//-------------------------------------------------------
//
//-------------------------------------------------------
typedef struct {
    TE_CIA405_STATE _command;
    uint8_t          _sub_state_cmd;
    struct {
        uint8_t              _node_id;
        uint8_t              _node_qty;
        uint16_t             _index;
        uint8_t              _sub_index;
        uint64_t             _read_length;
        uint64_t             _timeout;
        CO_LSS_address_t    _lss_address;
        uint8_t              _lss_inquire;
    }_data;
    struct {
        bool _confirm;

        int32_t                  _code;

        CO_LSSmaster_return_t   _lss_error;
        CO_ReturnError_t        _nmt_error;
        CO_SDO_return_t         _sdo_error;
    }_error;

}T_CIA405_RESPONSE_PROCESS_COMMAND;
//-------------------------------------------------------
//
//-------------------------------------------------------
typedef struct {
    uint8_t                 _node_id;
    bool                    _active;
    bool                    _is_state_change;
    bool                    _is_checked;
    bool                    _is_configured;
    CIA405_NMT_STATE        _state;
    CIA405_NMT_STATE        _old_state;
    uint8_t                 _idx;
    T_1F81_SLAVE_ASSIGNMENT _assign;
    uint32_t                _vendorID;
    uint32_t                _productCode;
    uint32_t                _revisionNumber;
    uint32_t                _serialNumber;
}T_CIA405_SLAVE_NODE;

//-------------------------------------------------------
//
//-------------------------------------------------------
class CIA405 : public IDriver, public IClientCallback
{
public:
    explicit CIA405(const std::shared_ptr<IClientFactory>& _factory = nullptr);
    ~CIA405() override;

    void initialize() override;
    void shutdown() override;
    std::string name() const override;

    void onInit() override;
    void onExit() override;

    void updateInputs() override;
    void updateOutputs(const std::vector<OnDataChange> &data) override;

    // void log_printf(int priority, const char* format, ...);

protected:
    bool configuration();
    void start();

    void check_can_bus();

    void CIA405_process(CO_epoll_t* ep);

    bool can_init();
    bool lss_init();
    bool can_open_init();
    bool thread_init();
    bool PDO_init();
    uint8_t get_node_id_by_lss_address(uint32_t _vendorID, uint32_t _productCode, uint32_t _revisionNumber, uint32_t _serialNumber);
#if (CO_CONFIG_STORAGE) & CO_CONFIG_STORAGE_ENABLE
    bool storage_prepare();
    bool storage_init();
#endif
    void set_node_state(uint8_t _node_id, uint8_t _idx, CIA405_NMT_STATE _state, void* _object);
    void check_node_state();
    void check_node_emergency(uint16_t _ident, uint16_t _error_code, uint8_t _error_register, uint8_t _error_bit, uint32_t _info_code);

    void prepare_own_node();
    bool is_own_node(uint8_t _node_id);

#ifdef CIA405_MASTER_NODE
    bool scan_new_node();
    void sdo_configuration_node(uint8_t _node_id);
    void survey_optional_nodes();

    void transact_net_var_through_OD();
    void transact_net_var_through_SDO();
#endif

protected:
    bool nmt(uint16_t _node_id, CIA405_NMT_COMMAND _state);
    bool get_local_node_id(uint8_t &_node_id);
    bool sdo_read(uint64_t _timeout, uint16_t _node_id, uint16_t _index, uint8_t _subindex, SDO_MODE _mode, uint8_t *_data, uint64_t _data_length);
    bool sdo_write(uint64_t _timeout, uint16_t _node_id, uint16_t _index, uint8_t _subindex, SDO_MODE _mode, uint8_t *_data, uint64_t _data_length);


public:
#ifndef CO_SINGLE_THREAD
    void *rt_thread_function();
    void *service_function();
    void *main_function();
#endif
#if (CO_CONFIG_EM) & CO_CONFIG_EM_CONSUMER
    void emergency_rx_callback(const uint16_t ident, const uint16_t errorCode, const uint8_t errorRegister, const uint8_t errorBit, const uint32_t infoCode);
#endif
#if (CO_CONFIG_NMT) & CO_CONFIG_NMT_CALLBACK_CHANGE
    void nmt_changed_callback(CO_NMT_internalState_t state);
#endif
#if (CO_CONFIG_HB_CONS) & CO_CONFIG_HB_CONS_CALLBACK_CHANGE
    void heartbeat_nmt_changed_callback(uint8_t nodeId, uint8_t idx, CO_NMT_internalState_t state, void* object);
#endif
    bool_t lss_cfg_store_callback(void* object, uint8_t id, uint16_t bitRate);

    //void log_printf(int priority, const char* format, ...);

public:
#ifdef CIA405_MASTER_NODE
    // Network management
    void NMT(bool_t _enable = false,
             uint16_t _network = 1,
             uint64_t _timeout = 0,
             uint16_t _node_id = 0,
             CIA405_NMT_COMMAND _cmd = NMT_NO_COMMAND);

    void RECV_EMCY(bool_t _enable = false,
                   uint16_t _network = 1,
                   uint64_t _timeout = 0);

    void RECV_EMCY_DEV(bool_t _enable = false,
                       uint16_t _network = 1,
                       uint64_t _timeout = 0,
                       uint16_t _node_id = 0);
#endif
    // Owner node id
    void GET_LOCAL_NODE_ID(bool_t _enable = false,
                           uint16_t _network = 1,
                           uint64_t _timeout = 0);

    // Query state
    void GET_CANOPEN_KERNEL_STATE(bool_t _enable = false,
                                  uint16_t _network = 1,
                                  uint64_t _timeout = 0);

    void GET_STATE(bool_t _enable = false,
                   uint16_t _network = 1,
                   uint64_t _timeout = 0,
                   uint16_t _node_id = 0);

    // SDO access
    void SDO_READ_DATA(bool_t _enable = false,
                       uint16_t _network = 1,
                       uint64_t _timeout = 0,
                       uint16_t _node_id = 0,
                       uint16_t _sdo_channel = 1,
                       uint16_t _index = 0,
                       uint8_t _subindex = 0,
                       SDO_MODE _mode = SDO_MODE::AUTO,
                       uint8_t *_data = nullptr,
                       uint64_t _data_length = 0);

    void SDO_WRITE_DATA(bool_t _enable = false,
                        uint16_t _network = 1,
                        uint64_t _timeout = 0,
                        uint16_t _node_id = 0,
                        uint16_t _sdo_channel = 1,
                        uint16_t _index = 0,
                        uint8_t _subindex = 0,
                        SDO_MODE _mode = SDO_MODE::AUTO,
                        uint8_t *_data = nullptr,
                        uint64_t _data_length = 0);

    void SDO_READ4(bool_t _enable = false,
                    uint16_t _network = 1,
                    uint64_t _timeout = 0,
                    uint16_t _node_id = 0,
                    uint16_t _sdo_channel = 1,
                    uint16_t _index = 0,
                    uint8_t _subindex = 0,
                    uint8_t *_data = nullptr,
                    uint64_t _data_length = 0);

    void SDO_WRITE4(bool_t _enable = false,
                    uint16_t _network = 1,
                    uint64_t _timeout = 0,
                    uint16_t _node_id = 0,
                    uint16_t _sdo_channel = 1,
                    uint16_t _index = 0,
                    uint8_t _subindex = 0,
                    uint8_t *_data = nullptr,
                    uint64_t _data_length = 0);

    // obsolete (only for compatibility)
    void SDO_READ(bool_t _enable = false,
                   uint16_t _network = 1,
                   uint64_t _timeout = 0,
                   uint16_t _node_id = 0,
                   uint16_t _sdo_channel = 1,
                   uint16_t _index = 0,
                   uint8_t _subindex = 0,
                   uint8_t *_data = nullptr,
                   uint64_t _data_length = 0);

    void SDO_WRITE(bool_t _enable = false,
                   uint16_t _network = 1,
                   uint64_t _timeout = 0,
                   uint16_t _node_id = 0,
                   uint16_t _sdo_channel = 1,
                   uint16_t _index = 0,
                   uint8_t _subindex = 0,
                   uint8_t *_data = nullptr,
                   uint64_t _data_length = 0);

    // void REGISTER_COBID(bool_t _enable = false);
    // void PDO_READ8(bool_t _enable = false);
    // void PDO_WRITE8(bool_t _enable = false);
    // void WRITE_EMCY(bool_t _enable = false);

#ifdef CIA405_MASTER_NODE
    // void RECV_BOOTUP_DEV(bool_t _enable = false);
    // void RECV_BOOTUP(bool_t _enable = false);
    // void ENABLE_CYCLIC_SYNC(bool_t _enable = false);
    // void SEND_SYNC(bool_t _enable = false);
#endif

public:
    T_CIA405_STATE              _m_state;
    T_CIA405_EMCY_ERROR         _m_emcy;

protected:
    std::shared_ptr<IRegClient> _m_client;
    std::shared_ptr<IClientFactory> _m_factory;

    std::vector<RegItem> _m_reg_MS;
    std::vector<RegItem> _m_reg_SM;
    std::vector<ItemData> _m_reg_MS_data;
    std::vector<ItemData> _m_reg_SM_data;

    mutable std::mutex nv_mutex_;

protected:
    CO_CANptrSocketCan_t                    _m_CANptr = {0};
    T_Storage_t                             _m_storage;
    CO_LSS_address_t                        _m_lssAddress;
    CANopen                                 *_m_co_engien;
    CO_t                                    *_m_CO;
    T_1F80_NMT_START_UP                     _m_nmt_start_up;
    T_1F81_SLAVE_ASSIGNMENT                 _m_slave_assignment;
    CIA405_NMT_STATE                        _m_node_nmt_state;

    volatile __sig_atomic_t                 _m_endProgram;
    int                                     _m_programExit;
    uint32_t                                _m_errInfo;

    uint32_t                                _m_heapMemoryUsed;
    CO_config_t*                            _m_config_ptr;
#ifdef CO_MULTIPLE_OD
    /* example usage of CO_MULTIPLE_OD (but still single OD here) */
    CO_config_t                             _m_co_config = {0};
#endif
#if (CO_CONFIG_STORAGE) & CO_CONFIG_STORAGE_ENABLE
    CO_StorageLinux                         *_m_co_storage_linux_engien;
    CO_storage_t                            _m_storage;
    uint8_t                                 _m_storageEntriesCount;
    uint32_t                                _m_storageInitError;
    uint32_t                                _m_storageErrorPrev;
    uint32_t                                _m_storageIntervalTimer;
#endif

    CO_epoll_t                              _m_main_ep;
#ifndef CO_SINGLE_THREAD
    CO_epoll_t                              _m_rt_ep;
    pthread_t                               _m_rt_thread_id;
    int                                     _m_rt_priority;
    pthread_t                               _m_service_thread_id;
    int                                     _m_service_priority;
    pthread_t                               _m_main_thread_id;
    int                                     _m_main_priority;
#endif
#if (CO_CONFIG_GTW) & CO_CONFIG_GTW_ASCII
    CO_epoll_gtw_t                          _m_epGtw;
    qint32                                  _m_commandInterface;
#endif

    uint16_t                                _m_time_days;
    uint32_t                                _m_time_ms;

    T_CIA405_PROCESS_COMMAND                _m_process_cmd;
    T_CIA405_RESPONSE_PROCESS_COMMAND       _m_response_process_cmd;

#if (CO_CONFIG_TRACE) & CO_CONFIG_TRACE_ENABLE
    static CO_time_t                        CO_time; /* Object for current time */
#endif

    std::vector<T_NETWORK_VARIABLE_NEW>     _m_nv_ms;
    std::vector<T_NETWORK_VARIABLE_NEW>     _m_nv_sm;
    std::map<uint8_t, T_CIA405_SLAVE_NODE>  _m_own_node;
    bool                                    _m_is_SDO_configuration;

protected:
//    QList<QCanBusDeviceInfo>    _m_CAN_bus_list;
};

//-------------------------------------------------------
//
//-------------------------------------------------------

#endif // CIA405_H
