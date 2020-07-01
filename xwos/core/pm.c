/**
 * @file
 * @brief XuanWuOS内核：内核电源管理
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0 (the "MPL"). If a copy of the MPL was not distributed
 * > with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * >
 * > Alternatively, the contents of this file may be used under the
 * > terms of the GNU General Public License version 2 (the "GPL"), in
 * > which case the provisions of the GPL are applicable instead of the
 * > above. If you wish to allow the use of your version of this file
 * > only under the terms of the GPL and not to allow others to use your
 * > version of this file under the MPL, indicate your decision by
 * > deleting the provisions above and replace them with the notice and
 * > other provisions required by the GPL. If you do not delete the
 * > provisions above, a recipient may use your version of this file
 * > under either the MPL or the GPL.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwlog.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwaop.h>
#include <linux/notifier.h>
#include <linux/suspend.h>
#include <xwmd/pm/notifier.h>
#include <bdl/xwac/pm.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWOS_PM_STATE_ACTIVE             0
#define XWOS_PM_STATE_LPM                1

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
int xwos_pm_notifier_func(struct notifier_block *notifier,
                          unsigned long pmevt, void *unused);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      .data        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct notifier_block xwos_pm_notifier = {
        .notifier_call = xwos_pm_notifier_func,
        .next = NULL,
        .priority = 0,
};

__atomic xwssq_t xwos_pm_state = XWOS_PM_STATE_ACTIVE;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t xwos_pm_init(void)
{
        xwer_t rc;

        rc = register_pm_notifier(&xwos_pm_notifier);
        return rc;
}

void xwos_pm_exit(void)
{
        unregister_pm_notifier(&xwos_pm_notifier);
}

static
int xwos_pm_notifier_func(struct notifier_block * notifier,
                          unsigned long pmevt,
                          void * unused)
{
        switch (pmevt) {
        case PM_HIBERNATION_PREPARE:
        case PM_SUSPEND_PREPARE:
                xwoslogf(INFO, "suspending ...\n");
                xwaop_write(xwssq_t, &xwos_pm_state, XWOS_PM_STATE_LPM, NULL);
                break;
        case PM_POST_RESTORE:
        case PM_POST_HIBERNATION:
        case PM_POST_SUSPEND:
                xwoslogf(INFO, "resuming ...\n");
                xwaop_write(xwssq_t, &xwos_pm_state, XWOS_PM_STATE_ACTIVE, NULL);
                break;
        case PM_RESTORE_PREPARE:
        default:
                break;
        }
        xwmd_pm_notify(pmevt);
        brd_pm_notify(pmevt);
        return NOTIFY_DONE;
}

bool xwos_pm_tst_lpm(void)
{
        xwssq_t st;

        st = xwaop_load(xwssq_t, &xwos_pm_state, xwmb_modr_consume);
        return (XWOS_PM_STATE_LPM == st);
}
