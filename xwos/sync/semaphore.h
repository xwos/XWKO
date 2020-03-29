/**
 * @file
 * @brief XuanWuOS的同步机制：信号量
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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

#ifndef __xwos_sync_semaphore_h__
#define __xwos_sync_semaphore_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/object.h>
#include <xwos/lib/bclst.h>
#include <xwos/lock/spinlock.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 信号量
 */
struct xwsync_smr {
        struct xwos_object xwobj; /**<  OO in C: extends struct xwos_object */
        xwssq_t count; /**< counter of semaphire.
                            Negative value indicates a negative semaphire. */
        xwssq_t max; /**< Max value of counter */
        struct xwlib_bclst_head wq; /**< waiting-queue */
        struct xwlk_splk lock; /**< lock to protect this structure */
        struct {
                struct xwsync_evt * evt; /**< event object */
                xwsq_t pos; /**< bit position */
        } selector; /**< semaphore slector */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern
xwer_t xwsync_smr_cache_create(void);

extern
void xwsync_smr_cache_destroy(void);

extern
xwer_t xwsync_smr_xwfs_init(void);

extern
void xwsync_smr_xwfs_exit(void);

extern
xwer_t xwsync_smr_init(struct xwsync_smr * smr, xwssq_t val, xwssq_t max);

extern
xwer_t xwsync_smr_destroy(struct xwsync_smr * smr);

extern
xwer_t xwsync_smr_create(struct xwsync_smr ** ptrbuf, xwssq_t val, xwssq_t max);

extern
xwer_t xwsync_smr_delete(struct xwsync_smr * smr);

extern
xwer_t xwsync_smr_freeze(struct xwsync_smr * smr);

extern
xwer_t xwsync_smr_thaw(struct xwsync_smr * smr, xwssq_t val, xwssq_t max);

extern
xwer_t xwsync_smr_post(struct xwsync_smr * smr);

extern
xwer_t xwsync_smr_wait(struct xwsync_smr * smr);

extern
xwer_t xwsync_smr_trywait(struct xwsync_smr * smr);

extern
xwer_t xwsync_smr_timedwait(struct xwsync_smr * smr, xwtm_t * xwtm);

extern
xwer_t xwsync_smr_wait_unintr(struct xwsync_smr * smr);

extern
xwer_t xwsync_smr_getvalue(struct xwsync_smr * smr, xwssq_t * sval);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xw_inline
xwer_t xwsync_smr_grab(struct xwsync_smr * smr)
{
        return xwos_object_grab(&smr->xwobj);
}

static __xw_inline
xwer_t xwsync_smr_put(struct xwsync_smr * smr)
{
        return xwos_object_put(&smr->xwobj);
}

#endif /* xwos/sync/semaphore.h */
