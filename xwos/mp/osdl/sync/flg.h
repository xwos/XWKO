/**
 * @file
 * @brief 玄武OS内核操作系统接口描述层：事件标志
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

#ifndef __xwos_mp_osdl_sync_flg_h__
#define __xwos_mp_osdl_sync_flg_h__

#include <xwos/mp/osdl/sync/sel.h>
#include <xwos/mp/sync/evt.h>

#define xwosdl_flg xwmp_evt

#define XWOSDL_FLG_TRIGGER_SET_ALL      XWMP_FLG_TRIGGER_SET_ALL
#define XWOSDL_FLG_TRIGGER_SET_ANY      XWMP_FLG_TRIGGER_SET_ANY
#define XWOSDL_FLG_TRIGGER_CLR_ALL      XWMP_FLG_TRIGGER_CLR_ALL
#define XWOSDL_FLG_TRIGGER_CLR_ANY      XWMP_FLG_TRIGGER_CLR_ANY
#define XWOSDL_FLG_TRIGGER_TGL_ALL      XWMP_FLG_TRIGGER_TGL_ALL
#define XWOSDL_FLG_TRIGGER_TGL_ANY      XWMP_FLG_TRIGGER_TGL_ANY

#define XWOSDL_FLG_ACTION_NONE          XWMP_FLG_ACTION_NONE
#define XWOSDL_FLG_ACTION_CONSUMPTION   XWMP_FLG_ACTION_CONSUMPTION

static __xwcc_inline
xwer_t xwosdl_flg_init(struct xwosdl_flg * flg, xwsz_t num,
                       xwbmp_t * bmp, xwbmp_t * msk)
{
        return xwmp_evt_init(flg, XWMP_EVT_TYPE_FLG, num, bmp, msk);
}

static __xwcc_inline
xwer_t xwosdl_flg_destroy(struct xwosdl_flg * flg)
{
        return xwmp_evt_destroy(flg);
}

static __xwcc_inline
xwer_t xwosdl_flg_create(struct xwosdl_flg ** flgbuf, xwsz_t num)
{
        return xwmp_evt_create(flgbuf, XWMP_EVT_TYPE_FLG, num);
}

static __xwcc_inline
xwer_t xwosdl_flg_delete(struct xwosdl_flg * flg)
{
        return xwmp_evt_delete(flg);
}

static __xwcc_inline
xwsq_t xwosdl_flg_gettik(struct xwosdl_flg * flg)
{
        return xwmp_evt_gettik(flg);
}

static __xwcc_inline
xwer_t xwosdl_flg_acquire(struct xwosdl_flg * flg, xwsq_t tik)
{
        return xwmp_evt_acquire(flg, tik);
}

static __xwcc_inline
xwer_t xwosdl_flg_release(struct xwosdl_flg * flg, xwsq_t tik)
{
        return xwmp_evt_release(flg, tik);
}

static __xwcc_inline
xwer_t xwosdl_flg_grab(struct xwosdl_flg * flg)
{
        return xwmp_evt_grab(flg);
}

static __xwcc_inline
xwer_t xwosdl_flg_put(struct xwosdl_flg * flg)
{
        return xwmp_evt_put(flg);
}

static __xwcc_inline
xwer_t xwosdl_flg_bind(struct xwosdl_flg * flg, struct xwosdl_sel * sel, xwsq_t pos)
{
        return xwmp_evt_bind(flg, sel, pos);
}

static __xwcc_inline
xwer_t xwosdl_flg_unbind(struct xwosdl_flg * flg, struct xwosdl_sel * sel)
{
        return xwmp_evt_unbind(flg, sel);
}

static __xwcc_inline
xwer_t xwosdl_flg_intr_all(struct xwosdl_flg * flg)
{
        return xwmp_evt_intr_all(flg);
}

static __xwcc_inline
xwer_t xwosdl_flg_get_num(struct xwosdl_flg * flg, xwsz_t * numbuf)
{
        return xwmp_evt_get_num(flg, numbuf);
}

static __xwcc_inline
xwer_t xwosdl_flg_s1m(struct xwosdl_flg * flg, xwbmp_t msk[])
{
        return xwmp_flg_s1m(flg, msk);
}

static __xwcc_inline
xwer_t xwosdl_flg_s1i(struct xwosdl_flg * flg, xwsq_t pos)
{
        return xwmp_flg_s1i(flg, pos);
}

static __xwcc_inline
xwer_t xwosdl_flg_c0m(struct xwosdl_flg * flg, xwbmp_t msk[])
{
        return xwmp_flg_c0m(flg, msk);
}

static __xwcc_inline
xwer_t xwosdl_flg_c0i(struct xwosdl_flg * flg, xwsq_t pos)
{
        return xwmp_flg_c0i(flg, pos);
}

static __xwcc_inline
xwer_t xwosdl_flg_x1m(struct xwosdl_flg * flg, xwbmp_t msk[])
{
        return xwmp_flg_x1m(flg, msk);
}

static __xwcc_inline
xwer_t xwosdl_flg_x1i(struct xwosdl_flg * flg, xwsq_t pos)
{
        return xwmp_flg_x1i(flg, pos);
}

static __xwcc_inline
xwer_t xwosdl_flg_read(struct xwosdl_flg * flg, xwbmp_t out[])
{
        return xwmp_flg_read(flg, out);
}

static __xwcc_inline
xwer_t xwosdl_flg_wait(struct xwosdl_flg * flg, xwsq_t trigger, xwsq_t action,
                       xwbmp_t origin[], xwbmp_t msk[])
{
        return xwmp_flg_wait(flg, trigger, action, origin, msk);
}

static __xwcc_inline
xwer_t xwosdl_flg_trywait(struct xwosdl_flg * flg, xwsq_t trigger, xwsq_t action,
                          xwbmp_t origin[], xwbmp_t msk[])
{
        return xwmp_flg_trywait(flg, trigger, action, origin, msk);
}

static __xwcc_inline
xwer_t xwosdl_flg_timedwait(struct xwosdl_flg * flg, xwsq_t trigger, xwsq_t action,
                            xwbmp_t origin[], xwbmp_t msk[],
                            xwtm_t * xwtm)
{
        return xwmp_flg_timedwait(flg, trigger, action, origin, msk, xwtm);
}

#endif /* xwos/mp/osdl/sync/flg.h */
