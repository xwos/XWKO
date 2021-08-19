/**
 * @file
 * @brief 玄武OS内核：线程
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

#ifndef __xwos_mp_thd_h__
#define __xwos_mp_thd_h__

#include <linux/sched.h>
#include <xwos/standard.h>
#include <xwos/mp/ksym.h>
#include <xwos/mp/skd.h>

struct task_struct;
#define xwmp_thd task_struct
struct sighand_struct;
typedef xwer_t (* xwmp_thd_f)(void *);

KSYM_DCL(sched_setscheduler);

xwer_t xwmp_thd_grab(struct xwmp_thd * thd);
xwer_t xwmp_thd_put(struct xwmp_thd * thd);
xwer_t xwmp_thd_acquire(struct xwmp_thd * thd, xwsq_t tik);
xwer_t xwmp_thd_release(struct xwmp_thd * thd, xwsq_t tik);
xwsq_t xwmp_thd_gettik(struct xwmp_thd * thd);
xwer_t xwmp_thd_init(struct xwmp_thd * thd,
                     const char * name,
                     xwmp_thd_f mainfunc, void * arg,
                     xwstk_t * stack, xwsz_t stack_size,
                     xwpr_t priority, xwsq_t attr);
xwer_t xwmp_thd_create(struct xwmp_thd ** thdbuf, const char * name,
                       xwmp_thd_f mainfunc, void * arg,
                       xwsz_t stack_size,
                       xwpr_t priority, xwsq_t attr);
xwer_t xwmp_thd_stop(struct xwmp_thd * thd, xwer_t * trc);
xwer_t xwmp_thd_cancel(struct xwmp_thd * thd);
xwer_t xwmp_thd_join(struct xwmp_thd * thd, xwer_t * trc);
xwer_t xwmp_thd_detach(struct xwmp_thd * thd);
xwer_t xwmp_thd_intr(struct xwmp_thd * thd);
xwer_t xwmp_thd_migrate(struct xwmp_thd * thd, xwid_t dstcpu);
struct xwmp_thd * xwmp_cthd_self(void);
void xwmp_cthd_yield(void);
void xwmp_cthd_exit(xwer_t rc);
bool xwmp_cthd_shld_stop(void);
bool xwmp_cthd_shld_frz(void);
bool xwmp_cthd_frz_shld_stop(bool * frozen);

xwer_t xwmp_thd_tid2thd(xwsq_t tid, struct xwmp_thd ** ptrbuf);
xwer_t xwmp_thd_do_unlock(void * lock, xwid_t lktype, void * lkdata);
xwer_t xwmp_thd_do_lock(void * lock, xwid_t lktype, xwtm_t * xwtm, void * lkdata);
xwer_t xwmp_cthd_sleep(xwtm_t * xwtm);
xwer_t xwmp_cthd_sleep_from(xwtm_t * origin, xwtm_t inc);
xwer_t xwmp_cthd_freeze(void);
struct sighand_struct * linux_thd_lock_task_sighand(struct xwmp_thd * thd,
                                                    xwreg_t * flag);
void linux_thd_unlock_task_sighand(struct xwmp_thd * thd, xwreg_t flag);
void linux_thd_fake_signal(struct xwmp_thd * thd);
void linux_thd_signal_wake_up_state(struct xwmp_thd * thd, unsigned int state);
void linux_thd_fake_signal_wake_up(struct xwmp_thd * thd);
void linux_thd_clear_fake_signal(struct xwmp_thd * thd);

xwer_t linux_thd_ksym_load(void);

#endif /* xwos/mp/thd.h */
