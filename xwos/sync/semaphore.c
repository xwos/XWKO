/**
 * @file
 * @brief XuanWuOS的同步机制：信号量
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
#include <xwos/lock/spinlock.h>
#include <xwos/core/thread.h>
#include <xwos/core/pm.h>
#include <xwmd/xwfs/fs.h>
#include <xwmd/xwfs/bs.h>
#include <xwos/sync/event.h>
#include <xwos/sync/semaphore.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
enum xwsync_smr_waiter_flag_em {
        XWSYNC_SMR_F_UP = BIT(0), /**< wakeup flag */
        XWSYNC_SMR_F_INT = BIT(1), /**< interrupt flag */
};

struct xwsync_smr_waiter {
        struct xwlib_bclst_node node;
        struct task_struct * task;
        volatile xwsq_t flags;
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWSYNC_SMR_NEGATIVE     (-1L)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
xwer_t xwsync_smr_gc(void * obj);

static
void xwsync_smr_construct(void * data);

static
xwer_t xwsync_smr_activate(struct xwsync_smr * smr, xwssq_t val, xwssq_t max,
                           xwobj_gc_f gcfunc);

static
long xwsync_smr_xwfsnode_ioctl(struct xwfs_node * xwfsnode, struct file * file,
                               unsigned int cmd, unsigned long arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      .data        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct kmem_cache * xwsync_smr_cache;
struct xwfs_node * xwsync_smr_xwfsctrl = NULL;
struct xwfs_dir * xwsync_smr_xwfsdir = NULL;
const struct xwfs_operations xwsync_smr_xwfsnode_ops = {
        .unlocked_ioctl = xwsync_smr_xwfsnode_ioctl,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
void xwsync_smr_construct(void * data)
{
        struct xwsync_smr * smr = data;

        xwos_object_construct(&smr->xwobj);
        smr->count = 0;
        smr->max = 0;
        xwlib_bclst_init_head(&smr->wq);
        xwlk_splk_init(&smr->lock);
}

static
xwer_t xwsync_smr_gc(void * obj)
{
        kmem_cache_free(xwsync_smr_cache, obj);
        return OK;
}

xwer_t xwsync_smr_cache_create(void)
{
        xwer_t rc = OK;

        xwsync_smr_cache = kmem_cache_create("xwsync_smr_slab",
                                             sizeof(struct xwsync_smr),
                                             XWMMCFG_ALIGNMENT, SLAB_PANIC,
                                             xwsync_smr_construct);
        if (is_err_or_null(xwsync_smr_cache)) {
                rc = -ENOMEM;
                xwlogf(ERR, "Can't create semaphore slab!\n");
                goto err_slab_create;
        }

        xwlogf(INFO, "Create semaphore slab ... [OK]\n");
        return OK;

err_slab_create:
        return rc;
}

void xwsync_smr_cache_destroy(void)
{
        kmem_cache_destroy(xwsync_smr_cache);
}

static
xwer_t xwsync_smr_activate(struct xwsync_smr * smr, xwssq_t val,
                           xwssq_t max, xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwos_object_activate(&smr->xwobj, gcfunc);
        if (__unlikely(rc < 0)) {
                goto err_obj_activate;
        }
        smr->count = val;
        smr->max = max;
        return OK;

err_obj_activate:
        return rc;
}

xwer_t xwsync_smr_init(struct xwsync_smr * smr, xwssq_t val, xwssq_t max)
{
        xwer_t rc;

        xwsync_smr_construct(smr);
        rc = xwsync_smr_activate(smr, val, max, NULL);
        if (__unlikely(rc < 0)) {
                goto err_smr_activate;
        }
        return OK;

err_smr_activate:
        return rc;
}
EXPORT_SYMBOL(xwsync_smr_init);

xwer_t xwsync_smr_destroy(struct xwsync_smr * smr)
{
        return xwsync_smr_put(smr);
}
EXPORT_SYMBOL(xwsync_smr_destroy);

xwer_t xwsync_smr_create(struct xwsync_smr ** ptrbuf, xwssq_t val, xwssq_t max)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        if (__unlikely((val < 0) || (max < 0) || (val > max))) {
                rc = -EINVAL;
                goto err_invald_args;
        }

        smr = kmem_cache_alloc(xwsync_smr_cache, GFP_KERNEL);
        if (__unlikely(is_err_or_null(smr))) {
                rc = -ENOMEM;
                goto err_smr_alloc;
        }
        rc = xwsync_smr_activate(smr, val, max, xwsync_smr_gc);
        if (__unlikely(rc < 0)) {
                goto err_smr_activate;
        }
        *ptrbuf = smr;
        return OK;

err_smr_activate:
        kmem_cache_free(xwsync_smr_cache, smr);
err_smr_alloc:
err_invald_args:
        return rc;
}
EXPORT_SYMBOL(xwsync_smr_create);

xwer_t xwsync_smr_delete(struct xwsync_smr * smr)
{
        return xwsync_smr_put(smr);
}
EXPORT_SYMBOL(xwsync_smr_delete);

xwer_t xwsync_smr_bind(struct xwsync_smr * smr, struct xwsync_evt * evt, xwsq_t pos)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((smr), "nullptr", -EFAULT);

        xwlk_splk_lock_cpuirqsv(&smr->lock, &cpuirq);
        rc = xwsync_evt_smr_bind(evt, smr, pos);
        if ((OK == rc) && (smr->count > 0)) {
                rc = xwsync_evt_smr_s1i(evt, smr);
        }
        xwlk_splk_unlock_cpuirqrs(&smr->lock, cpuirq);
        return rc;
}
EXPORT_SYMBOL(xwsync_smr_bind);

xwer_t xwsync_smr_unbind(struct xwsync_smr * smr, struct xwsync_evt * evt)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((smr), "nullptr", -EFAULT);
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        xwlk_splk_lock_cpuirqsv(&smr->lock, &cpuirq);
        rc = xwsync_evt_smr_unbind(evt, smr);
        if (OK == rc) {
                rc = xwsync_evt_smr_c0i(evt, smr);
        }
        xwlk_splk_unlock_cpuirqrs(&smr->lock, cpuirq);

        return rc;
}
EXPORT_SYMBOL(xwsync_smr_unbind);

xwer_t xwsync_smr_freeze(struct xwsync_smr * smr)
{
        struct xwsync_evt * evt;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwsync_smr_grab(smr);
        if (__unlikely(rc < 0)) {
                rc = -EPERM;
        } else {
                xwlk_splk_lock_cpuirqsv(&smr->lock, &cpuirq);
                if (__unlikely(smr->count < 0)) {
                        rc = -EALREADY;
                } else {
                        smr->count = XWSYNC_SMR_NEGATIVE;

                        xwmb_smp_load_acquire(evt, &smr->selector.evt);
                        if (NULL != evt) {
                                xwsync_evt_smr_c0i(evt, smr);
                        }
                }
                xwlk_splk_unlock_cpuirqrs(&smr->lock, cpuirq);
                xwsync_smr_put(smr);
        }
        return rc;
}
EXPORT_SYMBOL(xwsync_smr_freeze);

xwer_t xwsync_smr_thaw(struct xwsync_smr * smr, xwssq_t val, xwssq_t max)
{
        struct xwsync_evt * evt;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwsync_smr_grab(smr);
        if (__unlikely(rc < 0)) {
                rc = -EINVAL;
                goto err_smr_grab;
        }
        xwlk_splk_lock_cpuirqsv(&smr->lock, &cpuirq);
        if (__unlikely(smr->count >= 0)) {
                rc = -EPERM;
                goto err_not_neg;
        }
        xwlib_bclst_init_head(&smr->wq);
        smr->max = max;
        smr->count = val;
        if (smr->count > 0) {
                xwmb_smp_load_acquire(evt, &smr->selector.evt);
                if (NULL != evt) {
                        xwsync_evt_smr_s1i(evt, smr);
                }
        }
        xwlk_splk_unlock_cpuirqrs(&smr->lock, cpuirq);
        xwsync_smr_put(smr);
        return OK;

err_not_neg:
        xwlk_splk_unlock_cpuirqrs(&smr->lock, cpuirq);
        xwsync_smr_put(smr);
err_smr_grab:
        return rc;
}
EXPORT_SYMBOL(xwsync_smr_thaw);

xwer_t xwsync_smr_post(struct xwsync_smr * smr)
{
        xwer_t rc;
        xwreg_t cpuirq;
        struct xwsync_evt * evt;
        struct xwsync_smr_waiter * waiter;

        rc = xwsync_smr_grab(smr);
        if (__unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_smr_grab;
        }

        xwlk_splk_lock_cpuirqsv(&smr->lock, &cpuirq);
        if (smr->count < 0) {
                rc = -ENEGATIVE;
                goto err_smr_neg;
        } else if (__likely(xwlib_bclst_tst_empty(&smr->wq))) {
                if (smr->count >= smr->max) {
                        rc = -ERANGE;
                        goto err_smr_range;
                } else {
                        smr->count++;
                }
                if (smr->count > 0) {
                        xwmb_smp_load_acquire(evt, &smr->selector.evt);
                        if (NULL != evt) {
                                xwsync_evt_smr_s1i(evt, smr);
                        }
                }
        } else {
                waiter = xwlib_bclst_first_entry(&smr->wq,
                                                 struct xwsync_smr_waiter,
                                                 node);
                xwlib_bclst_del_init(&waiter->node);
                waiter->flags = XWSYNC_SMR_F_UP;
                wake_up_process(waiter->task);
        }
        xwlk_splk_unlock_cpuirqrs(&smr->lock, cpuirq);
        xwsync_smr_put(smr);
        return OK;

err_smr_range:
err_smr_neg:
        xwlk_splk_unlock_cpuirqrs(&smr->lock, cpuirq);
        xwsync_smr_put(smr);
err_smr_grab:
        return rc;
}
EXPORT_SYMBOL(xwsync_smr_post);

xwer_t xwsync_smr_wait(struct xwsync_smr * smr)
{
        xwer_t rc;
        xwreg_t cpuirq;
        struct xwsync_evt * evt;
        struct xwsync_smr_waiter waiter;
        struct task_struct * task;

        rc = xwsync_smr_grab(smr);
        if (__unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_smr_grab;
        }
        xwlk_splk_lock_cpuirqsv(&smr->lock, &cpuirq);
        if (likely(smr->count > 0)) {
                smr->count--;
                rc = OK;
                if (0 == smr->count) {
                        xwmb_smp_load_acquire(evt, &smr->selector.evt);
                        if (NULL != evt) {
                                xwsync_evt_smr_c0i(evt, smr);
                        }
                }
        } else {
                task = current;
                xwlib_bclst_init_node(&waiter.node);
                xwlib_bclst_add_tail(&smr->wq, &waiter.node);
                waiter.task = task;
                waiter.flags = 0;
                for (;;) {
                        if (signal_pending_state(TASK_INTERRUPTIBLE, task)) {
                                xwlib_bclst_del_init(&waiter.node);
                                linux_thrd_clear_fake_signal(current);
                                rc = -EINTR;
                                break;
                        } else if (xwos_pm_tst_lpm()) {
                                xwlib_bclst_del_init(&waiter.node);
                                rc = -EINTR;
                                break;
                        }
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
                        __set_task_state(task, TASK_INTERRUPTIBLE);
#else
                        __set_current_state(TASK_INTERRUPTIBLE);
#endif
                        xwlk_splk_unlock_cpuirq(&smr->lock);
                        schedule();
                        xwlk_splk_lock_cpuirq(&smr->lock);
                        if (XWSYNC_SMR_F_UP == waiter.flags) {
                                rc = OK;
                                break;
                        }
                }
        }
        xwlk_splk_unlock_cpuirqrs(&smr->lock, cpuirq);
        xwsync_smr_put(smr);

err_smr_grab:
        return rc;
}
EXPORT_SYMBOL(xwsync_smr_wait);

xwer_t xwsync_smr_trywait(struct xwsync_smr * smr)
{
        xwer_t rc;
        xwreg_t cpuirq;
        struct xwsync_evt * evt;

        rc = xwsync_smr_grab(smr);
        if (__unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_smr_grab;
        }

        xwlk_splk_lock_cpuirqsv(&smr->lock, &cpuirq);
        if (likely(smr->count > 0)) {
                smr->count--;
                rc = OK;
                if (0 == smr->count) {
                        xwmb_smp_load_acquire(evt, &smr->selector.evt);
                        if (NULL != evt) {
                                xwsync_evt_smr_c0i(evt, smr);
                        }
                }
        } else {
                rc = -ENODATA;
        }
        xwlk_splk_unlock_cpuirqrs(&smr->lock, cpuirq);
        xwsync_smr_put(smr);

err_smr_grab:
        return rc;
}
EXPORT_SYMBOL(xwsync_smr_trywait);

xwer_t xwsync_smr_timedwait(struct xwsync_smr * smr, xwtm_t * xwtm)
{
        xwer_t rc;
        xwreg_t cpuirq;
        struct xwsync_smr_waiter waiter;
        ktime_t expires;
        ktime_t * kt;
        struct xwsync_evt * evt;
        struct hrtimer_sleeper hrts;
        struct task_struct * task;
        unsigned long slack;

        kt = (ktime_t *)xwtm;

        rc = xwsync_smr_grab(smr);
        if (__unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_smr_grab;
        }

        xwlk_splk_lock_cpuirqsv(&smr->lock, &cpuirq);
        if (__likely(smr->count > 0)) {
                smr->count--;
                rc = OK;
                if (0 == smr->count) {
                        xwmb_smp_load_acquire(evt, &smr->selector.evt);
                        if (NULL != evt) {
                                xwsync_evt_smr_c0i(evt, smr);
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
                xwlib_bclst_add_tail(&smr->wq, &waiter.node);
                waiter.task = task;
                waiter.flags = 0;
                rc = OK;
                while (true) {
                        if (signal_pending_state(TASK_INTERRUPTIBLE, task)) {
                                xwlib_bclst_del_init(&waiter.node);
                                linux_thrd_clear_fake_signal(current);
                                rc = -EINTR;
                                break;
                        } else if (xwos_pm_tst_lpm()) {
                                xwlib_bclst_del_init(&waiter.node);
                                rc = -EINTR;
                                break;
                        } else if (__unlikely(-ETIMEDOUT == rc)) {
                                xwlib_bclst_del_init(&waiter.node);
                                break;
                        }
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
                        __set_task_state(task, TASK_INTERRUPTIBLE);
#else
                        __set_current_state(TASK_INTERRUPTIBLE);
#endif
                        xwlk_splk_unlock_cpuirq(&smr->lock);
                        hrtimer_init_on_stack(&hrts.timer,
                                              HRTIMER_BASE_MONOTONIC,
                                              HRTIMER_MODE_ABS);
                        expires = ktime_add_safe(*kt,
                                                 hrts.timer.base->get_time());
                        hrtimer_set_expires_range_ns(&hrts.timer, expires, slack);
                        hrtimer_init_sleeper(&hrts, task);
                        hrtimer_start_expires(&hrts.timer, HRTIMER_MODE_ABS);
                        if (__likely(hrts.task)) {
                                schedule();
                        }
                        hrtimer_cancel(&hrts.timer);
                        rc = !hrts.task ? -ETIMEDOUT : OK;
                        *kt = ktime_sub(expires,
                                        hrts.timer.base->get_time());
                        /* destroy_hrtimer_on_stack(&hrts.timer); */
                        xwlk_splk_lock_cpuirq(&smr->lock);
                        if (XWSYNC_SMR_F_UP == waiter.flags) {
                                rc = OK;
                                break;
                        }
                }
        }
        xwlk_splk_unlock_cpuirqrs(&smr->lock, cpuirq);
        xwsync_smr_put(smr);
err_smr_grab:
        return rc;
}
EXPORT_SYMBOL(xwsync_smr_timedwait);

xwer_t xwsync_smr_wait_unintr(struct xwsync_smr * smr)
{
        xwer_t rc;
        xwreg_t cpuirq;
        struct xwsync_evt * evt;
        struct xwsync_smr_waiter waiter;
        struct task_struct * task;

        rc = xwsync_smr_grab(smr);
        if (__unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_smr_grab;
        }
        xwlk_splk_lock_cpuirqsv(&smr->lock, &cpuirq);
        if (likely(smr->count > 0)) {
                smr->count--;
                rc = OK;
                if (0 == smr->count) {
                        xwmb_smp_load_acquire(evt, &smr->selector.evt);
                        if (NULL != evt) {
                                xwsync_evt_smr_c0i(evt, smr);
                        }
                }
        } else {
                task = current;
                xwlib_bclst_init_node(&waiter.node);
                xwlib_bclst_add_tail(&smr->wq, &waiter.node);
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
                        xwlk_splk_unlock_cpuirq(&smr->lock);
                        schedule();
                        xwlk_splk_lock_cpuirq(&smr->lock);
                        if (XWSYNC_SMR_F_UP == waiter.flags) {
                                rc = OK;
                                break;
                        }
                }
        }
        xwlk_splk_unlock_cpuirqrs(&smr->lock, cpuirq);
        xwsync_smr_put(smr);

err_smr_grab:
        return rc;
}
EXPORT_SYMBOL(xwsync_smr_wait_unintr);

xwer_t xwsync_smr_getvalue(struct xwsync_smr * smr, xwssq_t * sval)
{
        *sval = smr->count;
        return OK;
}
EXPORT_SYMBOL(xwsync_smr_getvalue);

xwer_t xwsync_smr_xwfs_init(void)
{
        struct xwfs_dir * dir;
        struct xwfs_node * node;
        xwer_t rc;

        rc = xwfs_mknod("smr_ctrl", 0660, &xwsync_smr_xwfsnode_ops, NULL,
                        dir_sync, &node);
        if (__unlikely(rc < 0)) {
                goto err_mknod_xwfsctrl;
        }
        xwsync_smr_xwfsctrl = node;
        rc = xwfs_mkdir("smr_info", dir_sync, &dir);
        xwsync_smr_xwfsdir = dir;
        if (__unlikely(rc < 0)) {
                goto err_mknod_xwfsdir;
        }
        return OK;

err_mknod_xwfsdir:
        xwfs_rmnod(xwsync_smr_xwfsctrl);
        xwsync_smr_xwfsctrl = NULL;
err_mknod_xwfsctrl:
        return rc;
}

void xwsync_smr_xwfs_exit(void)
{
        xwfs_rmnod(xwsync_smr_xwfsctrl);
        xwsync_smr_xwfsctrl = NULL;
        xwfs_rmdir(xwsync_smr_xwfsdir);
        xwsync_smr_xwfsdir = NULL;
}

static
long xwsync_smr_xwfsnode_ioctl(struct xwfs_node * xwfsnode, struct file * file,
                               unsigned int cmd, unsigned long arg)
{
        return OK;
}
