/**
 * @file
 * @brief 玄武OS的同步机制：条件量
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
#include <xwos/mp/sync/cond.h>

#define LOGTAG "cond"

enum xwmp_cond_waiter_flag_em {
        XWMP_COND_F_UP = BIT(0), /**< wakeup flag */
        XWMP_COND_F_INT = BIT(1), /**< interrupt flag */
};

struct xwmp_cond_waiter {
        struct xwlib_bclst_node node;
        struct task_struct * task;
        volatile xwsq_t flags;
};

#define XWMP_COND_NEGATIVE                (-1L)

static
xwer_t xwmp_cond_gc(void * obj);

static
xwer_t xwmp_cond_do_wait(struct xwmp_cond * cond,
                         void * lock, xwid_t lktype, void * lkdata,
                         xwsq_t * lkst);

static
xwer_t xwmp_cond_do_timedwait(struct xwmp_cond * cond,
                              void * lock, xwid_t lktype, void * lkdata,
                              xwtm_t * xwtm, xwsq_t * lkst);

struct kmem_cache * xwmp_cond_cache;

void xwmp_cond_construct(void * anon)
{
        struct xwmp_cond * cond = anon;

        xwmp_synobj_construct(&cond->synobj);
        cond->count = 0;
        xwlib_bclst_init_head(&cond->wq);
        xwmp_splk_init(&cond->lock);
}

static
xwer_t xwmp_cond_gc(void * obj)
{
        kmem_cache_free(xwmp_cond_cache, obj);
        return XWOK;
}

xwer_t xwmp_cond_cache_create(void)
{
        xwer_t rc = XWOK;

        xwmp_cond_cache = kmem_cache_create("xwmp_cond_slab",
                                            sizeof(struct xwmp_cond),
                                            XWMM_ALIGNMENT, SLAB_PANIC,
                                            xwmp_cond_construct);
        if (is_err_or_null(xwmp_cond_cache)) {
                rc = -ENOMEM;
                xwlogf(ERR, LOGTAG, "Can't create cond slab!\n");
                goto err_slab_create;
        }

        xwlogf(INFO, LOGTAG, "Create cond slab ... [OK]\n");
        return XWOK;

err_slab_create:
        return rc;
}

void xwmp_cond_cache_destroy(void)
{
        kmem_cache_destroy(xwmp_cond_cache);
}

xwer_t xwmp_cond_activate(struct xwmp_cond * cond, xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwmp_synobj_activate(&cond->synobj, gcfunc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_obj_activate;
        }
        cond->count = 0;
        return XWOK;

err_obj_activate:
        return rc;
}

xwer_t xwmp_cond_init(struct xwmp_cond * cond)
{
        xwmp_cond_construct(cond);
        return xwmp_cond_activate(cond, NULL);
}
EXPORT_SYMBOL(xwmp_cond_init);

xwer_t xwmp_cond_fini(struct xwmp_cond * cond)
{
        return xwmp_cond_put(cond);
}
EXPORT_SYMBOL(xwmp_cond_fini);

xwer_t xwmp_cond_create(struct xwmp_cond ** ptrbuf)
{
        xwer_t rc;
        struct xwmp_cond * cond;

        cond = kmem_cache_alloc(xwmp_cond_cache, GFP_KERNEL);
        if (__xwcc_unlikely(is_err_or_null(cond))) {
                rc = -ENOMEM;
                goto err_cond_alloc;
        }
        rc = xwmp_cond_activate(cond, xwmp_cond_gc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_cond_activate;
        }
        *ptrbuf = cond;
        return XWOK;

err_cond_activate:
        kmem_cache_free(xwmp_cond_cache, cond);
err_cond_alloc:
        return rc;
}
EXPORT_SYMBOL(xwmp_cond_create);

xwer_t xwmp_cond_delete(struct xwmp_cond * cond)
{
        return xwmp_cond_put(cond);
}
EXPORT_SYMBOL(xwmp_cond_delete);

xwer_t xwmp_cond_acquire(struct xwmp_cond * cond, xwsq_t tik)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);
        return xwmp_synobj_acquire(&cond->synobj, tik);
}

xwer_t xwmp_cond_release(struct xwmp_cond * cond, xwsq_t tik)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);
        return xwmp_synobj_release(&cond->synobj, tik);
}

xwer_t xwmp_cond_grab(struct xwmp_cond * cond)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);
        return xwmp_synobj_grab(&cond->synobj);
}

xwer_t xwmp_cond_put(struct xwmp_cond * cond)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);
        return xwmp_synobj_put(&cond->synobj);
}

xwsq_t xwmp_cond_gettik(struct xwmp_cond * cond)
{
        xwsq_t tik;

        if (cond) {
                tik = cond->synobj.xwobj.tik;
        } else {
                tik = 0;
        }
        return tik;
}

xwer_t xwmp_cond_bind(struct xwmp_cond * cond, struct xwmp_evt * evt, xwsq_t pos)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        rc = xwmp_cond_grab(cond);
        if (__xwcc_likely(XWOK == rc)) {
                xwmp_splk_lock_cpuirqsv(&cond->lock, &cpuirq);
                rc = xwmp_sel_obj_bind(evt, &cond->synobj, pos, false);
                xwmp_splk_unlock_cpuirqrs(&cond->lock, cpuirq);
        }
        return rc;
}
EXPORT_SYMBOL(xwmp_cond_bind);

xwer_t xwmp_cond_unbind(struct xwmp_cond * cond, struct xwmp_evt * evt)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        xwmp_splk_lock_cpuirqsv(&cond->lock, &cpuirq);
        rc = xwmp_sel_obj_unbind(evt, &cond->synobj, false);
        xwmp_splk_unlock_cpuirqrs(&cond->lock, cpuirq);
        if (XWOK == rc) {
                xwmp_cond_put(cond);
        }
        return rc;
}
EXPORT_SYMBOL(xwmp_cond_unbind);

xwer_t xwmp_cond_freeze(struct xwmp_cond * cond)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        rc = xwmp_cond_grab(cond);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
        } else {
                xwmp_splk_lock_cpuirqsv(&cond->lock, &cpuirq);
                if (__xwcc_unlikely(cond->count < 0)) {
                        rc = -EALREADY;
                } else {
                        cond->count = XWMP_COND_NEGATIVE;
                }
                xwmp_splk_unlock_cpuirqrs(&cond->lock, cpuirq);
                xwmp_cond_put(cond);
        }
        return rc;
}
EXPORT_SYMBOL(xwmp_cond_freeze);

xwer_t xwmp_cond_thaw(struct xwmp_cond * cond)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        rc = xwmp_cond_grab(cond);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_cond_grab;
        }
        xwmp_splk_lock_cpuirqsv(&cond->lock, &cpuirq);
        if (__xwcc_unlikely(cond->count >= 0)) {
                rc = -EPERM;
                goto err_cond_not_neg;
        }

        xwlib_bclst_init_head(&cond->wq);
        cond->count = 0;
        xwmp_splk_unlock_cpuirqrs(&cond->lock, cpuirq);
        xwmp_cond_put(cond);
        return XWOK;

err_cond_not_neg:
        xwmp_splk_unlock_cpuirqrs(&cond->lock, cpuirq);
        xwmp_cond_put(cond);
err_cond_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_cond_thaw);

xwer_t xwmp_cond_intr_all(struct xwmp_cond * cond)
{
        xwer_t rc;
        xwreg_t cpuirq;
        struct xwmp_cond_waiter * waiter;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        rc = xwmp_cond_grab(cond);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
        } else {
                xwmp_splk_lock_cpuirqsv(&cond->lock, &cpuirq);
                while (true) {
                        waiter = xwlib_bclst_first_entry(&cond->wq,
                                                         struct xwmp_cond_waiter,
                                                         node);
                        if (&waiter->node != &cond->wq) {
                                xwlib_bclst_del_init(&waiter->node);
                                waiter->flags = XWMP_COND_F_INT;
                                wake_up_process(waiter->task);
                                xwmp_splk_unlock_cpuirqrs(&cond->lock, cpuirq);
                                /* Unlock then re-lock because of the real-time
                                   preformance of interrupt */
                                xwmp_splk_lock_cpuirqsv(&cond->lock, &cpuirq);
                        } else {
                                break;
                        }
                }
                xwmp_splk_unlock_cpuirqrs(&cond->lock, cpuirq);
                xwmp_cond_put(cond);
        }
        return rc;
}
EXPORT_SYMBOL(xwmp_cond_intr_all);

xwer_t xwmp_cond_broadcast(struct xwmp_cond * cond)
{
        xwer_t rc;
        xwreg_t cpuirq;
        struct xwmp_synobj * synobj;
        struct xwmp_evt * evt;
        struct xwmp_cond_waiter * waiter;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        rc = xwmp_cond_grab(cond);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_cond_grab;
        }

        synobj = &cond->synobj;
        xwmp_splk_lock_cpuirqsv(&cond->lock, &cpuirq);
        if (__xwcc_unlikely(cond->count < 0)) {
                rc = -ENEGATIVE;
                goto err_cond_neg;
        } else {
                while (true) {
                        waiter = xwlib_bclst_first_entry(&cond->wq,
                                                         struct xwmp_cond_waiter,
                                                         node);
                        if (&waiter->node != &cond->wq) {
                                xwlib_bclst_del_init(&waiter->node);
                                waiter->flags = XWMP_COND_F_UP;
                                wake_up_process(waiter->task);
                                xwmp_splk_unlock_cpuirqrs(&cond->lock, cpuirq);
                                /* Unlock then re-lock because of the real-time
                                   preformance of interrupt */
                                xwmp_splk_lock_cpuirqsv(&cond->lock, &cpuirq);
                        } else {
                                break;
                        }
                }
                xwmb_mp_load_acquire(struct xwmp_evt *, evt, &synobj->sel.evt);
                if (NULL != evt) {
                        xwmp_sel_obj_s1i(evt, synobj);
                }
        }
        xwmp_splk_unlock_cpuirqrs(&cond->lock, cpuirq);
        xwmp_cond_put(cond);
        return XWOK;

err_cond_neg:
        xwmp_splk_unlock_cpuirqrs(&cond->lock, cpuirq);
        xwmp_cond_put(cond);
err_cond_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_cond_broadcast);

xwer_t xwmp_cond_unicast(struct xwmp_cond * cond)
{
        xwer_t rc;
        xwreg_t cpuirq;
        struct xwmp_cond_waiter * waiter;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        rc = xwmp_cond_grab(cond);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_cond_grab;
        }
        xwmp_splk_lock_cpuirqsv(&cond->lock, &cpuirq);
        if (__xwcc_unlikely(cond->count < 0)) {
                rc = -ENEGATIVE;
                goto err_cond_neg;
        }

        if (!xwlib_bclst_tst_empty(&cond->wq)) {
                waiter = xwlib_bclst_first_entry(&cond->wq,
                                                 struct xwmp_cond_waiter,
                                                 node);
                xwlib_bclst_del_init(&waiter->node);
                waiter->flags = XWMP_COND_F_UP;
                wake_up_process(waiter->task);
        } else {
        }
        xwmp_splk_unlock_cpuirqrs(&cond->lock, cpuirq);
        xwmp_cond_put(cond);
        return XWOK;

err_cond_neg:
        xwmp_splk_unlock_cpuirqrs(&cond->lock, cpuirq);
        xwmp_cond_put(cond);
err_cond_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_cond_unicast);

static
xwer_t xwmp_cond_do_wait(struct xwmp_cond * cond,
                         void * lock, xwid_t lktype, void * lkdata,
                         xwsq_t * lkst)
{
        xwer_t rc;
        xwreg_t cpuirq;
        struct xwmp_cond_waiter waiter;

        *lkst = XWOS_LKST_LOCKED;
        rc = xwmp_cond_grab(cond);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_cond_grab;
        }

	xwlib_bclst_init_node(&waiter.node);
        waiter.task = current;
	waiter.flags = 0;
        xwmp_splk_lock_cpuirqsv(&cond->lock, &cpuirq);
        set_current_state(TASK_INTERRUPTIBLE);
        xwlib_bclst_add_tail(&cond->wq, &waiter.node);
        xwmp_splk_unlock_cpuirqrs(&cond->lock, cpuirq);
        rc = xwmp_thd_do_unlock(lock, lktype, lkdata);
        if (XWOK == rc) {
                *lkst = XWOS_LKST_UNLOCKED;
        }
        xwmp_splk_lock_cpuirqsv(&cond->lock, &cpuirq);
        while (1) {
                if (signal_pending_state(TASK_INTERRUPTIBLE, current)) {
                        xwlib_bclst_del_init(&waiter.node);
                        linux_thd_clear_fake_signal(current);
                        rc = -EINTR;
                        break;
                } else if (xwmp_pm_tst_lpm()) {
                        xwlib_bclst_del_init(&waiter.node);
                        rc = -EINTR;
                        break;
                } else if (XWMP_COND_F_INT == waiter.flags) {
                        rc = -EINTR;
                        break;
                } else if (XWMP_COND_F_UP == waiter.flags) {
                        break;
                } else {
                        set_current_state(TASK_INTERRUPTIBLE);
                }
                xwmp_splk_unlock_cpuirq(&cond->lock);
                schedule();
                xwmp_splk_lock_cpuirq(&cond->lock);
        }
        xwmp_splk_unlock_cpuirqrs(&cond->lock, cpuirq);
        xwmp_cond_put(cond);
        if (XWOK == rc) {
                if (XWOS_LKST_UNLOCKED == *lkst) {
                        rc = xwmp_thd_do_lock(lock, lktype, NULL, lkdata);
                        if (XWOK == rc) {
                                *lkst = XWOS_LKST_LOCKED;
                        }
                }
        }

err_cond_grab:
        return rc;
}

xwer_t xwmp_cond_wait(struct xwmp_cond * cond,
                      void * lock, xwid_t lktype, void * lkdata,
                      xwsq_t * lkst)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);
        XWOS_VALIDATE((lkst), "nullptr", -EFAULT);
        XWOS_VALIDATE(((NULL == lock) || (lktype < XWOS_LK_NUM)),
                      "invalid-type", -EINVAL);

        return xwmp_cond_do_wait(cond, lock, lktype, lkdata, lkst);
}
EXPORT_SYMBOL(xwmp_cond_wait);

static
xwer_t xwmp_cond_do_timedwait(struct xwmp_cond * cond,
                              void * lock, xwid_t lktype, void * lkdata,
                              xwtm_t * xwtm, xwsq_t * lkst)
{
        xwer_t rc;
        struct xwmp_cond_waiter waiter;
        struct hrtimer_sleeper hrts;
        unsigned long slack;
        ktime_t expires;
        ktime_t * kt;
        xwreg_t cpuirq;

        kt = (ktime_t *)xwtm;
        *lkst = XWOS_LKST_LOCKED;
        if (__xwcc_unlikely(is_err_or_null(xwtm))) {
                rc = -EINVAL;
                goto err_inval;
        }
        rc = xwmp_cond_grab(cond);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_cond_grab;
        }

	xwlib_bclst_init_node(&waiter.node);
        waiter.task = current;
	waiter.flags = 0;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)
        hrtimer_init_sleeper_on_stack(&hrts, HRTIMER_BASE_MONOTONIC, HRTIMER_MODE_ABS);
#else
        hrtimer_init_on_stack(&hrts.timer, HRTIMER_BASE_MONOTONIC, HRTIMER_MODE_ABS);
        hrtimer_init_sleeper(&hrts, current);
#endif
        if (rt_task(current)) {
                slack = 0;
        } else {
                slack = current->timer_slack_ns;
        }

        xwmp_splk_lock_cpuirqsv(&cond->lock, &cpuirq);
        if (__xwcc_unlikely(0 == *xwtm)) {
                xwmp_splk_unlock_cpuirqrs(&cond->lock, cpuirq);
                xwmp_cond_put(cond);
                rc = xwmp_thd_do_unlock(lock, lktype, lkdata);
                if (XWOK == rc) {
                        *lkst = XWOS_LKST_UNLOCKED;
                }
                rc = -ETIMEDOUT;
                goto err_zerotm;
        }
        set_current_state(TASK_INTERRUPTIBLE);
        xwlib_bclst_add_tail(&cond->wq, &waiter.node);

        expires = ktime_add_safe(*kt, hrts.timer.base->get_time());
        hrtimer_set_expires_range_ns(&hrts.timer, expires, slack);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)
        hrtimer_sleeper_start_expires(&hrts, HRTIMER_MODE_ABS);
#else
        hrtimer_start_expires(&hrts.timer, HRTIMER_MODE_ABS);
#endif
        xwmp_splk_unlock_cpuirqrs(&cond->lock, cpuirq);
        rc = xwmp_thd_do_unlock(lock, lktype, lkdata);
        if (XWOK == rc) {
                *lkst = XWOS_LKST_UNLOCKED;
        }
        xwmp_splk_lock_cpuirqsv(&cond->lock, &cpuirq);
        while (1) {
                if (signal_pending_state(TASK_INTERRUPTIBLE, current)) {
                        xwlib_bclst_del_init(&waiter.node);
                        linux_thd_clear_fake_signal(current);
                        rc = -EINTR;
                        break;
                } else if (xwmp_pm_tst_lpm()) {
                        xwlib_bclst_del_init(&waiter.node);
                        rc = -EINTR;
                        break;
                } else if (XWMP_COND_F_INT == waiter.flags) {
                        rc = -EINTR;
                        break;
                } else if (XWMP_COND_F_UP == waiter.flags) {
                        break;
                } else {
                        set_current_state(TASK_INTERRUPTIBLE);
                }
                if (__xwcc_likely(hrts.task)) {
                        xwmp_splk_unlock_cpuirq(&cond->lock);
                        schedule();
                        xwmp_splk_lock_cpuirq(&cond->lock);
                } else {
                        xwlib_bclst_del_init(&waiter.node);
                        rc = -ETIMEDOUT;
                        break;
                }
        }
        xwmp_splk_unlock_cpuirqrs(&cond->lock, cpuirq);
        hrtimer_cancel(&hrts.timer);
        *kt = ktime_sub(expires, hrts.timer.base->get_time());
        destroy_hrtimer_on_stack(&hrts.timer);
        xwmp_cond_put(cond);
        if (XWOK == rc) {
                if (XWOS_LKST_UNLOCKED == *lkst) {
                        rc = xwmp_thd_do_lock(lock, lktype, xwtm, lkdata);
                        if (XWOK == rc) {
                                *lkst = XWOS_LKST_LOCKED;
                        }
                }
        }

err_zerotm:
err_cond_grab:
err_inval:
        return rc;
}

xwer_t xwmp_cond_timedwait(struct xwmp_cond * cond,
                           void * lock, xwid_t lktype, void * lkdata,
                           xwtm_t * xwtm, xwsq_t * lkst)
{
        xwer_t rc;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((lkst), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm_cmp(*xwtm, 0) >= 0), "out-of-time", -ETIMEDOUT);
        XWOS_VALIDATE(((NULL == lock) || (lktype < XWOS_LK_NUM)),
                      "invalid-type", -EINVAL);

        if (xwtm_cmp(*xwtm, XWTM_MAX) == 0) {
                rc = xwmp_cond_do_wait(cond, lock, lktype, lkdata, lkst);
        } else {
                rc = xwmp_cond_do_timedwait(cond, lock, lktype, lkdata,
                                            xwtm, lkst);
        }
        return rc;
}
EXPORT_SYMBOL(xwmp_cond_timedwait);
