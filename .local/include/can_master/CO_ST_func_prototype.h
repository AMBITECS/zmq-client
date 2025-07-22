#ifndef CO_ST_FUNC_PROTOTYPE_H
#define CO_ST_FUNC_PROTOTYPE_H

#include "CIA405.h"

#ifdef CIA405_MASTER_NODE
//-------------------------------------------------------
//
//-------------------------------------------------------
void CIA405::NMT(bool_t _enable,            /* = FALSE*/
                 uint16_t _network,          /* = 1*/
                 uint64_t _timeout,          /* = 0*/
                 uint16_t _node_id,          /* = 0*/
                 CIA405_NMT_COMMAND _cmd)   /* = NMT_NO_COMMAND*/
{
    if(!_enable) return;
    _m_state.CONFIRM  = nmt(_node_id, _cmd);
    _m_state.STATE    = _m_response_process_cmd._error._nmt_error;
}
//-------------------------------------------------------
//
//-------------------------------------------------------
void CIA405::RECV_EMCY(bool_t _enable,    /* = FALSE*/
                       uint16_t _network,  /* = 1*/
                       uint64_t _timeout)  /* = 0*/
{
    if(!_enable) return;
}
//-------------------------------------------------------
//
//-------------------------------------------------------
void CIA405::RECV_EMCY_DEV(bool_t _enable,      /* = FALSE*/
                           uint16_t _network,    /* = 1*/
                           uint64_t _timeout,    /* = 0*/
                           uint16_t _node_id)    /* = 0*/
{
    if(!_enable) return;
}
#endif
//-------------------------------------------------------
//
//-------------------------------------------------------
void CIA405::GET_LOCAL_NODE_ID(bool_t _enable,      /* = FALSE*/
                               uint16_t _network,    /* = 1*/
                               uint64_t _timeout)    /* = 0*/
{
    uint8_t _node_id;
    if(!_enable) return;
    _m_state.CONFIRM  = get_local_node_id(_node_id);
    _m_state.STATE    = _node_id;
}
//-------------------------------------------------------
//
//-------------------------------------------------------
void CIA405::GET_CANOPEN_KERNEL_STATE(bool_t _enable,    /* = FALSE*/
                                      uint16_t _network,  /* = 1*/
                                      uint64_t _timeout)  /* = 0*/
{
    if(!_enable) return;
}
//-------------------------------------------------------
//
//-------------------------------------------------------
void CIA405::GET_STATE(bool_t _enable,              /* = FALSE*/
                       uint16_t _network,            /* = 1*/
                       uint64_t _timeout,            /* = 0*/
                       uint16_t _node_id)            /* = 0*/
{
    if(!_enable) return;
}
//-------------------------------------------------------
//
//-------------------------------------------------------
void CIA405::SDO_READ_DATA(bool_t _enable,          /* = FALSE*/
                           uint16_t _network,        /* = 1*/
                           uint64_t _timeout,        /* = 0*/
                           uint16_t _node_id,        /* = 0*/
                           uint16_t _sdo_channel,    /* = 1*/
                           uint16_t _index,          /* = 0*/
                           uint8_t _subindex,        /* = 0*/
                           SDO_MODE _mode,          /* = SDO_MODE::AUTO*/
                           uint8_t *_data,           /* = nullptr*/
                           uint64_t _data_length)    /* = 0*/
{
    if(!_enable) return;
    _m_state.CONFIRM  = sdo_read(_timeout, _node_id, _index, _subindex, _mode, _data, _data_length);
    _m_state._bufSize = _m_response_process_cmd._data._read_length;
    _m_state.STATE    = _m_response_process_cmd._error._sdo_error;
}
//-------------------------------------------------------
//
//-------------------------------------------------------
void CIA405::SDO_WRITE_DATA(bool_t _enable,         /* = FALSE*/
                            uint16_t _network,        /* = 1*/
                            uint64_t _timeout,        /* = 0*/
                            uint16_t _node_id,        /* = 0*/
                            uint16_t _sdo_channel,    /* = 1*/
                            uint16_t _index,          /* = 0*/
                            uint8_t _subindex,        /* = 0*/
                            SDO_MODE _mode,          /* = SDO_MODE::AUTO*/
                            uint8_t *_data,           /* = nullptr*/
                            uint64_t _data_length)    /* = 0*/
{
    if(!_enable) return;
    _m_state.CONFIRM  = sdo_write(_timeout, _node_id, _index, _subindex, _mode, _data, _data_length);
    _m_state._bufSize = _m_response_process_cmd._data._read_length;
    _m_state.STATE    = _m_response_process_cmd._error._sdo_error;
}
//-------------------------------------------------------
//
//-------------------------------------------------------
void CIA405::SDO_READ4(bool_t _enable,          /* = FALSE*/
                       uint16_t _network,        /* = 1*/
                       uint64_t _timeout,        /* = 0*/
                       uint16_t _node_id,        /* = 0*/
                       uint16_t _sdo_channel,    /* = 1*/
                       uint16_t _index,          /* = 0*/
                       uint8_t _subindex,        /* = 0*/
                       uint8_t *_data,           /* = nullptr*/
                       uint64_t _data_length)    /* = 0*/
{
    SDO_READ_DATA(_enable, _network, _timeout, _node_id, _sdo_channel, _index, _subindex, SDO_MODE::AUTO, _data, (_data_length > 4) ? 4 : _data_length);
}
//-------------------------------------------------------
//
//-------------------------------------------------------
void CIA405::SDO_WRITE4(bool_t _enable,            /* = FALSE*/
                        uint16_t _network,          /* = 1*/
                        uint64_t _timeout,          /* = 0*/
                        uint16_t _node_id,          /* = 0*/
                        uint16_t _sdo_channel,      /* = 1*/
                        uint16_t _index,            /* = 0*/
                        uint8_t _subindex,          /* = 0*/
                        uint8_t *_data,             /* = nullptr*/
                        uint64_t _data_length)      /* = 0*/
{
    SDO_WRITE_DATA(_enable, _network, _timeout, _node_id, _sdo_channel, _index, _subindex, SDO_MODE::AUTO, _data, (_data_length > 4) ? 4 : _data_length);
}
//-------------------------------------------------------
//
//-------------------------------------------------------
void CIA405::SDO_READ(bool_t _enable,             /* = FALSE*/
                      uint16_t _network,           /* = 1*/
                      uint64_t _timeout,           /* = 0*/
                      uint16_t _node_id,           /* = 0*/
                      uint16_t _sdo_channel,       /* = 1*/
                      uint16_t _index,             /* = 0*/
                      uint8_t _subindex,           /* = 0*/
                      uint8_t *_data,              /* = nullptr*/
                      uint64_t _data_length)       /* = 0*/
{
    SDO_READ_DATA(_enable, _network, _timeout, _node_id, _sdo_channel, _index, _subindex, SDO_MODE::AUTO, _data, _data_length);
}
//-------------------------------------------------------
//
//-------------------------------------------------------
void CIA405::SDO_WRITE(bool_t _enable,          /* = FALSE*/
                       uint16_t _network,       /* = 1*/
                       uint64_t _timeout,       /* = 0*/
                       uint16_t _node_id,       /* = 0*/
                       uint16_t _sdo_channel,   /* = 1*/
                       uint16_t _index,         /* = 0*/
                       uint8_t _subindex,       /* = 0*/
                       uint8_t *_data,          /* = nullptr*/
                       uint64_t _data_length)   /* = 0*/
{
    SDO_WRITE_DATA(_enable, _network, _timeout, _node_id, _sdo_channel, _index, _subindex, SDO_MODE::AUTO, _data, _data_length);
}

//-------------------------------------------------------
//
//-------------------------------------------------------


#endif // CO_ST_FUNC_PROTOTYPE_H
