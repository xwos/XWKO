/**
 * @file
 * @brief 玄武OS内核操作系统接口描述层：自旋锁
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

#ifndef __xwos_mp_osdl_lock_spinlock_h__
#define __xwos_mp_osdl_lock_spinlock_h__

#include <xwos/mp/lock/spinlock.h>

typedef struct xwmp_splk xwosdl_splk_t;

static __xwcc_inline
void xwosdl_splk_init(xwosdl_splk_t * ossl)
{
        xwmp_splk_init(ossl);
}

static __xwcc_inline
void xwosdl_splk_lock(xwosdl_splk_t * ossl)
{
        xwmp_splk_lock(ossl);
}

static __xwcc_inline
xwer_t xwosdl_splk_trylock(xwosdl_splk_t * ossl)
{
        return xwmp_splk_trylock(ossl);
}

static __xwcc_inline
void xwosdl_splk_unlock(xwosdl_splk_t * ossl)
{
        xwmp_splk_unlock(ossl);
}

static __xwcc_inline
void xwosdl_splk_lock_cpuirq(xwosdl_splk_t * ossl)
{
        xwmp_splk_lock_cpuirq(ossl);
}

static __xwcc_inline
xwer_t xwosdl_splk_trylock_cpuirq(xwosdl_splk_t * ossl)
{
        return xwmp_splk_trylock_cpuirq(ossl);
}

static __xwcc_inline
void xwosdl_splk_unlock_cpuirq(xwosdl_splk_t * ossl)
{
        xwmp_splk_unlock_cpuirq(ossl);
}

static __xwcc_inline
void xwosdl_splk_lock_cpuirqsv(xwosdl_splk_t * ossl, xwreg_t * cpuirq)
{
        xwmp_splk_lock_cpuirqsv(ossl, cpuirq);
}

static __xwcc_inline
xwer_t xwosdl_splk_trylock_cpuirqsv(xwosdl_splk_t * ossl, xwreg_t * cpuirq)
{
        return xwmp_splk_trylock_cpuirqsv(ossl, cpuirq);
}

static __xwcc_inline
void xwosdl_splk_unlock_cpuirqrs(xwosdl_splk_t * ossl, xwreg_t cpuirq)
{
        xwmp_splk_unlock_cpuirqrs(ossl, cpuirq);
}

static __xwcc_inline
void xwosdl_splk_lock_irqs(xwosdl_splk_t * ossl,
                           const struct xwos_irq_resource * irqs, xwsz_t num)
{
        xwmp_splk_lock_irqs(ossl, irqs, num);
}

static __xwcc_inline
xwer_t xwosdl_splk_trylock_irqs(xwosdl_splk_t * ossl,
                                const struct xwos_irq_resource * irqs, xwsz_t num)
{
        return xwmp_splk_trylock_irqs(ossl, irqs, num);
}

static __xwcc_inline
void xwosdl_splk_unlock_irqs(xwosdl_splk_t * ossl,
                             const struct xwos_irq_resource * irqs, xwsz_t num)
{
        xwmp_splk_unlock_irqs(ossl, irqs, num);
}

static __xwcc_inline
void xwosdl_splk_lock_irqssv(xwosdl_splk_t * ossl,
                             const struct xwos_irq_resource * irqs,
                             xwreg_t flags[], xwsz_t num)
{
        xwmp_splk_lock_irqssv(ossl, irqs, flags, num);
}

static __xwcc_inline
xwer_t xwosdl_splk_trylock_irqssv(xwosdl_splk_t * ossl,
                                  const struct xwos_irq_resource * irqs,
                                  xwreg_t flags[], xwsz_t num)
{
        return xwmp_splk_trylock_irqssv(ossl, irqs, flags, num);
}

static __xwcc_inline
void xwosdl_splk_unlock_irqsrs(xwosdl_splk_t * ossl,
                               const struct xwos_irq_resource * irqs,
                               xwreg_t flags[], xwsz_t num)
{
        xwmp_splk_unlock_irqsrs(ossl, irqs, flags, num);
}

static __xwcc_inline
void xwosdl_splk_lock_bh(xwosdl_splk_t * ossl)
{
        xwmp_splk_lock_bh(ossl);
}

static __xwcc_inline
xwer_t xwosdl_splk_trylock_bh(xwosdl_splk_t * ossl)
{
        return xwmp_splk_trylock_bh(ossl);
}

static __xwcc_inline
void xwosdl_splk_unlock_bh(xwosdl_splk_t * ossl)
{
        xwmp_splk_unlock_bh(ossl);
}

#endif /* xwos/mp/osdl/lock/spinlock.h */
