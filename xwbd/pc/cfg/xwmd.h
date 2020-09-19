/**
 * @file
 * @brief 中间件/模块/驱动配置
 */

#ifndef __cfg_xwmd_h__
#define __cfg_xwmd_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********    base config    ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWMDCFG_CHECK_PARAMETERS                                0

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********  Power Management ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWMDCFG_pm                                              1

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       xwsys       ******** ******** ********
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
#define XWMDCFG_isc_xwpcp_PRIORITY_NUM                          (4U)
#define XWMDCFG_isc_xwpcp_RETRY_PERIOD                          (1 * XWTM_S)
#define XWMDCFG_isc_xwpcp_RETRY_NUM                             (8U)
#define XWMDCFG_isc_xwpcp_MEMBLK_SIZE                           (64U)
#define XWMDCFG_isc_xwpcp_MEMBLK_NUM                            (64U)
#define XWMDCFG_isc_xwpcp_SYNC_KEY                              ('P')
#define XWMDCFG_isc_xwpcp_RXTHRD_PRIORITY                       \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 20)
#define XWMDCFG_isc_xwpcp_TXTHRD_PRIORITY                       \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 20)

/******** ******** simple communication protocol ******** ********/
#define XWMDCFG_isc_xwscp                                       1
#define XWMDCFG_isc_xwscp_LOG                                   1
#define XWMDCFG_isc_xwscp_PERIOD                                (1 * XWTM_S)
#define XWMDCFG_isc_xwscp_RETRY_NUM                             (8U)
#define XWMDCFG_isc_xwscp_SDU_MAX_SIZE                          (40U)
#define XWMDCFG_isc_xwscp_FRMSLOT_NUM                           (8U)
#define XWMDCFG_isc_xwscp_SYNC_KEY                              ('S')
#define XWMDCFG_isc_xwscp_THRD_PRIORITY                         \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 20)

#endif /* cfg/xwmd.h */
