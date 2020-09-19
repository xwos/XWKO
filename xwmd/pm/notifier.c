/**
 * @file
 * @brief 板级描述层：板级电源管理
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright MIT license <https://mit-license.org>
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/core/pm.h>
#include <xwos/osal/thread.h>
#include <xwmd/isc/xwpcp/usi.h>
#include <xwmd/isc/xwscp/usi.h>
#include <xwmd/pm/notifier.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function declarations    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      .data        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWMDCFG_isc_xwpcp) && (1 == XWMDCFG_isc_xwpcp)
extern xwid_t xwpcp_rxthrd_tid;
extern xwid_t xwpcp_txthrd_tid;
#endif

#if defined(XWMDCFG_isc_xwscp) && (1 == XWMDCFG_isc_xwscp)
extern xwid_t xwscp_thrd_tid;
#endif

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
void xwmd_pm_notify(unsigned long pmevt)
{
        switch (pmevt) {
        case PM_HIBERNATION_PREPARE:
        case PM_SUSPEND_PREPARE:
#if defined(XWMDCFG_isc_xwpcp) && (1 == XWMDCFG_isc_xwpcp)
                if (USI_XWPCP_STATE_START == usi_xwpcp_get_state()) {
                        xwosal_thrd_intr(xwpcp_rxthrd_tid);
                        xwosal_thrd_intr(xwpcp_txthrd_tid);
                }
#endif
#if defined(XWMDCFG_isc_xwscp) && (1 == XWMDCFG_isc_xwscp)
                if (USI_XWSCP_STATE_START == usi_xwscp_get_state()) {
                        xwosal_thrd_intr(xwscp_thrd_tid);
                }
#endif
                break;
        case PM_POST_RESTORE:
        case PM_POST_HIBERNATION:
        case PM_POST_SUSPEND:
                break;
        case PM_RESTORE_PREPARE:
        default:
                break;
        }
}
