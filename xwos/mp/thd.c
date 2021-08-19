/**
 * @file
 * @brief 玄武OS内核：线程
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

#include <xwos/standard.h>
#include <linux/sched.h>
#include <linux/freezer.h>
#include <linux/hrtimer.h>
#include <linux/jiffies.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0)
#  include <linux/sched/rt.h>
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
#  include <linux/sched/signal.h>
#  include <uapi/linux/sched/types.h>
#endif
#include <linux/preempt.h>
#include <linux/kthread.h>

#include <xwos/lib/xwlog.h>
#include <xwos/mp/ksym.h>
#include <xwos/mp/skd.h>
#include <xwos/mp/thd.h>
#include <xwos/mp/pm.h>
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/lock/seqlock.h>
#include <xwos/mp/lock/mtx.h>

#define LOGTAG "thd"

KSYM_DEF(__lock_task_sighand);
KSYM_DEF(signal_wake_up_state);
KSYM_DEF(find_task_by_vpid);
KSYM_DEF(sched_setscheduler);

xwer_t xwmp_thd_grab(struct xwmp_thd * thd)
{
        get_task_struct(thd);
        return XWOK;
}
EXPORT_SYMBOL(xwmp_thd_grab);

xwer_t xwmp_thd_put(struct xwmp_thd * thd)
{
        put_task_struct(thd);
        return XWOK;
}
EXPORT_SYMBOL(xwmp_thd_put);

xwer_t xwmp_thd_acquire(struct xwmp_thd * thd, xwsq_t tik)
{
        pid_t tid;
        xwer_t rc;

        tid = (pid_t)task_pid_vnr(thd);
        if (tik == tid) {
                get_task_struct(thd);
                rc = XWOK;
        } else {
                rc = -EACCES;
        }
        return rc;
}
EXPORT_SYMBOL(xwmp_thd_acquire);

xwer_t xwmp_thd_release(struct xwmp_thd * thd, xwsq_t tik)
{
        pid_t tid;
        xwer_t rc;

        tid = (xwid_t)task_pid_vnr(thd);
        if (tik == tid) {
                put_task_struct(thd);
                rc = XWOK;
        } else {
                rc = -EACCES;
        }
        return rc;
}
EXPORT_SYMBOL(xwmp_thd_release);

xwsq_t xwmp_thd_gettik(struct xwmp_thd * thd)
{
        return (xwsq_t)task_pid_vnr(thd);
}
EXPORT_SYMBOL(xwmp_thd_gettik);

xwer_t xwmp_thd_create(struct xwmp_thd ** thdbuf, const char * name,
                       xwmp_thd_f mainfunc, void * arg,
                       xwsz_t stack_size,
                       xwpr_t priority, xwsq_t attr)
{
        struct task_struct * ts;
        struct sched_param schparam;
        xwer_t rc;

        (void)stack_size;
        ts = kthread_create(mainfunc, arg, name);
        if (is_err(ts)) {
                rc = ptr_err(ts);
                goto err_kthread_create;
        }
        ts->flags &= ~PF_NOFREEZE;
        schparam.sched_priority = priority;
        KSYM_CALL(sched_setscheduler, ts, SCHED_FIFO, &schparam);
        if (!(attr & XWMP_SKDATTR_DETACHED)) {
                get_task_struct(ts);
        }
        wake_up_process(ts);
        *thdbuf = ts;
        return XWOK;

err_kthread_create:
        return rc;
}
EXPORT_SYMBOL(xwmp_thd_create);

void xwmp_cthd_yield(void)
{
        yield();
}
EXPORT_SYMBOL(xwmp_cthd_yield);

void xwmp_cthd_exit(xwer_t rc)
{
        do_exit((long)rc);
}
EXPORT_SYMBOL(xwmp_cthd_exit);

xwer_t xwmp_thd_stop(struct xwmp_thd * thd, xwer_t * trc)
{
        xwer_t rc;

        linux_thd_fake_signal(thd);
        rc = (xwer_t)kthread_stop(thd);
        if (trc) {
                *trc = rc;
        }
        put_task_struct(thd);
        return XWOK;
}
EXPORT_SYMBOL(xwmp_thd_stop);

xwer_t xwmp_thd_cancel(struct xwmp_thd * thd)
{
        linux_thd_fake_signal(thd);
        kthread_stop(thd);
        return XWOK;
}
EXPORT_SYMBOL(xwmp_thd_cancel);

xwer_t xwmp_thd_join(struct xwmp_thd * thd, xwer_t * trc)
{
        xwer_t rc;

        if (task_is_stopped(thd)) {
                rc = (xwer_t)thd->exit_code;
        } else {
                rc = (xwer_t)kthread_stop(thd);
        }
        if (trc) {
                *trc = rc;
        }
        put_task_struct(thd);
        return XWOK;
}
EXPORT_SYMBOL(xwmp_thd_join);

xwer_t xwmp_thd_detach(struct xwmp_thd * thd)
{
        put_task_struct(thd);
        return XWOK;
}
EXPORT_SYMBOL(xwmp_thd_detach);

xwer_t xwmp_thd_intr(struct xwmp_thd * thd)
{
        linux_thd_fake_signal_wake_up(thd);
        return XWOK;
}
EXPORT_SYMBOL(xwmp_thd_intr);

struct xwmp_thd * xwmp_cthd_self(void)
{
        return current;
}
EXPORT_SYMBOL(xwmp_cthd_self);

bool xwmp_cthd_shld_stop(void)
{
        return kthread_should_stop();
}
EXPORT_SYMBOL(xwmp_cthd_shld_stop);

bool xwmp_cthd_shld_frz(void)
{
        return freezing(current) || xwmp_pm_tst_lpm();
}
EXPORT_SYMBOL(xwmp_cthd_shld_frz);

bool xwmp_cthd_frz_shld_stop(bool * frozen)
{
        xwer_t rc;

        rc = -EPERM;
        if (xwmp_cthd_shld_frz()) {
                rc = xwmp_cthd_freeze();
        }
        if (frozen) {
                if (XWOK == rc) {
                        *frozen = true;
                } else {
                        *frozen = false;
                }
        }
        return xwmp_cthd_shld_stop();
}
EXPORT_SYMBOL(xwmp_cthd_frz_shld_stop);

xwer_t xwmp_thd_tid2thd(xwsq_t tid, struct xwmp_thd ** ptrbuf)
{
        struct xwmp_thd * thd;
        xwer_t rc;

        rcu_read_lock();
        thd = KSYM_CALL(find_task_by_vpid, tid);
        if (thd) {
                xwmp_thd_grab(thd);
                rcu_read_unlock();
                *ptrbuf = thd;
                rc = XWOK;
        } else {
                rcu_read_unlock();
                rc = -ESTALE;
        }
        return rc;
}
EXPORT_SYMBOL(xwmp_thd_tid2thd);

xwer_t xwmp_thd_do_unlock(void * lock, xwid_t lktype, void * lkdata)
{
        xwer_t rc;
        union {
                struct xwmp_mtx * mtx;
                struct xwmp_splk * sp;
                struct xwmp_sqlk * sq;
                struct xwos_cblk * cb;
                void ** anon;
        } lk;

        lk.anon = lock;
        rc = XWOK;
        switch (lktype) {
        case XWOS_LK_MTX:
        case XWOS_LK_MTX_UNINTR:
                rc = xwmp_mtx_unlock(lk.mtx);
                break;
        case XWOS_LK_SPLK:
                xwmp_splk_unlock(lk.sp);
                break;
        case XWOS_LK_SQLK_WR:
                xwmp_sqlk_wr_unlock(lk.sq);
                break;
        case XWOS_LK_SQLK_RDEX:
                xwmp_sqlk_rdex_unlock(lk.sq);
                break;
        case XWOS_LK_CALLBACK:
                if (lk.cb->unlock) {
                        rc = lk.cb->unlock(lkdata);
                }
                break;
        default:
                break;
        }
        return rc;
}

xwer_t xwmp_thd_do_lock(void * lock, xwid_t lktype, xwtm_t * xwtm, void * lkdata)
{
        xwer_t rc;
        union {
                struct xwmp_mtx * mtx;
                struct xwmp_splk * sp;
                struct xwmp_sqlk * sq;
                struct xwos_cblk * cb;
                void ** anon;
        } lk;

        lk.anon = lock;
        rc = XWOK;
        switch (lktype) {
        case XWOS_LK_MTX:
                if (xwtm) {
                        rc = xwmp_mtx_timedlock(lk.mtx, xwtm);
                } else {
                        rc = xwmp_mtx_lock(lk.mtx);
                }
                break;
        case XWOS_LK_MTX_UNINTR:
                rc = xwmp_mtx_lock_unintr(lk.mtx);
                break;
        case XWOS_LK_SPLK:
                xwmp_splk_lock(lk.sp);
                break;
        case XWOS_LK_SQLK_WR:
                xwmp_sqlk_wr_lock(lk.sq);
                break;
        case XWOS_LK_SQLK_RDEX:
                xwmp_sqlk_rdex_lock(lk.sq);
                break;
        case XWOS_LK_CALLBACK:
                if (lk.cb->lock) {
                        rc = lk.cb->lock(lkdata);
                }
                break;
        default:
                break;
        }
        return rc;
}

xwer_t xwmp_cthd_sleep(xwtm_t * xwtm)
{
        xwer_t rc;
        ktime_t expires;
        ktime_t * kt;
        struct hrtimer_sleeper hrts;

        kt = (ktime_t *)xwtm;
        set_current_state(TASK_INTERRUPTIBLE);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)
        hrtimer_init_sleeper_on_stack(&hrts, HRTIMER_BASE_MONOTONIC, HRTIMER_MODE_ABS);
        expires = ktime_add_safe(*kt, hrts.timer.base->get_time());
        hrtimer_set_expires_range_ns(&hrts.timer, expires, 0);
        hrtimer_sleeper_start_expires(&hrts, HRTIMER_MODE_ABS);
#else
        hrtimer_init_on_stack(&hrts.timer, HRTIMER_BASE_MONOTONIC, HRTIMER_MODE_ABS);
        expires = ktime_add_safe(*kt, hrts.timer.base->get_time());
        hrtimer_init_sleeper(&hrts, current);
        hrtimer_set_expires_range_ns(&hrts.timer, expires, 0);
        hrtimer_start_expires(&hrts.timer, HRTIMER_MODE_ABS);
#endif
        if (__xwcc_likely(hrts.task)) {
                schedule();
        }
        hrtimer_cancel(&hrts.timer);
        rc = !hrts.task ? 0 : -EINTR;
        *kt = ktime_sub(expires, hrts.timer.base->get_time());
        destroy_hrtimer_on_stack(&hrts.timer);

        set_current_state(TASK_RUNNING);
        linux_thd_clear_fake_signal(current);
        return rc;
}
EXPORT_SYMBOL(xwmp_cthd_sleep);

xwer_t xwmp_cthd_sleep_from(xwtm_t * origin, xwtm_t inc)
{
        xwer_t rc;
        ktime_t expires;
        ktime_t * ktorigin;
        ktime_t ktinc;
        struct hrtimer_sleeper hrts;

        ktorigin = (ktime_t *)origin;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,10,0)
        ktinc = (s64)inc;
#else
        ktinc.tv64 = (s64)inc;
#endif

        set_current_state(TASK_INTERRUPTIBLE);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)
        hrtimer_init_sleeper_on_stack(&hrts, HRTIMER_BASE_MONOTONIC, HRTIMER_MODE_ABS);
        expires = ktime_add_safe(*ktorigin, ktinc);
        hrtimer_set_expires_range_ns(&hrts.timer, expires, 0);
        hrtimer_sleeper_start_expires(&hrts, HRTIMER_MODE_ABS);
#else
        hrtimer_init_on_stack(&hrts.timer, HRTIMER_BASE_MONOTONIC, HRTIMER_MODE_ABS);
        expires = ktime_add_safe(*ktorigin, ktinc);
        hrtimer_set_expires_range_ns(&hrts.timer, expires, 0);
        hrtimer_init_sleeper(&hrts, current);
        hrtimer_start_expires(&hrts.timer, HRTIMER_MODE_ABS);
#endif
        if (likely(hrts.task)) {
                schedule();
        }
        hrtimer_cancel(&hrts.timer);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,10,0)
        *origin = (xwtm_t)hrts.timer.base->get_time();
#else
        *origin = (xwtm_t)hrts.timer.base->get_time().tv64;
#endif
        rc = !hrts.task ? 0 : -EINTR;
        destroy_hrtimer_on_stack(&hrts.timer);
        set_current_state(TASK_RUNNING);
        linux_thd_clear_fake_signal(current);
        return rc;
}
EXPORT_SYMBOL(xwmp_cthd_sleep_from);

xwer_t xwmp_cthd_freeze(void)
{
        bool frozen;
        xwer_t rc;

        frozen = __refrigerator(true);
        if (frozen) {
                rc = XWOK;
        } else {
                rc = -EPERM;
        }
        return rc;
}
EXPORT_SYMBOL(xwmp_cthd_freeze);

struct sighand_struct * linux_thd_lock_task_sighand(struct xwmp_thd * thd,
                                                    xwreg_t * flag)
{
	struct sighand_struct * ret;

        ret = KSYM_CALL(__lock_task_sighand, thd, (unsigned long *)flag);
        (void)__cond_lock(&thd->sighand->siglock, ret);
        return ret;
}
EXPORT_SYMBOL(linux_thd_lock_task_sighand);

void linux_thd_unlock_task_sighand(struct xwmp_thd * thd, xwreg_t flag)
{
        spin_unlock_irqrestore(&thd->sighand->siglock, (unsigned long)flag);
}
EXPORT_SYMBOL(linux_thd_unlock_task_sighand);

void linux_thd_signal_wake_up_state(struct xwmp_thd * thd, unsigned int state)
{
        KSYM_CALL(signal_wake_up_state, thd, state);
}
EXPORT_SYMBOL(linux_thd_signal_wake_up_state);

void linux_thd_fake_signal_wake_up(struct xwmp_thd * thd)
{
        xwreg_t flag;

        if (linux_thd_lock_task_sighand(thd, &flag)) {
                linux_thd_signal_wake_up_state(thd, 0);
                linux_thd_unlock_task_sighand(thd, flag);
        }
}
EXPORT_SYMBOL(linux_thd_fake_signal_wake_up);

void linux_thd_fake_signal(struct xwmp_thd * thd)
{
        xwreg_t flag;

        if (linux_thd_lock_task_sighand(thd, &flag)) {
                set_tsk_thread_flag(thd, TIF_SIGPENDING);
                linux_thd_unlock_task_sighand(thd, flag);
        }
}
EXPORT_SYMBOL(linux_thd_fake_signal);

void linux_thd_clear_fake_signal(struct xwmp_thd * thd)
{
        xwreg_t flag;

        if (PF_KTHREAD & thd->flags) {
                if (linux_thd_lock_task_sighand(thd, &flag)) {
                        clear_tsk_thread_flag(thd, TIF_SIGPENDING);
                        linux_thd_unlock_task_sighand(thd, flag);
                }
        }
}
EXPORT_SYMBOL(linux_thd_clear_fake_signal);

xwer_t linux_thd_ksym_load(void)
{
        xwer_t rc;

        rc = KSYM_LOAD(__lock_task_sighand);
        if (__xwcc_unlikely(rc < 0)) {
                xwlogf(ERR, LOGTAG,
                       "Load kernel symbol __lock_task_sighand ... errno:%d\n",
                       rc);
                goto err_ksymload;
        }
        rc = KSYM_LOAD(signal_wake_up_state);
        if (__xwcc_unlikely(rc < 0)) {
                xwlogf(ERR, LOGTAG,
                       "Load kernel symbol signal_wake_up_state ... errno: %d\n",
                       rc);
                goto err_ksymload;
        }
        rc = KSYM_LOAD(find_task_by_vpid);
        if (__xwcc_unlikely(rc < 0)) {
                xwlogf(ERR, LOGTAG,
                       "Load kernel symbol find_task_by_vpid ... errno: %d\n",
                       rc);
                goto err_ksymload;
        }
        rc = KSYM_LOAD(sched_setscheduler);
        if (__xwcc_unlikely(rc < 0)) {
                xwlogf(ERR, LOGTAG,
                       "Load kernel symbol sched_setscheduler ... errno: %d\n",
                       rc);
                goto err_ksymload;
        }

        return XWOK;

err_ksymload:
        return rc;
}
