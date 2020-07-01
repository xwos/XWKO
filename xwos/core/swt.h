/**
 * @file
 * @brief XuanWuOS内核：软件定时器
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

#ifndef __xwos_core_swt_h__
#define __xwos_core_swt_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/object.h>
#include <linux/hrtimer.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
enum xwos_swt_flag_em {
        XWOS_SWT_FLAG_NULL = 0, /**< null */
        XWOS_SWT_FLAG_RESTART = (1 << 0), /**< auto-restart */
        XWOS_SWT_FLAG_AUTORM = (1 << 1), /**< auto-remove  */
};

struct xwos_swt;

typedef void (*xwos_swt_f)(struct xwos_swt *, void *);

struct xwos_swt {
        struct xwos_object xwobj; /**< OO in C: Inherit struct xwos_object */
        const char * name; /**< name */
        xwsq_t flag; /**< flag */
        xwos_swt_f cb; /**< callback funtion */
        void * arg; /**< argument of callback function */
        xwtm_t period; /**< timer period */
        struct hrtimer hrt; /**< Linux hrtimer */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t xwos_swt_cache_create(void);

void xwos_swt_cache_destroy(void);

xwer_t xwos_swt_init(struct xwos_swt * swt,
                     const char * name,
                     xwsq_t flag);

xwer_t xwos_swt_destroy(struct xwos_swt * swt);

xwer_t xwos_swt_create(struct xwos_swt ** ptrbuf, const char * name, xwsq_t flag);

xwer_t xwos_swt_delete(struct xwos_swt * swt);

xwer_t xwos_swt_start(struct xwos_swt * swt, xwtm_t base, xwtm_t period,
                      xwos_swt_f cb, void * arg);

xwer_t xwos_swt_stop(struct xwos_swt * swt);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xw_inline
xwer_t xwos_swt_grab(struct xwos_swt * swt)
{
        return xwos_object_grab(&swt->xwobj);
}

static __xw_inline
xwer_t xwos_swt_put(struct xwos_swt * swt)
{
        return xwos_object_put(&swt->xwobj);
}

#endif /* xwos/core/swt.h */
