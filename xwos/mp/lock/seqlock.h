/**
 * @file
 * @brief 玄武OS内核锁机制：顺序锁
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

#ifndef __xwos_mp_lock_seqlock_h__
#define __xwos_mp_lock_seqlock_h__

#include <xwos/standard.h>
#include <linux/seqlock.h>

#define XWMP_SQLK_GRANULARITY           (1U)

/**
 * @brief 顺序锁
 */
struct xwmp_sqlk {
        seqlock_t lsql;
};

struct xwos_irq_resource;

static __xwcc_inline
void xwmp_sqlk_init(struct xwmp_sqlk *sql)
{
        seqlock_init(&sql->lsql);
}

static __xwcc_inline
xwsq_t xwmp_sqlk_rd_begin(struct xwmp_sqlk *sql)
{
        return read_seqbegin(&sql->lsql);
}

static __xwcc_inline
bool xwmp_sqlk_rd_retry(struct xwmp_sqlk *sql, xwsq_t start)
{
        return read_seqretry(&sql->lsql, start);
}

static __xwcc_inline
xwsq_t xwmp_sqlk_get_seq(struct xwmp_sqlk *sql)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 17, 0)
        return raw_read_seqcount(&sql->lsql.seqcount);
#else
        unsigned ret;

        ret = ACCESS_ONCE(&sql->lsql.seqcount);
        __xw_smp_rmb();
        return ret;
#endif
}

static __xwcc_inline
void xwmp_sqlk_rdex_lock(struct xwmp_sqlk *sql)
{
        spin_lock(&sql->lsql.lock);
}

static __xwcc_inline
xwer_t xwmp_sqlk_rdex_trylock(struct xwmp_sqlk *sql)
{
        int acquired;
        xwer_t rc;

        acquired = spin_trylock(&sql->lsql.lock);
        if (acquired) {
                rc = XWOK;
        } else {
                rc = -EAGAIN;
        }
        return rc;
}

static __xwcc_inline
void xwmp_sqlk_rdex_unlock(struct xwmp_sqlk *sql)
{
        spin_unlock(&sql->lsql.lock);
}

static __xwcc_inline
void xwmp_sqlk_rdex_lock_cpuirq(struct xwmp_sqlk *sql)
{
        spin_lock_irq(&sql->lsql.lock);
}

static __xwcc_inline
xwer_t xwmp_sqlk_rdex_trylock_cpuirq(struct xwmp_sqlk *sql)
{
        int acquired;
        xwer_t rc;

        acquired = spin_trylock_irq(&sql->lsql.lock);
        if (acquired) {
                rc = XWOK;
        } else {
                rc = -EAGAIN;
        }
        return rc;
}

static __xwcc_inline
void xwmp_sqlk_rdex_unlock_cpuirq(struct xwmp_sqlk *sql)
{
        spin_unlock_irq(&sql->lsql.lock);
}

static __xwcc_inline
void xwmp_sqlk_rdex_lock_cpuirqsv(struct xwmp_sqlk *sql, xwreg_t *flag)
{
        spin_lock_irqsave(&sql->lsql.lock, (*flag));
}

static __xwcc_inline
xwer_t xwmp_sqlk_rdex_trylock_cpuirqsv(struct xwmp_sqlk *sql, xwreg_t *flag)
{
        int acquired;
        xwer_t rc;

        acquired = spin_trylock_irqsave(&sql->lsql.lock, (*flag));
        if (acquired)
                rc = XWOK;
        else
                rc = -EAGAIN;
        return rc;
}

static __xwcc_inline
void xwmp_sqlk_rdex_unlock_cpuirqrs(struct xwmp_sqlk *sql, xwreg_t flag)
{
        spin_unlock_irqrestore(&sql->lsql.lock, flag);
}

void xwmp_sqlk_rdex_lock_irqs(struct xwmp_sqlk *sql,
                              const struct xwos_irq_resource * irqs,
                              xwsz_t num);

xwer_t xwmp_sqlk_rdex_trylock_irqs(struct xwmp_sqlk *sql,
                                   const struct xwos_irq_resource * irqs,
                                   xwsz_t num);

void xwmp_sqlk_rdex_unlock_irqs(struct xwmp_sqlk *sql,
                                const struct xwos_irq_resource * irqs,
                                xwsz_t num);

void xwmp_sqlk_rdex_lock_irqssv(struct xwmp_sqlk *sql,
                                const struct xwos_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num);

xwer_t xwmp_sqlk_rdex_trylock_irqssv(struct xwmp_sqlk *sql,
                                     const struct xwos_irq_resource * irqs,
                                     xwreg_t flags[], xwsz_t num);

void xwmp_sqlk_rdex_unlock_irqsrs(struct xwmp_sqlk *sql,
                                  const struct xwos_irq_resource * irqs,
                                  xwreg_t flags[], xwsz_t num);

static __xwcc_inline
void xwmp_sqlk_rdex_lock_bh(struct xwmp_sqlk *sql)
{
        spin_lock_bh(&sql->lsql.lock);
}

static __xwcc_inline
xwer_t xwmp_sqlk_rdex_trylock_bh(struct xwmp_sqlk *sql)
{
        int acquired;
        xwer_t rc;

        acquired = spin_trylock_bh(&sql->lsql.lock);
        if (acquired)
                rc = XWOK;
        else
                rc = -EAGAIN;
        return rc;
}

static __xwcc_inline
void xwmp_sqlk_rdex_unlock_bh(struct xwmp_sqlk *sql)
{
        spin_unlock_bh(&sql->lsql.lock);
}

static __xwcc_inline
void xwmp_sqlk_wr_lock(struct xwmp_sqlk *sql)
{
        write_seqlock(&sql->lsql);
}

static __xwcc_inline
xwer_t xwmp_sqlk_wr_trylock(struct xwmp_sqlk *sql)
{
        int acquired;
        xwer_t rc;

        acquired = spin_trylock(&sql->lsql.lock);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 12, 0)
        if (acquired) {
                write_seqcount_begin(&sql->lsql.seqcount);
                rc = XWOK;
        } else {
                rc = -EAGAIN;
        }
#else
        if (acquired) {
                sql->lsql.sequence ++;
                rc = XWOK;
        } else {
                rc = -EAGAIN;
        }
#endif
        return rc;
}

static __xwcc_inline
void xwmp_sqlk_wr_unlock(struct xwmp_sqlk *sql)
{
        write_sequnlock(&sql->lsql);
}

static __xwcc_inline
void xwmp_sqlk_wr_lock_cpuirq(struct xwmp_sqlk *sql)
{
        write_seqlock_irq(&sql->lsql);
}

static __xwcc_inline
xwer_t xwmp_sqlk_wr_trylock_cpuirq(struct xwmp_sqlk *sql)
{
        int acquired;
        xwer_t rc;

        acquired = spin_trylock_irq(&sql->lsql.lock);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 12, 0)
        if (acquired) {
                write_seqcount_begin(&sql->lsql.seqcount);
                rc = XWOK;
        } else {
                rc = -EAGAIN;
        }
#else
        if (acquired) {
                sql->lsql.sequence ++;
                rc = XWOK;
        } else {
                rc = -EAGAIN;
        }
#endif
        return rc;
}

static __xwcc_inline
void xwmp_sqlk_wr_unlock_cpuirq(struct xwmp_sqlk *sql)
{
        write_sequnlock_irq(&sql->lsql);
}

static __xwcc_inline
void xwmp_sqlk_wr_lock_cpuirqsv(struct xwmp_sqlk *sql, xwreg_t *flag)
{
        write_seqlock_irqsave(&sql->lsql, (*flag));
}

static __xwcc_inline
xwer_t xwmp_sqlk_wr_trylock_cpuirqsv(struct xwmp_sqlk *sql, xwreg_t *flag)
{
        int acquired;
        xwer_t rc;

        acquired = spin_trylock_irqsave(&sql->lsql.lock, (*flag));
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 12, 0)
        if (acquired) {
                write_seqcount_begin(&sql->lsql.seqcount);
                rc = XWOK;
        } else {
                rc = -EAGAIN;
        }
#else
        if (acquired) {
                sql->lsql.sequence ++;
                rc = XWOK;
        } else {
                rc = -EAGAIN;
        }
#endif
        return rc;
}

static __xwcc_inline
void xwmp_sqlk_wr_unlock_cpuirqrs(struct xwmp_sqlk *sql, xwreg_t flag)
{
        write_sequnlock_irqrestore(&sql->lsql, flag);
}

void xwmp_sqlk_wr_lock_irqs(struct xwmp_sqlk *sql,
                            const struct xwos_irq_resource * irqs,
                            xwsz_t num);

xwer_t xwmp_sqlk_wr_trylock_irqs(struct xwmp_sqlk *sql,
                                 const struct xwos_irq_resource * irqs,
                                 xwsz_t num);

void xwmp_sqlk_wr_unlock_irqs(struct xwmp_sqlk *sql,
                              const struct xwos_irq_resource * irqs,
                              xwsz_t num);

void xwmp_sqlk_wr_lock_irqssv(struct xwmp_sqlk *sql,
                              const struct xwos_irq_resource * irqs,
                              xwreg_t flags[], xwsz_t num);

xwer_t xwmp_sqlk_wr_trylock_irqssv(struct xwmp_sqlk *sql,
                                   const struct xwos_irq_resource * irqs,
                                   xwreg_t flags[], xwsz_t num);

void xwmp_sqlk_wr_unlock_irqsrs(struct xwmp_sqlk *sql,
                                const struct xwos_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num);

static __xwcc_inline
void xwmp_sqlk_wr_lock_bh(struct xwmp_sqlk *sql)
{
        write_seqlock_bh(&sql->lsql);
}

static __xwcc_inline
xwer_t xwmp_sqlk_wr_trylock_bh(struct xwmp_sqlk *sql)
{
        int acquired;
        xwer_t rc;

        acquired = spin_trylock_bh(&sql->lsql.lock);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 12, 0)
        if (acquired) {
                write_seqcount_begin(&sql->lsql.seqcount);
                rc = XWOK;
        } else {
                rc = -EAGAIN;
        }
#else
        if (acquired) {
                sql->lsql.sequence ++;
                rc = XWOK;
        } else {
                rc = -EAGAIN;
        }
#endif
        return rc;
}

static __xwcc_inline
void xwmp_sqlk_wr_unlock_bh(struct xwmp_sqlk *sql)
{
        write_sequnlock_bh(&sql->lsql);
}

#endif /* xwos/lock/seqlock.h */
