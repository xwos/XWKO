/**
 * @file
 * @brief 玄武OS内核操作系统接口描述层：信号量
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

#ifndef __xwos_mp_osdl_sync_sem_h__
#define __xwos_mp_osdl_sync_sem_h__

#include <xwos/mp/osdl/sync/sel.h>
#include <xwos/mp/sync/sem.h>

#define xwosdl_sem xwmp_sem

static __xwcc_inline
xwer_t xwosdl_sem_init(struct xwosdl_sem * sem, xwssq_t val, xwssq_t max)
{
        return xwmp_sem_init(sem, val, max);
}

static __xwcc_inline
xwer_t xwosdl_sem_fini(struct xwosdl_sem * sem)
{
        return xwmp_sem_fini(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_create(struct xwosdl_sem ** semp, xwssq_t val, xwssq_t max)
{
        return xwmp_sem_create(semp, val, max);
}

static __xwcc_inline
xwer_t xwosdl_sem_delete(struct xwosdl_sem * sem)
{
        return xwmp_sem_delete(sem);
}

static __xwcc_inline
xwsq_t xwosdl_sem_gettik(struct xwosdl_sem * sem)
{
        return xwmp_sem_gettik(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_acquire(struct xwosdl_sem * sem, xwsq_t tik)
{
        return xwmp_sem_acquire(sem, tik);
}

static __xwcc_inline
xwer_t xwosdl_sem_release(struct xwosdl_sem * sem, xwsq_t tik)
{
        return xwmp_sem_release(sem, tik);
}

static __xwcc_inline
xwer_t xwosdl_sem_grab(struct xwosdl_sem * sem)
{
        return xwmp_sem_grab(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_put(struct xwosdl_sem * sem)
{
        return xwmp_sem_put(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_bind(struct xwosdl_sem * sem, struct xwosdl_sel * sel, xwsq_t pos)
{
        return xwmp_sem_bind(sem, sel, pos);
}

static __xwcc_inline
xwer_t xwosdl_sem_unbind(struct xwosdl_sem * sem, struct xwosdl_sel * sel)
{
        return xwmp_sem_unbind(sem, sel);
}

static __xwcc_inline
xwer_t xwosdl_sem_freeze(struct xwosdl_sem * sem)
{
        return xwmp_sem_freeze(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_thaw(struct xwosdl_sem * sem)
{
        return xwmp_sem_thaw(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_post(struct xwosdl_sem * sem)
{
        return xwmp_sem_post(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_wait(struct xwosdl_sem * sem)
{
        return xwmp_sem_wait(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_trywait(struct xwosdl_sem * sem)
{
        return xwmp_sem_trywait(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_timedwait(struct xwosdl_sem * sem, xwtm_t * xwtm)
{
        return xwmp_sem_timedwait(sem, xwtm);
}

static __xwcc_inline
xwer_t xwosdl_sem_wait_unintr(struct xwosdl_sem * sem)
{
        return xwmp_sem_wait_unintr(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_getvalue(struct xwosdl_sem * sem, xwssq_t * sval)
{
        return xwmp_sem_getvalue(sem, sval);
}

#endif /* xwos/mp/osdl/sync/sem.h */
