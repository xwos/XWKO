/**
 * @file
 * @brief 玄武OS的同步机制：信号量
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

#include <xwos/standard.h>
#include <linux/slab.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
#  include <linux/sched/signal.h>
#endif
#include <linux/sched/rt.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwlog.h>
#include <xwos/lib/bclst.h>
#include <xwos/mm/common.h>
#include <xwos/mp/thd.h>
#include <xwos/mp/pm.h>
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/sync/obj.h>
#include <xwos/mp/sync/evt.h>
#include <xwos/mp/sync/sem.h>

#define LOGTAG "sem"

enum xwmp_sem_waiter_flag_em {
        XWMP_SEM_F_UP = BIT(0), /**< wakeup flag */
        XWMP_SEM_F_INT = BIT(1), /**< interrupt flag */
};

struct xwmp_sem_waiter {
        struct xwlib_bclst_node node;
        struct task_struct * task;
        volatile xwsq_t flags;
};

#define XWMP_SEM_NEGATIVE     (-1L)

static
xwer_t xwmp_sem_gc(void * obj);

static
void xwmp_sem_construct(void * data);

static
xwer_t xwmp_sem_activate(struct xwmp_sem * sem, xwssq_t val, xwssq_t max,
                         xwobj_gc_f gcfunc);

struct kmem_cache * xwmp_sem_cache;

static
void xwmp_sem_construct(void * data)
{
        struct xwmp_sem * sem = data;

        xwmp_synobj_construct(&sem->synobj);
        sem->count = 0;
        sem->max = 0;
        xwlib_bclst_init_head(&sem->wq);
        xwmp_splk_init(&sem->lock);
}

static
xwer_t xwmp_sem_gc(void * obj)
{
        kmem_cache_free(xwmp_sem_cache, obj);
        return XWOK;
}

xwer_t xwmp_sem_cache_create(void)
{
        xwer_t rc = XWOK;

        xwmp_sem_cache = kmem_cache_create("xwmp_sem_slab",
                                           sizeof(struct xwmp_sem),
                                           XWMM_ALIGNMENT, SLAB_PANIC,
                                           xwmp_sem_construct);
        if (is_err_or_null(xwmp_sem_cache)) {
                rc = -ENOMEM;
                xwlogf(ERR, LOGTAG, "Can't create sem slab ... errno:%d\n", rc);
                goto err_slab_create;
        }

        xwlogf(INFO, LOGTAG, "Create sem slab ... [OK]\n");
        return XWOK;

err_slab_create:
        return rc;
}

void xwmp_sem_cache_destroy(void)
{
        kmem_cache_destroy(xwmp_sem_cache);
}

static
xwer_t xwmp_sem_activate(struct xwmp_sem * sem, xwssq_t val,
                         xwssq_t max, xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwmp_synobj_activate(&sem->synobj, gcfunc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_synobj_activate;
        }
        sem->count = val;
        sem->max = max;
        return XWOK;

err_synobj_activate:
        return rc;
}

xwer_t xwmp_sem_init(struct xwmp_sem * sem, xwssq_t val, xwssq_t max)
{
        xwer_t rc;

        xwmp_sem_construct(sem);
        rc = xwmp_sem_activate(sem, val, max, NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_activate;
        }
        return XWOK;

err_sem_activate:
        return rc;
}
EXPORT_SYMBOL(xwmp_sem_init);

xwer_t xwmp_sem_fini(struct xwmp_sem * sem)
{
        return xwmp_sem_put(sem);
}
EXPORT_SYMBOL(xwmp_sem_fini);

xwer_t xwmp_sem_create(struct xwmp_sem ** ptrbuf, xwssq_t val, xwssq_t max)
{
        struct xwmp_sem * sem;
        xwer_t rc;

        if (__xwcc_unlikely((val < 0) || (max < 0) || (val > max))) {
                rc = -EINVAL;
                goto err_invald_args;
        }
        sem = kmem_cache_alloc(xwmp_sem_cache, GFP_KERNEL);
        if (__xwcc_unlikely(is_err_or_null(sem))) {
                rc = -ENOMEM;
                goto err_sem_alloc;
        }
        rc = xwmp_sem_activate(sem, val, max, xwmp_sem_gc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_activate;
        }
        *ptrbuf = sem;
        return XWOK;

err_sem_activate:
        kmem_cache_free(xwmp_sem_cache, sem);
err_sem_alloc:
err_invald_args:
        return rc;
}
EXPORT_SYMBOL(xwmp_sem_create);

xwer_t xwmp_sem_delete(struct xwmp_sem * sem)
{
        return xwmp_sem_put(sem);
}
EXPORT_SYMBOL(xwmp_sem_delete);

xwer_t xwmp_sem_acquire(struct xwmp_sem * sem, xwsq_t tik)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        return xwmp_synobj_acquire(&sem->synobj, tik);
}
EXPORT_SYMBOL(xwmp_sem_acquire);

xwer_t xwmp_sem_release(struct xwmp_sem * sem, xwsq_t tik)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        return xwmp_synobj_release(&sem->synobj, tik);
}
EXPORT_SYMBOL(xwmp_sem_release);

xwer_t xwmp_sem_grab(struct xwmp_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        return xwmp_synobj_grab(&sem->synobj);
}
EXPORT_SYMBOL(xwmp_sem_grab);

xwer_t xwmp_sem_put(struct xwmp_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        return xwmp_synobj_put(&sem->synobj);
}
EXPORT_SYMBOL(xwmp_sem_put);

xwsq_t xwmp_sem_gettik(struct xwmp_sem * sem)
{
        xwsq_t tik;

        if (sem) {
                tik = sem->synobj.xwobj.tik;
        } else {
                tik = 0;
        }
        return tik;
}
EXPORT_SYMBOL(xwmp_sem_gettik);

xwer_t xwmp_sem_bind(struct xwmp_sem * sem, struct xwmp_evt * evt, xwsq_t pos)
{
        struct xwmp_synobj * synobj;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        synobj = &sem->synobj;
        xwmp_splk_lock_cpuirqsv(&sem->lock, &cpuirq);
        rc = xwmp_sel_obj_bind(evt, synobj, pos, true);
        if ((XWOK == rc) && (sem->count > 0)) {
                rc = xwmp_sel_obj_s1i(evt, synobj);
        }
        xwmp_splk_unlock_cpuirqrs(&sem->lock, cpuirq);
        return rc;
}
EXPORT_SYMBOL(xwmp_sem_bind);

xwer_t xwmp_sem_unbind(struct xwmp_sem * sem, struct xwmp_evt * evt)
{
        struct xwmp_synobj * synobj;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        synobj = &sem->synobj;
        xwmp_splk_lock_cpuirqsv(&sem->lock, &cpuirq);
        rc = xwmp_sel_obj_unbind(evt, synobj, true);
        if (XWOK == rc) {
                rc = xwmp_sel_obj_c0i(evt, synobj);
        }
        xwmp_splk_unlock_cpuirqrs(&sem->lock, cpuirq);

        return rc;
}
EXPORT_SYMBOL(xwmp_sem_unbind);

xwer_t xwmp_sem_freeze(struct xwmp_sem * sem)
{
        struct xwmp_synobj * synobj;
        struct xwmp_evt * evt;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwmp_sem_grab(sem);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
        } else {
                synobj = &sem->synobj;
                xwmp_splk_lock_cpuirqsv(&sem->lock, &cpuirq);
                if (__xwcc_unlikely(sem->count < 0)) {
                        rc = -EALREADY;
                } else {
                        sem->count = XWMP_SEM_NEGATIVE;
                        xwmb_mp_load_acquire(struct xwmp_evt *, evt, &synobj->sel.evt);
                        if (NULL != evt) {
                                xwmp_sel_obj_c0i(evt, synobj);
                        }
                }
                xwmp_splk_unlock_cpuirqrs(&sem->lock, cpuirq);
                xwmp_sem_put(sem);
        }
        return rc;
}
EXPORT_SYMBOL(xwmp_sem_freeze);

xwer_t xwmp_sem_thaw(struct xwmp_sem * sem)
{
        struct xwmp_synobj * synobj;
        struct xwmp_evt * evt;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwmp_sem_grab(sem);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EINVAL;
                goto err_sem_grab;
        }

        synobj = &sem->synobj;
        xwmp_splk_lock_cpuirqsv(&sem->lock, &cpuirq);
        if (__xwcc_unlikely(sem->count >= 0)) {
                rc = -EPERM;
                goto err_not_neg;
        }
        xwlib_bclst_init_head(&sem->wq);
        sem->count = 0;
        if (sem->count > 0) {
                xwmb_mp_load_acquire(struct xwmp_evt *, evt, &synobj->sel.evt);
                if (NULL != evt) {
                        xwmp_sel_obj_s1i(evt, synobj);
                }
        }
        xwmp_splk_unlock_cpuirqrs(&sem->lock, cpuirq);
        xwmp_sem_put(sem);
        return XWOK;

err_not_neg:
        xwmp_splk_unlock_cpuirqrs(&sem->lock, cpuirq);
        xwmp_sem_put(sem);
err_sem_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_sem_thaw);

xwer_t xwmp_sem_post(struct xwmp_sem * sem)
{
        struct xwmp_synobj * synobj;
        struct xwmp_evt * evt;
        struct xwmp_sem_waiter * waiter;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwmp_sem_grab(sem);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_sem_grab;
        }

        synobj = &sem->synobj;
        xwmp_splk_lock_cpuirqsv(&sem->lock, &cpuirq);
        if (sem->count < 0) {
                rc = -ENEGATIVE;
                goto err_sem_neg;
        } else if (__xwcc_likely(xwlib_bclst_tst_empty(&sem->wq))) {
                if (sem->count >= sem->max) {
                        rc = -ERANGE;
                        goto err_sem_range;
                } else {
                        sem->count++;
                }
                if (sem->count > 0) {
                        xwmb_mp_load_acquire(struct xwmp_evt *, evt, &synobj->sel.evt);
                        if (NULL != evt) {
                                xwmp_sel_obj_s1i(evt, synobj);
                        }
                }
        } else {
                waiter = xwlib_bclst_first_entry(&sem->wq,
                                                 struct xwmp_sem_waiter,
                                                 node);
                xwlib_bclst_del_init(&waiter->node);
                waiter->flags = XWMP_SEM_F_UP;
                wake_up_process(waiter->task);
        }
        xwmp_splk_unlock_cpuirqrs(&sem->lock, cpuirq);
        xwmp_sem_put(sem);
        return XWOK;

err_sem_range:
err_sem_neg:
        xwmp_splk_unlock_cpuirqrs(&sem->lock, cpuirq);
        xwmp_sem_put(sem);
err_sem_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_sem_post);

xwer_t xwmp_sem_wait(struct xwmp_sem * sem)
{
        struct xwmp_synobj * synobj;
        struct xwmp_evt * evt;
        struct xwmp_sem_waiter waiter;
        struct task_struct * task;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwmp_sem_grab(sem);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_sem_grab;
        }

        synobj = &sem->synobj;
        xwmp_splk_lock_cpuirqsv(&sem->lock, &cpuirq);
        if (likely(sem->count > 0)) {
                sem->count--;
                rc = XWOK;
                if (0 == sem->count) {
                        xwmb_mp_load_acquire(struct xwmp_evt *, evt, &synobj->sel.evt);
                        if (NULL != evt) {
                                xwmp_sel_obj_c0i(evt, synobj);
                        }
                }
        } else {
                task = current;
                xwlib_bclst_init_node(&waiter.node);
                xwlib_bclst_add_tail(&sem->wq, &waiter.node);
                waiter.task = task;
                waiter.flags = 0;
                for (;;) {
                        if (signal_pending_state(TASK_INTERRUPTIBLE, task)) {
                                xwlib_bclst_del_init(&waiter.node);
                                linux_thd_clear_fake_signal(current);
                                rc = -EINTR;
                                break;
                        } else if (xwmp_pm_tst_lpm()) {
                                xwlib_bclst_del_init(&waiter.node);
                                rc = -EINTR;
                                break;
                        }
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
                        __set_task_state(task, TASK_INTERRUPTIBLE);
#else
                        __set_current_state(TASK_INTERRUPTIBLE);
#endif
                        xwmp_splk_unlock_cpuirq(&sem->lock);
                        schedule();
                        xwmp_splk_lock_cpuirq(&sem->lock);
                        if (XWMP_SEM_F_UP == waiter.flags) {
                                rc = XWOK;
                                break;
                        }
                }
        }
        xwmp_splk_unlock_cpuirqrs(&sem->lock, cpuirq);
        xwmp_sem_put(sem);

err_sem_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_sem_wait);

xwer_t xwmp_sem_trywait(struct xwmp_sem * sem)
{
        xwer_t rc;
        xwreg_t cpuirq;
        struct xwmp_synobj * synobj;
        struct xwmp_evt * evt;

        rc = xwmp_sem_grab(sem);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_sem_grab;
        }

        synobj = &sem->synobj;
        xwmp_splk_lock_cpuirqsv(&sem->lock, &cpuirq);
        if (likely(sem->count > 0)) {
                sem->count--;
                rc = XWOK;
                if (0 == sem->count) {
                        xwmb_mp_load_acquire(struct xwmp_evt *, evt, &synobj->sel.evt);
                        if (NULL != evt) {
                                xwmp_sel_obj_c0i(evt, synobj);
                        }
                }
        } else {
                rc = -ENODATA;
        }
        xwmp_splk_unlock_cpuirqrs(&sem->lock, cpuirq);
        xwmp_sem_put(sem);

err_sem_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_sem_trywait);

xwer_t xwmp_sem_timedwait(struct xwmp_sem * sem, xwtm_t * xwtm)
{
        xwer_t rc;
        xwreg_t cpuirq;
        struct xwmp_sem_waiter waiter;
        ktime_t expires;
        ktime_t * kt;
        struct xwmp_synobj * synobj;
        struct xwmp_evt * evt;
        struct hrtimer_sleeper hrts;
        struct task_struct * task;
        unsigned long slack;

        kt = (ktime_t *)xwtm;

        rc = xwmp_sem_grab(sem);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_sem_grab;
        }

        synobj = &sem->synobj;
        xwmp_splk_lock_cpuirqsv(&sem->lock, &cpuirq);
        if (__xwcc_likely(sem->count > 0)) {
                sem->count--;
                rc = XWOK;
                if (0 == sem->count) {
                        xwmb_mp_load_acquire(struct xwmp_evt *, evt, &synobj->sel.evt);
                        if (NULL != evt) {
                                xwmp_sel_obj_c0i(evt, synobj);
                        }
                }
        } else if (0 == *xwtm) {
                rc = -ETIMEDOUT;
        } else {
                task = current;
                slack = task->timer_slack_ns;
                if (rt_task(task)) {
                        slack = 0;
                }
                xwlib_bclst_init_node(&waiter.node);
                xwlib_bclst_add_tail(&sem->wq, &waiter.node);
                waiter.task = task;
                waiter.flags = 0;
                rc = XWOK;
                while (true) {
                        if (signal_pending_state(TASK_INTERRUPTIBLE, task)) {
                                xwlib_bclst_del_init(&waiter.node);
                                linux_thd_clear_fake_signal(current);
                                rc = -EINTR;
                                break;
                        } else if (xwmp_pm_tst_lpm()) {
                                xwlib_bclst_del_init(&waiter.node);
                                rc = -EINTR;
                                break;
                        } else if (__xwcc_unlikely(-ETIMEDOUT == rc)) {
                                xwlib_bclst_del_init(&waiter.node);
                                break;
                        }
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
                        __set_task_state(task, TASK_INTERRUPTIBLE);
#else
                        __set_current_state(TASK_INTERRUPTIBLE);
#endif
                        xwmp_splk_unlock_cpuirq(&sem->lock);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)
                        hrtimer_init_sleeper_on_stack(&hrts,
                                                      HRTIMER_BASE_MONOTONIC,
                                                      HRTIMER_MODE_ABS);
                        expires = ktime_add_safe(*kt,
                                                 hrts.timer.base->get_time());
                        hrtimer_set_expires_range_ns(&hrts.timer, expires, slack);
                        hrtimer_sleeper_start_expires(&hrts, HRTIMER_MODE_ABS);
#else
                        hrtimer_init_on_stack(&hrts.timer,
                                              HRTIMER_BASE_MONOTONIC,
                                              HRTIMER_MODE_ABS);
                        expires = ktime_add_safe(*kt,
                                                 hrts.timer.base->get_time());
                        hrtimer_set_expires_range_ns(&hrts.timer, expires, slack);
                        hrtimer_init_sleeper(&hrts, task);
                        hrtimer_start_expires(&hrts.timer, HRTIMER_MODE_ABS);
#endif
                        if (__xwcc_likely(hrts.task)) {
                                schedule();
                        }
                        hrtimer_cancel(&hrts.timer);
                        rc = !hrts.task ? -ETIMEDOUT : XWOK;
                        *kt = ktime_sub(expires,
                                        hrts.timer.base->get_time());
                        destroy_hrtimer_on_stack(&hrts.timer);
                        xwmp_splk_lock_cpuirq(&sem->lock);
                        if (XWMP_SEM_F_UP == waiter.flags) {
                                rc = XWOK;
                                break;
                        }
                }
        }
        xwmp_splk_unlock_cpuirqrs(&sem->lock, cpuirq);
        xwmp_sem_put(sem);
err_sem_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_sem_timedwait);

xwer_t xwmp_sem_wait_unintr(struct xwmp_sem * sem)
{
        xwer_t rc;
        xwreg_t cpuirq;
        struct xwmp_synobj * synobj;
        struct xwmp_evt * evt;
        struct xwmp_sem_waiter waiter;
        struct task_struct * task;

        rc = xwmp_sem_grab(sem);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_sem_grab;
        }

        synobj = &sem->synobj;
        xwmp_splk_lock_cpuirqsv(&sem->lock, &cpuirq);
        if (likely(sem->count > 0)) {
                sem->count--;
                rc = XWOK;
                if (0 == sem->count) {
                        xwmb_mp_load_acquire(struct xwmp_evt *, evt, &synobj->sel.evt);
                        if (NULL != evt) {
                                xwmp_sel_obj_c0i(evt, synobj);
                        }
                }
        } else {
                task = current;
                xwlib_bclst_init_node(&waiter.node);
                xwlib_bclst_add_tail(&sem->wq, &waiter.node);
                waiter.task = task;
                waiter.flags = 0;
                for (;;) {
                        if (signal_pending_state(TASK_UNINTERRUPTIBLE, task)) {
                                xwlib_bclst_del_init(&waiter.node);
                                rc = -EINTR;
                                break;
                        }
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
                        __set_task_state(task, TASK_UNINTERRUPTIBLE);
#else
                        __set_current_state(TASK_UNINTERRUPTIBLE);
#endif
                        xwmp_splk_unlock_cpuirq(&sem->lock);
                        schedule();
                        xwmp_splk_lock_cpuirq(&sem->lock);
                        if (XWMP_SEM_F_UP == waiter.flags) {
                                rc = XWOK;
                                break;
                        }
                }
        }
        xwmp_splk_unlock_cpuirqrs(&sem->lock, cpuirq);
        xwmp_sem_put(sem);

err_sem_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_sem_wait_unintr);

xwer_t xwmp_sem_getvalue(struct xwmp_sem * sem, xwssq_t * sval)
{
        *sval = sem->count;
        return XWOK;
}
EXPORT_SYMBOL(xwmp_sem_getvalue);
