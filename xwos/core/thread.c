/**
 * @file
 * @brief XuanWuOS内核：线程
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwlog.h>
#include <linux/sched.h>
#include <linux/freezer.h>
#include <linux/hrtimer.h>
#include <linux/jiffies.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0)
  #include <linux/sched/rt.h>
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
  #include <linux/sched/signal.h>
  #include <uapi/linux/sched/types.h>
#endif
#include <linux/preempt.h>
#include <linux/kthread.h>

#include <xwos/core/ksym.h>
#include <xwos/core/scheduler.h>
#include <xwos/core/thread.h>
#include <xwos/core/pm.h>
#include <xwos/lock/spinlock.h>
#include <xwos/lock/seqlock.h>
#include <xwos/lock/mutex.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
KSYM_DCLR(__lock_task_sighand);
KSYM_DCLR(signal_wake_up_state);
KSYM_DCLR(find_task_by_vpid);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t xwos_thrd_grab(struct xwos_tcb * tcb)
{
        get_task_struct(tcb);
        return XWOK;
}
EXPORT_SYMBOL(xwos_thrd_grab);

xwer_t xwos_thrd_put(struct xwos_tcb * tcb)
{
        put_task_struct(tcb);
        return XWOK;
}
EXPORT_SYMBOL(xwos_thrd_put);

xwer_t xwos_thrd_create(xwid_t *tidbuf, const char *name,
                        xwos_thrd_f mainfunc, void *arg,
                        xwsz_t stack_size,
                        xwpr_t priority, xwsq_t attr)
{
        struct task_struct *ts;
        struct sched_param schparam;
        xwer_t rc;

        (void)stack_size;
        (void)attr;
        ts = kthread_create(mainfunc, arg, name);
        if (is_err(ts)) {
                rc = ptr_err(ts);
                goto err_kthread_create;
        }
        ts->flags &= ~PF_NOFREEZE;
        schparam.sched_priority = priority;
        sched_setscheduler(ts, SCHED_FIFO, &schparam);
        wake_up_process(ts);
        *tidbuf = (xwid_t)task_pid_vnr(ts);
        return XWOK;

err_kthread_create:
        return rc;
}
EXPORT_SYMBOL(xwos_thrd_create);

xwer_t xwos_thrd_delete(xwid_t tid)
{
        XWOS_UNUSED(tid);
        return XWOK;
}
EXPORT_SYMBOL(xwos_thrd_delete);

void xwos_cthrd_yield(void)
{
        yield();
}
EXPORT_SYMBOL(xwos_cthrd_yield);

void xwos_cthrd_exit(xwer_t rc)
{
        do_exit((long)rc);
}
EXPORT_SYMBOL(xwos_cthrd_exit);

xwer_t xwos_thrd_terminate(xwid_t tid, xwer_t * trc)
{
        struct xwos_tcb * tcb;
        xwer_t rc;

        rcu_read_lock();
        tcb = KSYM_CALL(find_task_by_vpid, tid);
        if (tcb) {
                xwos_thrd_grab(tcb);
                rcu_read_unlock();
                linux_thrd_fake_signal(tcb);
                rc = (xwer_t)kthread_stop(tcb);
                xwos_thrd_put(tcb);
                if (trc) {
                        *trc = rc;
                }
        } else {
                rcu_read_unlock();
                rc = -ESTALE;
        }
        return XWOK;
}
EXPORT_SYMBOL(xwos_thrd_terminate);

void xwos_cthrd_wait_exit(void)
{
        while (true) {
                if (kthread_should_stop()) {
                        break;
                }
                set_current_state(TASK_UNINTERRUPTIBLE);
                schedule();
                set_current_state(TASK_RUNNING);
        }
}
EXPORT_SYMBOL(xwos_cthrd_wait_exit);

bool xwos_cthrd_shld_stop(void)
{
        return kthread_should_stop();
}
EXPORT_SYMBOL(xwos_cthrd_shld_stop);

bool xwos_cthrd_shld_frz(void)
{
        return freezing(current) || xwos_pm_tst_lpm();
}
EXPORT_SYMBOL(xwos_cthrd_shld_frz);

bool xwos_cthrd_frz_shld_stop(bool * frozen)
{
        xwer_t rc;

        rc = -EPERM;
        if (xwos_cthrd_shld_frz()) {
                rc = xwos_cthrd_freeze();
        }
        if (frozen) {
                if (XWOK == rc) {
                        *frozen = true;
                } else {
                        *frozen = false;
                }
        }
        return xwos_cthrd_shld_stop();
}
EXPORT_SYMBOL(xwos_cthrd_frz_shld_stop);

xwid_t xwos_cthrd_get_id(void)
{
        return (xwid_t)task_pid_vnr(current);
}
EXPORT_SYMBOL(xwos_cthrd_get_id);

xwid_t xwos_thrd_get_id(struct xwos_tcb * tcb)
{
        return (xwid_t)task_pid_vnr(tcb);
}
EXPORT_SYMBOL(xwos_thrd_get_id);

xwer_t xwos_thrd_do_unlock(void * lock, xwid_t lktype, void * lkdata)
{
        xwer_t rc;
        union {
                struct xwlk_mtx * mtx;
                struct xwlk_splk * sp;
                struct xwlk_sqlk * sq;
                struct xwlk_cblk * cb;
                void ** anon;
        } lk;

        lk.anon = lock;
        rc = XWOK;
        switch (lktype) {
        case XWLK_TYPE_MTX:
        case XWLK_TYPE_MTX_UNINTR:
                rc = xwlk_mtx_unlock(lk.mtx);
                break;
        case XWLK_TYPE_SPLK:
                xwlk_splk_unlock(lk.sp);
                break;
        case XWLK_TYPE_SQLK_WR:
                xwlk_sqlk_wr_unlock(lk.sq);
                break;
        case XWLK_TYPE_SQLK_RDEX:
                xwlk_sqlk_rdex_unlock(lk.sq);
                break;
        case XWLK_TYPE_CALLBACK:
                if (lk.cb->unlock) {
                        rc = lk.cb->unlock(lkdata);
                }
                break;
        default:
                break;
        }
        return rc;
}

xwer_t xwos_thrd_do_lock(void * lock, xwid_t lktype, xwtm_t * xwtm, void * lkdata)
{
        xwer_t rc;
        union {
                struct xwlk_mtx * mtx;
                struct xwlk_splk * sp;
                struct xwlk_sqlk * sq;
                struct xwlk_cblk * cb;
                void ** anon;
        } lk;

        lk.anon = lock;
        rc = XWOK;
        switch (lktype) {
        case XWLK_TYPE_MTX:
                if (xwtm) {
                        rc = xwlk_mtx_timedlock(lk.mtx, xwtm);
                } else {
                        rc = xwlk_mtx_lock(lk.mtx);
                }
                break;
        case XWLK_TYPE_MTX_UNINTR:
                rc = xwlk_mtx_lock_unintr(lk.mtx);
                break;
        case XWLK_TYPE_SPLK:
                xwlk_splk_lock(lk.sp);
                break;
        case XWLK_TYPE_SQLK_WR:
                xwlk_sqlk_wr_lock(lk.sq);
                break;
        case XWLK_TYPE_SQLK_RDEX:
                xwlk_sqlk_rdex_lock(lk.sq);
                break;
        case XWLK_TYPE_CALLBACK:
                if (lk.cb->lock) {
                        rc = lk.cb->lock(lkdata);
                }
                break;
        default:
                break;
        }
        return rc;
}

xwer_t xwos_cthrd_sleep(xwtm_t * xwtm)
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
        if (__likely(hrts.task)) {
                schedule();
        }
        hrtimer_cancel(&hrts.timer);
        rc = !hrts.task ? 0 : -EINTR;
        *kt = ktime_sub(expires, hrts.timer.base->get_time());
        destroy_hrtimer_on_stack(&hrts.timer);

        set_current_state(TASK_RUNNING);
        linux_thrd_clear_fake_signal(current);
        return rc;
}
EXPORT_SYMBOL(xwos_cthrd_sleep);

xwer_t xwos_cthrd_sleep_from(xwtm_t * origin, xwtm_t inc)
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
        linux_thrd_clear_fake_signal(current);
        return rc;
}
EXPORT_SYMBOL(xwos_cthrd_sleep_from);

xwer_t xwos_cthrd_freeze(void)
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
EXPORT_SYMBOL(xwos_cthrd_freeze);

xwer_t xwos_thrd_intr(xwid_t tid)
{
        struct xwos_tcb * tcb;
        xwer_t rc;

        rc = -EPERM;
        rcu_read_lock();
        tcb = KSYM_CALL(find_task_by_vpid, tid);
        if (tcb) {
                linux_thrd_fake_signal_wake_up(tcb);
                rc = XWOK;
        }
        rcu_read_unlock();
        return rc;
}
EXPORT_SYMBOL(xwos_thrd_intr);

xwer_t xwos_thrd_get_tcb_by_tid(xwid_t tid, struct xwos_tcb ** ptrbuf)
{
        struct xwos_tcb * tcb;
        xwer_t rc;

        rcu_read_lock();
        tcb = KSYM_CALL(find_task_by_vpid, tid);
        if (tcb) {
                xwos_thrd_grab(tcb);
                rcu_read_unlock();
                *ptrbuf = tcb;
                rc = XWOK;
        } else {
                rcu_read_unlock();
                rc = -ESTALE;
        }
        return rc;
}
EXPORT_SYMBOL(xwos_thrd_get_tcb_by_tid);

struct sighand_struct * linux_thrd_lock_task_sighand(struct xwos_tcb * tcb,
                                                     xwreg_t * flag)
{
	struct sighand_struct * ret;

        ret = KSYM_CALL(__lock_task_sighand, tcb, (unsigned long *)flag);
        (void)__cond_lock(&tcb->sighand->siglock, ret);
        return ret;
}
EXPORT_SYMBOL(linux_thrd_lock_task_sighand);

void linux_thrd_unlock_task_sighand(struct xwos_tcb * tcb, xwreg_t flag)
{
        spin_unlock_irqrestore(&tcb->sighand->siglock, (unsigned long)flag);
}
EXPORT_SYMBOL(linux_thrd_unlock_task_sighand);

void linux_thrd_signal_wake_up_state(struct xwos_tcb * tcb, unsigned int state)
{
        KSYM_CALL(signal_wake_up_state, tcb, state);
}
EXPORT_SYMBOL(linux_thrd_signal_wake_up_state);

void linux_thrd_fake_signal_wake_up(struct xwos_tcb * tcb)
{
        xwreg_t flag;

        if (linux_thrd_lock_task_sighand(tcb, &flag)) {
                linux_thrd_signal_wake_up_state(tcb, 0);
                linux_thrd_unlock_task_sighand(tcb, flag);
        }
}
EXPORT_SYMBOL(linux_thrd_fake_signal_wake_up);

void linux_thrd_fake_signal(struct xwos_tcb * tcb)
{
        xwreg_t flag;

        if (linux_thrd_lock_task_sighand(tcb, &flag)) {
                set_tsk_thread_flag(tcb, TIF_SIGPENDING);
                linux_thrd_unlock_task_sighand(tcb, flag);
        }
}
EXPORT_SYMBOL(linux_thrd_fake_signal);

void linux_thrd_clear_fake_signal(struct xwos_tcb * tcb)
{
        xwreg_t flag;

        if (PF_KTHREAD & tcb->flags) {
                if (linux_thrd_lock_task_sighand(tcb, &flag)) {
                        clear_tsk_thread_flag(tcb, TIF_SIGPENDING);
                        linux_thrd_unlock_task_sighand(tcb, flag);
                }
        }
}
EXPORT_SYMBOL(linux_thrd_clear_fake_signal);

xwer_t linux_thrd_ksym_load(void)
{
        xwer_t rc;

        rc = KSYM_LOAD(__lock_task_sighand);
        if (__unlikely(rc < 0)) {
                xwoslogf(ERR,
                         "Failed to load kernel symbol: __lock_task_sighand. rc: %d\n",
                         rc);
                goto err_ksymload;
        }
        rc = KSYM_LOAD(signal_wake_up_state);
        if (__unlikely(rc < 0)) {
                xwoslogf(ERR,
                         "Failed to load kernel symbol: signal_wake_up_state. rc: %d\n",
                         rc);
                goto err_ksymload;
        }
        rc = KSYM_LOAD(find_task_by_vpid);
        if (__unlikely(rc < 0)) {
                xwoslogf(ERR,
                         "Failed to load kernel symbol: find_task_by_vpid. rc: %d\n",
                         rc);
                goto err_ksymload;
        }

        return XWOK;

err_ksymload:
        return rc;
}

xwer_t linux_thrd_ksym_unload(void)
{
        return XWOK;
}
