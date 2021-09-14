/**
 * @file
 * @brief 玄武OS内核同步机制：信号量
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

#ifndef __xwos_mp_sync_sem_h__
#define __xwos_mp_sync_sem_h__

#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/mp/sync/obj.h>
#include <xwos/mp/lock/spinlock.h>

/**
 * @brief 信号量
 */
struct xwmp_sem {
        struct xwmp_synobj synobj; /**< C语言面向对象：继承struct xwmp_synobj */
        xwssq_t count; /**< 信号量计数器：<0，信号量处于负状态 */
        xwssq_t max; /**< 信号量计数器的最大值 */
        struct xwlib_bclst_head wq; /**< 等待队列 */
        struct xwmp_splk lock; /**< 保护此结构体的锁 */
};

xwer_t xwmp_sem_cache_create(void);
void xwmp_sem_cache_destroy(void);

xwer_t xwmp_sem_init(struct xwmp_sem * sem, xwssq_t val, xwssq_t max);
xwer_t xwmp_sem_fini(struct xwmp_sem * sem);
xwer_t xwmp_sem_create(struct xwmp_sem ** ptrbuf, xwssq_t val, xwssq_t max);
xwer_t xwmp_sem_delete(struct xwmp_sem * sem);

xwer_t xwmp_sem_acquire(struct xwmp_sem * sem, xwsq_t tik);
xwer_t xwmp_sem_release(struct xwmp_sem * sem, xwsq_t tik);
xwer_t xwmp_sem_grab(struct xwmp_sem * sem);
xwer_t xwmp_sem_put(struct xwmp_sem * sem);
xwsq_t xwmp_sem_gettik(struct xwmp_sem * sem);

xwer_t xwmp_sem_bind(struct xwmp_sem * sem, struct xwmp_evt * evt, xwsq_t pos);
xwer_t xwmp_sem_unbind(struct xwmp_sem * sem, struct xwmp_evt * evt);
xwer_t xwmp_sem_freeze(struct xwmp_sem * sem);
xwer_t xwmp_sem_thaw(struct xwmp_sem * sem);
xwer_t xwmp_sem_post(struct xwmp_sem * sem);
xwer_t xwmp_sem_wait(struct xwmp_sem * sem);
xwer_t xwmp_sem_trywait(struct xwmp_sem * sem);
xwer_t xwmp_sem_timedwait(struct xwmp_sem * sem, xwtm_t * xwtm);
xwer_t xwmp_sem_wait_unintr(struct xwmp_sem * sem);
xwer_t xwmp_sem_getvalue(struct xwmp_sem * sem, xwssq_t * sval);

#endif /* xwos/mp/sync/sem.h */
