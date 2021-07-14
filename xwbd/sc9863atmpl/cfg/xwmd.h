/**
 * @file
 * @brief 中间件/模块/驱动配置
 */

#ifndef __cfg_xwmd_h__
#define __cfg_xwmd_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********    Base Config    ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWMDCFG_CHECK_PARAMETERS                                0

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********  Power Management ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWMDCFG_pm                                              1

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       XWSYS       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWMDCFG_sysfs                                           1
#define XWMDCFG_sysfs_LOG                                       1

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       XWFS        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWMDCFG_xwfs                                            1
#define XWMDCFG_xwfs_LOG                                        1

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     Inter-System Communication      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** point to point communication protocol ******** ********/
#define XWMDCFG_isc_xwpcp                                       1
#define XWMDCFG_isc_xwpcp_LOG                                   1
#define XWMDCFG_isc_xwpcp_PORT_NUM                              (12U)
#define XWMDCFG_isc_xwpcp_PRI_NUM                               (4U)
#define XWMDCFG_isc_xwpcp_RETRY_PERIOD                          (1 * XWTM_S)
#define XWMDCFG_isc_xwpcp_RETRY_NUM                             (8U)
#define XWMDCFG_isc_xwpcp_MEMBLK_SIZE                           (64U)
#define XWMDCFG_isc_xwpcp_MEMBLK_ODR                            (6U)
#define XWMDCFG_isc_xwpcp_RXTHD_PRIORITY                        \
        XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 20)
#define XWMDCFG_isc_xwpcp_TXTHD_PRIORITY                        \
        XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 20)

/******** ******** simple communication protocol ******** ********/
#define XWMDCFG_isc_xwscp                                       1
#define XWMDCFG_isc_xwscp_LOG                                   1
#define XWMDCFG_isc_xwscp_PERIOD                                (1 * XWTM_S)
#define XWMDCFG_isc_xwscp_MEMBLK_SIZE                           (64U)
#define XWMDCFG_isc_xwscp_MEMBLK_ODR                            (6U)
#define XWMDCFG_isc_xwscp_THD_PRIORITY                          \
        XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 20)

#endif /* cfg/xwmd.h */
