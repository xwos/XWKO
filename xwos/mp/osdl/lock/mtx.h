/**
 * @file
 * @brief 玄武OS内核操作系统接口描述层：互斥锁
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

#ifndef __xwos_mp_osdl_lock_mtx_h__
#define __xwos_mp_osdl_lock_mtx_h__

#include <xwos/mp/lock/mtx.h>

#define xwosdl_mtx xwmp_mtx

static __xwcc_inline
xwer_t xwosdl_mtx_init(struct xwosdl_mtx * mtx, xwpr_t sprio)
{
        return xwmp_mtx_init(mtx, sprio);
}

static __xwcc_inline
xwer_t xwosdl_mtx_fini(struct xwosdl_mtx * mtx)
{
        return xwmp_mtx_fini(mtx);
}

static __xwcc_inline
xwer_t xwosdl_mtx_create(struct xwosdl_mtx ** mtxbuf, xwpr_t sprio)
{
        return xwmp_mtx_create(mtxbuf, sprio);
}

static __xwcc_inline
xwer_t xwosdl_mtx_delete(struct xwosdl_mtx * mtx)
{
        return xwmp_mtx_delete(mtx);
}

static __xwcc_inline
xwsq_t xwosdl_mtx_gettik(struct xwosdl_mtx * mtx)
{
        return xwmp_mtx_gettik(mtx);
}

static __xwcc_inline
xwer_t xwosdl_mtx_acquire(struct xwosdl_mtx * mtx, xwsq_t tik)
{
        return xwmp_mtx_acquire(mtx, tik);
}

static __xwcc_inline
xwer_t xwosdl_mtx_release(struct xwosdl_mtx * mtx, xwsq_t tik)
{
        return xwmp_mtx_release(mtx, tik);
}

static __xwcc_inline
xwer_t xwosdl_mtx_grab(struct xwosdl_mtx * mtx)
{
        return xwmp_mtx_grab(mtx);
}

static __xwcc_inline
xwer_t xwosdl_mtx_put(struct xwosdl_mtx * mtx)
{
        return xwmp_mtx_put(mtx);
}

static __xwcc_inline
xwer_t xwosdl_mtx_unlock(struct xwosdl_mtx * mtx)
{
        return xwmp_mtx_unlock(mtx);
}

static __xwcc_inline
xwer_t xwosdl_mtx_lock(struct xwosdl_mtx * mtx)
{
        return xwmp_mtx_lock(mtx);
}

static __xwcc_inline
xwer_t xwosdl_mtx_trylock(struct xwosdl_mtx * mtx)
{
        return xwmp_mtx_trylock(mtx);
}

static __xwcc_inline
xwer_t xwosdl_mtx_timedlock(struct xwosdl_mtx * mtx, xwtm_t * xwtm)
{
        return xwmp_mtx_timedlock(mtx, xwtm);
}

static __xwcc_inline
xwer_t xwosdl_mtx_lock_unintr(struct xwosdl_mtx * mtx)
{
        return xwmp_mtx_lock_unintr(mtx);
}

static __xwcc_inline
xwer_t xwosdl_mtx_getlkst(struct xwosdl_mtx * mtx, xwsq_t * lkst)
{
        return xwmp_mtx_getlkst(mtx, lkst);
}

#endif /* xwos/mp/osdl/lock/mtx.h */
