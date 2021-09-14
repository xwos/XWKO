/**
 * @file
 * @brief 板级描述层：板级电源管理
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright MIT license <https://mit-license.org>
 */

#include <xwos/standard.h>
#include <xwos/mp/pm.h>
#include <xwmd/pm/notifier.h>
#include <xwmd/isc/xwpcp/usi.h>
#include <xwmd/isc/xwscp/usi.h>

void xwmd_pm_notify(unsigned long pmevt)
{
        switch (pmevt) {
        case PM_HIBERNATION_PREPARE:
        case PM_SUSPEND_PREPARE:
#if defined(XWMDCFG_isc_xwpcp) && (1 == XWMDCFG_isc_xwpcp)
                usi_xwpcp_pm_notify(pmevt);
#endif
#if defined(XWMDCFG_isc_xwscp) && (1 == XWMDCFG_isc_xwscp)
                usi_xwscp_pm_notify(pmevt);
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
