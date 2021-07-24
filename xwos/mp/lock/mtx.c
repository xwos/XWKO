/**
 * @brief 玄武OS的锁机制：互斥锁
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
#include <linux/hrtimer.h>
#include <linux/rtmutex.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0)
  #include <linux/sched/rt.h>
#endif
#include <xwos/lib/xwlog.h>
#include <xwos/mp/thd.h>
#include <xwos/mp/lock/mtx.h>
#include <xwmd/xwfs/fs.h>
#include <xwmd/xwfs/bs.h>

#define LOGTAG "mtx"

static
xwer_t xwmp_mtx_gc(void * obj);

static
void xwmp_mtx_construct(void * data);

static
xwer_t xwmp_mtx_activate(struct xwmp_mtx * mtx, xwpr_t sprio, xwobj_gc_f gcfunc);

static
long xwmp_mtx_xwfsnode_ioctl(struct xwfs_node * xwfsnode, struct file * file,
                             unsigned int cmd, unsigned long arg);

struct kmem_cache * xwmp_mtx_cache;
struct xwfs_node * xwmp_mtx_xwfsctrl = NULL;
struct xwfs_dir * xwmp_mtx_xwfsdir = NULL;
const struct xwfs_operations xwmp_mtx_xwfsnode_ops = {
        .unlocked_ioctl = xwmp_mtx_xwfsnode_ioctl,
};

static
void xwmp_mtx_construct(void * data)
{
        struct xwmp_mtx * mtx;

        mtx = data;
        xwos_object_construct(&mtx->xwobj);
}

static
xwer_t xwmp_mtx_gc(void * obj)
{
        kmem_cache_free(xwmp_mtx_cache, obj);
        return XWOK;
}

xwer_t xwmp_mtx_cache_create(void)
{
        xwer_t rc;

        rc = XWOK;
        xwmp_mtx_cache = kmem_cache_create("xwmp_mtx_slab",
                                           sizeof(struct xwmp_mtx),
                                           XWMMCFG_ALIGNMENT, SLAB_PANIC,
                                           xwmp_mtx_construct);
        if (is_err_or_null(xwmp_mtx_cache)) {
                rc = -ENOMEM;
                xwlogf(ERR, LOGTAG, "Can't create mtx slab!\n");
                goto err_slab_create;
        }

        xwlogf(INFO, LOGTAG, "Create mtx slab ... [OK]\n");
        return XWOK;

err_slab_create:
        return rc;
}

void xwmp_mtx_cache_destroy(void)
{
        kmem_cache_destroy(xwmp_mtx_cache);
}

static
xwer_t xwmp_mtx_activate(struct xwmp_mtx * mtx, xwpr_t sprio, xwobj_gc_f gcfunc)
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

xwer_t xwmp_mtx_init(struct xwmp_mtx * mtx, xwpr_t sprio)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        xwmp_mtx_construct(mtx);
        return xwmp_mtx_activate(mtx, sprio, NULL);
}
EXPORT_SYMBOL(xwmp_mtx_init);

xwer_t xwmp_mtx_fini(struct xwmp_mtx * mtx)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        return xwmp_mtx_put(mtx);
}
EXPORT_SYMBOL(xwmp_mtx_fini);

xwer_t xwmp_mtx_create(struct xwmp_mtx ** ptrbuf, xwpr_t sprio)
{
        struct xwmp_mtx * mtx;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);

        mtx = kmem_cache_alloc(xwmp_mtx_cache, GFP_KERNEL);
        if (__xwcc_unlikely(is_err_or_null(mtx))) {
                rc = -ENOMEM;
                goto err_mtx_alloc;
        }
        rc = xwmp_mtx_activate(mtx, sprio, xwmp_mtx_gc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_activate;
        }
        *ptrbuf = mtx;
        return XWOK;

err_mtx_activate:
        kmem_cache_free(xwmp_mtx_cache, mtx);
err_mtx_alloc:
        return rc;
}
EXPORT_SYMBOL(xwmp_mtx_create);

xwer_t xwmp_mtx_delete(struct xwmp_mtx * mtx)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        return xwmp_mtx_put(mtx);
}
EXPORT_SYMBOL(xwmp_mtx_delete);

xwer_t xwmp_mtx_acquire(struct xwmp_mtx * mtx, xwsq_t tik)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        return xwos_object_acquire(&mtx->xwobj, tik);
}
EXPORT_SYMBOL(xwmp_mtx_acquire);

xwer_t xwmp_mtx_release(struct xwmp_mtx * mtx, xwsq_t tik)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        return xwos_object_release(&mtx->xwobj, tik);
}
EXPORT_SYMBOL(xwmp_mtx_release);

xwer_t xwmp_mtx_grab(struct xwmp_mtx * mtx)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        return xwos_object_grab(&mtx->xwobj);
}
EXPORT_SYMBOL(xwmp_mtx_grab);

xwer_t xwmp_mtx_put(struct xwmp_mtx * mtx)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        return xwos_object_put(&mtx->xwobj);
}
EXPORT_SYMBOL(xwmp_mtx_put);

xwsq_t xwmp_mtx_gettik(struct xwmp_mtx * mtx)
{
        xwsq_t tik;

        if (mtx) {
                tik = mtx->xwobj.tik;
        } else {
                tik = 0;
        }
        return tik;
}
EXPORT_SYMBOL(xwmp_mtx_gettik);

xwer_t xwmp_mtx_unlock(struct xwmp_mtx * mtx)
{
        rt_mutex_unlock(&mtx->lrtmtx);
        xwmp_mtx_put(mtx);
        return XWOK;
}
EXPORT_SYMBOL(xwmp_mtx_unlock);

xwer_t xwmp_mtx_lock(struct xwmp_mtx * mtx)
{
        xwer_t rc;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);

        rc = xwmp_mtx_grab(mtx);
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
        xwmp_mtx_put(mtx);
err_mtx_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_mtx_lock);

xwer_t xwmp_mtx_trylock(struct xwmp_mtx * mtx)
{
        xwer_t rc;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);

        rc = xwmp_mtx_grab(mtx);
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
        xwmp_mtx_put(mtx);
err_mtx_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_mtx_trylock);

xwer_t xwmp_mtx_timedlock(struct xwmp_mtx * mtx, xwtm_t * xwtm)
{
        xwer_t rc;
        struct hrtimer_sleeper hrts;
        unsigned long slack;
        ktime_t expires;
        ktime_t * kt;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);

        kt = (ktime_t *)xwtm;
        rc = xwmp_mtx_grab(mtx);
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
        linux_thd_clear_fake_signal(current);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_lock;
        }
        return XWOK;

err_mtx_lock:
        xwmp_mtx_put(mtx);
err_mtx_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_mtx_timedlock);

xwer_t xwmp_mtx_lock_unintr(struct xwmp_mtx * mtx)
{
        xwer_t rc;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);

        rc = xwmp_mtx_grab(mtx);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_mtx_grab;
        }
        rt_mutex_lock(&mtx->lrtmtx);
        return XWOK;

err_mtx_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_mtx_lock_unintr);

xwer_t xwmp_mtx_getlkst(struct xwmp_mtx * mtx, xwsq_t * lkst)
{
        xwer_t rc;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        XWOS_VALIDATE((lkst), "nullptr", -EFAULT);

        rc = xwmp_mtx_grab(mtx);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_grab;
        }
        if (rt_mutex_is_locked(&mtx->lrtmtx)) {
                *lkst = XWOS_LKST_LOCKED;
        } else {
                *lkst = XWOS_LKST_UNLOCKED;
        }
        xwmp_mtx_put(mtx);
        return XWOK;

err_mtx_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_mtx_getlkst);

xwer_t xwmp_mtx_xwfs_init(void)
{
        struct xwfs_dir * dir;
        struct xwfs_node * node;
        xwer_t rc;

        rc = xwfs_mknod("mtx_ctrl", 0660, &xwmp_mtx_xwfsnode_ops, NULL,
                        xwfs_dir_lock, &node);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mknod_xwfsctrl;
        }
        xwmp_mtx_xwfsctrl = node;
        rc = xwfs_mkdir("mtx_info", xwfs_dir_lock, &dir);
        xwmp_mtx_xwfsdir = dir;
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mknod_xwfsdir;
        }
        return XWOK;

err_mknod_xwfsdir:
        xwfs_rmnod(xwmp_mtx_xwfsctrl);
        xwmp_mtx_xwfsctrl = NULL;
err_mknod_xwfsctrl:
        return rc;
}

void xwmp_mtx_xwfs_exit(void)
{
        xwfs_rmnod(xwmp_mtx_xwfsctrl);
        xwmp_mtx_xwfsctrl = NULL;
        xwfs_rmdir(xwmp_mtx_xwfsdir);
        xwmp_mtx_xwfsdir = NULL;
}

static
long xwmp_mtx_xwfsnode_ioctl(struct xwfs_node * xwfsnode, struct file * file,
                             unsigned int cmd, unsigned long arg)
{
        return XWOK;
}
