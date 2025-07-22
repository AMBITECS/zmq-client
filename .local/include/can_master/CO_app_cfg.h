#ifndef CO_APP_CFG_H
#define CO_APP_CFG_H

/*******************************************************************************
 FEATURES
*******************************************************************************/
#define CIA405_MASTER_NODE
#define CIA405_SLAVE_NODE

#ifdef CIA405_MASTER_NODE
    #undef CIA405_SLAVE_NODE
#endif

#define CO_MULTIPLE_OD
//#define CO_USE_APPLICATION

#ifdef CIA405_MASTER_NODE
    #define CO_CONFIG_NODE_GUARDING (CO_CONFIG_NODE_GUARDING_MASTER_ENABLE)
#endif
#ifdef CIA405_SLAVE_NODE
    #define CO_CONFIG_NODE_GUARDING (CO_CONFIG_NODE_GUARDING_SLAVE_ENABLE)
    #define CO_CONFIG_HB_CONS (0)
#endif

#define CO_CONFIG_STORAGE (0)
#define CO_CONFIG_LEDS (0)
#define CO_CONFIG_GTW (0)
/*******************************************************************************

*******************************************************************************/

#endif // CO_APP_CFG_H
