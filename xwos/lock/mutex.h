/**
 * @brief XuanWuOS的锁机制：互斥锁
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

#ifndef __xwos_lock_mutex_h__
#define __xwos_lock_mutex_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/object.h>
#include <linux/rtmutex.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwlk_mtx {
        struct xwos_object xwobj; /**< OO in C: extends struct xwos_object */
        struct rt_mutex lrtmtx; /**< Linux Realtime Mutex */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern
xwer_t xwlk_mtx_cache_create(void);

extern
void xwlk_mtx_cache_destroy(void);

extern
xwer_t xwlk_mtx_xwfs_init(void);

extern
void xwlk_mtx_xwfs_exit(void);

extern
xwer_t xwlk_mtx_init(struct xwlk_mtx * mtx, xwpr_t sprio);

extern
xwer_t xwlk_mtx_destroy(struct xwlk_mtx * mtx);

extern
xwer_t xwlk_mtx_create(struct xwlk_mtx ** ptrbuf, xwpr_t sprio);

extern
xwer_t xwlk_mtx_delete(struct xwlk_mtx * mtx);

extern
xwer_t xwlk_mtx_unlock(struct xwlk_mtx * mtx);

extern
xwer_t xwlk_mtx_lock(struct xwlk_mtx * mtx);

extern
xwer_t xwlk_mtx_trylock(struct xwlk_mtx * mtx);

extern
xwer_t xwlk_mtx_timedlock(struct xwlk_mtx * mtx, xwtm_t * xwtm);

extern
xwer_t xwlk_mtx_lock_unintr(struct xwlk_mtx * mtx);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xw_inline
xwer_t xwlk_mtx_grab(struct xwlk_mtx * mtx)
{
        return xwos_object_grab(&mtx->xwobj);
}

static __xw_inline
xwer_t xwlk_mtx_put(struct xwlk_mtx * mtx)
{
        return xwos_object_put(&mtx->xwobj);
}

#endif /* xwos/lock/mutex.h */
