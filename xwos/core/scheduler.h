/**
 * @file
 * @brief XuanWuOS内核：调度器
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

#ifndef __xwos_core_scheduler_h__
#define __xwos_core_scheduler_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <linux/sched.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWOS_SD_PRIORITY_RT_MIN                 (MAX_RT_PRIO)
#define XWOS_SD_PRIORITY_RT_MAX                 (1)
#define XWOS_SD_PRIORITY_INVALID                MAX_PRIO
#define XWOS_SD_PRIORITY_RAISE(base, inc)       ((base) - (inc))
#define XWOS_SD_PRIORITY_DROP(base, dec)        ((base) + (dec))

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t xwos_scheduler_start_lc(void);

xwtm_t xwos_scheduler_get_timetick_lc(void);

xwu64_t xwos_scheduler_get_tickcount_lc(void);

xwtm_t xwos_scheduler_get_timestamp_lc(void);

void xwos_scheduler_dspmpt_lc(void);

void xwos_scheduler_enpmpt_lc(void);

bool xwos_scheduler_prio_tst_valid(xwpr_t prio);

xwer_t xwos_scheduler_init(void);

void xwos_scheduler_exit(void);

xwer_t xwos_scheduler_xwfs_init(void);

void xwos_scheduler_xwfs_exit(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   inline function implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwos/core/scheduler.h */
