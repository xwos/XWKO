/**
 * @file
 * @brief XuanWuOS的同步机制：条件量
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwlog.h>
#include <xwos/lib/bclst.h>
#include <linux/slab.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
  #include <linux/sched/signal.h>
#endif
#include <xwmd/xwfs/fs.h>
#include <xwmd/xwfs/bs.h>
#include <xwos/lock/spinlock.h>
#include <xwos/core/thread.h>
#include <xwos/core/pm.h>
#include <xwos/sync/object.h>
#include <xwos/sync/event.h>
#include <xwos/sync/condition.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
enum xwsync_cdt_waiter_flag_em {
        XWSYNC_CDT_F_UP = BIT(0), /**< wakeup flag */
        XWSYNC_CDT_F_INT = BIT(1), /**< interrupt flag */
};

struct xwsync_cdt_waiter {
        struct xwlib_bclst_node node;
        struct task_struct * task;
        volatile xwsq_t flags;
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWSYNC_CDT_NEGATIVE                (-1L)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
xwer_t xwsync_cdt_gc(void * obj);

static
xwer_t xwsync_cdt_do_wait(struct xwsync_cdt * cdt,
                          void * lock, xwid_t lktype, void * lkdata,
                          xwsq_t * lkst);

static
xwer_t xwsync_cdt_do_timedwait(struct xwsync_cdt * cdt,
                               void * lock, xwid_t lktype, void * lkdata,
                               xwtm_t * xwtm, xwsq_t * lkst);

static
long xwsync_cdt_xwfsnode_ioctl(struct xwfs_node * xwfsnode, struct file * file,
                               unsigned int cmd, unsigned long arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct kmem_cache * xwsync_cdt_cache;
struct xwfs_node * xwsync_cdt_xwfsctrl = NULL;
struct xwfs_dir * xwsync_cdt_xwfsdir = NULL;
const struct xwfs_operations xwsync_cdt_xwfsnode_ops = {
        .unlocked_ioctl = xwsync_cdt_xwfsnode_ioctl,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
void xwsync_cdt_construct(void * anon)
{
        struct xwsync_cdt * cdt = anon;

        xwsync_object_construct(&cdt->xwsyncobj);
        cdt->count = 0;
        xwlib_bclst_init_head(&cdt->wq);
        xwlk_splk_init(&cdt->lock);
}

static
xwer_t xwsync_cdt_gc(void * obj)
{
        kmem_cache_free(xwsync_cdt_cache, obj);
        return OK;
}

xwer_t xwsync_cdt_cache_create(void)
{
        xwer_t rc = OK;

        xwsync_cdt_cache = kmem_cache_create("xwsync_cdt_slab",
                                             sizeof(struct xwsync_cdt),
                                             XWMMCFG_ALIGNMENT, SLAB_PANIC,
                                             xwsync_cdt_construct);
        if (is_err_or_null(xwsync_cdt_cache)) {
                rc = -ENOMEM;
                xwoslogf(ERR, "Can't create condition slab!\n");
                goto err_slab_create;
        }

        xwoslogf(INFO, "Create condition slab ... [OK]\n");
        return OK;

err_slab_create:
        return rc;
}

void xwsync_cdt_cache_destroy(void)
{
        kmem_cache_destroy(xwsync_cdt_cache);
}

xwer_t xwsync_cdt_activate(struct xwsync_cdt * cdt, xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwsync_object_activate(&cdt->xwsyncobj, gcfunc);
        if (__unlikely(rc < 0)) {
                goto err_obj_activate;
        }
        cdt->count = 0;
        return OK;

err_obj_activate:
        return rc;
}

xwer_t xwsync_cdt_init(struct xwsync_cdt * cdt)
{
        xwsync_cdt_construct(cdt);
        return xwsync_cdt_activate(cdt, NULL);
}
EXPORT_SYMBOL(xwsync_cdt_init);

xwer_t xwsync_cdt_destroy(struct xwsync_cdt * cdt)
{
        return xwsync_cdt_put(cdt);
}
EXPORT_SYMBOL(xwsync_cdt_destroy);

xwer_t xwsync_cdt_create(struct xwsync_cdt ** ptrbuf)
{
        xwer_t rc;
        struct xwsync_cdt * cdt;

        cdt = kmem_cache_alloc(xwsync_cdt_cache, GFP_KERNEL);
        if (__unlikely(is_err_or_null(cdt))) {
                rc = -ENOMEM;
                goto err_cdt_alloc;
        }
        rc = xwsync_cdt_activate(cdt, xwsync_cdt_gc);
        if (__unlikely(rc < 0)) {
                goto err_cdt_activate;
        }
        *ptrbuf = cdt;
        return OK;

err_cdt_activate:
        kmem_cache_free(xwsync_cdt_cache, cdt);
err_cdt_alloc:
        return rc;
}
EXPORT_SYMBOL(xwsync_cdt_create);

xwer_t xwsync_cdt_delete(struct xwsync_cdt * cdt)
{
        return xwsync_cdt_put(cdt);
}
EXPORT_SYMBOL(xwsync_cdt_delete);

xwer_t xwsync_cdt_bind(struct xwsync_cdt * cdt, struct xwsync_evt * evt, xwsq_t pos)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);

        rc = xwsync_cdt_grab(cdt);
        if (__likely(OK == rc)) {
                xwlk_splk_lock_cpuirqsv(&cdt->lock, &cpuirq);
                rc = xwsync_evt_obj_bind(evt, &cdt->xwsyncobj, pos, false);
                xwlk_splk_unlock_cpuirqrs(&cdt->lock, cpuirq);
        }
        return rc;
}
EXPORT_SYMBOL(xwsync_cdt_bind);

xwer_t xwsync_cdt_unbind(struct xwsync_cdt * cdt, struct xwsync_evt * evt)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        xwlk_splk_lock_cpuirqsv(&cdt->lock, &cpuirq);
        rc = xwsync_evt_obj_unbind(evt, &cdt->xwsyncobj, false);
        xwlk_splk_unlock_cpuirqrs(&cdt->lock, cpuirq);
        if (OK == rc) {
                xwsync_cdt_put(cdt);
        }
        return rc;
}
EXPORT_SYMBOL(xwsync_cdt_unbind);

xwer_t xwsync_cdt_freeze(struct xwsync_cdt * cdt)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);

        rc = xwsync_cdt_grab(cdt);
        if (__unlikely(rc < 0)) {
                rc = -EPERM;
        } else {
                xwlk_splk_lock_cpuirqsv(&cdt->lock, &cpuirq);
                if (__unlikely(cdt->count < 0)) {
                        rc = -EALREADY;
                } else {
                        cdt->count = XWSYNC_CDT_NEGATIVE;
                }
                xwlk_splk_unlock_cpuirqrs(&cdt->lock, cpuirq);
                xwsync_cdt_put(cdt);
        }
        return rc;
}
EXPORT_SYMBOL(xwsync_cdt_freeze);

xwer_t xwsync_cdt_thaw(struct xwsync_cdt * cdt)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);

        rc = xwsync_cdt_grab(cdt);
        if (__unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_cdt_grab;
        }
        xwlk_splk_lock_cpuirqsv(&cdt->lock, &cpuirq);
        if (__unlikely(cdt->count >= 0)) {
                rc = -EPERM;
                goto err_cdt_not_neg;
        }

        xwlib_bclst_init_head(&cdt->wq);
        cdt->count = 0;
        xwlk_splk_unlock_cpuirqrs(&cdt->lock, cpuirq);
        xwsync_cdt_put(cdt);
        return OK;

err_cdt_not_neg:
        xwlk_splk_unlock_cpuirqrs(&cdt->lock, cpuirq);
        xwsync_cdt_put(cdt);
err_cdt_grab:
        return rc;
}
EXPORT_SYMBOL(xwsync_cdt_thaw);

xwer_t xwsync_cdt_intr_all(struct xwsync_cdt * cdt)
{
        xwer_t rc;
        xwreg_t cpuirq;
        struct xwsync_cdt_waiter * waiter;

        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);

        rc = xwsync_cdt_grab(cdt);
        if (__unlikely(rc < 0)) {
                rc = -EPERM;
        } else {
                xwlk_splk_lock_cpuirqsv(&cdt->lock, &cpuirq);
                while (true) {
                        waiter = xwlib_bclst_first_entry(&cdt->wq,
                                                         struct xwsync_cdt_waiter,
                                                         node);
                        if (&waiter->node != &cdt->wq) {
                                xwlib_bclst_del_init(&waiter->node);
                                waiter->flags = XWSYNC_CDT_F_INT;
                                wake_up_process(waiter->task);
                                xwlk_splk_unlock_cpuirqrs(&cdt->lock, cpuirq);
                                /* Unlock then re-lock because of the real-time
                                   preformance of interrupt */
                                xwlk_splk_lock_cpuirqsv(&cdt->lock, &cpuirq);
                        } else {
                                break;
                        }
                }
                xwlk_splk_unlock_cpuirqrs(&cdt->lock, cpuirq);
                xwsync_cdt_put(cdt);
        }
        return rc;
}
EXPORT_SYMBOL(xwsync_cdt_intr_all);

xwer_t xwsync_cdt_broadcast(struct xwsync_cdt * cdt)
{
        xwer_t rc;
        xwreg_t cpuirq;
        struct xwsync_object * xwsyncobj;
        struct xwsync_evt * evt;
        struct xwsync_cdt_waiter * waiter;

        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);

        rc = xwsync_cdt_grab(cdt);
        if (__unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_cdt_grab;
        }

        xwsyncobj = &cdt->xwsyncobj;
        xwlk_splk_lock_cpuirqsv(&cdt->lock, &cpuirq);
        if (__unlikely(cdt->count < 0)) {
                rc = -ENEGATIVE;
                goto err_cdt_neg;
        } else {
                while (true) {
                        waiter = xwlib_bclst_first_entry(&cdt->wq,
                                                         struct xwsync_cdt_waiter,
                                                         node);
                        if (&waiter->node != &cdt->wq) {
                                xwlib_bclst_del_init(&waiter->node);
                                waiter->flags = XWSYNC_CDT_F_UP;
                                wake_up_process(waiter->task);
                                xwlk_splk_unlock_cpuirqrs(&cdt->lock, cpuirq);
                                /* Unlock then re-lock because of the real-time
                                   preformance of interrupt */
                                xwlk_splk_lock_cpuirqsv(&cdt->lock, &cpuirq);
                        } else {
                                break;
                        }
                }
                xwmb_smp_load_acquire(evt, &xwsyncobj->selector.evt);
                if (NULL != evt) {
                        xwsync_evt_obj_s1i(evt, xwsyncobj);
                }
        }
        xwlk_splk_unlock_cpuirqrs(&cdt->lock, cpuirq);
        xwsync_cdt_put(cdt);
        return OK;

err_cdt_neg:
        xwlk_splk_unlock_cpuirqrs(&cdt->lock, cpuirq);
        xwsync_cdt_put(cdt);
err_cdt_grab:
        return rc;
}
EXPORT_SYMBOL(xwsync_cdt_broadcast);

xwer_t xwsync_cdt_unicast(struct xwsync_cdt * cdt)
{
        xwer_t rc;
        xwreg_t cpuirq;
        struct xwsync_cdt_waiter * waiter;

        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);

        rc = xwsync_cdt_grab(cdt);
        if (__unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_cdt_grab;
        }
        xwlk_splk_lock_cpuirqsv(&cdt->lock, &cpuirq);
        if (__unlikely(cdt->count < 0)) {
                rc = -ENEGATIVE;
                goto err_cdt_neg;
        }

        if (!xwlib_bclst_tst_empty(&cdt->wq)) {
                waiter = xwlib_bclst_first_entry(&cdt->wq,
                                                 struct xwsync_cdt_waiter,
                                                 node);
                xwlib_bclst_del_init(&waiter->node);
                waiter->flags = XWSYNC_CDT_F_UP;
                wake_up_process(waiter->task);
        } else {
        }
        xwlk_splk_unlock_cpuirqrs(&cdt->lock, cpuirq);
        xwsync_cdt_put(cdt);
        return OK;

err_cdt_neg:
        xwlk_splk_unlock_cpuirqrs(&cdt->lock, cpuirq);
        xwsync_cdt_put(cdt);
err_cdt_grab:
        return rc;
}
EXPORT_SYMBOL(xwsync_cdt_unicast);

static
xwer_t xwsync_cdt_do_wait(struct xwsync_cdt * cdt,
                          void * lock, xwid_t lktype, void * lkdata,
                          xwsq_t * lkst)
{
        xwer_t rc;
        xwreg_t cpuirq;
        struct xwsync_cdt_waiter waiter;

        *lkst = XWLK_STATE_LOCKED;
        rc = xwsync_cdt_grab(cdt);
        if (__unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_cdt_grab;
        }

	xwlib_bclst_init_node(&waiter.node);
        waiter.task = current;
	waiter.flags = 0;
        xwlk_splk_lock_cpuirqsv(&cdt->lock, &cpuirq);
        set_current_state(TASK_INTERRUPTIBLE);
        xwlib_bclst_add_tail(&cdt->wq, &waiter.node);
        xwlk_splk_unlock_cpuirqrs(&cdt->lock, cpuirq);
        rc = xwos_thrd_do_unlock(lock, lktype, lkdata);
        if (OK == rc) {
                *lkst = XWLK_STATE_UNLOCKED;
        }
        xwlk_splk_lock_cpuirqsv(&cdt->lock, &cpuirq);
        while (1) {
                if (signal_pending_state(TASK_INTERRUPTIBLE, current)) {
                        xwlib_bclst_del_init(&waiter.node);
                        linux_thrd_clear_fake_signal(current);
                        rc = -EINTR;
                        break;
                } else if (xwos_pm_tst_lpm()) {
                        xwlib_bclst_del_init(&waiter.node);
                        rc = -EINTR;
                        break;
                } else if (XWSYNC_CDT_F_INT == waiter.flags) {
                        rc = -EINTR;
                        break;
                } else if (XWSYNC_CDT_F_UP == waiter.flags) {
                        break;
                } else {
                        set_current_state(TASK_INTERRUPTIBLE);
                }
                xwlk_splk_unlock_cpuirq(&cdt->lock);
                schedule();
                xwlk_splk_lock_cpuirq(&cdt->lock);
        }
        xwlk_splk_unlock_cpuirqrs(&cdt->lock, cpuirq);
        xwsync_cdt_put(cdt);
        if (OK == rc) {
                if (XWLK_STATE_UNLOCKED == *lkst) {
                        rc = xwos_thrd_do_lock(lock, lktype, NULL, lkdata);
                        if (OK == rc) {
                                *lkst = XWLK_STATE_LOCKED;
                        }
                }
        }

err_cdt_grab:
        return rc;
}

xwer_t xwsync_cdt_wait(struct xwsync_cdt * cdt,
                       void * lock, xwid_t lktype, void * lkdata,
                       xwsq_t * lkst)
{
        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);
        XWOS_VALIDATE((lkst), "nullptr", -EFAULT);
        XWOS_VALIDATE(((NULL == lock) || (lktype < XWLK_TYPE_NUM)),
                      "invalid-type", -EINVAL);

        return xwsync_cdt_do_wait(cdt, lock, lktype, lkdata, lkst);
}
EXPORT_SYMBOL(xwsync_cdt_wait);

static
xwer_t xwsync_cdt_do_timedwait(struct xwsync_cdt * cdt,
                               void * lock, xwid_t lktype, void * lkdata,
                               xwtm_t * xwtm, xwsq_t * lkst)
{
        xwer_t rc;
        struct xwsync_cdt_waiter waiter;
        struct hrtimer_sleeper hrts;
        unsigned long slack;
        ktime_t expires;
        ktime_t * kt;
        xwreg_t cpuirq;

        kt = (ktime_t *)xwtm;
        *lkst = XWLK_STATE_LOCKED;
        if (__unlikely(is_err_or_null(xwtm))) {
                rc = -EINVAL;
                goto err_inval;
        }
        rc = xwsync_cdt_grab(cdt);
        if (__unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_cdt_grab;
        }

	xwlib_bclst_init_node(&waiter.node);
        waiter.task = current;
	waiter.flags = 0;
        hrtimer_init_on_stack(&hrts.timer, HRTIMER_BASE_MONOTONIC, HRTIMER_MODE_ABS);
        hrtimer_init_sleeper(&hrts, current);
        if (rt_task(current)) {
                slack = 0;
        } else {
                slack = current->timer_slack_ns;
        }

        xwlk_splk_lock_cpuirqsv(&cdt->lock, &cpuirq);
        if (__unlikely(0 == *xwtm)) {
                xwlk_splk_unlock_cpuirqrs(&cdt->lock, cpuirq);
                xwsync_cdt_put(cdt);
                rc = xwos_thrd_do_unlock(lock, lktype, lkdata);
                if (OK == rc) {
                        *lkst = XWLK_STATE_UNLOCKED;
                }
                rc = -ETIMEDOUT;
                goto err_zerotm;
        }
        set_current_state(TASK_INTERRUPTIBLE);
        xwlib_bclst_add_tail(&cdt->wq, &waiter.node);
        expires = ktime_add_safe(*kt, hrts.timer.base->get_time());
        hrtimer_set_expires_range_ns(&hrts.timer, expires, slack);
        hrtimer_start_expires(&hrts.timer, HRTIMER_MODE_ABS);
        xwlk_splk_unlock_cpuirqrs(&cdt->lock, cpuirq);
        rc = xwos_thrd_do_unlock(lock, lktype, lkdata);
        if (OK == rc) {
                *lkst = XWLK_STATE_UNLOCKED;
        }
        xwlk_splk_lock_cpuirqsv(&cdt->lock, &cpuirq);
        while (1) {
                if (signal_pending_state(TASK_INTERRUPTIBLE, current)) {
                        xwlib_bclst_del_init(&waiter.node);
                        linux_thrd_clear_fake_signal(current);
                        rc = -EINTR;
                        break;
                } else if (xwos_pm_tst_lpm()) {
                        xwlib_bclst_del_init(&waiter.node);
                        rc = -EINTR;
                        break;
                } else if (XWSYNC_CDT_F_INT == waiter.flags) {
                        rc = -EINTR;
                        break;
                } else if (XWSYNC_CDT_F_UP == waiter.flags) {
                        break;
                } else {
                        set_current_state(TASK_INTERRUPTIBLE);
                }
                if (__likely(hrts.task)) {
                        xwlk_splk_unlock_cpuirq(&cdt->lock);
                        schedule();
                        xwlk_splk_lock_cpuirq(&cdt->lock);
                } else {
                        xwlib_bclst_del_init(&waiter.node);
                        rc = -ETIMEDOUT;
                        break;
                }
        }
        xwlk_splk_unlock_cpuirqrs(&cdt->lock, cpuirq);
        hrtimer_cancel(&hrts.timer);
        *kt = ktime_sub(expires, hrts.timer.base->get_time());
        /* destroy_hrtimer_on_stack(&hrts.timer); */
        xwsync_cdt_put(cdt);
        if (OK == rc) {
                if (XWLK_STATE_UNLOCKED == *lkst) {
                        rc = xwos_thrd_do_lock(lock, lktype, xwtm, lkdata);
                        if (OK == rc) {
                                *lkst = XWLK_STATE_LOCKED;
                        }
                }
        }

err_zerotm:
err_cdt_grab:
err_inval:
        return rc;
}

xwer_t xwsync_cdt_timedwait(struct xwsync_cdt * cdt,
                            void * lock, xwid_t lktype, void * lkdata,
                            xwtm_t * xwtm, xwsq_t * lkst)
{
        xwer_t rc;

        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((lkst), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm_cmp(*xwtm, 0) >= 0), "out-of-time", -ETIMEDOUT);
        XWOS_VALIDATE(((NULL == lock) || (lktype < XWLK_TYPE_NUM)),
                      "invalid-type", -EINVAL);

        if (xwtm_cmp(*xwtm, XWTM_MAX) == 0) {
                rc = xwsync_cdt_do_wait(cdt, lock, lktype, lkdata, lkst);
        } else {
                rc = xwsync_cdt_do_timedwait(cdt, lock, lktype, lkdata,
                                             xwtm, lkst);
        }
        return rc;
}
EXPORT_SYMBOL(xwsync_cdt_timedwait);

xwer_t xwsync_cdt_xwfs_init(void)
{
        struct xwfs_dir * dir;
        struct xwfs_node * node;
        xwer_t rc;

        rc = xwfs_mknod("cdt_ctrl", 0660, &xwsync_cdt_xwfsnode_ops, NULL,
                        dir_sync, &node);
        if (__unlikely(rc < 0)) {
                goto err_mknod_xwfsctrl;
        }
        xwsync_cdt_xwfsctrl = node;
        rc = xwfs_mkdir("cdt_info", dir_sync, &dir);
        xwsync_cdt_xwfsdir = dir;
        if (__unlikely(rc < 0)) {
                goto err_mknod_xwfsdir;
        }
        return OK;

err_mknod_xwfsdir:
        xwfs_rmnod(xwsync_cdt_xwfsctrl);
        xwsync_cdt_xwfsctrl = NULL;
err_mknod_xwfsctrl:
        return rc;
}

void xwsync_cdt_xwfs_exit(void)
{
        xwfs_rmnod(xwsync_cdt_xwfsctrl);
        xwsync_cdt_xwfsctrl = NULL;
        xwfs_rmdir(xwsync_cdt_xwfsdir);
        xwsync_cdt_xwfsdir = NULL;
}

static
long xwsync_cdt_xwfsnode_ioctl(struct xwfs_node * xwfsnode, struct file * file,
                               unsigned int cmd, unsigned long arg)
{
        return OK;
}
