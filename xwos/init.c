/**
 * @file
 * @brief 玄武OS的初始化
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
#include <xwos/core/scheduler.h>
#include <xwos/core/swt.h>
#include <xwos/core/pm.h>
#include <xwos/lock/mutex.h>
#include <xwos/sync/semaphore.h>
#include <xwos/sync/condition.h>
#include <xwos/mm/kma.h>
#include <xwos/mm/bma.h>
#include <xwos/init.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       function implementations      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t xwos_init(void)
{
        xwer_t rc;

        /******** KAL ********/
        rc = xwos_scheduler_init();
        if (__unlikely(rc < 0)) {
                goto err_xwos_scheduler_init;
        }

        rc = xwos_pm_init();
        if (__unlikely(rc < 0)) {
                goto err_xwos_pm_init;
        }

        /******** object cache ********/
        rc = xwos_swt_cache_create();
        if (__unlikely(rc < 0)) {
                xwoslogf(ERR, "Create xwos/core/swt cache ... [FAILED], rc:%d\n", rc);
                goto err_swt_cache_create;
        }
        xwoslogf(INFO, "Create xwos/core/swt cache ... [OK]\n");

        rc = xwsync_smr_cache_create();
        if (__unlikely(rc < 0)) {
                xwoslogf(ERR,
                         "Create xwos/sync/smr cache ... [FAILED], rc:%d\n",
                         rc);
                goto err_smr_cache_create;
        }
        xwoslogf(INFO, "Create xwos/sync/smr cache ... [OK]\n");

        rc = xwsync_cdt_cache_create();
        if (__unlikely(rc < 0)) {
                xwoslogf(ERR,
                         "Create xwos/sync/cdt cache ... [FAILED], rc:%d\n",
                         rc);
                goto err_cdt_cache_create;
        }
        xwoslogf(INFO, "create xwos/sync/cdt cache ... [OK]\n");

        rc = xwlk_mtx_cache_create();
        if (__unlikely(rc < 0)) {
                xwoslogf(ERR,
                         "Create xwos/lock/mutex cache ... [FAILED], rc:%d\n",
                         rc);
                goto err_mtx_cache_create;
        }
        xwoslogf(INFO, "Create xwos/lock/mutex cache ... [OK]\n");

        /******** MM ********/
        rc = xwmm_dkma_init(XWMMCFG_DKMA_SIZE);
        if (__unlikely(rc < 0)) {
                xwoslogf(ERR,
                         "Init xwos/mm/dkma ... [FAILED], rc:%d\n",
                         rc);
                goto err_xwmm_dkma_init;
        }
        xwoslogf(INFO, "Init xwos/mm/dkma ... [OK]\n");

        return XWOK;

err_xwmm_dkma_init:
        xwlk_mtx_cache_destroy();
err_mtx_cache_create:
        xwsync_cdt_cache_destroy();
err_cdt_cache_create:
        xwsync_smr_cache_destroy();
err_smr_cache_create:
        xwos_swt_cache_destroy();
err_swt_cache_create:
        xwos_pm_exit();
err_xwos_pm_init:
        xwos_scheduler_exit();
err_xwos_scheduler_init:
        return rc;
}

void xwos_exit(void)
{
        xwmm_dkma_exit();
        xwlk_mtx_cache_destroy();
        xwsync_cdt_cache_destroy();
        xwsync_smr_cache_destroy();
        xwos_swt_cache_destroy();
        xwos_pm_exit();
        xwos_scheduler_exit();
}
