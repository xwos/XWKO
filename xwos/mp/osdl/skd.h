/**
 * @file
 * @brief 玄武OS内核操作系统接口描述层：调度
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

#ifndef __xwos_mp_osdl_skd_h__
#define __xwos_mp_osdl_skd_h__

#include <xwos/mp/skd.h>

#define XWOSDL_SKDATTR_PRIVILEGED               XWMP_SKDATTR_PRIVILEGED
#define XWOSDL_SKDATTR_DETACHED                 XWMP_SKDATTR_DETACHED
#define XWOSDL_SKDATTR_JOINABLE                 XWMP_SKDATTR_JOINABLE
#define XWOSDL_SKD_PRIORITY_RT_MIN              XWMP_SKD_PRIORITY_RT_MIN
#define XWOSDL_SKD_PRIORITY_RT_MAX              XWMP_SKD_PRIORITY_RT_MAX
#define XWOSDL_SKD_PRIORITY_INVALID             XWMP_SKD_PRIORITY_INVALID
#define XWOSDL_SKD_PRIORITY_RAISE(base, inc)    XWMP_SKD_PRIORITY_RAISE(base, inc)
#define XWOSDL_SKD_PRIORITY_DROP(base, dec)     XWMP_SKD_PRIORITY_DROP(base, dec)

#define XWOSDL_SKD_CONTEXT_THD                  XWMP_SKD_CONTEXT_THD
#define XWOSDL_SKD_CONTEXT_ISR                  XWMP_SKD_CONTEXT_ISR

#define XWOSDL_THD_LOCAL_DATA_NUM               0

static __xwcc_inline
bool xwosdl_skd_prio_tst_valid(xwpr_t prio)
{
        return xwmp_skd_prio_tst_valid(prio);
}

static __xwcc_inline
xwer_t xwosdl_skd_start_lc(void)
{
        return xwmp_skd_start_lc();
}

static __xwcc_inline
xwid_t xwosdl_skd_id_lc(void)
{
        return xwmp_skd_id_lc();
}

static __xwcc_inline
void xwosdl_skd_get_context_lc(xwsq_t * ctxbuf, xwirq_t * irqnbuf)
{
        xwmp_skd_get_context_lc(ctxbuf, irqnbuf);
}

static __xwcc_inline
xwtm_t xwosdl_skd_get_timetick_lc(void)
{
        return xwmp_skd_get_timetick_lc();
}

static __xwcc_inline
xwu64_t xwosdl_skd_get_tickcount_lc(void)
{
        return xwmp_skd_get_tickcount_lc();
}

static __xwcc_inline
xwtm_t xwosdl_skd_get_timestamp_lc(void)
{
        return xwmp_skd_get_timestamp_lc();
}

static __xwcc_inline
void xwosdl_skd_dspmpt_lc(void)
{
        xwmp_skd_dspmpt_lc();
}

static __xwcc_inline
void xwosdl_skd_enpmpt_lc(void)
{
        xwmp_skd_enpmpt_lc();
}

#include <xwos/mp/thd.h>

#define xwosdl_thd xwmp_thd
typedef xwmp_thd_f xwosdl_thd_f;

static __xwcc_inline
xwer_t xwosdl_thd_init(struct xwosdl_thd * thd, const char * name,
                       xwosdl_thd_f mainfunc, void * arg,
                       xwstk_t * stack, xwsz_t stack_size,
                       xwpr_t priority, xwsq_t attr)
{
        return xwmp_thd_init(thd, name,
                             (xwmp_thd_f)mainfunc, arg,
                             stack, stack_size,
                             priority, attr);
}

static __xwcc_inline
xwer_t xwosdl_thd_create(struct xwosdl_thd ** thdbuf, const char * name,
                         xwosdl_thd_f mainfunc, void * arg,
                         xwsz_t stack_size,
                         xwpr_t priority, xwsq_t attr)
{
        return xwmp_thd_create(thdbuf, name, mainfunc, arg,
                               stack_size, priority, attr);
}

static __xwcc_inline
xwsq_t xwosdl_thd_gettik(struct xwosdl_thd * thd)
{
        return xwmp_thd_gettik(thd);
}

static __xwcc_inline
xwer_t xwosdl_thd_acquire(struct xwosdl_thd * thd, xwsq_t tik)
{
        return xwmp_thd_acquire(thd, tik);
}

static __xwcc_inline
xwer_t xwosdl_thd_release(struct xwosdl_thd * thd, xwsq_t tik)
{
        return xwmp_thd_release(thd, tik);
}

static __xwcc_inline
xwer_t xwosdl_thd_grab(struct xwosdl_thd * thd)
{
        return xwmp_thd_grab(thd);
}

static __xwcc_inline
xwer_t xwosdl_thd_put(struct xwosdl_thd * thd)
{
        return xwmp_thd_put(thd);
}

static __xwcc_inline
xwer_t xwosdl_thd_stop(struct xwosdl_thd * thd, xwer_t * trc)
{
        return xwmp_thd_stop(thd, trc);
}

static __xwcc_inline
xwer_t xwosdl_thd_cancel(struct xwosdl_thd * thd)
{
        return xwmp_thd_cancel(thd);
}

static __xwcc_inline
xwer_t xwosdl_thd_join(struct xwosdl_thd * thd, xwer_t * trc)
{
        return xwmp_thd_join(thd, trc);
}

static __xwcc_inline
xwer_t xwosdl_thd_detach(struct xwosdl_thd * thd)
{
        return xwmp_thd_detach(thd);
}

static __xwcc_inline
xwer_t xwosdl_thd_intr(struct xwosdl_thd * thd)
{
        return xwmp_thd_intr(thd);
}

static __xwcc_inline
xwer_t xwosdl_thd_migrate(struct xwosdl_thd * thd, xwid_t dstcpu)
{
        return xwmp_thd_migrate(thd, dstcpu);
}

static __xwcc_inline
struct xwosdl_thd * xwosdl_cthd_self(void)
{
        return xwmp_cthd_self();
}

static __xwcc_inline
void xwosdl_cthd_yield(void)
{
        xwmp_cthd_yield();
}

static __xwcc_inline
void xwosdl_cthd_exit(xwer_t rc)
{
        xwmp_cthd_exit(rc);
}

static __xwcc_inline
bool xwosdl_cthd_shld_frz(void)
{
        return xwmp_cthd_shld_frz();
}

static __xwcc_inline
bool xwosdl_cthd_shld_stop(void)
{
        return xwmp_cthd_shld_stop();
}

static __xwcc_inline
bool xwosdl_cthd_frz_shld_stop(bool * frozen)
{
        return xwmp_cthd_frz_shld_stop(frozen);
}

static __xwcc_inline
xwer_t xwosdl_cthd_sleep(xwtm_t * xwtm)
{
        return xwmp_cthd_sleep(xwtm);
}

static __xwcc_inline
xwer_t xwosdl_cthd_sleep_from(xwtm_t * origin, xwtm_t inc)
{
        return xwmp_cthd_sleep_from(origin, inc);
}

static __xwcc_inline
xwer_t xwosdl_cthd_freeze(void)
{
        return xwmp_cthd_freeze();
}

#endif /* xwos/mp/osdl/skd.h */
