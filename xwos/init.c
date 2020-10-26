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

#include <xwos/standard.h>
#include <xwos/lib/xwlog.h>
#include <xwos/mp/ksym.h>
#include <xwos/mp/skd.h>
#include <xwos/mp/swt.h>
#include <xwos/mp/pm.h>
#include <xwos/mp/lock/mtx.h>
#include <xwos/mp/sync/sem.h>
#include <xwos/mp/sync/cond.h>
#include <xwos/mm/kma.h>
#include <xwos/mm/bma.h>
#include <xwos/init.h>

#define LOGTAG  "init"

xwer_t xwos_init(void)
{
        xwer_t rc;

        /******** ksym ********/
        rc = xwmp_ksym_init();
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xwmp_ksym_init;
        }

        /******** KAL ********/
        rc = xwmp_skd_init();
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xwmp_skd_init;
        }

        rc = xwmp_pm_init();
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xwmp_pm_init;
        }

        /******** object cache ********/
        rc = xwmp_swt_cache_create();
        if (__xwcc_unlikely(rc < 0)) {
                xwlogf(ERR, LOGTAG,
                       "Create xwos/mp/swt cache ... [FAILED], rc:%d\n", rc);
                goto err_swt_cache_create;
        }
        xwlogf(INFO, LOGTAG,
               "Create xwos/mp/swt cache ... [OK]\n");

        rc = xwmp_sem_cache_create();
        if (__xwcc_unlikely(rc < 0)) {
                xwlogf(ERR, LOGTAG,
                       "Create xwos/mp/sync/sem cache ... [FAILED], rc:%d\n", rc);
                goto err_sem_cache_create;
        }
        xwlogf(INFO, LOGTAG,
               "Create xwos/mp/sync/sem cache ... [OK]\n");

        rc = xwmp_cond_cache_create();
        if (__xwcc_unlikely(rc < 0)) {
                xwlogf(ERR, LOGTAG,
                       "Create xwos/mp/sync/cond cache ... [FAILED], rc:%d\n", rc);
                goto err_cond_cache_create;
        }
        xwlogf(INFO, LOGTAG,
               "create xwos/mp/sync/cond cache ... [OK]\n");

        rc = xwmp_mtx_cache_create();
        if (__xwcc_unlikely(rc < 0)) {
                xwlogf(ERR, LOGTAG,
                       "Create xwos/mp/lock/mtx cache ... [FAILED], rc:%d\n", rc);
                goto err_mtx_cache_create;
        }
        xwlogf(INFO, LOGTAG, "Create xwos/mp/lock/mtx cache ... [OK]\n");

        /******** MM ********/
        rc = xwmm_kma_init(XWMMCFG_KMA_SIZE);
        if (__xwcc_unlikely(rc < 0)) {
                xwlogf(ERR, LOGTAG,
                       "Init xwos/mm/kma ... [FAILED], rc:%d\n", rc);
                goto err_xwmm_kma_init;
        }
        xwlogf(INFO, LOGTAG, "Init xwos/mm/kma ... [OK]\n");

        return XWOK;

err_xwmm_kma_init:
        xwmp_mtx_cache_destroy();
err_mtx_cache_create:
        xwmp_cond_cache_destroy();
err_cond_cache_create:
        xwmp_sem_cache_destroy();
err_sem_cache_create:
        xwmp_swt_cache_destroy();
err_swt_cache_create:
        xwmp_pm_exit();
err_xwmp_pm_init:
        xwmp_skd_exit();
err_xwmp_skd_init:
err_xwmp_ksym_init:
        return rc;
}

void xwos_exit(void)
{
        xwmm_kma_exit();
        xwmp_mtx_cache_destroy();
        xwmp_cond_cache_destroy();
        xwmp_sem_cache_destroy();
        xwmp_swt_cache_destroy();
        xwmp_pm_exit();
        xwmp_skd_exit();
}
