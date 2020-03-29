/**
 * @file
 * @brief 操作系统接口描述
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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

#ifndef __xwos_osdl_os_h__
#define __xwos_osdl_os_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********        cpu        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/core/irq.h>

static __xw_inline
void xwosdl_cpuirq_enable_lc(void)
{
        xwos_cpuirq_enable_lc();
}

static __xw_inline
void xwosdl_cpuirq_disable_lc(void)
{
        xwos_cpuirq_disable_lc();
}

static __xw_inline
void xwosdl_cpuirq_restore_lc(xwreg_t flag)
{
        xwos_cpuirq_restore_lc(flag);
}

static __xw_inline
void xwosdl_cpuirq_save_lc(xwreg_t * flag)
{
        xwos_cpuirq_save_lc(flag);
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********     scheduler     ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/core/scheduler.h>

#define XWOSDL_SD_PRIORITY_RT_MIN           XWOS_SD_PRIORITY_RT_MIN
#define XWOSDL_SD_PRIORITY_RT_MAX           XWOS_SD_PRIORITY_RT_MAX
#define XWOSDL_SD_PRIORITY_INVALID          XWOS_SD_PRIORITY_INVALID
#define XWOSDL_SD_PRIORITY_RAISE(base, inc) XWOS_SD_PRIORITY_RAISE(base, inc)
#define XWOSDL_SD_PRIORITY_DROP(base, dec)  XWOS_SD_PRIORITY_DROP(base, dec)

static __xw_inline
bool xwosdl_scheduler_prio_tst_valid(xwpr_t prio)
{
        return xwos_scheduler_prio_tst_valid(prio);
}

static __xw_inline
xwer_t xwosdl_scheduler_start_lc(void)
{
        return xwos_scheduler_start_lc();
}

static __xw_inline
xwtm_t xwosdl_scheduler_get_timetick_lc(void)
{
        return xwos_scheduler_get_timetick_lc();
}

static __xw_inline
xwu64_t xwosdl_scheduler_get_tickcount_lc(void)
{
        return xwos_scheduler_get_tickcount_lc();
}

static __xw_inline
xwtm_t xwosdl_scheduler_get_timestamp_lc(void)
{
        return xwos_scheduler_get_timestamp_lc();
}

static __xw_inline
void xwosdl_scheduler_dspmpt_lc(void)
{
        xwos_scheduler_dspmpt_lc();
}

static __xw_inline
void xwosdl_scheduler_enpmpt_lc(void)
{
        xwos_scheduler_enpmpt_lc();
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       thread      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/core/thread.h>

#define xwosdl_tcb xwos_tcb
typedef xwos_thrd_f xwosdl_thrd_f;

static __xw_inline
xwer_t xwosdl_thrd_init(struct xwosdl_tcb * tcb,
                        const char * name,
                        xwosdl_thrd_f mainfunc, void * arg,
                        xwstk_t * stack, xwsz_t stack_size,
                        xwpr_t priority, xwsq_t attr)
{
        return xwos_thrd_init(tcb,
                              name,
                              (xwos_thrd_f)mainfunc, arg,
                              stack, stack_size, priority, attr);
}

static __xw_inline
xwer_t xwosdl_thrd_destroy(struct xwosdl_tcb * tcb)
{
        return xwos_thrd_destroy(tcb);
}

static __xw_inline
xwer_t xwosdl_thrd_create(xwid_t * tidbuf, const char * name,
                          xwosdl_thrd_f mainfunc, void * arg, xwsz_t stack_size,
                          xwpr_t priority, xwsq_t attr)
{
        return xwos_thrd_create(tidbuf, name,
                                (xwos_thrd_f)mainfunc, arg,
                                stack_size, priority, attr);
}

static __xw_inline
xwer_t xwosdl_thrd_delete(xwid_t tid)
{
        return xwos_thrd_delete(tid);
}

static __xw_inline
xwid_t xwosdl_thrd_get_id(struct xwosdl_tcb * tcb)
{
        return xwos_thrd_get_id(tcb);
}

static __xw_inline
xwid_t xwosdl_cthrd_get_id(void)
{
        return xwos_cthrd_get_id();
}

static __xw_inline
void xwosdl_cthrd_yield(void)
{
        xwos_cthrd_yield();
}

static __xw_inline
void xwosdl_cthrd_exit(xwer_t rc)
{
        xwos_cthrd_exit(rc);
}

static __xw_inline
xwer_t xwosdl_thrd_terminate(xwid_t tid, xwer_t * rc)
{
        return xwos_thrd_terminate(tid, rc);
}

static __xw_inline
void xwosdl_cthrd_wait_exit(void)
{
        xwos_cthrd_wait_exit();
}

static __xw_inline
bool xwosdl_cthrd_shld_frz(void)
{
        return xwos_cthrd_shld_frz();
}

static __xw_inline
bool xwosdl_cthrd_shld_stop(void)
{
        return xwos_cthrd_shld_stop();
}

static __xw_inline
bool xwosdl_cthrd_frz_shld_stop(bool * frozen)
{
        return xwos_cthrd_frz_shld_stop(frozen);
}

static __xw_inline
xwer_t xwosdl_thrd_continue(xwid_t tid)
{
        return xwos_thrd_continue(tid);
}

static __xw_inline
xwer_t xwosdl_cthrd_pause(union xwlk_ulock lock, xwsq_t lktype,
                          void * lkdata, xwsz_t datanum,
                          xwsq_t * lkst)
{
        /* FIXME: find lock from ID table */
        return xwos_cthrd_pause(lock.anon, lktype, lkdata, datanum, lkst);
}

static __xw_inline
xwer_t xwosdl_cthrd_timedpause(union xwlk_ulock lock, xwsq_t lktype,
                               void * lkdata, xwsz_t datanum,
                               xwtm_t * xwtm, xwsq_t * lkst)
{
        /* FIXME: find lock from ID table */
        return xwos_cthrd_timedpause(lock.anon, lktype,
                                     lkdata, datanum,
                                     xwtm, lkst);
}

static __xw_inline
xwer_t xwosdl_cthrd_sleep(xwtm_t * xwtm)
{
        return xwos_cthrd_sleep(xwtm);
}

static __xw_inline
xwer_t xwosdl_cthrd_sleep_from(xwtm_t * origin, xwtm_t inc)
{
        return xwos_cthrd_sleep_from(origin, inc);
}

static __xw_inline
xwer_t xwosdl_thrd_intr(xwid_t tid)
{
        return xwos_thrd_intr(tid);
}

static __xw_inline
xwer_t xwosdl_cthrd_freeze(void)
{
        return xwos_cthrd_freeze();
}

static __xw_inline
xwer_t xwosdl_thrd_migrate(xwid_t tid, xwid_t dstcpu)
{
        return xwos_thrd_migrate(tid, dstcpu);
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********        swt        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/core/swt.h>

#define XWOSDL_SWT_FLAG_NULL            XWOS_SWT_FLAG_NULL
#define XWOSDL_SWT_FLAG_RESTART         XWOS_SWT_FLAG_RESTART
#define XWOSDL_SWT_FLAG_AUTORM          XWOS_SWT_FLAG_AUTORM

#define xwosdl_swt xwos_swt
typedef xwos_swt_f xwosdl_swt_f;

static __xw_inline
xwid_t xwosdl_swt_get_id(struct xwosdl_swt * swt)
{
        /* FIXME: Add ID table */
        return (xwid_t)swt;
}

static __xw_inline
struct xwosdl_swt * xwosdl_swt_get_obj(xwid_t swtid)
{
        /* FIXME: Add ID table */
        return (struct xwosdl_swt *)swtid;
}

static __xw_inline
xwer_t xwosdl_swt_init(struct xwosdl_swt * swt, const char * name, xwsq_t flag)
{
        return xwos_swt_init(swt, name, flag);
}

static __xw_inline
xwer_t xwosdl_swt_destroy(struct xwosdl_swt * swt)
{
        return xwos_swt_destroy(swt);
}

static __xw_inline
xwer_t xwosdl_swt_create(xwid_t * swtidbuf, const char * name, xwsq_t flag)
{
        struct xwosdl_swt * ostmr;
        xwer_t rc;

        ostmr = NULL;
        rc = xwos_swt_create(&ostmr, name, flag);
        if (OK == rc) {
                *swtidbuf = xwosdl_swt_get_id(ostmr);
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_swt_delete(xwid_t swtid)
{
        struct xwos_swt * swt;
        xwer_t rc;

        swt = xwosdl_swt_get_obj(swtid);
        rc = xwos_swt_delete(swt);
        if (OK == rc) {
                /* FIXME: Add ID table */
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_swt_start(xwid_t swtid, xwtm_t base, xwtm_t period,
                        xwosdl_swt_f callback, void * arg)
{
        struct xwos_swt * swt;
        xwer_t rc;

        swt = xwosdl_swt_get_obj(swtid);
        rc = xwos_swt_start(swt, base, period, (xwos_swt_f)callback, arg);
        return rc;
}

static __xw_inline
xwer_t xwosdl_swt_stop(xwid_t swtid)
{
        struct xwos_swt * swt;
        xwer_t rc;

        swt = xwosdl_swt_get_obj(swtid);
        rc = xwos_swt_stop(swt);
        return rc;
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       sync        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** semaphore ********/
#include <xwos/sync/semaphore.h>

#define xwosdl_smr xwsync_smr

static __xw_inline
xwid_t xwosdl_smr_get_id(struct xwosdl_smr * smr)
{
        /* FIXME: Add ID table */
        return (xwid_t)smr;
}

static __xw_inline
struct xwosdl_smr * xwosdl_smr_get_obj(xwid_t smrid)
{
        /* FIXME: Add ID table */
        return (struct xwosdl_smr *)smrid;
}

static __xw_inline
xwer_t xwosdl_smr_getvalue(xwid_t smrid, xwssq_t * sval)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_smr_getvalue(smr, sval);
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_init(struct xwosdl_smr * smr, xwssq_t val, xwssq_t max)
{
        return xwsync_smr_init(smr, val, max);
}

static __xw_inline
xwer_t xwosdl_smr_destroy(struct xwosdl_smr * smr)
{
        return xwsync_smr_destroy(smr);
}

static __xw_inline
xwer_t xwosdl_smr_create(xwid_t * smridbuf, xwssq_t val, xwssq_t max)
{
        struct xwosdl_smr * smr;
        xwer_t rc;

        smr = NULL;
        rc = xwsync_smr_create(&smr, val, max);
        if (OK == rc) {
                /* FIXME: Add ID table */
                *smridbuf = (xwid_t)smr;
        } else {
                *smridbuf = (xwid_t)0;
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_delete(xwid_t smrid)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_smr_delete(smr);
        if (OK == rc) {
                /* FIXME: Add ID table */
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_freeze(xwid_t smrid)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_smr_freeze(smr);
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_thaw(xwid_t smrid, xwssq_t val, xwssq_t max)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_smr_thaw(smr, val, max);
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_post(xwid_t smrid)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_smr_post(smr);
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_wait(xwid_t smrid)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_smr_wait(smr);
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_trywait(xwid_t smrid)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_smr_trywait(smr);
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_timedwait(xwid_t smrid, xwtm_t * xwtm)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_smr_timedwait(smr, xwtm);
        return rc;
}

static __xw_inline
xwer_t xwosdl_smr_wait_unintr(xwid_t smrid)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        smr = xwosdl_smr_get_obj(smrid);
        rc = xwsync_smr_wait_unintr(smr);
        return rc;
}

/******** condition ********/
#include <xwos/sync/condition.h>

#define xwosdl_cdt xwsync_cdt

static __xw_inline
xwid_t xwosdl_cdt_get_id(struct xwosdl_cdt * cdt)
{
        /* FIXME: Add ID table */
        return (xwid_t)cdt;
}

static __xw_inline
struct xwosdl_cdt * xwosdl_cdt_get_obj(xwid_t cdtid)
{
        /* FIXME: Add ID table */
        return (struct xwosdl_cdt *)cdtid;
}

static __xw_inline
xwer_t xwosdl_cdt_init(struct xwosdl_cdt * cdt)
{
        return xwsync_cdt_init(cdt);
}

static __xw_inline
xwer_t xwosdl_cdt_destroy(struct xwosdl_cdt * cdt)
{
        return xwsync_cdt_destroy(cdt);
}

static __xw_inline
xwer_t xwosdl_cdt_create(xwid_t * cdtidbuf)
{
        struct xwsync_cdt * cdt;
        xwer_t rc;

        cdt = NULL;
        rc = xwsync_cdt_create(&cdt);
        if (OK == rc) {
                /* FIXME: Add ID table */
                *cdtidbuf = (xwid_t)cdt;
        } else {
                *cdtidbuf = (xwid_t)0;
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_cdt_delete(xwid_t cdtid)
{
        struct xwosdl_cdt * cdt;
        xwer_t rc;

        cdt = xwosdl_cdt_get_obj(cdtid);
        rc = xwsync_cdt_delete(cdt);
        if (OK == rc) {
                /* FIXME: Add ID table */
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_cdt_thaw(xwid_t cdtid)
{
        struct xwosdl_cdt * cdt;
        xwer_t rc;

        cdt = xwosdl_cdt_get_obj(cdtid);
        rc = xwsync_cdt_thaw(cdt);
        return rc;
}

static __xw_inline
xwer_t xwosdl_cdt_freeze(xwid_t cdtid)
{
        struct xwosdl_cdt * cdt;
        xwer_t rc;

        cdt = xwosdl_cdt_get_obj(cdtid);
        rc = xwsync_cdt_freeze(cdt);
        return rc;
}

static __xw_inline
xwer_t xwosdl_cdt_intr_all(xwid_t cdtid)
{
        struct xwosdl_cdt * cdt;
        xwer_t rc;

        cdt = xwosdl_cdt_get_obj(cdtid);
        rc = xwsync_cdt_intr_all(cdt);
        return rc;
}

static __xw_inline
xwer_t xwosdl_cdt_broadcast(xwid_t cdtid)
{
        struct xwosdl_cdt * cdt;
        xwer_t rc;

        cdt = xwosdl_cdt_get_obj(cdtid);
        rc = xwsync_cdt_broadcast(cdt);
        return rc;
}

static __xw_inline
xwer_t xwosdl_cdt_unicast(xwid_t cdtid)
{
        struct xwosdl_cdt * cdt;
        xwer_t rc;

        cdt = xwosdl_cdt_get_obj(cdtid);
        rc = xwsync_cdt_unicast(cdt);
        return rc;
}

static __xw_inline
xwer_t xwosdl_cdt_wait(xwid_t cdtid,
                       union xwlk_ulock lock, xwsq_t lktype,
                       void * lkdata, xwsz_t datanum,
                       xwsq_t * lkst)
{
        struct xwosdl_cdt * cdt;
        xwer_t rc;

        cdt = xwosdl_cdt_get_obj(cdtid);
        /* FIXME: find lock from ID table */
        rc = xwsync_cdt_wait(cdt,
                             lock.anon, lktype,
                             lkdata, datanum,
                             lkst);
        return rc;
}

static __xw_inline
xwer_t xwosdl_cdt_timedwait(xwid_t cdtid,
                            union xwlk_ulock lock, xwsq_t lktype,
                            void * lkdata, xwsz_t datanum,
                            xwtm_t * xwtm, xwsq_t * lkst)
{
        struct xwosdl_cdt * cdt;
        xwer_t rc;

        cdt = xwosdl_cdt_get_obj(cdtid);
        /* FIXME: find lock from ID table */
        rc = xwsync_cdt_timedwait(cdt,
                                  lock.anon, lktype,
                                  lkdata, datanum,
                                  xwtm,
                                  lkst);
        return rc;
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       lock        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** spinlock ********/
#include <xwos/lock/spinlock.h>

typedef struct xwlk_splk xwosdl_splk_t;

static __xw_inline
void xwosdl_splk_init(xwosdl_splk_t * ossl)
{
        xwlk_splk_init(ossl);
}

static __xw_inline
void xwosdl_splk_lock(xwosdl_splk_t * ossl)
{
        xwlk_splk_lock(ossl);
}

static __xw_inline
xwer_t xwosdl_splk_trylock(xwosdl_splk_t * ossl)
{
        return xwlk_splk_trylock(ossl);
}

static __xw_inline
void xwosdl_splk_unlock(xwosdl_splk_t * ossl)
{
        xwlk_splk_unlock(ossl);
}

static __xw_inline
void xwosdl_splk_lock_cpuirq(xwosdl_splk_t * ossl)
{
        xwlk_splk_lock_cpuirq(ossl);
}

static __xw_inline
xwer_t xwosdl_splk_trylock_cpuirq(xwosdl_splk_t * ossl)
{
        return xwlk_splk_trylock_cpuirq(ossl);
}

static __xw_inline
void xwosdl_splk_unlock_cpuirq(xwosdl_splk_t * ossl)
{
        xwlk_splk_unlock_cpuirq(ossl);
}

static __xw_inline
void xwosdl_splk_lock_cpuirqsv(xwosdl_splk_t * ossl, xwreg_t * flag)
{
        xwlk_splk_lock_cpuirqsv(ossl, flag);
}

static __xw_inline
xwer_t xwosdl_splk_trylock_cpuirqsv(xwosdl_splk_t * ossl, xwreg_t * flag)
{
        return xwlk_splk_trylock_cpuirqsv(ossl, flag);
}

static __xw_inline
void xwosdl_splk_unlock_cpuirqrs(xwosdl_splk_t * ossl, xwreg_t flag)
{
        xwlk_splk_unlock_cpuirqrs(ossl, flag);
}

static __xw_inline
void xwosdl_splk_lock_irqs(xwosdl_splk_t * ossl,
                       const struct xwos_irq_resource * irqs, xwsz_t num)
{
        xwlk_splk_lock_irqs(ossl, irqs, num);
}

static __xw_inline
xwer_t xwosdl_splk_trylock_irqs(xwosdl_splk_t * ossl,
                                const struct xwos_irq_resource * irqs, xwsz_t num)
{
        return xwlk_splk_trylock_irqs(ossl, irqs, num);
}

static __xw_inline
void xwosdl_splk_unlock_irqs(xwosdl_splk_t * ossl,
                             const struct xwos_irq_resource * irqs, xwsz_t num)
{
        xwlk_splk_unlock_irqs(ossl, irqs, num);
}

static __xw_inline
void xwosdl_splk_lock_irqssv(xwosdl_splk_t * ossl,
                             const struct xwos_irq_resource * irqs,
                             xwreg_t flags[], xwsz_t num)
{
        xwlk_splk_lock_irqssv(ossl, irqs, flags, num);
}

static __xw_inline
xwer_t xwosdl_splk_trylock_irqssv(xwosdl_splk_t * ossl,
                                  const struct xwos_irq_resource * irqs,
                                  xwreg_t flags[], xwsz_t num)
{
        return xwlk_splk_trylock_irqssv(ossl, irqs, flags, num);
}

static __xw_inline
void xwosdl_splk_unlock_irqsrs(xwosdl_splk_t * ossl,
                               const struct xwos_irq_resource * irqs,
                               xwreg_t flags[], xwsz_t num)
{
        xwlk_splk_unlock_irqsrs(ossl, irqs, flags, num);
}

static __xw_inline
void xwosdl_splk_lock_bh(xwosdl_splk_t * ossl)
{
        xwlk_splk_lock_bh(ossl);
}

static __xw_inline
xwer_t xwosdl_splk_trylock_bh(xwosdl_splk_t * ossl)
{
        return xwlk_splk_trylock_bh(ossl);
}

static __xw_inline
void xwosdl_splk_unlock_bh(xwosdl_splk_t * ossl)
{
        xwlk_splk_unlock_bh(ossl);
}

/******** seqlock ********/
#include <xwos/lock/seqlock.h>

#define XWOSDL_SQLK_GRANULARITY         XWLK_SQLK_GRANULARITY

typedef struct xwlk_sqlk xwosdl_sqlk_t;

static __xw_inline
void xwosdl_sqlk_init(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_init(ossql);
}

static __xw_inline
xwsq_t xwosdl_sqlk_rd_begin(xwosdl_sqlk_t * ossql)
{
        return xwlk_sqlk_rd_begin(ossql);
}

static __xw_inline
bool xwosdl_sqlk_rd_retry(xwosdl_sqlk_t * ossql, xwsq_t start)
{
        return xwlk_sqlk_rd_retry(ossql, start);
}

static __xw_inline
xwsq_t xwosdl_sqlk_get_seq(xwosdl_sqlk_t * ossql)
{
        return xwlk_sqlk_get_seq(ossql);
}

static __xw_inline
void xwosdl_sqlk_rdex_lock(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_rdex_lock(ossql);
}

static __xw_inline
xwer_t xwosdl_sqlk_rdex_trylock(xwosdl_sqlk_t * ossql)
{
        return xwlk_sqlk_rdex_trylock(ossql);
}

static __xw_inline
void xwosdl_sqlk_rdex_unlock(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_rdex_unlock(ossql);
}

static __xw_inline
void xwosdl_sqlk_rdex_lock_cpuirq(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_rdex_lock_cpuirq(ossql);
}

static __xw_inline
xwer_t xwosdl_sqlk_rdex_trylock_cpuirq(xwosdl_sqlk_t * ossql)
{
        return xwlk_sqlk_rdex_trylock_cpuirq(ossql);
}

static __xw_inline
void xwosdl_sqlk_rdex_unlock_cpuirq(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_rdex_unlock_cpuirq(ossql);
}

static __xw_inline
void xwosdl_sqlk_rdex_lock_cpuirqsv(xwosdl_sqlk_t * ossql, xwreg_t * flag)
{
        xwlk_sqlk_rdex_lock_cpuirqsv(ossql, flag);
}

static __xw_inline
xwer_t xwosdl_sqlk_rdex_trylock_cpuirqsv(xwosdl_sqlk_t * ossql, xwreg_t * flag)
{
        return xwlk_sqlk_rdex_trylock_cpuirqsv(ossql, flag);
}

static __xw_inline
void xwosdl_sqlk_rdex_unlock_cpuirqrs(xwosdl_sqlk_t * ossql, xwreg_t flag)
{
        xwlk_sqlk_rdex_unlock_cpuirqrs(ossql, flag);
}

static __xw_inline
void xwosdl_sqlk_rdex_lock_irqs(xwosdl_sqlk_t * ossql,
                                const struct xwos_irq_resource * irqs,
                                xwsz_t num)
{
        xwlk_sqlk_rdex_lock_irqs(ossql, irqs, num);
}

static __xw_inline
xwer_t xwosdl_sqlk_rdex_trylock_irqs(xwosdl_sqlk_t * ossql,
                                     const struct xwos_irq_resource * irqs,
                                     xwsz_t num)
{
        return xwlk_sqlk_rdex_trylock_irqs(ossql, irqs, num);
}

static __xw_inline
void xwosdl_sqlk_rdex_unlock_irqs(xwosdl_sqlk_t * ossql,
                                  const struct xwos_irq_resource * irqs,
                                  xwsz_t num)
{
        xwlk_sqlk_rdex_unlock_irqs(ossql, irqs, num);
}

static __xw_inline
void xwosdl_sqlk_rdex_lock_irqssv(xwosdl_sqlk_t * ossql,
                                  const struct xwos_irq_resource * irqs,
                                  xwreg_t flags[], xwsz_t num)
{
        xwlk_sqlk_rdex_lock_irqssv(ossql, irqs, flags, num);
}

static __xw_inline
xwer_t xwosdl_sqlk_rdex_trylock_irqssv(xwosdl_sqlk_t * ossql,
                                       const struct xwos_irq_resource * irqs,
                                       xwreg_t flags[], xwsz_t num)
{
        return xwlk_sqlk_rdex_trylock_irqssv(ossql, irqs, flags, num);
}

static __xw_inline
void xwosdl_sqlk_rdex_unlock_irqsrs(xwosdl_sqlk_t * ossql,
                                    const struct xwos_irq_resource * irqs,
                                    xwreg_t flags[], xwsz_t num)
{
        xwlk_sqlk_rdex_unlock_irqsrs(ossql, irqs, flags, num);
}

static __xw_inline
void xwosdl_sqlk_rdex_lock_bh(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_rdex_lock_bh(ossql);
}

static __xw_inline
xwer_t xwosdl_sqlk_rdex_trylock_bh(xwosdl_sqlk_t * ossql)
{
        return xwlk_sqlk_rdex_trylock_bh(ossql);
}

static __xw_inline
void xwosdl_sqlk_rdex_unlock_bh(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_rdex_unlock_bh(ossql);
}

static __xw_inline
void xwosdl_sqlk_wr_lock(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_wr_lock(ossql);
}

static __xw_inline
xwer_t xwosdl_sqlk_wr_trylock(xwosdl_sqlk_t * ossql)
{
        return xwlk_sqlk_wr_trylock(ossql);
}

static __xw_inline
void xwosdl_sqlk_wr_unlock(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_wr_unlock(ossql);
}

static __xw_inline
void xwosdl_sqlk_wr_lock_cpuirq(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_wr_lock_cpuirq(ossql);
}

static __xw_inline
xwer_t xwosdl_sqlk_wr_trylock_cpuirq(xwosdl_sqlk_t * ossql)
{
        return xwlk_sqlk_wr_trylock_cpuirq(ossql);
}

static __xw_inline
void xwosdl_sqlk_wr_unlock_cpuirq(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_wr_unlock_cpuirq(ossql);
}

static __xw_inline
void xwosdl_sqlk_wr_lock_cpuirqsv(xwosdl_sqlk_t * ossql, xwreg_t * flag)
{
        xwlk_sqlk_wr_lock_cpuirqsv(ossql, flag);
}


static __xw_inline
xwer_t xwosdl_sqlk_wr_trylock_cpuirqsv(xwosdl_sqlk_t * ossql, xwreg_t * flag)
{
        return xwlk_sqlk_wr_trylock_cpuirqsv(ossql, flag);
}

static __xw_inline
void xwosdl_sqlk_wr_unlock_cpuirqrs(xwosdl_sqlk_t * ossql, xwreg_t flag)
{
        xwlk_sqlk_wr_unlock_cpuirqrs(ossql, flag);
}

static __xw_inline
void xwosdl_sqlk_wr_lock_irqs(xwosdl_sqlk_t * ossql,
                              const struct xwos_irq_resource * irqs,
                              xwsz_t num)
{
        xwlk_sqlk_wr_lock_irqs(ossql, irqs, num);
}

static __xw_inline
xwer_t xwosdl_sqlk_wr_trylock_irqs(xwosdl_sqlk_t * ossql,
                                   const struct xwos_irq_resource * irqs,
                                   xwsz_t num)
{
        return xwlk_sqlk_wr_trylock_irqs(ossql, irqs, num);
}

static __xw_inline
void xwosdl_sqlk_wr_unlock_irqs(xwosdl_sqlk_t * ossql,
                                const struct xwos_irq_resource * irqs,
                                xwsz_t num)
{
        xwlk_sqlk_wr_unlock_irqs(ossql, irqs, num);
}

static __xw_inline
void xwosdl_sqlk_wr_lock_irqssv(xwosdl_sqlk_t * ossql,
                                const struct xwos_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num)
{
        xwlk_sqlk_wr_lock_irqssv(ossql, irqs, flags, num);
}

static __xw_inline
xwer_t xwosdl_sqlk_wr_trylock_irqssv(xwosdl_sqlk_t * ossql,
                                     const struct xwos_irq_resource * irqs,
                                     xwreg_t flags[], xwsz_t num)
{
        return xwlk_sqlk_wr_trylock_irqssv(ossql, irqs, flags, num);
}

static __xw_inline
void xwosdl_sqlk_wr_unlock_irqsrs(xwosdl_sqlk_t * ossql,
                                  const struct xwos_irq_resource * irqs,
                                  xwreg_t flags[], xwsz_t num)
{
        xwlk_sqlk_wr_unlock_irqsrs(ossql, irqs, flags, num);
}

static __xw_inline
void xwosdl_sqlk_wr_lock_bh(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_wr_lock_bh(ossql);
}

static __xw_inline
xwer_t xwosdl_sqlk_wr_trylock_bh(xwosdl_sqlk_t * ossql)
{
        return xwlk_sqlk_wr_trylock_bh(ossql);
}

static __xw_inline
void xwosdl_sqlk_wr_unlock_bh(xwosdl_sqlk_t * ossql)
{
        xwlk_sqlk_wr_unlock_bh(ossql);
}

/******** mutex ********/
#include <xwos/lock/mutex.h>

#define xwosdl_mtx xwlk_mtx

static __xw_inline
xwid_t xwosdl_mtx_get_id(struct xwosdl_mtx * mtx)
{
        /* FIXME: Add ID table */
        return (xwid_t)mtx;
}

static __xw_inline
struct xwosdl_mtx * xwosdl_mtx_get_obj(xwid_t mtxid)
{
        /* FIXME: Add ID table */
        return (struct xwosdl_mtx *)mtxid;
}

static __xw_inline
xwer_t xwosdl_mtx_init(struct xwosdl_mtx * mtx, xwpr_t sprio)
{
        return xwlk_mtx_init(mtx, sprio);
}

static __xw_inline
xwer_t xwosdl_mtx_destroy(struct xwosdl_mtx * mtx)
{
        return xwlk_mtx_destroy(mtx);
}

static __xw_inline
xwer_t xwosdl_mtx_create(xwid_t * mtxidbuf, xwpr_t sprio)
{
        struct xwosdl_mtx * mtx;
        xwer_t rc;

        mtx = NULL;
        rc = xwlk_mtx_create(&mtx, sprio);
        if (OK == rc) {
                /* FIXME: Add ID table */
                *mtxidbuf = (xwid_t)mtx;
        } else {
                *mtxidbuf = (xwid_t)0;
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_mtx_delete(xwid_t mtxid)
{
        struct xwlk_mtx * mtx;
        xwer_t rc;

        mtx = xwosdl_mtx_get_obj(mtxid);
        rc = xwlk_mtx_delete(mtx);
        if (OK == rc) {
                /* FIXME: Add ID table */
        }
        return rc;
}

static __xw_inline
xwer_t xwosdl_mtx_unlock(xwid_t mtxid)
{
        struct xwlk_mtx * mtx;
        xwer_t rc;

        mtx = xwosdl_mtx_get_obj(mtxid);
        rc = xwlk_mtx_unlock(mtx);
        return rc;
}

static __xw_inline
xwer_t xwosdl_mtx_lock(xwid_t mtxid)
{
        struct xwlk_mtx * mtx;
        xwer_t rc;

        mtx = xwosdl_mtx_get_obj(mtxid);
        rc = xwlk_mtx_lock(mtx);
        return rc;
}

static __xw_inline
xwer_t xwosdl_mtx_trylock(xwid_t mtxid)
{
        struct xwlk_mtx * mtx;
        xwer_t rc;

        mtx = xwosdl_mtx_get_obj(mtxid);
        rc = xwlk_mtx_trylock(mtx);
        return rc;
}

static __xw_inline
xwer_t xwosdl_mtx_timedlock(xwid_t mtxid, xwtm_t * xwtm)
{
        struct xwlk_mtx * mtx;
        xwer_t rc;

        mtx = xwosdl_mtx_get_obj(mtxid);
        rc = xwlk_mtx_timedlock(mtx, xwtm);
        return rc;
}

static __xw_inline
xwer_t xwosdl_mtx_lock_unintr(xwid_t mtxid)
{
        struct xwlk_mtx * mtx;
        xwer_t rc;

        mtx = xwosdl_mtx_get_obj(mtxid);
        rc = xwlk_mtx_lock_unintr(mtx);
        return rc;
}

#endif /* xwos/osdl/os.h */
