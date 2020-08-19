/**
 * @file
 * @brief XuanWuOS的同步机制：条件量
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

#ifndef __xwos_sync_condition_h__
#define __xwos_sync_condition_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/sync/object.h>
#include <xwos/lib/bclst.h>
#include <xwos/lock/spinlock.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 条件量
 */
struct xwsync_cdt {
        struct xwsync_object xwsyncobj; /**< C语言面向对象：继承struct xwsync_object */
        xwssq_t count; /**< 计数器：<0，条件量处于负状态；*/
        struct xwlib_bclst_head wq; /**< 等待队列 */
        struct xwlk_splk lock; /**< 保护此锁的结构体 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ********       internal inline function implementations        ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwcc_inline
xwer_t xwsync_cdt_grab(struct xwsync_cdt * cdt)
{
        return xwsync_object_grab(&cdt->xwsyncobj);
}

static __xwcc_inline
xwer_t xwsync_cdt_put(struct xwsync_cdt * cdt)
{
        return xwsync_object_put(&cdt->xwsyncobj);
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     internal function prototypes    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t xwsync_cdt_cache_create(void);

void xwsync_cdt_cache_destroy(void);

xwer_t xwsync_cdt_xwfs_init(void);

void xwsync_cdt_xwfs_exit(void);

void xwsync_cdt_construct(void * anon);

xwer_t xwsync_cdt_activate(struct xwsync_cdt * cdt, xwobj_gc_f gcfunc);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       API function prototypes       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_api
xwer_t xwsync_cdt_init(struct xwsync_cdt * cdt);

__xwos_api
xwer_t xwsync_cdt_destroy(struct xwsync_cdt * cdt);

__xwos_api
xwer_t xwsync_cdt_create(struct xwsync_cdt ** ptrbuf);

__xwos_api
xwer_t xwsync_cdt_delete(struct xwsync_cdt * cdt);

__xwos_api
xwer_t xwsync_cdt_bind(struct xwsync_cdt * cdt, struct xwsync_evt * evt, xwsq_t pos);

__xwos_api
xwer_t xwsync_cdt_unbind(struct xwsync_cdt * cdt, struct xwsync_evt * evt);

__xwos_api
xwer_t xwsync_cdt_freeze(struct xwsync_cdt * cdt);

__xwos_api
xwer_t xwsync_cdt_thaw(struct xwsync_cdt * cdt);

__xwos_api
xwer_t xwsync_cdt_intr_all(struct xwsync_cdt * cdt);

__xwos_api
xwer_t xwsync_cdt_broadcast(struct xwsync_cdt * cdt);

__xwos_api
xwer_t xwsync_cdt_unicast(struct xwsync_cdt * cdt);

__xwos_api
xwer_t xwsync_cdt_wait(struct xwsync_cdt * cdt,
                       void * lock, xwid_t locktype, void * lockdata,
                       xwsq_t * lkst);

__xwos_api
xwer_t xwsync_cdt_timedwait(struct xwsync_cdt * cdt,
                            void * lock, xwid_t locktype, void * lockdata,
                            xwtm_t * xwtm, xwsq_t * lkst);

#endif /* xwos/sync/condition.h */
