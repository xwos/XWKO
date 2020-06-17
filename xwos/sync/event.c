/**
 * @file
 * @brief XuanWuOS的同步机制：事件
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
#include <xwos/object.h>
#include <xwos/lib/string.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwlog.h>
#include <linux/slab.h>
#include <xwos/core/irq.h>
#include <xwos/core/thread.h>
#include <xwos/lock/spinlock.h>
#include <xwos/sync/object.h>
#include <xwos/sync/condition.h>
#include <xwos/sync/semaphore.h>
#include <xwos/sync/event.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct kmem_cache * xwsync_evt_cache;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
void xwsync_evt_construct(void * anon);

static
xwer_t xwsync_evt_gc(void * evt);

static
xwer_t xwsync_evt_activate(struct xwsync_evt * evt, xwbmp_t initval[],
                           xwsq_t attr, xwobj_gc_f gcfunc);

static
xwer_t xwsync_evt_timedwait_level(struct xwsync_evt * evt,
                                  xwsq_t trigger, xwsq_t action,
                                  xwbmp_t msk[],
                                  xwtm_t * xwtm);

static
xwer_t xwsync_evt_timedwait_edge(struct xwsync_evt * evt, xwsq_t trigger,
                                 xwbmp_t origin[], xwbmp_t msk[],
                                 xwtm_t * xwtm);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t xwsync_evt_cache_create(void)
{
        xwer_t rc = OK;

        xwsync_evt_cache = kmem_cache_create("xwsync_evt_slab",
                                             sizeof(struct xwsync_evt),
                                             XWMMCFG_ALIGNMENT, SLAB_PANIC,
                                             xwsync_evt_construct);
        if (is_err_or_null(xwsync_evt_cache)) {
                rc = -ENOMEM;
                xwoslogf(ERR, "Can't create condition slab!\n");
                goto err_slab_create;
        }

        xwoslogf(INFO, "Create condition slab ... [OK]\n");
        return OK;

err_slab_create:
        return rc;
}

void xwsync_evt_cache_destroy(void)
{
        kmem_cache_destroy(xwsync_evt_cache);
}

static
void xwsync_evt_construct(void * anon)
{
        struct xwsync_evt * evt;

        evt = anon;
        xwsync_cdt_construct(&evt->cdt);
}

static
xwer_t xwsync_evt_gc(void * obj)
{
        kmem_cache_free(xwsync_evt_cache, obj);
        return OK;
}

static
xwer_t xwsync_evt_activate(struct xwsync_evt * evt, xwbmp_t initval[],
                           xwsq_t attr, xwobj_gc_f gcfunc)
{
        xwer_t rc;
        xwsz_t size;

        size = BITS_TO_BMPS(XWSYNC_EVT_MAXNUM);
        rc = xwsync_cdt_activate(&evt->cdt, gcfunc);
        if (__likely(OK == rc)) {
                evt->attr = attr;
                switch (attr & XWSYNC_EVT_TYPE_MASK) {
                case XWSYNC_EVT_TYPE_FLAG:
                        memset(evt->msk, 0xFF, size);
                        break;
                case XWSYNC_EVT_TYPE_BARRIER:
                        memset(evt->msk, 0, size);
                        break;
                case XWSYNC_EVT_TYPE_SELECTOR:
                        memset(evt->msk, 0, size);
                        break;
                default:
                        memset(evt->msk, 0xFF, size);
                        break;
                }
                if (NULL != initval) {
                        memcpy(evt->bmp, initval, size);
                } else {
                        memset(evt->bmp, 0, size);
                }
                xwlk_splk_init(&evt->lock);
        }/* else {} */
        return rc;
}

xwer_t xwsync_evt_create(struct xwsync_evt ** ptrbuf, xwbmp_t initval[], xwsq_t attr)
{
        struct xwsync_evt * evt;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);
        XWOS_VALIDATE(((attr & XWSYNC_EVT_TYPE_MASK) < XWSYNC_EVT_TYPE_MAX),
                      "type-error", -EINVAL);

        *ptrbuf = NULL;
        evt = kmem_cache_alloc(xwsync_evt_cache, GFP_KERNEL);
        if (__unlikely(is_err_or_null(evt))) {
                rc = -ENOMEM;
                goto err_evt_alloc;
        }
        rc = xwsync_evt_activate(evt, initval, attr, xwsync_evt_gc);
        if (__unlikely(rc < 0)) {
                goto err_evt_activate;
        }
        *ptrbuf = evt;
        return OK;

err_evt_activate:
        kmem_cache_free(xwsync_evt_cache, evt);
err_evt_alloc:
        return rc;
}
EXPORT_SYMBOL(xwsync_evt_create);

xwer_t xwsync_evt_delete(struct xwsync_evt * evt)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        return xwsync_evt_put(evt);
}
EXPORT_SYMBOL(xwsync_evt_delete);

xwer_t xwsync_evt_init(struct xwsync_evt * evt, xwbmp_t initval[], xwsq_t attr)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE(((attr & XWSYNC_EVT_TYPE_MASK) < XWSYNC_EVT_TYPE_MAX),
                      "type-error", -EINVAL);

        xwsync_evt_construct(evt);
        return xwsync_evt_activate(evt, initval, attr, NULL);
}
EXPORT_SYMBOL(xwsync_evt_init);

xwer_t xwsync_evt_destroy(struct xwsync_evt * evt)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        return xwsync_evt_put(evt);
}
EXPORT_SYMBOL(xwsync_evt_destroy);

xwer_t xwsync_evt_bind(struct xwsync_evt * evt, struct xwsync_evt * slt, xwsq_t pos)
{
        return xwsync_cdt_bind(&evt->cdt, slt, pos);
}
EXPORT_SYMBOL(xwsync_evt_bind);

xwer_t xwsync_evt_unbind(struct xwsync_evt * evt, struct xwsync_evt * slt)
{
        return xwsync_cdt_unbind(&evt->cdt, slt);
}
EXPORT_SYMBOL(xwsync_evt_unbind);

xwer_t xwsync_evt_intr_all(struct xwsync_evt * evt)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        return xwsync_cdt_intr_all(&evt->cdt);
}
EXPORT_SYMBOL(xwsync_evt_intr_all);

/******** type:XWSYNC_EVT_TYPE_FLAG ********/
xwer_t xwsync_evt_s1m(struct xwsync_evt * evt, xwbmp_t msk[])
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_FLAG),
                      "type-error", -ETYPE);

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_s1m(evt->bmp, msk, XWSYNC_EVT_MAXNUM);
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_cdt_broadcast(&evt->cdt);
        xwsync_evt_put(evt);
        return OK;

err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwsync_evt_s1m);

xwer_t xwsync_evt_s1i(struct xwsync_evt * evt, xwsq_t pos)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_FLAG),
                      "type-error", -ETYPE);

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_s1i(evt->bmp, pos);
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_cdt_broadcast(&evt->cdt);
        xwsync_evt_put(evt);
        return OK;

err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwsync_evt_s1i);

xwer_t xwsync_evt_c0m(struct xwsync_evt * evt, xwbmp_t msk[])
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_FLAG),
                      "type-error", -ETYPE);

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_c0m(evt->bmp, msk, XWSYNC_EVT_MAXNUM);
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_cdt_broadcast(&evt->cdt);
        xwsync_evt_put(evt);
        return OK;

err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwsync_evt_c0m);

xwer_t xwsync_evt_c0i(struct xwsync_evt * evt, xwsq_t pos)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((pos < XWSYNC_EVT_MAXNUM), "out-of-range", -ECHRNG);
        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_FLAG),
                      "type-error", -ETYPE);

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_c0i(evt->bmp, pos);
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_cdt_broadcast(&evt->cdt);
        xwsync_evt_put(evt);
        return OK;

err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwsync_evt_c0i);

xwer_t xwsync_evt_x1m(struct xwsync_evt * evt, xwbmp_t msk[])
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_FLAG),
                      "type-error", -ETYPE);

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_x1m(evt->bmp, msk, XWSYNC_EVT_MAXNUM);
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_cdt_broadcast(&evt->cdt);
        xwsync_evt_put(evt);
        return OK;

err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwsync_evt_x1m);

xwer_t xwsync_evt_x1i(struct xwsync_evt * evt, xwsq_t pos)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((pos < XWSYNC_EVT_MAXNUM), "out-of-range", -ECHRNG);
        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_FLAG),
                      "type-error", -ETYPE);

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_x1i(evt->bmp, pos);
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_cdt_broadcast(&evt->cdt);
        xwsync_evt_put(evt);
        return OK;

err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwsync_evt_x1i);

xwer_t xwsync_evt_read(struct xwsync_evt * evt, xwbmp_t out[])
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((out), "nullptr", -EFAULT);

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_assign(out, evt->bmp, XWSYNC_EVT_MAXNUM);
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_evt_put(evt);
        return OK;

err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwsync_evt_read);

static
xwer_t xwsync_evt_trywait_level(struct xwsync_evt * evt,
                                xwsq_t trigger, xwsq_t action,
                                xwbmp_t msk[])
{
        xwreg_t cpuirq;
        bool triggered;
        xwer_t rc;

        XWOS_VALIDATE((trigger <= XWSYNC_EVT_TRIGGER_CLR_ANY),
                      "illegal-trigger", -EINVAL);
        XWOS_VALIDATE((action < XWSYNC_EVT_ACTION_NUM),
                      "illegal-action", -EINVAL);

        rc = OK;
        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        if (XWSYNC_EVT_ACTION_CONSUMPTION == action) {
                switch (trigger) {
                case XWSYNC_EVT_TRIGGER_SET_ALL:
                        triggered = xwbmpop_t1ma_then_c0m(evt->bmp, msk,
                                                          XWSYNC_EVT_MAXNUM);
                        break;
                case XWSYNC_EVT_TRIGGER_SET_ANY:
                        triggered = xwbmpop_t1mo_then_c0m(evt->bmp, msk,
                                                          XWSYNC_EVT_MAXNUM);
                        break;
                case XWSYNC_EVT_TRIGGER_CLR_ALL:
                        triggered = xwbmpop_t0ma_then_s1m(evt->bmp, msk,
                                                          XWSYNC_EVT_MAXNUM);
                        break;
                case XWSYNC_EVT_TRIGGER_CLR_ANY:
                        triggered = xwbmpop_t0mo_then_s1m(evt->bmp, msk,
                                                          XWSYNC_EVT_MAXNUM);
                        break;
                default:
                        triggered = true;
                        rc = -EINVAL;
                        break;
                }
        } else {
                switch (trigger) {
                case XWSYNC_EVT_TRIGGER_SET_ALL:
                        triggered = xwbmpop_t1ma(evt->bmp, msk,
                                                 XWSYNC_EVT_MAXNUM);
                        break;
                case XWSYNC_EVT_TRIGGER_SET_ANY:
                        triggered = xwbmpop_t1mo(evt->bmp, msk,
                                                 XWSYNC_EVT_MAXNUM);
                        break;
                case XWSYNC_EVT_TRIGGER_CLR_ALL:
                        triggered = xwbmpop_t0ma(evt->bmp, msk,
                                                 XWSYNC_EVT_MAXNUM);
                        break;
                case XWSYNC_EVT_TRIGGER_CLR_ANY:
                        triggered = xwbmpop_t0mo(evt->bmp, msk,
                                                 XWSYNC_EVT_MAXNUM);
                        break;
                default:
                        triggered = true;
                        rc = -EINVAL;
                        break;
                }
        }
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        if (!triggered) {
                rc = -ENODATA;
        }
        return rc;
}

static
xwer_t xwsync_evt_trywait_edge(struct xwsync_evt * evt, xwsq_t trigger,
                               xwbmp_t origin[], xwbmp_t msk[])
{
        xwreg_t cpuirq;
        xwssq_t cmprc;
        bool triggered;
        xwer_t rc;
        xwsync_evt_declare_bitmap(cur);
        xwsync_evt_declare_bitmap(tmp);

        XWOS_VALIDATE((origin), "nullptr", -EFAULT);

        xwbmpop_and(origin, msk, XWSYNC_EVT_MAXNUM);
        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_assign(cur, evt->bmp, XWSYNC_EVT_MAXNUM);
        xwbmpop_and(cur, msk, XWSYNC_EVT_MAXNUM);
        if (XWSYNC_EVT_TRIGGER_TGL_ALL == trigger) {
                xwbmpop_assign(tmp, cur, XWSYNC_EVT_MAXNUM);
                xwbmpop_xor(tmp, origin, XWSYNC_EVT_MAXNUM);
                cmprc = xwbmpop_cmp(tmp, msk, XWSYNC_EVT_MAXNUM);
                if (0 == cmprc) {
                        triggered = true;
                        rc = OK;
                } else {
                        triggered = false;
                        rc = -ENODATA;
                }
        } else if (XWSYNC_EVT_TRIGGER_TGL_ANY == trigger) {
                cmprc = xwbmpop_cmp(origin, cur, XWSYNC_EVT_MAXNUM);
                if (0 == cmprc) {
                        triggered = false;
                        rc = -ENODATA;
                } else {
                        triggered = true;
                        rc = OK;
                }
        } else {
                triggered = true;
                rc = -EINVAL;
        }
        if (triggered) {
                xwbmpop_assign(origin, cur, XWSYNC_EVT_MAXNUM);
                xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        } else {
                xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        }
        return rc;
}

__xwos_api
xwer_t xwsync_evt_trywait(struct xwsync_evt * evt,
                          xwsq_t trigger, xwsq_t action,
                          xwbmp_t origin[], xwbmp_t msk[])
{
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_FLAG),
                      "type-error", -ETYPE);

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        if (trigger <= XWSYNC_EVT_TRIGGER_CLR_ANY) {
                rc = xwsync_evt_trywait_level(evt, trigger, action, msk);
        } else {
                rc = xwsync_evt_trywait_edge(evt, trigger, origin, msk);
        }
        xwsync_evt_put(evt);

err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwsync_evt_trywait);

static
xwer_t xwsync_evt_timedwait_level(struct xwsync_evt * evt,
                                  xwsq_t trigger, xwsq_t action,
                                  xwbmp_t msk[],
                                  xwtm_t * xwtm)
{
        xwreg_t cpuirq;
        bool triggered;
        xwsq_t lkst;
        xwer_t rc;

        XWOS_VALIDATE((trigger <= XWSYNC_EVT_TRIGGER_CLR_ANY),
                      "illegal-trigger", -EINVAL);
        XWOS_VALIDATE((action < XWSYNC_EVT_ACTION_NUM),
                      "illegal-action", -EINVAL);

        rc = OK;
        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        while (true) {
                if (XWSYNC_EVT_ACTION_CONSUMPTION == action) {
                        switch (trigger) {
                        case XWSYNC_EVT_TRIGGER_SET_ALL:
                                triggered = xwbmpop_t1ma_then_c0m(evt->bmp, msk,
                                                                  XWSYNC_EVT_MAXNUM);
                                break;
                        case XWSYNC_EVT_TRIGGER_SET_ANY:
                                triggered = xwbmpop_t1mo_then_c0m(evt->bmp, msk,
                                                                  XWSYNC_EVT_MAXNUM);
                                break;
                        case XWSYNC_EVT_TRIGGER_CLR_ALL:
                                triggered = xwbmpop_t0ma_then_s1m(evt->bmp, msk,
                                                                  XWSYNC_EVT_MAXNUM);
                                break;
                        case XWSYNC_EVT_TRIGGER_CLR_ANY:
                                triggered = xwbmpop_t0mo_then_s1m(evt->bmp, msk,
                                                                  XWSYNC_EVT_MAXNUM);
                                break;
                        default:
                                triggered = true;
                                rc = -EINVAL;
                                break;
                        }
                } else {
                        switch (trigger) {
                        case XWSYNC_EVT_TRIGGER_SET_ALL:
                                triggered = xwbmpop_t1ma(evt->bmp, msk,
                                                         XWSYNC_EVT_MAXNUM);
                                break;
                        case XWSYNC_EVT_TRIGGER_SET_ANY:
                                triggered = xwbmpop_t1mo(evt->bmp, msk,
                                                         XWSYNC_EVT_MAXNUM);
                                break;
                        case XWSYNC_EVT_TRIGGER_CLR_ALL:
                                triggered = xwbmpop_t0ma(evt->bmp, msk,
                                                         XWSYNC_EVT_MAXNUM);
                                break;
                        case XWSYNC_EVT_TRIGGER_CLR_ANY:
                                triggered = xwbmpop_t0mo(evt->bmp, msk,
                                                         XWSYNC_EVT_MAXNUM);
                                break;
                        default:
                                triggered = true;
                                rc = -EINVAL;
                                break;
                        }
                }
                if (triggered) {
                        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        break;
                } else {
                        rc = xwsync_cdt_timedwait(&evt->cdt,
                                                  &evt->lock, XWLK_TYPE_SPLK, NULL,
                                                  xwtm, &lkst);
                        if (OK == rc) {
                                if (XWLK_STATE_UNLOCKED == lkst) {
                                        xwlk_splk_lock(&evt->lock);
                                }
                        } else {
                                if (XWLK_STATE_LOCKED == lkst) {
                                        xwlk_splk_unlock(&evt->lock);
                                }
                                xwos_cpuirq_restore_lc(cpuirq);
                                break;
                        }
                }
        }
        return rc;
}

static
xwer_t xwsync_evt_timedwait_edge(struct xwsync_evt * evt, xwsq_t trigger,
                                 xwbmp_t origin[], xwbmp_t msk[],
                                 xwtm_t * xwtm)
{
        xwreg_t cpuirq;
        xwsq_t lkst;
        xwssq_t cmprc;
        bool triggered;
        xwer_t rc;
        xwsync_evt_declare_bitmap(cur);
        xwsync_evt_declare_bitmap(tmp);

        XWOS_VALIDATE((origin), "nullptr", -EFAULT);

        xwbmpop_and(origin, msk, XWSYNC_EVT_MAXNUM);
        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        while (true) {
                xwbmpop_assign(cur, evt->bmp, XWSYNC_EVT_MAXNUM);
                xwbmpop_and(cur, msk, XWSYNC_EVT_MAXNUM);
                if (XWSYNC_EVT_TRIGGER_TGL_ALL == trigger) {
                        xwbmpop_assign(tmp, cur, XWSYNC_EVT_MAXNUM);
                        xwbmpop_xor(tmp, origin, XWSYNC_EVT_MAXNUM);
                        cmprc = xwbmpop_cmp(tmp, msk, XWSYNC_EVT_MAXNUM);
                        if (0 == cmprc) {
                                triggered = true;
                                rc = OK;
                        } else {
                                triggered = false;
                        }
                } else if (XWSYNC_EVT_TRIGGER_TGL_ANY == trigger) {
                        cmprc = xwbmpop_cmp(origin, cur, XWSYNC_EVT_MAXNUM);
                        if (0 == cmprc) {
                                triggered = false;
                        } else {
                                triggered = true;
                                rc = OK;
                        }
                } else {
                        triggered = true;
                        rc = -EINVAL;
                }
                if (triggered) {
                        xwbmpop_assign(origin, cur, XWSYNC_EVT_MAXNUM);
                        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        break;
                } else {
                        rc = xwsync_cdt_timedwait(&evt->cdt,
                                                  &evt->lock, XWLK_TYPE_SPLK, NULL,
                                                  xwtm, &lkst);
                        if (OK == rc) {
                                if (XWLK_STATE_UNLOCKED == lkst) {
                                        xwlk_splk_lock(&evt->lock);
                                }
                        } else {
                                if (XWLK_STATE_LOCKED == lkst) {
                                        xwlk_splk_unlock(&evt->lock);
                                }
                                xwos_cpuirq_restore_lc(cpuirq);
                                break;
                        }
                }
        }
        return rc;
}

xwer_t xwsync_evt_timedwait(struct xwsync_evt * evt,
                            xwsq_t trigger, xwsq_t action,
                            xwbmp_t origin[], xwbmp_t msk[],
                            xwtm_t * xwtm)
{
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_FLAG),
                      "type-error", -ETYPE);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHRD == xwos_irq_get_id(NULL)),
                      "not-in-thrd", -ENOTINTHRD);

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        if (trigger <= XWSYNC_EVT_TRIGGER_CLR_ANY) {
                rc = xwsync_evt_timedwait_level(evt, trigger, action, msk, xwtm);
        } else {
                rc = xwsync_evt_timedwait_edge(evt, trigger, origin, msk, xwtm);
        }
        xwsync_evt_put(evt);

err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwsync_evt_timedwait);

/******** type:XWSYNC_EVT_TYPE_SELECTOR ********/
xwer_t xwsync_evt_obj_bind(struct xwsync_evt * evt,
                           struct xwsync_object * obj,
                           xwsq_t pos,
                           bool exclusive)
{
        struct xwsync_evt * owner;
        xwreg_t cpuirq;
        bool existed;
        xwer_t rc;

        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_SELECTOR),
                      "type-error", -ETYPE);
        XWOS_VALIDATE((pos < XWSYNC_EVT_MAXNUM), "out-of-range", -ECHRNG);

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }

        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        owner = obj->selector.evt;
        if (NULL != owner) {
                rc = -EALREADY;
                goto err_already;
        }
        existed = xwbmpop_t1i(evt->msk, pos);
        if (existed) {
                rc = -EBUSY;
                goto err_busy;
        }
        if (exclusive) {
                xwbmpop_s1i(evt->msk, pos);
        }
        obj->selector.evt = evt;
        obj->selector.pos = pos;
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return OK;

err_busy:
err_already:
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_evt_put(evt);
err_evt_grab:
        return rc;
}

xwer_t xwsync_evt_obj_unbind(struct xwsync_evt * evt,
                             struct xwsync_object * obj,
                             bool exclusive)
{
        struct xwsync_evt * owner;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_SELECTOR),
                      "type-error", -ETYPE);

        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        owner = obj->selector.evt;
        if (evt != owner) {
                rc = -ENOTCONN;
                goto err_notconn;
        }
        if (exclusive) {
                xwbmpop_c0i(evt->msk, obj->selector.pos);
        }
        obj->selector.evt = NULL;
        obj->selector.pos = XWSYNC_EVT_MAXNUM;
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_evt_put(evt);
        return OK;

err_notconn:
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return rc;
}

xwer_t xwsync_evt_obj_s1i(struct xwsync_evt * evt, struct xwsync_object * obj)
{
        struct xwsync_evt * owner;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }

        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        owner = obj->selector.evt;
        if (evt != owner) {
                rc = -ENOTCONN;
                goto err_notconn;
        }
        xwbmpop_s1i(evt->bmp, obj->selector.pos);
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_cdt_broadcast(&evt->cdt);
        xwsync_evt_put(evt);
        return OK;

err_notconn:
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_evt_put(evt);
err_evt_grab:
        return rc;
}

xwer_t xwsync_evt_obj_c0i(struct xwsync_evt * evt, struct xwsync_object * obj)
{
        struct xwsync_evt * owner;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }

        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        owner = obj->selector.evt;
        if (evt != owner) {
                rc = -ENOTCONN;
                goto err_notconn;
        }
        xwbmpop_c0i(evt->bmp, obj->selector.pos);
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_evt_put(evt);
        return OK;

err_notconn:
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_evt_put(evt);
err_evt_grab:
        return rc;
}

xwer_t xwsync_evt_tryselect(struct xwsync_evt * evt, xwbmp_t msk[], xwbmp_t trg[])
{
        xwer_t rc;
        xwreg_t cpuirq;
        bool triggered;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_SELECTOR),
                      "type-error", -ETYPE);

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }

        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        triggered = xwbmpop_t1mo(evt->bmp, msk, XWSYNC_EVT_MAXNUM);
        if (triggered) {
                if (NULL != trg) {
                        xwbmpop_assign(trg, evt->bmp, XWSYNC_EVT_MAXNUM);
                        /* Clear non-exclusive bits */
                        xwbmpop_and(evt->bmp, evt->msk, XWSYNC_EVT_MAXNUM);
                        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        xwbmpop_and(trg, msk, XWSYNC_EVT_MAXNUM);
                } else {
                        /* Clear non-exclusive bits */
                        xwbmpop_and(evt->bmp, evt->msk, XWSYNC_EVT_MAXNUM);
                        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                }
                rc = OK;
        } else {
                rc = -ENODATA;
                xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        }
        xwsync_evt_put(evt);

err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwsync_evt_tryselect);

xwer_t xwsync_evt_timedselect(struct xwsync_evt * evt, xwbmp_t msk[], xwbmp_t trg[],
                              xwtm_t * xwtm)
{
        xwer_t rc;
        xwreg_t cpuirq;
        bool triggered;
        xwsq_t lkst;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_SELECTOR),
                      "type-error", -ETYPE);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHRD == xwos_irq_get_id(NULL)),
                      "not-in-thrd", -ENOTINTHRD);

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }

        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        while (true) {
                triggered = xwbmpop_t1mo(evt->bmp, msk, XWSYNC_EVT_MAXNUM);
                if (triggered) {
                        if (NULL != trg) {
                                xwbmpop_assign(trg, evt->bmp, XWSYNC_EVT_MAXNUM);
                                /* Clear non-exclusive bits */
                                xwbmpop_and(evt->bmp, evt->msk, XWSYNC_EVT_MAXNUM);
                                xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                                xwbmpop_and(trg, msk, XWSYNC_EVT_MAXNUM);
                        } else {
                                /* Clear non-exclusive bits */
                                xwbmpop_and(evt->bmp, evt->msk, XWSYNC_EVT_MAXNUM);
                                xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        }
                        rc = OK;
                        break;
                } else {
                        /* Clear non-exclusive bits */
                        xwbmpop_and(evt->bmp, evt->msk, XWSYNC_EVT_MAXNUM);
                        rc = xwsync_cdt_timedwait(&evt->cdt,
                                                  &evt->lock, XWLK_TYPE_SPLK, NULL,
                                                  xwtm, &lkst);
                        if (OK == rc) {
                                if (XWLK_STATE_UNLOCKED == lkst) {
                                        xwlk_splk_lock(&evt->lock);
                                }
                        } else {
                                if (XWLK_STATE_LOCKED == lkst) {
                                        xwlk_splk_unlock(&evt->lock);
                                }
                                xwos_cpuirq_restore_lc(cpuirq);
                                if (NULL != trg) {
                                        xwbmpop_c0all(trg, XWSYNC_EVT_MAXNUM);
                                }
                                break;
                        }
                }
        }
        xwsync_evt_put(evt);

err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwsync_evt_timedselect);

/******** type:XWSYNC_EVT_TYPE_BARRIER ********/
xwer_t xwsync_evt_timedsync(struct xwsync_evt * evt, xwsq_t pos, xwbmp_t sync[],
                            xwtm_t * xwtm)
{
        xwreg_t cpuirq;
        bool triggered;
        xwsq_t lkst;
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((sync), "nullptr", -EFAULT);
        XWOS_VALIDATE((pos < XWSYNC_EVT_MAXNUM), "out-of-range", -ECHRNG);
        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_BARRIER),
                      "type-error", -ETYPE);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHRD == xwos_irq_get_id(NULL)),
                      "not-in-thrd", -ENOTINTHRD);

        rc = xwsync_evt_grab(evt);
        if (__unlikely(rc < 0)) {
                goto err_evt_grab;
        }

        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_s1i(evt->bmp, pos);
        triggered = xwbmpop_t1ma(evt->bmp, sync, XWSYNC_EVT_MAXNUM);
        if (triggered) {
                xwbmpop_c0i(evt->bmp, pos);
                xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                xwsync_cdt_broadcast(&evt->cdt);
                xwos_cthrd_yield();
                rc = OK;
        } else {
                rc = xwsync_cdt_timedwait(&evt->cdt,
                                          &evt->lock, XWLK_TYPE_SPLK, NULL,
                                          xwtm, &lkst);
                if (XWLK_STATE_UNLOCKED == lkst) {
                        xwlk_splk_lock(&evt->lock);
                }
                xwbmpop_c0i(evt->bmp, pos);
                xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        }
        xwsync_evt_put(evt);

err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwsync_evt_timedsync);
