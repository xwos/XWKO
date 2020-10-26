/**
 * @file
 * @brief 玄武OS内核：软件定时器
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
#include <xwos/lib/xwlog.h>
#include <xwos/mp/swt.h>

#define LOGTAG "swt"

static
void xwmp_swt_construct(void * data);

static
xwer_t xwmp_swt_gc(void * swt);

static
enum hrtimer_restart xwmp_swt_hrtimer_cb(struct hrtimer * hrt);

static
xwer_t xwmp_swt_activate(struct xwmp_swt * swt, const char * name, xwsq_t flag,
                         xwobj_gc_f gcfunc);

struct kmem_cache * xwmp_swt_cache;

static
void xwmp_swt_construct(void * data)
{
        struct xwmp_swt * swt = data;

        xwos_object_construct(&swt->xwobj);
        swt->name = NULL;
        swt->flag = 0;
        swt->cb = NULL;
        swt->arg = NULL;
        swt->period = 0;
}

static
xwer_t xwmp_swt_gc(void * swt)
{
        kmem_cache_free(xwmp_swt_cache, swt);
        return XWOK;
}

xwer_t xwmp_swt_cache_create(void)
{
        xwer_t rc;

        xwmp_swt_cache = kmem_cache_create("xwmp_swt_slab",
                                           sizeof(struct xwmp_swt),
                                           XWMMCFG_ALIGNMENT, SLAB_PANIC,
                                           xwmp_swt_construct);
        if (is_err_or_null(xwmp_swt_cache)) {
                rc = -ENOMEM;
                xwlogf(ERR, LOGTAG, "Can't create swt slab!\n");
                goto err_slab_create;
        }

        xwlogf(INFO, LOGTAG , "Create swt slab ... [OK]\n");
        return XWOK;

err_slab_create:
        return rc;
}

void xwmp_swt_cache_destroy(void)
{
        kmem_cache_destroy(xwmp_swt_cache);
}

static
enum hrtimer_restart xwmp_swt_hrtimer_cb(struct hrtimer * hrt)
{
        struct xwmp_swt *swt;
        enum hrtimer_restart ret;
        ktime_t kt;

        swt = xwcc_baseof(hrt, struct xwmp_swt, hrt);
        swt->cb(swt, swt->arg);
        if (XWMP_SWT_FLAG_RESTART & swt->flag) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,10,0)
                kt = (ktime_t)swt->period;
#else
                kt.tv64 = (s64)swt->period;
#endif
                hrtimer_forward(hrt, hrt->base->get_time(), kt);
                ret = HRTIMER_RESTART;
        } else if (XWMP_SWT_FLAG_AUTORM & swt->flag) {
                ret = HRTIMER_NORESTART;
                xwmp_swt_put(swt);
        } else {
                ret = HRTIMER_NORESTART;
        }
	return ret;
}

static
xwer_t xwmp_swt_activate(struct xwmp_swt * swt, const char * name, xwsq_t flag,
                         xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwos_object_activate(&swt->xwobj, gcfunc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xwobj_activate;
        }

        swt->name = name;
        swt->flag = flag;
        hrtimer_init(&swt->hrt, HRTIMER_BASE_MONOTONIC, HRTIMER_MODE_ABS);
        swt->hrt.function = xwmp_swt_hrtimer_cb;
        return XWOK;

err_xwobj_activate:
        return rc;
}

xwer_t xwmp_swt_init(struct xwmp_swt * swt, const char * name, xwsq_t flag)
{
        XWOS_VALIDATE((swt), "nullptr", -EFAULT);

        xwmp_swt_construct(swt);
        return xwmp_swt_activate(swt, name, flag, NULL);
}
EXPORT_SYMBOL(xwmp_swt_init);

xwer_t xwmp_swt_destroy(struct xwmp_swt * swt)
{
        XWOS_VALIDATE((swt), "nullptr", -EFAULT);

        return xwmp_swt_put(swt);
}
EXPORT_SYMBOL(xwmp_swt_destroy);

xwer_t xwmp_swt_create(struct xwmp_swt ** ptrbuf, const char * name, xwsq_t flag)
{
        struct xwmp_swt *swt;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);

        swt = kmem_cache_alloc(xwmp_swt_cache, GFP_KERNEL);
        if (__xwcc_unlikely(is_err_or_null(swt))) {
                rc = -ENOMEM;
                goto err_swt_alloc;
        }
        rc = xwmp_swt_activate(swt, name, flag, xwmp_swt_gc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_swt_activate;
        }
        *ptrbuf = (struct xwmp_swt *)swt;
        return XWOK;

err_swt_activate:
        kmem_cache_free(xwmp_swt_cache, swt);
err_swt_alloc:
        return rc;
}
EXPORT_SYMBOL(xwmp_swt_create);

xwer_t xwmp_swt_delete(struct xwmp_swt * swt)
{
        XWOS_VALIDATE((swt), "nullptr", -EFAULT);
        return xwmp_swt_put(swt);
}
EXPORT_SYMBOL(xwmp_swt_delete);

xwsq_t xwmp_swt_gettik(struct xwmp_swt * swt)
{
        xwsq_t tik;

        if (swt) {
                tik = swt->xwobj.tik;
        } else {
                tik = 0;
        }
        return tik;
}
EXPORT_SYMBOL(xwmp_swt_gettik);

xwer_t xwmp_swt_acquire(struct xwmp_swt * swt, xwsq_t tik)
{
        XWOS_VALIDATE((swt), "nullptr", -EFAULT);
        return xwos_object_acquire(&swt->xwobj, tik);
}
EXPORT_SYMBOL(xwmp_swt_acquire);

xwer_t xwmp_swt_release(struct xwmp_swt * swt, xwsq_t tik)
{
        XWOS_VALIDATE((swt), "nullptr", -EFAULT);
        return xwos_object_release(&swt->xwobj, tik);
}
EXPORT_SYMBOL(xwmp_swt_release);

xwer_t xwmp_swt_grab(struct xwmp_swt * swt)
{
        XWOS_VALIDATE((swt), "nullptr", -EFAULT);
        return xwos_object_grab(&swt->xwobj);
}
EXPORT_SYMBOL(xwmp_swt_grab);

xwer_t xwmp_swt_put(struct xwmp_swt * swt)
{
        XWOS_VALIDATE((swt), "nullptr", -EFAULT);
        return xwos_object_put(&swt->xwobj);
}
EXPORT_SYMBOL(xwmp_swt_put);

xwer_t xwmp_swt_start(struct xwmp_swt * swt, xwtm_t base, xwtm_t period,
                      xwmp_swt_f cb, void * arg)
{
        ktime_t hrtbase, ktperiod;
        xwer_t rc;

        XWOS_VALIDATE((swt), "nullptr", -EFAULT);
        XWOS_VALIDATE((cb), "nullptr", -EFAULT);

        if (__xwcc_unlikely((xwtm_cmp(base, 0) < 0) ||
                       (xwtm_cmp(period, 0) <= 0) ||
                       (is_err_or_null(cb)))) {
                rc = -EINVAL;
        } else if (hrtimer_active(&swt->hrt)) {
                rc = -EALREADY;
        } else {
                swt->period = period;
                swt->cb = cb;
                swt->arg = arg;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,10,0)
                hrtbase = (s64)base;
                ktperiod = (s64)period;
#else
                hrtbase.tv64 = (s64)base;
                ktperiod.tv64 = (s64)period;
#endif
                hrtbase = ktime_add_safe(hrtbase, ktperiod);
                hrtimer_start(&swt->hrt, hrtbase, HRTIMER_MODE_ABS);
                rc = XWOK;
        }
        return rc;
}
EXPORT_SYMBOL(xwmp_swt_start);

xwer_t xwmp_swt_stop(struct xwmp_swt * swt)
{
        XWOS_VALIDATE((swt), "nullptr", -EFAULT);

        if (hrtimer_active(&swt->hrt)) {
                hrtimer_cancel(&swt->hrt);
        }
        return XWOK;
}
EXPORT_SYMBOL(xwmp_swt_stop);
