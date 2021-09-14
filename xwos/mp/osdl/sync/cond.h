/**
 * @file
 * @brief 玄武OS内核操作系统接口描述层：条件量
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

#ifndef __xwos_mp_osdl_sync_cond_h__
#define __xwos_mp_osdl_sync_cond_h__

#include <xwos/mp/osdl/sync/sel.h>
#include <xwos/mp/sync/cond.h>

#define xwosdl_cond xwmp_cond

static __xwcc_inline
xwer_t xwosdl_cond_init(struct xwosdl_cond * cond)
{
        return xwmp_cond_init(cond);
}

static __xwcc_inline
xwer_t xwosdl_cond_fini(struct xwosdl_cond * cond)
{
        return xwmp_cond_fini(cond);
}

static __xwcc_inline
xwer_t xwosdl_cond_create(struct xwosdl_cond ** condbuf)
{
        return xwmp_cond_create(condbuf);
}

static __xwcc_inline
xwer_t xwosdl_cond_delete(struct xwosdl_cond * cond)
{
        return xwmp_cond_delete(cond);
}

static __xwcc_inline
xwsq_t xwosdl_cond_gettik(struct xwosdl_cond * cond)
{
        return xwmp_cond_gettik(cond);
}

static __xwcc_inline
xwer_t xwosdl_cond_acquire(struct xwosdl_cond * cond, xwsq_t tik)
{
        return xwmp_cond_acquire(cond, tik);
}

static __xwcc_inline
xwer_t xwosdl_cond_release(struct xwosdl_cond * cond, xwsq_t tik)
{
        return xwmp_cond_release(cond, tik);
}

static __xwcc_inline
xwer_t xwosdl_cond_grab(struct xwosdl_cond * cond)
{
        return xwmp_cond_grab(cond);
}

static __xwcc_inline
xwer_t xwosdl_cond_put(struct xwosdl_cond * cond)
{
        return xwmp_cond_put(cond);
}

static __xwcc_inline
xwer_t xwosdl_cond_bind(struct xwosdl_cond * cond, struct xwosdl_sel * sel,
                        xwsq_t pos)
{
        return xwmp_cond_bind(cond, sel, pos);
}

static __xwcc_inline
xwer_t xwosdl_cond_unbind(struct xwosdl_cond * cond, struct xwosdl_sel * sel)
{
        return xwmp_cond_unbind(cond, sel);
}

static __xwcc_inline
xwer_t xwosdl_cond_thaw(struct xwosdl_cond * cond)
{
        return xwmp_cond_thaw(cond);
}

static __xwcc_inline
xwer_t xwosdl_cond_freeze(struct xwosdl_cond * cond)
{
        return xwmp_cond_freeze(cond);
}

static __xwcc_inline
xwer_t xwosdl_cond_intr_all(struct xwosdl_cond * cond)
{
        return xwmp_cond_intr_all(cond);
}

static __xwcc_inline
xwer_t xwosdl_cond_broadcast(struct xwosdl_cond * cond)
{
        return xwmp_cond_broadcast(cond);
}

static __xwcc_inline
xwer_t xwosdl_cond_unicast(struct xwosdl_cond * cond)
{
        return xwmp_cond_unicast(cond);
}

static __xwcc_inline
xwer_t xwosdl_cond_wait(struct xwosdl_cond * cond,
                        union xwos_ulock lock, xwsq_t lktype, void * lkdata,
                        xwsq_t * lkst)
{
        return xwmp_cond_wait(cond,
                              lock.anon, lktype, lkdata,
                              lkst);
}

static __xwcc_inline
xwer_t xwosdl_cond_timedwait(struct xwosdl_cond * cond,
                             union xwos_ulock lock, xwsq_t lktype, void * lkdata,
                             xwtm_t * xwtm, xwsq_t * lkst)
{
        return xwmp_cond_timedwait(cond,
                                   lock.anon, lktype, lkdata,
                                   xwtm, lkst);
}

#endif /* xwos/mp/osdl/sync/cond.h */
