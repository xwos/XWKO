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

#ifndef __xwos_mp_swt_h__
#define __xwos_mp_swt_h__

#include <linux/hrtimer.h>
#include <xwos/standard.h>
#include <xwos/lib/object.h>

enum xwmp_swt_flag_em {
        XWMP_SWT_FLAG_NULL = 0, /**< null */
        XWMP_SWT_FLAG_RESTART = (1 << 0), /**< auto-restart */
};

struct xwmp_swt;

typedef void (* xwmp_swt_f)(struct xwmp_swt *, void *);

struct xwmp_swt {
        struct xwos_object xwobj; /**< OO in C: Inherit struct xwos_object */
        const char * name; /**< name */
        xwsq_t flag; /**< flag */
        xwmp_swt_f cb; /**< callback funtion */
        void * arg; /**< argument of callback function */
        xwtm_t period; /**< timer period */
        struct hrtimer hrt; /**< Linux hrtimer */
};

xwer_t xwmp_swt_cache_create(void);
void xwmp_swt_cache_destroy(void);
xwer_t xwmp_swt_init(struct xwmp_swt * swt,
                     const char * name,
                     xwsq_t flag);
xwer_t xwmp_swt_fini(struct xwmp_swt * swt);
xwer_t xwmp_swt_create(struct xwmp_swt ** ptrbuf, const char * name, xwsq_t flag);
xwer_t xwmp_swt_delete(struct xwmp_swt * swt);

xwsq_t xwmp_swt_gettik(struct xwmp_swt * swt);
xwer_t xwmp_swt_acquire(struct xwmp_swt * swt, xwsq_t tik);
xwer_t xwmp_swt_release(struct xwmp_swt * swt, xwsq_t tik);
xwer_t xwmp_swt_grab(struct xwmp_swt * swt);
xwer_t xwmp_swt_put(struct xwmp_swt * swt);

xwer_t xwmp_swt_start(struct xwmp_swt * swt, xwtm_t base, xwtm_t period,
                      xwmp_swt_f cb, void * arg);
xwer_t xwmp_swt_stop(struct xwmp_swt * swt);

#endif /* xwos/mp/swt.h */
