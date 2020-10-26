/**
 * @file
 * @brief 玄武OS内核：内核电源管理
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

#include <xwos/standard.h>
#include <linux/notifier.h>
#include <linux/suspend.h>
#include <xwos/lib/xwlog.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwaop.h>
#include <xwmd/pm/notifier.h>
#include <bdl/xwac/pm.h>

#define LOGTAG "pm"

#define XWMP_PM_STATE_ACTIVE            0
#define XWMP_PM_STATE_LPM               1

static
int xwmp_pm_notifier_func(struct notifier_block *notifier,
                          unsigned long pmevt, void *unused);

struct notifier_block xwmp_pm_notifier = {
        .notifier_call = xwmp_pm_notifier_func,
        .next = NULL,
        .priority = 0,
};

xwssq_a xwmp_pm_state = XWMP_PM_STATE_ACTIVE;

xwer_t xwmp_pm_init(void)
{
        xwer_t rc;

        rc = register_pm_notifier(&xwmp_pm_notifier);
        return rc;
}

void xwmp_pm_exit(void)
{
        unregister_pm_notifier(&xwmp_pm_notifier);
}

static
int xwmp_pm_notifier_func(struct notifier_block * notifier,
                         unsigned long pmevt,
                         void * unused)
{
        switch (pmevt) {
        case PM_HIBERNATION_PREPARE:
        case PM_SUSPEND_PREPARE:
                xwlogf(INFO, LOGTAG, "suspending ...\n");
                xwaop_write(xwssq, &xwmp_pm_state, XWMP_PM_STATE_LPM, NULL);
                break;
        case PM_POST_RESTORE:
        case PM_POST_HIBERNATION:
        case PM_POST_SUSPEND:
                xwlogf(INFO, LOGTAG, "resuming ...\n");
                xwaop_write(xwssq, &xwmp_pm_state, XWMP_PM_STATE_ACTIVE, NULL);
                break;
        case PM_RESTORE_PREPARE:
        default:
                break;
        }
        xwmd_pm_notify(pmevt);
        brd_pm_notify(pmevt);
        return NOTIFY_DONE;
}

bool xwmp_pm_tst_lpm(void)
{
        xwssq_t st;

        st = xwaop_load(xwssq, &xwmp_pm_state, xwmb_modr_consume);
        return (XWMP_PM_STATE_LPM == st);
}
