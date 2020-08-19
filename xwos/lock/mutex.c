/**
 * @brief XuanWuOS的锁机制：互斥锁
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
#include <linux/slab.h>
#include <linux/hrtimer.h>
#include <linux/rtmutex.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0)
  #include <linux/sched/rt.h>
#endif
#include <xwmd/xwfs/fs.h>
#include <xwmd/xwfs/bs.h>
#include <xwos/core/thread.h>
#include <xwos/lock/mutex.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
xwer_t xwlk_mtx_gc(void * obj);

static
void xwlk_mtx_construct(void * data);

static
xwer_t xwlk_mtx_activate(struct xwlk_mtx * mtx, xwpr_t sprio, xwobj_gc_f gcfunc);

static
long xwlk_mtx_xwfsnode_ioctl(struct xwfs_node * xwfsnode, struct file * file,
                             unsigned int cmd, unsigned long arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct kmem_cache * xwlk_mtx_cache;
struct xwfs_node * xwlk_mtx_xwfsctrl = NULL;
struct xwfs_dir * xwlk_mtx_xwfsdir = NULL;
const struct xwfs_operations xwlk_mtx_xwfsnode_ops = {
        .unlocked_ioctl = xwlk_mtx_xwfsnode_ioctl,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
void xwlk_mtx_construct(void * data)
{
        struct xwlk_mtx * mtx;

        mtx = data;
        xwos_object_construct(&mtx->xwobj);
}

static
xwer_t xwlk_mtx_gc(void * obj)
{
        kmem_cache_free(xwlk_mtx_cache, obj);
        return XWOK;
}

xwer_t xwlk_mtx_cache_create(void)
{
        xwer_t rc;

        rc = XWOK;
        xwlk_mtx_cache = kmem_cache_create("xwlk_mtx_slab",
                                           sizeof(struct xwlk_mtx),
                                           XWMMCFG_ALIGNMENT, SLAB_PANIC,
                                           xwlk_mtx_construct);
        if (is_err_or_null(xwlk_mtx_cache)) {
                rc = -ENOMEM;
                xwoslogf(ERR, "Can't create mutex slab!\n");
                goto err_slab_create;
        }

        xwoslogf(INFO, "Create mutex slab ... [OK]\n");
        return XWOK;

err_slab_create:
        return rc;
}

void xwlk_mtx_cache_destroy(void)
{
        kmem_cache_destroy(xwlk_mtx_cache);
}

static
xwer_t xwlk_mtx_activate(struct xwlk_mtx * mtx, xwpr_t sprio, xwobj_gc_f gcfunc)
{
        xwer_t rc;

        XWOS_UNUSED(sprio);
        rc = xwos_object_activate(&mtx->xwobj, gcfunc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_obj_activate;
        }
        rt_mutex_init(&mtx->lrtmtx);
        return XWOK;

err_obj_activate:
        return rc;
}

xwer_t xwlk_mtx_init(struct xwlk_mtx * mtx, xwpr_t sprio)
{
        xwlk_mtx_construct(mtx);
        return xwlk_mtx_activate(mtx, sprio, NULL);
}
EXPORT_SYMBOL(xwlk_mtx_init);

xwer_t xwlk_mtx_destroy(struct xwlk_mtx * mtx)
{
        return xwlk_mtx_put(mtx);
}
EXPORT_SYMBOL(xwlk_mtx_destroy);

xwer_t xwlk_mtx_create(struct xwlk_mtx ** ptrbuf, xwpr_t sprio)
{
        struct xwlk_mtx * mtx;
        xwer_t rc;

        mtx = kmem_cache_alloc(xwlk_mtx_cache, GFP_KERNEL);
        if (__xwcc_unlikely(is_err_or_null(mtx))) {
                rc = -ENOMEM;
                goto err_mtx_alloc;
        }
        rc = xwlk_mtx_activate(mtx, sprio, xwlk_mtx_gc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_activate;
        }
        *ptrbuf = mtx;
        return XWOK;

err_mtx_activate:
        kmem_cache_free(xwlk_mtx_cache, mtx);
err_mtx_alloc:
        return rc;
}
EXPORT_SYMBOL(xwlk_mtx_create);

xwer_t xwlk_mtx_delete(struct xwlk_mtx * mtx)
{
        return xwlk_mtx_put(mtx);
}
EXPORT_SYMBOL(xwlk_mtx_delete);

xwer_t xwlk_mtx_unlock(struct xwlk_mtx * mtx)
{
        rt_mutex_unlock(&mtx->lrtmtx);
        xwlk_mtx_put(mtx);
        return XWOK;
}
EXPORT_SYMBOL(xwlk_mtx_unlock);

xwer_t xwlk_mtx_lock(struct xwlk_mtx * mtx)
{
        xwer_t rc;

        rc = xwlk_mtx_grab(mtx);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_mtx_grab;
        }
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 17, 0)
        rc = rt_mutex_lock_interruptible(&mtx->lrtmtx);
#else
        rc = rt_mutex_lock_interruptible(&mtx->lrtmtx, true);
#endif
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_lock;
        }
        return XWOK;

err_mtx_lock:
        xwlk_mtx_put(mtx);
err_mtx_grab:
        return rc;
}
EXPORT_SYMBOL(xwlk_mtx_lock);

xwer_t xwlk_mtx_trylock(struct xwlk_mtx * mtx)
{
        xwer_t rc;

        rc = xwlk_mtx_grab(mtx);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_mtx_grab;
        }
        rc = rt_mutex_trylock(&mtx->lrtmtx);
        if (rc < 0) {
                rc = -EAGAIN;
                goto err_mtx_trylock;
        }
        return XWOK;

err_mtx_trylock:
        xwlk_mtx_put(mtx);
err_mtx_grab:
        return rc;
}
EXPORT_SYMBOL(xwlk_mtx_trylock);

xwer_t xwlk_mtx_timedlock(struct xwlk_mtx * mtx, xwtm_t * xwtm)
{
        xwer_t rc;
        struct hrtimer_sleeper hrts;
        unsigned long slack;
        ktime_t expires;
        ktime_t * kt;

        kt = (ktime_t *)xwtm;
        rc = xwlk_mtx_grab(mtx);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_mtx_grab;
        }

        slack = current->timer_slack_ns;
        if (rt_task(current)) {
                slack = 0;
        }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)
        hrtimer_init_sleeper_on_stack(&hrts, HRTIMER_BASE_MONOTONIC, HRTIMER_MODE_ABS);
#else
        hrtimer_init_on_stack(&hrts.timer, HRTIMER_BASE_MONOTONIC, HRTIMER_MODE_ABS);
        hrtimer_init_sleeper(&hrts, current);
#endif
        expires = ktime_add_safe(*kt, hrts.timer.base->get_time());
        hrtimer_set_expires_range_ns(&hrts.timer, expires, slack);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 17, 0)
        rc = rt_mutex_timed_lock(&mtx->lrtmtx, &hrts);
#else
        rc = rt_mutex_timed_lock(&mtx->lrtmtx, &hrts, true);
#endif
        *kt = ktime_sub(expires, hrts.timer.base->get_time());
        destroy_hrtimer_on_stack(&hrts.timer);
        linux_thrd_clear_fake_signal(current);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_lock;
        }
        return XWOK;

err_mtx_lock:
        xwlk_mtx_put(mtx);
err_mtx_grab:
        return rc;
}
EXPORT_SYMBOL(xwlk_mtx_timedlock);

xwer_t xwlk_mtx_lock_unintr(struct xwlk_mtx * mtx)
{
        xwer_t rc;

        rc = xwlk_mtx_grab(mtx);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_mtx_grab;
        }
        rt_mutex_lock(&mtx->lrtmtx);
        return XWOK;

err_mtx_grab:
        return rc;
}
EXPORT_SYMBOL(xwlk_mtx_lock_unintr);

xwer_t xwlk_mtx_xwfs_init(void)
{
        struct xwfs_dir * dir;
        struct xwfs_node * node;
        xwer_t rc;

        rc = xwfs_mknod("mtx_ctrl", 0660, &xwlk_mtx_xwfsnode_ops, NULL,
                        dir_locks, &node);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mknod_xwfsctrl;
        }
        xwlk_mtx_xwfsctrl = node;
        rc = xwfs_mkdir("mtx_info", dir_locks, &dir);
        xwlk_mtx_xwfsdir = dir;
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mknod_xwfsdir;
        }
        return XWOK;

err_mknod_xwfsdir:
        xwfs_rmnod(xwlk_mtx_xwfsctrl);
        xwlk_mtx_xwfsctrl = NULL;
err_mknod_xwfsctrl:
        return rc;
}

void xwlk_mtx_xwfs_exit(void)
{
        xwfs_rmnod(xwlk_mtx_xwfsctrl);
        xwlk_mtx_xwfsctrl = NULL;
        xwfs_rmdir(xwlk_mtx_xwfsdir);
        xwlk_mtx_xwfsdir = NULL;
}

static
long xwlk_mtx_xwfsnode_ioctl(struct xwfs_node * xwfsnode, struct file * file,
                             unsigned int cmd, unsigned long arg)
{
        return XWOK;
}
