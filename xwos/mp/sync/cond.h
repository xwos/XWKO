/**
 * @file
 * @brief 玄武OS的同步机制：条件量
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

#ifndef __xwos_sync_cond_h__
#define __xwos_sync_cond_h__

#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/mp/sync/obj.h>
#include <xwos/mp/lock/spinlock.h>

/**
 * @brief 条件量
 */
struct xwmp_cond {
        struct xwmp_synobj synobj; /**< C语言面向对象：继承struct xwmp_synobj */
        xwssq_t count; /**< 计数器：<0，条件量处于负状态；*/
        struct xwlib_bclst_head wq; /**< 等待队列 */
        struct xwmp_splk lock; /**< 保护此锁的结构体 */
};

xwer_t xwmp_cond_cache_create(void);
void xwmp_cond_cache_destroy(void);
void xwmp_cond_construct(void * anon);
xwer_t xwmp_cond_activate(struct xwmp_cond * cond, xwobj_gc_f gcfunc);

xwer_t xwmp_cond_init(struct xwmp_cond * cond);
xwer_t xwmp_cond_fini(struct xwmp_cond * cond);
xwer_t xwmp_cond_create(struct xwmp_cond ** ptrbuf);
xwer_t xwmp_cond_delete(struct xwmp_cond * cond);

xwer_t xwmp_cond_acquire(struct xwmp_cond * cond, xwsq_t tik);
xwer_t xwmp_cond_release(struct xwmp_cond * cond, xwsq_t tik);
xwer_t xwmp_cond_grab(struct xwmp_cond * cond);
xwer_t xwmp_cond_put(struct xwmp_cond * cond);
xwsq_t xwmp_cond_gettik(struct xwmp_cond * cond);

xwer_t xwmp_cond_bind(struct xwmp_cond * cond, struct xwmp_evt * evt, xwsq_t pos);
xwer_t xwmp_cond_unbind(struct xwmp_cond * cond, struct xwmp_evt * evt);
xwer_t xwmp_cond_freeze(struct xwmp_cond * cond);
xwer_t xwmp_cond_thaw(struct xwmp_cond * cond);
xwer_t xwmp_cond_intr_all(struct xwmp_cond * cond);
xwer_t xwmp_cond_broadcast(struct xwmp_cond * cond);
xwer_t xwmp_cond_unicast(struct xwmp_cond * cond);
xwer_t xwmp_cond_wait(struct xwmp_cond * cond,
                      void * lock, xwid_t locktype, void * lockdata,
                      xwsq_t * lkst);
xwer_t xwmp_cond_timedwait(struct xwmp_cond * cond,
                           void * lock, xwid_t locktype, void * lockdata,
                           xwtm_t * xwtm, xwsq_t * lkst);

#endif /* xwos/sync/cond.h */
