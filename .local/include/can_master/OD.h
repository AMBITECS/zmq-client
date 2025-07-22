#ifndef OD_H
#define OD_H

#include<vector>

using namespace std;

#include "QCANopenNode/301/CO_ODinterface.h"

/*******************************************************************************
 CAN device param
*******************************************************************************/
#define CAN_DEVICE "can1"
//#define CAN_DEVICE "vcan0"

/*******************************************************************************
 Storage path param
*******************************************************************************/
#define STORAGE_PATH ""

/*******************************************************************************
 Bit timing param
*******************************************************************************/
#define BITRATE 500

/*******************************************************************************
 Node ID param
*******************************************************************************/
#define NODE_ID 127

/*******************************************************************************
 Slave Setup SDO param
*******************************************************************************/
#define OD_CNT_SLAVE_NODE 0x03
#define OD_CNT_SLAVE_NODE_SDO_SETUP 0x4E

typedef struct T_SDO_STORAGE
{
    uint8_t      _node_id;
    uint16_t     _index;
    uint8_t      _sub_index;
    uint32_t     _value;
    uint8_t      _value_size;
    bool        _abort_by_error;
    bool        _goto_next_point;
    uint32_t     _next_point;
}T_SDO_STORAGE;

/*******************************************************************************
 Network Variable Link
*******************************************************************************/
#define NET_VAR_SLAVE_TO_MASTER_QTY 37
#define NET_VAR_MASTER_TO_SLAVE_QTY 5

typedef struct T_NETWORK_VARIABLE_NEW
{
    uint8_t	_node_id;
    bool	_direct;
    void	*_value;
    char	_mem_link[15];
    uint32_t	_mem_index;
    uint8_t	_part;
    uint8_t	_shift;
    uint8_t	_var_type;
    uint16_t	_index;
    uint8_t	_sub_index;
    uint16_t	_rem_index;
    uint8_t	_rem_sub_index;
    bool	_type;
    uint8_t	_base_var_type;
}T_NETWORK_VARIABLE_NEW;

typedef struct T_NETWORK_VARIABLE
{
    char	_var_name[100];
    void	*_value;
    uint8_t	_var_type;
    char	_mem_link[15];
    uint16_t	_index;
    uint8_t	_sub_index;
    uint8_t	_node_id;
    bool	_type;
    bool	_direct;
    uint8_t	_shift;
}T_NETWORK_VARIABLE;

/*******************************************************************************
 Counters of OD objects
*******************************************************************************/
#define OD_CNT_NMT 1
#define OD_CNT_EM 1
#define OD_CNT_SYNC 1
#define OD_CNT_SYNC_PROD 1
#define OD_CNT_STORAGE 1
#define OD_CNT_TIME 1
#define OD_CNT_EM_PROD 1
#define OD_CNT_HB_CONS 1
#define OD_CNT_HB_PROD 1
#define OD_CNT_SDO_SRV 1
#define OD_CNT_SDO_CLI 1
#define OD_CNT_RPDO 15
#define OD_CNT_TPDO 2


/*******************************************************************************
 Sizes of OD arrays
*******************************************************************************/
#define OD_CNT_ARR_1003 0x08
#define OD_CNT_ARR_1010 0x04
#define OD_CNT_ARR_1011 0x04
#define OD_CNT_ARR_1016 0x03
#define OD_CNT_ARR_1F81 0x7f
#define OD_CNT_ARR_1F82 0x7f
#define OD_CNT_ARR_1F83 0x7f
#define OD_CNT_ARR_1F84 0x7f
#define OD_CNT_ARR_1F85 0x7f
#define OD_CNT_ARR_1F86 0x7f
#define OD_CNT_ARR_1F87 0x7f
#define OD_CNT_ARR_1F88 0x7f
#define OD_CNT_ARR_A000 0
#define OD_CNT_ARR_A040 0x0D
#define OD_CNT_ARR_A080 0
#define OD_CNT_ARR_A0C0 0
#define OD_CNT_ARR_A100 0
#define OD_CNT_ARR_A140 0
#define OD_CNT_ARR_A180 0
#define OD_CNT_ARR_A1C0 0
#define OD_CNT_ARR_A200 0x08
#define OD_CNT_ARR_A240 0x10
#define OD_CNT_ARR_A280 0
#define OD_CNT_ARR_A2C0 0
#define OD_CNT_ARR_A300 0
#define OD_CNT_ARR_A340 0
#define OD_CNT_ARR_A380 0
#define OD_CNT_ARR_A3C0 0
#define OD_CNT_ARR_A400 0
#define OD_CNT_ARR_A440 0
#define OD_CNT_ARR_A480 0
#define OD_CNT_ARR_A4C0 0x05
#define OD_CNT_ARR_A500 0
#define OD_CNT_ARR_A540 0
#define OD_CNT_ARR_A580 0
#define OD_CNT_ARR_A5C0 0
#define OD_CNT_ARR_A600 0
#define OD_CNT_ARR_A640 0
#define OD_CNT_ARR_A680 0
#define OD_CNT_ARR_A6C0 0
#define OD_CNT_ARR_A700 0
#define OD_CNT_ARR_A740 0
#define OD_CNT_ARR_A780 0
#define OD_CNT_ARR_A7C0 0
#define OD_CNT_ARR_A800 0
#define OD_CNT_ARR_A840 0
#define OD_CNT_ARR_A880 0
#define OD_CNT_ARR_A8C0 0
#define OD_CNT_ARR_A900 0
#define OD_CNT_ARR_A940 0


/*******************************************************************************
 OD data declaration of all groups
*******************************************************************************/
typedef struct {
    uint32_t x1000_Device_type;
    uint32_t x1005_COB_ID_SYNC_message;
    uint32_t x1006_Communication_cycle_period;
    uint32_t x1007_Synchronous_window_length;
    uint32_t x1012_COB_ID_time_stamp_object;
    uint32_t x1013_High_resolution_time_stamp;
    uint32_t x1014_COB_ID_EMCY;
    uint16_t x1015_Inhibit_time_EMCY;
    uint8_t x1016_Consumer_heartbeat_time_sub0;
    uint32_t x1016_Consumer_heartbeat_time[OD_CNT_ARR_1016];
    uint16_t x1017_Producer_heartbeat_time;
    struct {
        uint8_t Highest_sub_index_supported;
        uint32_t Vendor_ID;
        uint32_t Product_Code;
        uint32_t Revision_Number;
        uint32_t Serial_Number;
    }x1018_Identity;
    struct {
        uint8_t Highest_sub_index_supported;
        uint32_t COB_ID_client_to_server_tx;
        uint32_t COB_ID_server_to_client_rx;
        uint8_t Node_ID_of_the_SDO_server;
    }x1280_SDO_client_parameter;
}OD_PERSIST_COMM_t;

typedef struct {
    uint8_t x1001_Error_register;
    uint8_t x1003_Pre_defined_error_field_sub0;
    uint32_t x1003_Pre_defined_error_field[OD_CNT_ARR_1003];
    uint8_t x1010_Store_parameters_sub0;
    uint32_t x1010_Store_parameters[OD_CNT_ARR_1010];
    uint8_t x1011_Restore_default_parameters_sub0;
    uint32_t x1011_Restore_default_parameters[OD_CNT_ARR_1011];
    struct {
        uint8_t Highest_sub_index_supported;
        uint32_t COB_ID_client_to_server_rx;
        uint32_t COB_ID_server_to_client_tx;
    }x1200_SDO_server_parameter;
    struct {
        uint8_t Highest_sub_index_supported;
        uint32_t COB_ID_used_by_RPDO;
        uint8_t Transmission_type;
        uint16_t Event_timer;
    }x1400_RPDO_communication_parameter;
    struct {
        uint8_t Highest_sub_index_supported;
        uint32_t COB_ID_used_by_RPDO;
        uint8_t Transmission_type;
        uint16_t Event_timer;
    }x1401_RPDO_communication_parameter;
    struct {
        uint8_t Highest_sub_index_supported;
        uint32_t COB_ID_used_by_RPDO;
        uint8_t Transmission_type;
        uint16_t Event_timer;
    }x1402_RPDO_communication_parameter;
    struct {
        uint8_t Highest_sub_index_supported;
        uint32_t COB_ID_used_by_RPDO;
        uint8_t Transmission_type;
        uint16_t Event_timer;
    }x1403_RPDO_communication_parameter;
    struct {
        uint8_t Highest_sub_index_supported;
        uint32_t COB_ID_used_by_RPDO;
        uint8_t Transmission_type;
        uint16_t Event_timer;
    }x1404_RPDO_communication_parameter;
    struct {
        uint8_t Highest_sub_index_supported;
        uint32_t COB_ID_used_by_RPDO;
        uint8_t Transmission_type;
        uint16_t Event_timer;
    }x1405_RPDO_communication_parameter;
    struct {
        uint8_t Highest_sub_index_supported;
        uint32_t COB_ID_used_by_RPDO;
        uint8_t Transmission_type;
        uint16_t Event_timer;
    }x1406_RPDO_communication_parameter;
    struct {
        uint8_t Highest_sub_index_supported;
        uint32_t COB_ID_used_by_RPDO;
        uint8_t Transmission_type;
        uint16_t Event_timer;
    }x1407_RPDO_communication_parameter;
    struct {
        uint8_t Highest_sub_index_supported;
        uint32_t COB_ID_used_by_RPDO;
        uint8_t Transmission_type;
        uint16_t Event_timer;
    }x1408_RPDO_communication_parameter;
    struct {
        uint8_t Highest_sub_index_supported;
        uint32_t COB_ID_used_by_RPDO;
        uint8_t Transmission_type;
        uint16_t Event_timer;
    }x1409_RPDO_communication_parameter;
    struct {
        uint8_t Highest_sub_index_supported;
        uint32_t COB_ID_used_by_RPDO;
        uint8_t Transmission_type;
        uint16_t Event_timer;
    }x140A_RPDO_communication_parameter;
    struct {
        uint8_t Highest_sub_index_supported;
        uint32_t COB_ID_used_by_RPDO;
        uint8_t Transmission_type;
        uint16_t Event_timer;
    }x140B_RPDO_communication_parameter;
    struct {
        uint8_t Highest_sub_index_supported;
        uint32_t COB_ID_used_by_RPDO;
        uint8_t Transmission_type;
        uint16_t Event_timer;
    }x140C_RPDO_communication_parameter;
    struct {
        uint8_t Highest_sub_index_supported;
        uint32_t COB_ID_used_by_RPDO;
        uint8_t Transmission_type;
        uint16_t Event_timer;
    }x140D_RPDO_communication_parameter;
    struct {
        uint8_t Highest_sub_index_supported;
        uint32_t COB_ID_used_by_RPDO;
        uint8_t Transmission_type;
        uint16_t Event_timer;
    }x140E_RPDO_communication_parameter;
    struct {
        uint8_t Number_of_mapped_application_objects_in_PDO;
        uint32_t App_object_1;
        uint32_t App_object_2;
    }x1600_RPDO_mapping_parameter;
    struct {
        uint8_t Number_of_mapped_application_objects_in_PDO;
        uint32_t App_object_1;
        uint32_t App_object_2;
    }x1601_RPDO_mapping_parameter;
    struct {
        uint8_t Number_of_mapped_application_objects_in_PDO;
        uint32_t App_object_1;
        uint32_t App_object_2;
    }x1602_RPDO_mapping_parameter;
    struct {
        uint8_t Number_of_mapped_application_objects_in_PDO;
        uint32_t App_object_1;
        uint32_t App_object_2;
    }x1603_RPDO_mapping_parameter;
    struct {
        uint8_t Number_of_mapped_application_objects_in_PDO;
        uint32_t App_object_1;
        uint32_t App_object_2;
    }x1604_RPDO_mapping_parameter;
    struct {
        uint8_t Number_of_mapped_application_objects_in_PDO;
        uint32_t App_object_1;
        uint32_t App_object_2;
    }x1605_RPDO_mapping_parameter;
    struct {
        uint8_t Number_of_mapped_application_objects_in_PDO;
        uint32_t App_object_1;
        uint32_t App_object_2;
    }x1606_RPDO_mapping_parameter;
    struct {
        uint8_t Number_of_mapped_application_objects_in_PDO;
        uint32_t App_object_1;
        uint32_t App_object_2;
    }x1607_RPDO_mapping_parameter;
    struct {
        uint8_t Number_of_mapped_application_objects_in_PDO;
        uint32_t App_object_1;
        uint32_t App_object_2;
        uint32_t App_object_3;
        uint32_t App_object_4;
        uint32_t App_object_5;
    }x1608_RPDO_mapping_parameter;
    struct {
        uint8_t Number_of_mapped_application_objects_in_PDO;
        uint32_t App_object_1;
        uint32_t App_object_2;
    }x1609_RPDO_mapping_parameter;
    struct {
        uint8_t Number_of_mapped_application_objects_in_PDO;
        uint32_t App_object_1;
        uint32_t App_object_2;
    }x160A_RPDO_mapping_parameter;
    struct {
        uint8_t Number_of_mapped_application_objects_in_PDO;
        uint32_t App_object_1;
        uint32_t App_object_2;
    }x160B_RPDO_mapping_parameter;
    struct {
        uint8_t Number_of_mapped_application_objects_in_PDO;
        uint32_t App_object_1;
        uint32_t App_object_2;
    }x160C_RPDO_mapping_parameter;
    struct {
        uint8_t Number_of_mapped_application_objects_in_PDO;
        uint32_t App_object_1;
        uint32_t App_object_2;
        uint32_t App_object_3;
        uint32_t App_object_4;
    }x160D_RPDO_mapping_parameter;
    struct {
        uint8_t Number_of_mapped_application_objects_in_PDO;
        uint32_t App_object_1;
        uint32_t App_object_2;
        uint32_t App_object_3;
        uint32_t App_object_4;
    }x160E_RPDO_mapping_parameter;
    struct {
        uint8_t Highest_sub_index_supported;
        uint32_t COB_ID_used_by_TPDO;
        uint8_t Transmission_type;
        uint16_t Inhibit_time;
        uint16_t Event_timer;
        uint8_t SYNC_start_value;
    }x1800_TPDO_communication_parameter;
    struct {
        uint8_t Highest_sub_index_supported;
        uint32_t COB_ID_used_by_TPDO;
        uint8_t Transmission_type;
        uint16_t Inhibit_time;
        uint16_t Event_timer;
        uint8_t SYNC_start_value;
    }x1801_TPDO_communication_parameter;
    struct {
        uint8_t Number_of_mapped_application_objects_in_PDO;
        uint32_t App_object_1;
    }x1A00_TPDO_mapping_parameter;
    struct {
        uint8_t Number_of_mapped_application_objects_in_PDO;
        uint32_t App_object_1;
        uint32_t App_object_2;
        uint32_t App_object_3;
        uint32_t App_object_4;
    }x1A01_TPDO_mapping_parameter;
    uint32_t x1F80_NMTStartup;
    uint8_t x1F81_SlaveAssignment_sub0;
    uint32_t x1F81_SlaveAssignment[OD_CNT_ARR_1F81];
    uint8_t x1F82_RequestNMT_sub0;
    uint8_t x1F82_RequestNMT[OD_CNT_ARR_1F82];
    uint8_t x1F83_RequestGuarding_sub0;
    uint8_t x1F83_RequestGuarding[OD_CNT_ARR_1F83];
    uint8_t x1F84_DeviceTypeIdentification_sub0;
    uint32_t x1F84_DeviceTypeIdentification[OD_CNT_ARR_1F84];
    uint8_t x1F85_VendorIdentification_sub0;
    uint32_t x1F85_VendorIdentification[OD_CNT_ARR_1F85];
    uint8_t x1F86_ProductCode_sub0;
    uint32_t x1F86_ProductCode[OD_CNT_ARR_1F86];
    uint8_t x1F87_RevisionNumber_sub0;
    uint32_t x1F87_RevisionNumber[OD_CNT_ARR_1F87];
    uint8_t x1F88_SerialNumber_sub0;
    uint32_t x1F88_SerialNumber[OD_CNT_ARR_1F88];
    uint32_t x1F89_BootTime;
    struct {
        uint8_t Highest_sub_index_supported;
        uint8_t Upper_Limit;
        uint8_t Lower_Limit;
    }x3001_Interval_limits_of_the_network_scan;
    struct {
        uint8_t Highest_sub_index_supported;
        uint32_t COBID_node_state_message;
    }x3002_COBID_for_Node_State_Messages;
    struct {
        uint8_t Highest_sub_index_supported;
        uint32_t Boot_Network_Delay;
        uint32_t PLC_Start_Delay;
    }x3003_Defining_waiting_periods;
    uint8_t xA040_uint8_sub0;
    uint8_t xA040_uint8[OD_CNT_ARR_A040];
    uint8_t xA200_uint32_sub0;
    uint32_t xA200_uint32[OD_CNT_ARR_A200];
    uint8_t xA240_float32_sub0;
    float32_t xA240_float32[OD_CNT_ARR_A240];
    uint8_t xA4C0_uint8_sub0;
    uint8_t xA4C0_uint8[OD_CNT_ARR_A4C0];
}OD_RAM_t;

#ifndef OD_ATTR_PERSIST_COMM
#define OD_ATTR_PERSIST_COMM
#endif
extern OD_ATTR_PERSIST_COMM OD_PERSIST_COMM_t OD_PERSIST_COMM;

#ifndef OD_ATTR_RAM
#define OD_ATTR_RAM
#endif
extern OD_ATTR_RAM OD_RAM_t OD_RAM;

#ifndef OD_ATTR_OD
#define OD_ATTR_OD
#endif
extern OD_ATTR_OD OD_t *OD;


/*******************************************************************************
 Object dictionary entries - shortcuts
*******************************************************************************/
#define OD_ENTRY_H1000 &OD->list[0]
#define OD_ENTRY_H1001 &OD->list[1]
#define OD_ENTRY_H1003 &OD->list[2]
#define OD_ENTRY_H1005 &OD->list[3]
#define OD_ENTRY_H1006 &OD->list[4]
#define OD_ENTRY_H1007 &OD->list[5]
#define OD_ENTRY_H1010 &OD->list[6]
#define OD_ENTRY_H1011 &OD->list[7]
#define OD_ENTRY_H1012 &OD->list[8]
#define OD_ENTRY_H1013 &OD->list[9]
#define OD_ENTRY_H1014 &OD->list[10]
#define OD_ENTRY_H1015 &OD->list[11]
#define OD_ENTRY_H1016 &OD->list[12]
#define OD_ENTRY_H1017 &OD->list[13]
#define OD_ENTRY_H1018 &OD->list[14]
#define OD_ENTRY_H1200 &OD->list[15]
#define OD_ENTRY_H1280 &OD->list[16]
#define OD_ENTRY_H1400 &OD->list[17]
#define OD_ENTRY_H1401 &OD->list[18]
#define OD_ENTRY_H1402 &OD->list[19]
#define OD_ENTRY_H1403 &OD->list[20]
#define OD_ENTRY_H1404 &OD->list[21]
#define OD_ENTRY_H1405 &OD->list[22]
#define OD_ENTRY_H1406 &OD->list[23]
#define OD_ENTRY_H1407 &OD->list[24]
#define OD_ENTRY_H1408 &OD->list[25]
#define OD_ENTRY_H1409 &OD->list[26]
#define OD_ENTRY_H140A &OD->list[27]
#define OD_ENTRY_H140B &OD->list[28]
#define OD_ENTRY_H140C &OD->list[29]
#define OD_ENTRY_H140D &OD->list[30]
#define OD_ENTRY_H140E &OD->list[31]
#define OD_ENTRY_H1600 &OD->list[32]
#define OD_ENTRY_H1601 &OD->list[33]
#define OD_ENTRY_H1602 &OD->list[34]
#define OD_ENTRY_H1603 &OD->list[35]
#define OD_ENTRY_H1604 &OD->list[36]
#define OD_ENTRY_H1605 &OD->list[37]
#define OD_ENTRY_H1606 &OD->list[38]
#define OD_ENTRY_H1607 &OD->list[39]
#define OD_ENTRY_H1608 &OD->list[40]
#define OD_ENTRY_H1609 &OD->list[41]
#define OD_ENTRY_H160A &OD->list[42]
#define OD_ENTRY_H160B &OD->list[43]
#define OD_ENTRY_H160C &OD->list[44]
#define OD_ENTRY_H160D &OD->list[45]
#define OD_ENTRY_H160E &OD->list[46]
#define OD_ENTRY_H1800 &OD->list[47]
#define OD_ENTRY_H1801 &OD->list[48]
#define OD_ENTRY_H1A00 &OD->list[49]
#define OD_ENTRY_H1A01 &OD->list[50]
#define OD_ENTRY_H1F80 &OD->list[51]
#define OD_ENTRY_H1F81 &OD->list[52]
#define OD_ENTRY_H1F82 &OD->list[53]
#define OD_ENTRY_H1F83 &OD->list[54]
#define OD_ENTRY_H1F84 &OD->list[55]
#define OD_ENTRY_H1F85 &OD->list[56]
#define OD_ENTRY_H1F86 &OD->list[57]
#define OD_ENTRY_H1F87 &OD->list[58]
#define OD_ENTRY_H1F88 &OD->list[59]
#define OD_ENTRY_H1F89 &OD->list[60]
#define OD_ENTRY_H3001 &OD->list[61]
#define OD_ENTRY_H3002 &OD->list[62]
#define OD_ENTRY_H3003 &OD->list[63]
#define OD_ENTRY_HA000 &OD->list[64]
#define OD_ENTRY_HA040 &OD->list[65]
#define OD_ENTRY_HA080 &OD->list[66]
#define OD_ENTRY_HA0C0 &OD->list[67]
#define OD_ENTRY_HA100 &OD->list[68]
#define OD_ENTRY_HA140 &OD->list[69]
#define OD_ENTRY_HA180 &OD->list[70]
#define OD_ENTRY_HA1C0 &OD->list[71]
#define OD_ENTRY_HA200 &OD->list[72]
#define OD_ENTRY_HA240 &OD->list[73]
#define OD_ENTRY_HA280 &OD->list[74]
#define OD_ENTRY_HA2C0 &OD->list[75]
#define OD_ENTRY_HA300 &OD->list[76]
#define OD_ENTRY_HA340 &OD->list[77]
#define OD_ENTRY_HA380 &OD->list[78]
#define OD_ENTRY_HA3C0 &OD->list[79]
#define OD_ENTRY_HA400 &OD->list[80]
#define OD_ENTRY_HA440 &OD->list[81]
#define OD_ENTRY_HA480 &OD->list[82]
#define OD_ENTRY_HA4C0 &OD->list[83]
#define OD_ENTRY_HA500 &OD->list[84]
#define OD_ENTRY_HA540 &OD->list[85]
#define OD_ENTRY_HA580 &OD->list[86]
#define OD_ENTRY_HA5C0 &OD->list[87]
#define OD_ENTRY_HA600 &OD->list[88]
#define OD_ENTRY_HA640 &OD->list[89]
#define OD_ENTRY_HA680 &OD->list[90]
#define OD_ENTRY_HA6C0 &OD->list[91]
#define OD_ENTRY_HA700 &OD->list[92]
#define OD_ENTRY_HA740 &OD->list[93]
#define OD_ENTRY_HA780 &OD->list[94]
#define OD_ENTRY_HA7C0 &OD->list[95]
#define OD_ENTRY_HA800 &OD->list[96]
#define OD_ENTRY_HA840 &OD->list[97]
#define OD_ENTRY_HA880 &OD->list[98]
#define OD_ENTRY_HA8C0 &OD->list[99]
#define OD_ENTRY_HA900 &OD->list[100]
#define OD_ENTRY_HA940 &OD->list[101]


/*******************************************************************************
 Object dictionary entries - shortcuts with names
*******************************************************************************/
#define OD_ENTRY_H1000_Device_type &OD->list[0]
#define OD_ENTRY_H1001_Error_register &OD->list[1]
#define OD_ENTRY_H1003_Pre_defined_error_field &OD->list[2]
#define OD_ENTRY_H1005_COB_ID_SYNC_message &OD->list[3]
#define OD_ENTRY_H1006_Communication_cycle_period &OD->list[4]
#define OD_ENTRY_H1007_Synchronous_window_length &OD->list[5]
#define OD_ENTRY_H1010_Store_parameters &OD->list[6]
#define OD_ENTRY_H1011_Restore_default_parameters &OD->list[7]
#define OD_ENTRY_H1012_COB_ID_time_stamp_object &OD->list[8]
#define OD_ENTRY_H1013_High_resolution_time_stamp &OD->list[9]
#define OD_ENTRY_H1014_COB_ID_EMCY &OD->list[10]
#define OD_ENTRY_H1015_Inhibit_time_EMCY &OD->list[11]
#define OD_ENTRY_H1016_Consumer_heartbeat_time &OD->list[12]
#define OD_ENTRY_H1017_Producer_heartbeat_time &OD->list[13]
#define OD_ENTRY_H1018_Identity &OD->list[14]
#define OD_ENTRY_H1200_SDO_server_parameter &OD->list[15]
#define OD_ENTRY_H1280_SDO_client_parameter &OD->list[16]
#define OD_ENTRY_H1400_RPDO_communication_parameter &OD->list[17]
#define OD_ENTRY_H1401_RPDO_communication_parameter &OD->list[18]
#define OD_ENTRY_H1402_RPDO_communication_parameter &OD->list[19]
#define OD_ENTRY_H1403_RPDO_communication_parameter &OD->list[20]
#define OD_ENTRY_H1404_RPDO_communication_parameter &OD->list[21]
#define OD_ENTRY_H1405_RPDO_communication_parameter &OD->list[22]
#define OD_ENTRY_H1406_RPDO_communication_parameter &OD->list[23]
#define OD_ENTRY_H1407_RPDO_communication_parameter &OD->list[24]
#define OD_ENTRY_H1408_RPDO_communication_parameter &OD->list[25]
#define OD_ENTRY_H1409_RPDO_communication_parameter &OD->list[26]
#define OD_ENTRY_H140A_RPDO_communication_parameter &OD->list[27]
#define OD_ENTRY_H140B_RPDO_communication_parameter &OD->list[28]
#define OD_ENTRY_H140C_RPDO_communication_parameter &OD->list[29]
#define OD_ENTRY_H140D_RPDO_communication_parameter &OD->list[30]
#define OD_ENTRY_H140E_RPDO_communication_parameter &OD->list[31]
#define OD_ENTRY_H1600_RPDO_mapping_parameter &OD->list[32]
#define OD_ENTRY_H1601_RPDO_mapping_parameter &OD->list[33]
#define OD_ENTRY_H1602_RPDO_mapping_parameter &OD->list[34]
#define OD_ENTRY_H1603_RPDO_mapping_parameter &OD->list[35]
#define OD_ENTRY_H1604_RPDO_mapping_parameter &OD->list[36]
#define OD_ENTRY_H1605_RPDO_mapping_parameter &OD->list[37]
#define OD_ENTRY_H1606_RPDO_mapping_parameter &OD->list[38]
#define OD_ENTRY_H1607_RPDO_mapping_parameter &OD->list[39]
#define OD_ENTRY_H1608_RPDO_mapping_parameter &OD->list[40]
#define OD_ENTRY_H1609_RPDO_mapping_parameter &OD->list[41]
#define OD_ENTRY_H160A_RPDO_mapping_parameter &OD->list[42]
#define OD_ENTRY_H160B_RPDO_mapping_parameter &OD->list[43]
#define OD_ENTRY_H160C_RPDO_mapping_parameter &OD->list[44]
#define OD_ENTRY_H160D_RPDO_mapping_parameter &OD->list[45]
#define OD_ENTRY_H160E_RPDO_mapping_parameter &OD->list[46]
#define OD_ENTRY_H1800_TPDO_communication_parameter &OD->list[47]
#define OD_ENTRY_H1801_TPDO_communication_parameter &OD->list[48]
#define OD_ENTRY_H1A00_TPDO_mapping_parameter &OD->list[49]
#define OD_ENTRY_H1A01_TPDO_mapping_parameter &OD->list[50]
#define OD_ENTRY_H1F80_NMTStartup &OD->list[51]
#define OD_ENTRY_H1F81_SlaveAssignment &OD->list[52]
#define OD_ENTRY_H1F82_RequestNMT &OD->list[53]
#define OD_ENTRY_H1F83_RequestGuarding &OD->list[54]
#define OD_ENTRY_H1F84_DeviceTypeIdentification &OD->list[55]
#define OD_ENTRY_H1F85_VendorIdentification &OD->list[56]
#define OD_ENTRY_H1F86_ProductCode &OD->list[57]
#define OD_ENTRY_H1F87_RevisionNumber &OD->list[58]
#define OD_ENTRY_H1F88_SerialNumber &OD->list[59]
#define OD_ENTRY_H1F89_BootTime &OD->list[60]
#define OD_ENTRY_H3001_Interval_limits_of_the_network_scan &OD->list[61]
#define OD_ENTRY_H3002_COBID_for_Node_State_Messages &OD->list[62]
#define OD_ENTRY_H3003_Defining_waiting_periods &OD->list[63]
#define OD_ENTRY_HA000_int8 &OD->list[64]
#define OD_ENTRY_HA040_uint8 &OD->list[65]
#define OD_ENTRY_HA080_Bool &OD->list[66]
#define OD_ENTRY_HA0C0_int16 &OD->list[67]
#define OD_ENTRY_HA100_uint16 &OD->list[68]
#define OD_ENTRY_HA140_int24 &OD->list[69]
#define OD_ENTRY_HA180_uint24 &OD->list[70]
#define OD_ENTRY_HA1C0_int32 &OD->list[71]
#define OD_ENTRY_HA200_uint32 &OD->list[72]
#define OD_ENTRY_HA240_float32 &OD->list[73]
#define OD_ENTRY_HA280_uint40 &OD->list[74]
#define OD_ENTRY_HA2C0_int40 &OD->list[75]
#define OD_ENTRY_HA300_uint48 &OD->list[76]
#define OD_ENTRY_HA340_int48 &OD->list[77]
#define OD_ENTRY_HA380_uint56 &OD->list[78]
#define OD_ENTRY_HA3C0_int56 &OD->list[79]
#define OD_ENTRY_HA400_int64 &OD->list[80]
#define OD_ENTRY_HA440_uint64 &OD->list[81]
#define OD_ENTRY_HA480_int8 &OD->list[82]
#define OD_ENTRY_HA4C0_uint8 &OD->list[83]
#define OD_ENTRY_HA500_bool &OD->list[84]
#define OD_ENTRY_HA540_int16 &OD->list[85]
#define OD_ENTRY_HA580_uint16 &OD->list[86]
#define OD_ENTRY_HA5C0_int24 &OD->list[87]
#define OD_ENTRY_HA600_uint24 &OD->list[88]
#define OD_ENTRY_HA640_int32 &OD->list[89]
#define OD_ENTRY_HA680_uint32 &OD->list[90]
#define OD_ENTRY_HA6C0_float32 &OD->list[91]
#define OD_ENTRY_HA700_uint40 &OD->list[92]
#define OD_ENTRY_HA740_int40 &OD->list[93]
#define OD_ENTRY_HA780_uint48 &OD->list[94]
#define OD_ENTRY_HA7C0_int48 &OD->list[95]
#define OD_ENTRY_HA800_uint56 &OD->list[96]
#define OD_ENTRY_HA840_int56 &OD->list[97]
#define OD_ENTRY_HA880_int64 &OD->list[98]
#define OD_ENTRY_HA8C0_uint64 &OD->list[99]
#define OD_ENTRY_HA900_float64 &OD->list[100]
#define OD_ENTRY_HA940_float64 &OD->list[101]


/*******************************************************************************
 OD config structure
*******************************************************************************/
#ifdef CO_MULTIPLE_OD
#define OD_INIT_CONFIG(config) {\
    (config).CNT_NMT = OD_CNT_NMT;\
    (config).ENTRY_H1017 = OD_ENTRY_H1017;\
    (config).CNT_HB_CONS = OD_CNT_HB_CONS;\
    (config).CNT_ARR_1016 = OD_CNT_ARR_1016;\
    (config).ENTRY_H1016 = OD_ENTRY_H1016;\
    (config).ENTRY_H100C = NULL;\
    (config).ENTRY_H100D = NULL;\
    (config).CNT_EM = OD_CNT_EM;\
    (config).ENTRY_H1001 = OD_ENTRY_H1001;\
    (config).ENTRY_H1014 = OD_ENTRY_H1014;\
    (config).ENTRY_H1015 = OD_ENTRY_H1015;\
    (config).CNT_ARR_1003 = OD_CNT_ARR_1003;\
    (config).ENTRY_H1003 = OD_ENTRY_H1003;\
    (config).CNT_SDO_SRV = OD_CNT_SDO_SRV;\
    (config).ENTRY_H1200 = OD_ENTRY_H1200;\
    (config).CNT_SDO_CLI = OD_CNT_SDO_CLI;\
    (config).ENTRY_H1280 = OD_ENTRY_H1280;\
    (config).CNT_TIME = OD_CNT_TIME;\
    (config).ENTRY_H1012 = OD_ENTRY_H1012;\
    (config).CNT_SYNC = OD_CNT_SYNC;\
    (config).ENTRY_H1005 = OD_ENTRY_H1005;\
    (config).ENTRY_H1006 = OD_ENTRY_H1006;\
    (config).ENTRY_H1007 = OD_ENTRY_H1007;\
    (config).ENTRY_H1019 = NULL;\
    (config).CNT_RPDO = OD_CNT_RPDO;\
    (config).ENTRY_H1400 = OD_ENTRY_H1400;\
    (config).ENTRY_H1600 = OD_ENTRY_H1600;\
    (config).CNT_TPDO = OD_CNT_TPDO;\
    (config).ENTRY_H1800 = OD_ENTRY_H1800;\
    (config).ENTRY_H1A00 = OD_ENTRY_H1A00;\
    (config).CNT_LEDS = 0;\
    (config).CNT_GFC = 0;\
    (config).ENTRY_H1300 = NULL;\
    (config).CNT_SRDO = 0;\
    (config).ENTRY_H1301 = NULL;\
    (config).ENTRY_H1381 = NULL;\
    (config).ENTRY_H13FE = NULL;\
    (config).ENTRY_H13FF = NULL;\
    (config).CNT_LSS_SLV = 1;\
    (config).CNT_LSS_MST = 1;\
    (config).CNT_GTWA = 0;\
    (config).CNT_TRACE = 0;\
}
#endif

#endif /* OD_H */
