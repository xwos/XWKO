/**
 * @file
 * @brief 玄武OS内核锁机制：自旋锁
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

#ifndef __xwos_mp_lock_spinlock_h__
#define __xwos_mp_lock_spinlock_h__

#include <xwos/standard.h>
#include <linux/spinlock.h>

/**
 * @brief 内核spinlock
 */
struct xwmp_splk {
        spinlock_t lspl; /**< Linux spinlock */
};

struct xwos_irq_resource;

static __xwcc_inline
void xwmp_splk_init(struct xwmp_splk * spl)
{
        spin_lock_init(&spl->lspl);
}

static __xwcc_inline
void xwmp_splk_lock(struct xwmp_splk * spl)
{
        spin_lock(&spl->lspl);
}

static __xwcc_inline
xwer_t xwmp_splk_trylock(struct xwmp_splk * spl)
{
        int acquired;
        xwer_t rc;

        acquired = spin_trylock(&spl->lspl);
        if (acquired) {
                rc = XWOK;
        } else {
                rc = -EAGAIN;
        }
        return rc;
}

static __xwcc_inline
void xwmp_splk_unlock(struct xwmp_splk * spl)
{
        spin_unlock(&spl->lspl);
}

static __xwcc_inline
void xwmp_splk_lock_cpuirq(struct xwmp_splk * spl)
{
        spin_lock_irq(&spl->lspl);
}

static __xwcc_inline
xwer_t xwmp_splk_trylock_cpuirq(struct xwmp_splk * spl)
{
        int acquired;
        xwer_t rc;

        acquired = spin_trylock_irq(&spl->lspl);
        if (acquired) {
                rc = XWOK;
        } else {
                rc = -EAGAIN;
        }
        return rc;
}

static __xwcc_inline
void xwmp_splk_unlock_cpuirq(struct xwmp_splk * spl)
{
        spin_unlock_irq(&spl->lspl);
}

static __xwcc_inline
void xwmp_splk_lock_cpuirqsv(struct xwmp_splk * spl, xwreg_t * flag)
{
        spin_lock_irqsave(&spl->lspl, (*flag));
}

static __xwcc_inline
xwer_t xwmp_splk_trylock_cpuirqsv(struct xwmp_splk * spl, xwreg_t * flag)
{
        int acquired;
        xwer_t rc;

        acquired = spin_trylock_irqsave(&spl->lspl, (*flag));
        if (acquired) {
                rc = XWOK;
        } else {
                rc = -EAGAIN;
        }
        return rc;
}

static __xwcc_inline
void xwmp_splk_unlock_cpuirqrs(struct xwmp_splk * spl, xwreg_t flag)
{
        spin_unlock_irqrestore(&spl->lspl, flag);
}

void xwmp_splk_lock_irqs(struct xwmp_splk *sl,
                         const struct xwos_irq_resource * irqs,
                         xwsz_t num);

xwer_t xwmp_splk_trylock_irqs(struct xwmp_splk *sl,
                              const struct xwos_irq_resource * irqs,
                              xwsz_t num);

void xwmp_splk_unlock_irqs(struct xwmp_splk *sl,
                           const struct xwos_irq_resource * irqs,
                           xwsz_t num);

void xwmp_splk_lock_irqssv(struct xwmp_splk *sl,
                           const struct xwos_irq_resource * irqs,
                           xwreg_t flags[], xwsz_t num);

xwer_t xwmp_splk_trylock_irqssv(struct xwmp_splk *sl,
                                const struct xwos_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num);

void xwmp_splk_unlock_irqsrs(struct xwmp_splk *sl,
                             const struct xwos_irq_resource * irqs,
                             xwreg_t flags[], xwsz_t num);

static __xwcc_inline
void xwmp_splk_lock_bh(struct xwmp_splk * spl)
{
        spin_lock_bh(&spl->lspl);
}

static __xwcc_inline
xwer_t xwmp_splk_trylock_bh(struct xwmp_splk * spl)
{
        int acquired;
        xwer_t rc;

        acquired = spin_trylock_bh(&spl->lspl);
        if (acquired) {
                rc = XWOK;
        } else {
                rc = -EAGAIN;
        }
        return rc;
}

static __xwcc_inline
void xwmp_splk_unlock_bh(struct xwmp_splk * spl)
{
        spin_unlock_bh(&spl->lspl);
}

#endif /* xwos/mp/lock/spinlock.h */
