/**
 * @file
 * @brief 玄武OS内核：调度器
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

#ifndef __xwos_mp_skd_h__
#define __xwos_mp_skd_h__

#include <linux/sched.h>
#include <xwos/standard.h>

#define XWMP_SKD_PRIORITY_RT_MIN                (MAX_RT_PRIO)
#define XWMP_SKD_PRIORITY_RT_MAX                (1)
#define XWMP_SKD_PRIORITY_INVALID               MAX_PRIO
#define XWMP_SKD_PRIORITY_RAISE(base, inc)      ((base) - (inc))
#define XWMP_SKD_PRIORITY_DROP(base, dec)       ((base) + (dec))
#define XWMP_SKDATTR_PRIVILEGED                 XWOS_UNUSED_ARGUMENT
#define XWMP_SKDATTR_DETACHED                   (1U << 1U) /**< DETACHED, Linux kernel is not support */
#define XWMP_SKDATTR_JOINABLE                   0 /**< JOINABLE */

enum xwmp_skd_context_em {
        XWMP_SKD_CONTEXT_THD = 1,
        XWMP_SKD_CONTEXT_ISR,
};

xwer_t xwmp_skd_start_lc(void);
xwid_t xwmp_skd_id_lc(void);
void xwmp_skd_get_context_lc(xwsq_t * ctxbuf, xwirq_t * irqnbuf);
xwtm_t xwmp_skd_get_timetick_lc(void);
xwu64_t xwmp_skd_get_tickcount_lc(void);
xwtm_t xwmp_skd_get_timestamp_lc(void);
void xwmp_skd_dspmpt_lc(void);
void xwmp_skd_enpmpt_lc(void);
bool xwmp_skd_prio_tst_valid(xwpr_t prio);
xwer_t xwmp_skd_init(void);
void xwmp_skd_exit(void);
xwer_t xwmp_skd_xwfs_init(void);
void xwmp_skd_xwfs_exit(void);

#endif /* xwos/mp/skd.h */
