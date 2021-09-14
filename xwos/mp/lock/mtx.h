/**
 * @brief 玄武OS的锁机制：互斥锁
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
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

#ifndef __xwos_mp_lock_mutex_h__
#define __xwos_mp_lock_mutex_h__

#include <xwos/standard.h>
#include <linux/rtmutex.h>
#include <xwos/lib/object.h>

struct xwmp_mtx {
        struct xwos_object xwobj; /**< OO in C: extends struct xwos_object */
        struct rt_mutex lrtmtx; /**< Linux Realtime Mutex */
};

xwer_t xwmp_mtx_cache_create(void);
void xwmp_mtx_cache_destroy(void);

xwer_t xwmp_mtx_init(struct xwmp_mtx * mtx, xwpr_t sprio);
xwer_t xwmp_mtx_fini(struct xwmp_mtx * mtx);
xwer_t xwmp_mtx_create(struct xwmp_mtx ** ptrbuf, xwpr_t sprio);
xwer_t xwmp_mtx_delete(struct xwmp_mtx * mtx);
xwer_t xwmp_mtx_acquire(struct xwmp_mtx * mtx, xwsq_t tik);
xwer_t xwmp_mtx_release(struct xwmp_mtx * mtx, xwsq_t tik);
xwer_t xwmp_mtx_grab(struct xwmp_mtx * mtx);
xwer_t xwmp_mtx_put(struct xwmp_mtx * mtx);
xwsq_t xwmp_mtx_gettik(struct xwmp_mtx * mtx);
xwer_t xwmp_mtx_unlock(struct xwmp_mtx * mtx);
xwer_t xwmp_mtx_lock(struct xwmp_mtx * mtx);
xwer_t xwmp_mtx_trylock(struct xwmp_mtx * mtx);
xwer_t xwmp_mtx_timedlock(struct xwmp_mtx * mtx, xwtm_t * xwtm);
xwer_t xwmp_mtx_lock_unintr(struct xwmp_mtx * mtx);
xwer_t xwmp_mtx_getlkst(struct xwmp_mtx * mtx, xwsq_t * lkst);

#endif /* xwos/mp/lock/mutex.h */
