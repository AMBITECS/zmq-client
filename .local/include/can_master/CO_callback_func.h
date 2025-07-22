#ifndef CO_CALLBACK_FUNC_H
#define CO_CALLBACK_FUNC_H
/*******************************************************************************
 * callback functions
 ******************************************************************************/
#if (CO_CONFIG_EM) & CO_CONFIG_EM_CONSUMER
/* callback for emergency messages */
static void EmergencyRxCallback(void *arg, const uint16_t ident, const uint16_t errorCode, const uint8_t errorRegister, const uint8_t errorBit, const uint32_t infoCode)
{
    ((CIA405 *)arg)->emergency_rx_callback(ident, errorCode, errorRegister, errorBit, infoCode);
}
void CIA405::emergency_rx_callback(const uint16_t ident, const uint16_t errorCode, const uint8_t errorRegister, const uint8_t errorBit, const uint32_t infoCode)
{
    int16_t nodeIdRx = ident ? (ident & 0x7F) : _m_storage._node_id;
    log_printf(LOG_NOTICE, DBG_EMERGENCY_RX, nodeIdRx, errorCode, errorRegister, errorBit, infoCode);
    check_node_emergency(ident, errorCode, errorRegister, errorBit, infoCode);
}
#endif
//-------------------------------------------------------
//
//-------------------------------------------------------
#if ((CO_CONFIG_NMT) & CO_CONFIG_NMT_CALLBACK_CHANGE) || ((CO_CONFIG_HB_CONS) & CO_CONFIG_HB_CONS_CALLBACK_CHANGE)
/* return string description of NMT state. */
static char* NmtState2Str(CO_NMT_internalState_t state)
{
    switch(state)
    {
    case CO_NMT_INITIALIZING:       return (char *)"initializing";
    case CO_NMT_PRE_OPERATIONAL:    return (char *)"pre-operational";
    case CO_NMT_OPERATIONAL:        return (char *)"operational";
    case CO_NMT_STOPPED:            return (char *)"stopped";
    default:                        return (char *)"unknown";
    }
}
#endif
//-------------------------------------------------------
//
//-------------------------------------------------------
#if (CO_CONFIG_NMT) & CO_CONFIG_NMT_CALLBACK_CHANGE
/* callback for NMT change messages */
static void NmtChangedCallback(void *arg, CO_NMT_internalState_t state)
{
    ((CIA405 *)arg)->nmt_changed_callback(state);
}
void CIA405::nmt_changed_callback(CO_NMT_internalState_t state)
{
    _m_node_nmt_state = (CIA405_NMT_STATE)state;
    log_printf(LOG_NOTICE, DBG_NMT_CHANGE, NmtState2Str(state), state);
}
#endif
//-------------------------------------------------------
//
//-------------------------------------------------------
#if (CO_CONFIG_HB_CONS) & CO_CONFIG_HB_CONS_CALLBACK_CHANGE
/* callback for monitoring Heartbeat remote NMT state change */
static void HeartbeatNmtChangedCallback(void *arg, uint8_t nodeId, uint8_t idx, CO_NMT_internalState_t state, void* object)
{
    ((CIA405 *)arg)->heartbeat_nmt_changed_callback(nodeId, idx, state, object);
}
void CIA405::heartbeat_nmt_changed_callback(uint8_t nodeId, uint8_t idx, CO_NMT_internalState_t state, void* object)
{
    log_printf(LOG_NOTICE, DBG_HB_CONS_NMT_CHANGE, nodeId, idx, NmtState2Str(state), state);
    set_node_state(nodeId, idx, (CIA405_NMT_STATE)state, object);
}
#endif
//-------------------------------------------------------
//
//-------------------------------------------------------
/* callback for storing node id and bitrate */
static bool_t LSScfgStoreCallback(void *arg, void* object, uint8_t id, uint16_t bitRate)
{
    return ((CIA405 *)arg)->lss_cfg_store_callback(object, id, bitRate);
}
bool_t CIA405::lss_cfg_store_callback(void* object, uint8_t id, uint16_t bitRate)
{
    T_Storage_t *_storage = (T_Storage_t *)object;
    _storage->_node_id  = id;
    _storage->_bitrate  = bitRate;
    return true;
}
/*******************************************************************************
 *
 ******************************************************************************/
#endif // CO_CALLBACK_FUNC_H
