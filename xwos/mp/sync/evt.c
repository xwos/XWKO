/**
 * @file
 * @brief 玄武OS同步机制：事件
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
#include <xwos/lib/string.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwlog.h>
#include <xwos/mp/irq.h>
#include <xwos/mp/thd.h>
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/sync/obj.h>
#include <xwos/mp/sync/cond.h>
#include <xwos/mp/sync/evt.h>

static
void xwmp_evt_construct(void * anon);

static
void xwmp_evt_setup(struct xwmp_evt * evt, xwsz_t num,
                    xwbmp_t * bmp, xwbmp_t * msk);

static
xwer_t xwmp_evt_gc(void * evt);

static
xwer_t xwmp_evt_activate(struct xwmp_evt * evt, xwsq_t type, xwobj_gc_f gcfunc);

static
xwer_t xwmp_flg_trywait_level(struct xwmp_evt * evt,
                              xwsq_t trigger, xwsq_t action,
                              xwbmp_t origin[], xwbmp_t msk[]);

static
xwer_t xwmp_flg_trywait_edge(struct xwmp_evt * evt, xwsq_t trigger,
                             xwbmp_t origin[], xwbmp_t msk[]);

static
xwer_t xwmp_flg_timedwait_level(struct xwmp_evt * evt,
                                xwsq_t trigger, xwsq_t action,
                                xwbmp_t origin[], xwbmp_t msk[],
                                xwtm_t * xwtm);

static
xwer_t xwmp_flg_timedwait_edge(struct xwmp_evt * evt, xwsq_t trigger,
                               xwbmp_t origin[], xwbmp_t msk[],
                               xwtm_t * xwtm);

static
void xwmp_evt_construct(void * anon)
{
        struct xwmp_evt * evt;

        evt = anon;
        xwmp_synobj_construct(&evt->cond.synobj);
}

void xwmp_evt_setup(struct xwmp_evt * evt, xwsz_t num,
                    xwbmp_t * bmp, xwbmp_t * msk)
{
        evt->num = num;
        evt->bmp = bmp;
        evt->msk = msk;
}

static __xwmp_code
struct xwmp_evt * xwmp_evt_alloc(xwsz_t num)
{
        struct xwmp_evt * evt;
        xwbmp_t * bmp, * msk;
        xwsz_t bmpnum, bmpsize;

        bmpnum = BITS_TO_XWBMP_T(num);
        bmpsize = bmpnum * sizeof(xwbmp_t);
        evt = kmalloc(sizeof(struct xwmp_evt) + bmpsize + bmpsize, GFP_KERNEL);
        if (NULL == evt) {
                evt = err_ptr(-ENOMEM);
        } else {
                bmp = (void *)&evt[1];
                msk = &bmp[bmpnum];
                xwmp_evt_construct(evt);
                xwmp_evt_setup(evt, num, bmp, msk);
        }
        return evt;
}

static __xwmp_code
void xwmp_evt_free(struct xwmp_evt * evt)
{
        kfree(evt);
}

static
xwer_t xwmp_evt_gc(void * synobj)
{
        xwmp_evt_free(synobj);
        return XWOK;
}

xwer_t xwmp_evt_acquire(struct xwmp_evt * evt, xwsq_t tik)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        return xwmp_synobj_acquire(&evt->cond.synobj, tik);
}
EXPORT_SYMBOL(xwmp_evt_acquire);

xwer_t xwmp_evt_release(struct xwmp_evt * evt, xwsq_t tik)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        return xwmp_synobj_release(&evt->cond.synobj, tik);
}
EXPORT_SYMBOL(xwmp_evt_release);

xwer_t xwmp_evt_grab(struct xwmp_evt * evt)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        return xwmp_synobj_grab(&evt->cond.synobj);
}
EXPORT_SYMBOL(xwmp_evt_grab);

xwer_t xwmp_evt_put(struct xwmp_evt * evt)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        return xwmp_synobj_put(&evt->cond.synobj);
}
EXPORT_SYMBOL(xwmp_evt_put);

xwsq_t xwmp_evt_gettik(struct xwmp_evt * evt)
{
        xwsq_t tik;

        if (evt) {
                tik = evt->cond.synobj.xwobj.tik;
        } else {
                tik = 0;
        }
        return tik;
}
EXPORT_SYMBOL(xwmp_evt_gettik);

static
xwer_t xwmp_evt_activate(struct xwmp_evt * evt, xwsq_t type, xwobj_gc_f gcfunc)
{
        xwer_t rc;
        xwsz_t size;

        size = BITS_TO_XWBMP_T(evt->num);
        rc = xwmp_cond_activate(&evt->cond, gcfunc);
        if (__xwcc_likely(XWOK == rc)) {
                evt->type = type;
                switch (type & XWMP_EVT_TYPE_MASK) {
                case XWMP_EVT_TYPE_FLG:
                        memset(evt->msk, 0xFF, size);
                        break;
                case XWMP_EVT_TYPE_BR:
                        memset(evt->msk, 0, size);
                        break;
                case XWMP_EVT_TYPE_SEL:
                        memset(evt->msk, 0, size);
                        break;
                default:
                        memset(evt->msk, 0xFF, size);
                        break;
                }
                memset(evt->bmp, 0, size);
                xwmp_splk_init(&evt->lock);
        }
        return rc;
}

xwer_t xwmp_evt_init(struct xwmp_evt * evt, xwsq_t type, xwsz_t num,
                     xwbmp_t * bmp, xwbmp_t * msk)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((bmp), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((type & XWMP_EVT_TYPE_MASK) < XWMP_EVT_TYPE_MAX),
                      "type-error", -EINVAL);

        xwmp_evt_construct(evt);
        xwmp_evt_setup(evt, num, bmp, msk);
        return xwmp_evt_activate(evt, type, NULL);
}
EXPORT_SYMBOL(xwmp_evt_init);

xwer_t xwmp_evt_destroy(struct xwmp_evt * evt)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        return xwmp_evt_put(evt);
}
EXPORT_SYMBOL(xwmp_evt_destroy);

xwer_t xwmp_evt_create(struct xwmp_evt ** ptrbuf, xwsq_t type, xwsz_t num)
{
        struct xwmp_evt * evt;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);
        XWOS_VALIDATE(((type & XWMP_EVT_TYPE_MASK) < XWMP_EVT_TYPE_MAX),
                      "type-error", -EINVAL);

        *ptrbuf = NULL;
        evt = xwmp_evt_alloc(num);
        if (__xwcc_unlikely(is_err(evt))) {
                rc = ptr_err(evt);
        } else {
                rc = xwmp_evt_activate(evt, type, xwmp_evt_gc);
                if (__xwcc_unlikely(rc < 0)) {
                        xwmp_evt_free(evt);
                } else {
                        *ptrbuf = evt;
                }
        }
        return rc;
}
EXPORT_SYMBOL(xwmp_evt_create);

xwer_t xwmp_evt_delete(struct xwmp_evt * evt)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        return xwmp_evt_put(evt);
}
EXPORT_SYMBOL(xwmp_evt_delete);

xwer_t xwmp_evt_bind(struct xwmp_evt * evt, struct xwmp_evt * sel, xwsq_t pos)
{
        return xwmp_cond_bind(&evt->cond, sel, pos);
}
EXPORT_SYMBOL(xwmp_evt_bind);

xwer_t xwmp_evt_unbind(struct xwmp_evt * evt, struct xwmp_evt * sel)
{
        return xwmp_cond_unbind(&evt->cond, sel);
}
EXPORT_SYMBOL(xwmp_evt_unbind);

xwer_t xwmp_evt_intr_all(struct xwmp_evt * evt)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        return xwmp_cond_intr_all(&evt->cond);
}
EXPORT_SYMBOL(xwmp_evt_intr_all);

xwer_t xwmp_evt_get_num(struct xwmp_evt * evt, xwsz_t * numbuf)
{
        xwer_t rc;
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        rc = xwmp_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        *numbuf = evt->num;
        xwmp_evt_put(evt);
        return XWOK;

err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_evt_get_num);

/******** type:XWMP_EVT_TYPE_FLG ********/
xwer_t xwmp_flg_s1m(struct xwmp_evt * evt, xwbmp_t msk[])
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->type & XWMP_EVT_TYPE_MASK) == XWMP_EVT_TYPE_FLG),
                      "type-error", -ETYPE);

        rc = xwmp_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        xwmp_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_s1m(evt->bmp, msk, evt->num);
        xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwmp_cond_broadcast(&evt->cond);
        xwmp_evt_put(evt);
        return XWOK;

err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_flg_s1m);

xwer_t xwmp_flg_s1i(struct xwmp_evt * evt, xwsq_t pos)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->type & XWMP_EVT_TYPE_MASK) == XWMP_EVT_TYPE_FLG),
                      "type-error", -ETYPE);

        rc = xwmp_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        if (pos >= evt->num) {
                rc = -ECHRNG;
                goto err_pos_range;
        }
        xwmp_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_s1i(evt->bmp, pos);
        xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwmp_cond_broadcast(&evt->cond);
        xwmp_evt_put(evt);
        return XWOK;

err_pos_range:
        xwmp_evt_put(evt);
err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_flg_s1i);

xwer_t xwmp_flg_c0m(struct xwmp_evt * evt, xwbmp_t msk[])
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->type & XWMP_EVT_TYPE_MASK) == XWMP_EVT_TYPE_FLG),
                      "type-error", -ETYPE);

        rc = xwmp_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        xwmp_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_c0m(evt->bmp, msk, evt->num);
        xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwmp_cond_broadcast(&evt->cond);
        xwmp_evt_put(evt);
        return XWOK;

err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_flg_c0m);

xwer_t xwmp_flg_c0i(struct xwmp_evt * evt, xwsq_t pos)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->type & XWMP_EVT_TYPE_MASK) == XWMP_EVT_TYPE_FLG),
                      "type-error", -ETYPE);

        rc = xwmp_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        if (pos >= evt->num) {
                rc = -ECHRNG;
                goto err_pos_range;
        }
        xwmp_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_c0i(evt->bmp, pos);
        xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwmp_cond_broadcast(&evt->cond);
        xwmp_evt_put(evt);
        return XWOK;

err_pos_range:
        xwmp_evt_put(evt);
err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_flg_c0i);

xwer_t xwmp_flg_x1m(struct xwmp_evt * evt, xwbmp_t msk[])
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->type & XWMP_EVT_TYPE_MASK) == XWMP_EVT_TYPE_FLG),
                      "type-error", -ETYPE);

        rc = xwmp_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        xwmp_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_x1m(evt->bmp, msk, evt->num);
        xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwmp_cond_broadcast(&evt->cond);
        xwmp_evt_put(evt);
        return XWOK;

err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_flg_x1m);

xwer_t xwmp_flg_x1i(struct xwmp_evt * evt, xwsq_t pos)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->type & XWMP_EVT_TYPE_MASK) == XWMP_EVT_TYPE_FLG),
                      "type-error", -ETYPE);

        rc = xwmp_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        if (pos >= evt->num) {
                rc = -ECHRNG;
                goto err_pos_range;
        }
        xwmp_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_x1i(evt->bmp, pos);
        xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwmp_cond_broadcast(&evt->cond);
        xwmp_evt_put(evt);
        return XWOK;

err_pos_range:
        xwmp_evt_put(evt);
err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_flg_x1i);

xwer_t xwmp_flg_read(struct xwmp_evt * evt, xwbmp_t out[])
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((out), "nullptr", -EFAULT);

        rc = xwmp_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        xwmp_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_assign(out, evt->bmp, evt->num);
        xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwmp_evt_put(evt);
        return XWOK;

err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_flg_read);

static
xwer_t xwmp_flg_trywait_level(struct xwmp_evt * evt,
                              xwsq_t trigger, xwsq_t action,
                              xwbmp_t origin[], xwbmp_t msk[])
{
        xwreg_t cpuirq;
        bool triggered;
        xwer_t rc;

        XWOS_VALIDATE((trigger <= XWMP_FLG_TRIGGER_CLR_ANY),
                      "illegal-trigger", -EINVAL);
        XWOS_VALIDATE((action < XWMP_FLG_ACTION_NUM),
                      "illegal-action", -EINVAL);

        rc = XWOK;
        xwmp_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        if (origin) {
                xwbmpop_assign(origin, evt->bmp, evt->num);
        }
        if (XWMP_FLG_ACTION_CONSUMPTION == action) {
                switch (trigger) {
                case XWMP_FLG_TRIGGER_SET_ALL:
                        triggered = xwbmpop_t1ma_then_c0m(evt->bmp, msk, evt->num);
                        break;
                case XWMP_FLG_TRIGGER_SET_ANY:
                        triggered = xwbmpop_t1mo_then_c0m(evt->bmp, msk, evt->num);
                        break;
                case XWMP_FLG_TRIGGER_CLR_ALL:
                        triggered = xwbmpop_t0ma_then_s1m(evt->bmp, msk, evt->num);
                        break;
                case XWMP_FLG_TRIGGER_CLR_ANY:
                        triggered = xwbmpop_t0mo_then_s1m(evt->bmp, msk, evt->num);
                        break;
                default:
                        triggered = true;
                        rc = -EINVAL;
                        break;
                }
        } else {
                switch (trigger) {
                case XWMP_FLG_TRIGGER_SET_ALL:
                        triggered = xwbmpop_t1ma(evt->bmp, msk, evt->num);
                        break;
                case XWMP_FLG_TRIGGER_SET_ANY:
                        triggered = xwbmpop_t1mo(evt->bmp, msk, evt->num);
                        break;
                case XWMP_FLG_TRIGGER_CLR_ALL:
                        triggered = xwbmpop_t0ma(evt->bmp, msk, evt->num);
                        break;
                case XWMP_FLG_TRIGGER_CLR_ANY:
                        triggered = xwbmpop_t0mo(evt->bmp, msk, evt->num);
                        break;
                default:
                        triggered = true;
                        rc = -EINVAL;
                        break;
                }
        }
        xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        if (!triggered) {
                rc = -ENODATA;
        }
        return rc;
}

static
xwer_t xwmp_flg_trywait_edge(struct xwmp_evt * evt, xwsq_t trigger,
                             xwbmp_t origin[], xwbmp_t msk[])
{
        xwreg_t cpuirq;
        xwssq_t cmprc;
        bool triggered;
        xwer_t rc;
        xwbmpop_declare(cur, evt->num);
        xwbmpop_declare(tmp, evt->num);

        XWOS_VALIDATE((origin), "nullptr", -EFAULT);

        xwbmpop_and(origin, msk, evt->num);
        xwmp_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_assign(cur, evt->bmp, evt->num);
        xwbmpop_and(cur, msk, evt->num);
        if (XWMP_FLG_TRIGGER_TGL_ALL == trigger) {
                xwbmpop_assign(tmp, cur, evt->num);
                xwbmpop_xor(tmp, origin, evt->num);
                cmprc = xwbmpop_cmp(tmp, msk, evt->num);
                if (0 == cmprc) {
                        triggered = true;
                        rc = XWOK;
                } else {
                        triggered = false;
                        rc = -ENODATA;
                }
        } else if (XWMP_FLG_TRIGGER_TGL_ANY == trigger) {
                cmprc = xwbmpop_cmp(origin, cur, evt->num);
                if (0 == cmprc) {
                        triggered = false;
                        rc = -ENODATA;
                } else {
                        triggered = true;
                        rc = XWOK;
                }
        } else {
                triggered = true;
                rc = -EINVAL;
        }
        if (triggered) {
                xwbmpop_assign(origin, cur, evt->num);
                xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        } else {
                xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        }
        return rc;
}

xwer_t xwmp_flg_wait(struct xwmp_evt * evt,
                     xwsq_t trigger, xwsq_t action,
                     xwbmp_t origin[], xwbmp_t msk[])
{
        xwtm_t expected = XWTM_MAX;
        return xwmp_flg_timedwait(evt, trigger, action, origin, msk, &expected);
}
EXPORT_SYMBOL(xwmp_flg_wait);

__xwos_api
xwer_t xwmp_flg_trywait(struct xwmp_evt * evt,
                        xwsq_t trigger, xwsq_t action,
                        xwbmp_t origin[], xwbmp_t msk[])
{
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->type & XWMP_EVT_TYPE_MASK) == XWMP_EVT_TYPE_FLG),
                      "type-error", -ETYPE);

        rc = xwmp_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        if (trigger <= XWMP_FLG_TRIGGER_CLR_ANY) {
                rc = xwmp_flg_trywait_level(evt, trigger, action,
                                            origin, msk);
        } else {
                rc = xwmp_flg_trywait_edge(evt, trigger, origin, msk);
        }
        xwmp_evt_put(evt);

err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_flg_trywait);

static
xwer_t xwmp_flg_timedwait_level(struct xwmp_evt * evt,
                                xwsq_t trigger, xwsq_t action,
                                xwbmp_t origin[], xwbmp_t msk[],
                                xwtm_t * xwtm)
{
        xwreg_t cpuirq;
        bool triggered;
        xwsq_t lkst;
        xwer_t rc;

        XWOS_VALIDATE((trigger <= XWMP_FLG_TRIGGER_CLR_ANY),
                      "illegal-trigger", -EINVAL);
        XWOS_VALIDATE((action < XWMP_FLG_ACTION_NUM),
                      "illegal-action", -EINVAL);

        rc = XWOK;
        xwmp_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        while (true) {
                if (origin) {
                        xwbmpop_assign(origin, evt->bmp, evt->num);
                }
                if (XWMP_FLG_ACTION_CONSUMPTION == action) {
                        switch (trigger) {
                        case XWMP_FLG_TRIGGER_SET_ALL:
                                triggered = xwbmpop_t1ma_then_c0m(evt->bmp, msk,
                                                                  evt->num);
                                break;
                        case XWMP_FLG_TRIGGER_SET_ANY:
                                triggered = xwbmpop_t1mo_then_c0m(evt->bmp, msk,
                                                                  evt->num);
                                break;
                        case XWMP_FLG_TRIGGER_CLR_ALL:
                                triggered = xwbmpop_t0ma_then_s1m(evt->bmp, msk,
                                                                  evt->num);
                                break;
                        case XWMP_FLG_TRIGGER_CLR_ANY:
                                triggered = xwbmpop_t0mo_then_s1m(evt->bmp, msk,
                                                                  evt->num);
                                break;
                        default:
                                triggered = true;
                                rc = -EINVAL;
                                break;
                        }
                } else {
                        switch (trigger) {
                        case XWMP_FLG_TRIGGER_SET_ALL:
                                triggered = xwbmpop_t1ma(evt->bmp, msk, evt->num);
                                break;
                        case XWMP_FLG_TRIGGER_SET_ANY:
                                triggered = xwbmpop_t1mo(evt->bmp, msk, evt->num);
                                break;
                        case XWMP_FLG_TRIGGER_CLR_ALL:
                                triggered = xwbmpop_t0ma(evt->bmp, msk, evt->num);
                                break;
                        case XWMP_FLG_TRIGGER_CLR_ANY:
                                triggered = xwbmpop_t0mo(evt->bmp, msk, evt->num);
                                break;
                        default:
                                triggered = true;
                                rc = -EINVAL;
                                break;
                        }
                }
                if (triggered) {
                        xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        break;
                } else {
                        rc = xwmp_cond_timedwait(&evt->cond,
                                                 &evt->lock, XWOS_LK_SPLK, NULL,
                                                 xwtm, &lkst);
                        if (XWOK == rc) {
                                if (XWOS_LKST_UNLOCKED == lkst) {
                                        xwmp_splk_lock(&evt->lock);
                                }
                        } else {
                                if (XWOS_LKST_LOCKED == lkst) {
                                        xwmp_splk_unlock(&evt->lock);
                                }
                                xwmp_cpuirq_restore_lc(cpuirq);
                                break;
                        }
                }
        }
        return rc;
}

static
xwer_t xwmp_flg_timedwait_edge(struct xwmp_evt * evt, xwsq_t trigger,
                               xwbmp_t origin[], xwbmp_t msk[],
                               xwtm_t * xwtm)
{
        xwreg_t cpuirq;
        xwsq_t lkst;
        xwssq_t cmprc;
        bool triggered;
        xwer_t rc;
        xwbmpop_declare(cur, evt->num);
        xwbmpop_declare(tmp, evt->num);

        XWOS_VALIDATE((origin), "nullptr", -EFAULT);

        xwbmpop_and(origin, msk, evt->num);
        xwmp_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        while (true) {
                xwbmpop_assign(cur, evt->bmp, evt->num);
                xwbmpop_and(cur, msk, evt->num);
                if (XWMP_FLG_TRIGGER_TGL_ALL == trigger) {
                        xwbmpop_assign(tmp, cur, evt->num);
                        xwbmpop_xor(tmp, origin, evt->num);
                        cmprc = xwbmpop_cmp(tmp, msk, evt->num);
                        if (0 == cmprc) {
                                triggered = true;
                                rc = XWOK;
                        } else {
                                triggered = false;
                        }
                } else if (XWMP_FLG_TRIGGER_TGL_ANY == trigger) {
                        cmprc = xwbmpop_cmp(origin, cur, evt->num);
                        if (0 == cmprc) {
                                triggered = false;
                        } else {
                                triggered = true;
                                rc = XWOK;
                        }
                } else {
                        triggered = true;
                        rc = -EINVAL;
                }
                if (triggered) {
                        xwbmpop_assign(origin, cur, evt->num);
                        xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        break;
                } else {
                        rc = xwmp_cond_timedwait(&evt->cond,
                                                 &evt->lock, XWOS_LK_SPLK, NULL,
                                                 xwtm, &lkst);
                        if (XWOK == rc) {
                                if (XWOS_LKST_UNLOCKED == lkst) {
                                        xwmp_splk_lock(&evt->lock);
                                }
                        } else {
                                if (XWOS_LKST_LOCKED == lkst) {
                                        xwmp_splk_unlock(&evt->lock);
                                }
                                xwmp_cpuirq_restore_lc(cpuirq);
                                break;
                        }
                }
        }
        return rc;
}

xwer_t xwmp_flg_timedwait(struct xwmp_evt * evt,
                          xwsq_t trigger, xwsq_t action,
                          xwbmp_t origin[], xwbmp_t msk[],
                          xwtm_t * xwtm)
{
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->type & XWMP_EVT_TYPE_MASK) == XWMP_EVT_TYPE_FLG),
                      "type-error", -ETYPE);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHD == xwmp_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        rc = xwmp_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        if (trigger <= XWMP_FLG_TRIGGER_CLR_ANY) {
                rc = xwmp_flg_timedwait_level(evt, trigger, action, origin, msk, xwtm);
        } else {
                rc = xwmp_flg_timedwait_edge(evt, trigger, origin, msk, xwtm);
        }
        xwmp_evt_put(evt);

err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_flg_timedwait);

/******** type:XWMP_EVT_TYPE_SEL ********/
xwer_t xwmp_sel_obj_bind(struct xwmp_evt * evt,
                         struct xwmp_synobj * synobj,
                         xwsq_t pos,
                         bool exclusive)
{
        struct xwmp_evt * owner;
        xwreg_t cpuirq;
        bool existed;
        xwer_t rc;

        XWOS_VALIDATE(((evt->type & XWMP_EVT_TYPE_MASK) == XWMP_EVT_TYPE_SEL),
                      "type-error", -ETYPE);

        rc = xwmp_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        if (pos >= evt->num) {
                rc = -ECHRNG;
                goto err_pos_range;
        }
        xwmp_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        owner = synobj->sel.evt;
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
        synobj->sel.evt = evt;
        synobj->sel.pos = pos;
        xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return XWOK;

err_busy:
err_already:
        xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
err_pos_range:
        xwmp_evt_put(evt);
err_evt_grab:
        return rc;
}

xwer_t xwmp_sel_obj_unbind(struct xwmp_evt * evt,
                           struct xwmp_synobj * synobj,
                           bool exclusive)
{
        struct xwmp_evt * owner;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE(((evt->type & XWMP_EVT_TYPE_MASK) == XWMP_EVT_TYPE_SEL),
                      "type-error", -ETYPE);

        xwmp_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        owner = synobj->sel.evt;
        if (evt != owner) {
                rc = -ENOTCONN;
                goto err_notconn;
        }
        if (exclusive) {
                xwbmpop_c0i(evt->msk, synobj->sel.pos);
        }
        synobj->sel.evt = NULL;
        synobj->sel.pos = 0;
        xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwmp_evt_put(evt);
        return XWOK;

err_notconn:
        xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return rc;
}

xwer_t xwmp_sel_obj_s1i(struct xwmp_evt * evt, struct xwmp_synobj * synobj)
{
        struct xwmp_evt * owner;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwmp_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }

        xwmp_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        owner = synobj->sel.evt;
        if (evt != owner) {
                rc = -ENOTCONN;
                goto err_notconn;
        }
        xwbmpop_s1i(evt->bmp, synobj->sel.pos);
        xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwmp_cond_broadcast(&evt->cond);
        xwmp_evt_put(evt);
        return XWOK;

err_notconn:
        xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwmp_evt_put(evt);
err_evt_grab:
        return rc;
}

xwer_t xwmp_sel_obj_c0i(struct xwmp_evt * evt, struct xwmp_synobj * synobj)
{
        struct xwmp_evt * owner;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwmp_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }

        xwmp_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        owner = synobj->sel.evt;
        if (evt != owner) {
                rc = -ENOTCONN;
                goto err_notconn;
        }
        xwbmpop_c0i(evt->bmp, synobj->sel.pos);
        xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwmp_evt_put(evt);
        return XWOK;

err_notconn:
        xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwmp_evt_put(evt);
err_evt_grab:
        return rc;
}

xwer_t xwmp_sel_select(struct xwmp_evt * evt, xwbmp_t msk[], xwbmp_t trg[])
{
        xwtm_t expected = XWTM_MAX;
        return xwmp_sel_timedselect(evt, msk, trg, &expected);
}
EXPORT_SYMBOL(xwmp_sel_select);

xwer_t xwmp_sel_tryselect(struct xwmp_evt * evt, xwbmp_t msk[], xwbmp_t trg[])
{
        xwer_t rc;
        xwreg_t cpuirq;
        bool triggered;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->type & XWMP_EVT_TYPE_MASK) == XWMP_EVT_TYPE_SEL),
                      "type-error", -ETYPE);

        rc = xwmp_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        xwmp_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        triggered = xwbmpop_t1mo(evt->bmp, msk, evt->num);
        if (triggered) {
                if (NULL != trg) {
                        xwbmpop_assign(trg, evt->bmp, evt->num);
                        /* Clear non-exclusive bits */
                        xwbmpop_and(evt->bmp, evt->msk, evt->num);
                        xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        xwbmpop_and(trg, msk, evt->num);
                } else {
                        /* Clear non-exclusive bits */
                        xwbmpop_and(evt->bmp, evt->msk, evt->num);
                        xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                }
                rc = XWOK;
        } else {
                rc = -ENODATA;
                xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        }
        xwmp_evt_put(evt);
        return XWOK;

err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_sel_tryselect);

xwer_t xwmp_sel_timedselect(struct xwmp_evt * evt, xwbmp_t msk[], xwbmp_t trg[],
                            xwtm_t * xwtm)
{
        xwer_t rc;
        xwreg_t cpuirq;
        bool triggered;
        xwsq_t lkst;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->type & XWMP_EVT_TYPE_MASK) == XWMP_EVT_TYPE_SEL),
                      "type-error", -ETYPE);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHD == xwmp_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        rc = xwmp_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        xwmp_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        while (true) {
                triggered = xwbmpop_t1mo(evt->bmp, msk, evt->num);
                if (triggered) {
                        if (NULL != trg) {
                                xwbmpop_assign(trg, evt->bmp, evt->num);
                                /* Clear non-exclusive bits */
                                xwbmpop_and(evt->bmp, evt->msk, evt->num);
                                xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                                xwbmpop_and(trg, msk, evt->num);
                        } else {
                                /* Clear non-exclusive bits */
                                xwbmpop_and(evt->bmp, evt->msk, evt->num);
                                xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        }
                        rc = XWOK;
                        break;
                } else {
                        /* Clear non-exclusive bits */
                        xwbmpop_and(evt->bmp, evt->msk, evt->num);
                        rc = xwmp_cond_timedwait(&evt->cond,
                                                 &evt->lock, XWOS_LK_SPLK, NULL,
                                                 xwtm, &lkst);
                        if (XWOK == rc) {
                                if (XWOS_LKST_UNLOCKED == lkst) {
                                        xwmp_splk_lock(&evt->lock);
                                }
                        } else {
                                if (XWOS_LKST_LOCKED == lkst) {
                                        xwmp_splk_unlock(&evt->lock);
                                }
                                xwmp_cpuirq_restore_lc(cpuirq);
                                if (NULL != trg) {
                                        xwbmpop_c0all(trg, evt->num);
                                }
                                break;
                        }
                }
        }
        xwmp_evt_put(evt);
        return XWOK;

err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_sel_timedselect);

/******** type:XWMP_EVT_TYPE_BR ********/
xwer_t xwmp_br_sync(struct xwmp_evt * evt, xwsq_t pos, xwbmp_t msk[])
{
        xwtm_t expected = XWTM_MAX;
        return xwmp_br_timedsync(evt, pos, msk, &expected);
}
EXPORT_SYMBOL(xwmp_br_sync);

xwer_t xwmp_br_timedsync(struct xwmp_evt * evt, xwsq_t pos, xwbmp_t msk[],
                         xwtm_t * xwtm)
{
        xwreg_t cpuirq;
        bool triggered;
        xwsq_t lkst;
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->type & XWMP_EVT_TYPE_MASK) == XWMP_EVT_TYPE_BR),
                      "type-error", -ETYPE);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHD == xwmp_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        rc = xwmp_evt_grab(evt);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_evt_grab;
        }
        if (pos >= evt->num) {
                rc = -ECHRNG;
                goto err_pos_range;
        }
        xwmp_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_s1i(evt->bmp, pos);
        triggered = xwbmpop_t1ma(evt->bmp, msk, evt->num);
        if (triggered) {
                xwbmpop_c0i(evt->bmp, pos);
                xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                xwmp_cond_broadcast(&evt->cond);
                xwmp_cthd_yield();
                rc = XWOK;
        } else {
                rc = xwmp_cond_timedwait(&evt->cond,
                                         &evt->lock, XWOS_LK_SPLK, NULL,
                                         xwtm, &lkst);
                if (XWOS_LKST_UNLOCKED == lkst) {
                        xwmp_splk_lock(&evt->lock);
                }
                xwbmpop_c0i(evt->bmp, pos);
                xwmp_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        }
        xwmp_evt_put(evt);
        return XWOK;

err_pos_range:
        xwmp_evt_put(evt);
err_evt_grab:
        return rc;
}
EXPORT_SYMBOL(xwmp_br_timedsync);
