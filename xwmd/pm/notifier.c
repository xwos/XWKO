/**
 * @file
 * @brief 板级描述层：板级电源管理
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
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
                        xwosal_thrd_intr(usi_xwpcp_txthrd);
                        xwosal_thrd_intr(usi_xwpcp_rxthrd);
                }
#endif
#if defined(XWMDCFG_isc_xwscp) && (1 == XWMDCFG_isc_xwscp)
                if (USI_XWSCP_STATE_START == usi_xwscp_get_state()) {
                        xwosal_thrd_intr(usi_xwscp_thrd);
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
