/**
 * @file
 * @brief XuanWuOS内核：线程
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

#ifndef __xwos_core_thread_h__
#define __xwos_core_thread_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/core/scheduler.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct task_struct;
#define xwos_tcb task_struct

struct sighand_struct;

typedef xwer_t (* xwos_thrd_f)(void *);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t xwos_thrd_grab(struct xwos_tcb * tcb);

xwer_t xwos_thrd_put(struct xwos_tcb * tcb);

xwer_t xwos_thrd_init(struct xwos_tcb * tcb,
                      const char * name,
                      xwos_thrd_f mainfunc, void * arg,
                      xwstk_t * stack, xwsz_t stack_size,
                      xwpr_t priority, xwsq_t attr);

xwer_t xwos_thrd_destroy(struct xwos_tcb *tcb);

xwer_t xwos_thrd_create(xwid_t * tidbuf, const char * name,
                        xwos_thrd_f mainfunc, void * arg,
                        xwsz_t stack_size,
                        xwpr_t priority, xwsq_t attr);

xwer_t xwos_thrd_delete(xwid_t tid);

void xwos_cthrd_yield(void);

void xwos_cthrd_exit(xwer_t rc);

xwer_t xwos_thrd_terminate(xwid_t tid, xwer_t * trc);

void xwos_cthrd_wait_exit(void);

bool xwos_cthrd_shld_stop(void);

bool xwos_cthrd_shld_frz(void);

bool xwos_cthrd_frz_shld_stop(bool * frozen);

xwid_t xwos_cthrd_get_id(void);

xwid_t xwos_thrd_get_id(struct xwos_tcb * tcb);

xwer_t xwos_thrd_do_unlock(void * lock, xwid_t lktype,
                           void * lkdata, xwsz_t datanum);

xwer_t xwos_thrd_do_lock(void * lock, xwid_t lktype, xwtm_t * xwtm,
                         void * lkdata, xwsz_t datanum);

xwer_t xwos_thrd_continue(xwid_t tid);

xwer_t xwos_cthrd_pause(void * lock, xwid_t lktype,
                        void * lkdata, xwsz_t datanum,
                        xwsq_t * lkst);

xwer_t xwos_cthrd_timedpause(void * lock, xwid_t lktype,
                             void * lkdata, xwsz_t datanum,
                             xwtm_t * xwtm, xwsq_t * lkst);

xwer_t xwos_cthrd_sleep(xwtm_t * xwtm);

xwer_t xwos_cthrd_sleep_from(xwtm_t * origin, xwtm_t inc);

xwer_t xwos_thrd_intr(xwid_t tid);

xwer_t xwos_cthrd_freeze(void);

xwer_t xwos_thrd_get_tcb_by_tid(xwid_t tid, struct xwos_tcb ** ptrbuf);

xwer_t xwos_thrd_migrate(xwid_t tid, xwid_t dstcpu);

struct sighand_struct * linux_thrd_lock_task_sighand(struct xwos_tcb * tcb,
                                                     xwreg_t * flag);

void linux_thrd_unlock_task_sighand(struct xwos_tcb * tcb, xwreg_t flag);

void linux_thrd_fake_signal(struct xwos_tcb * tcb);

void linux_thrd_signal_wake_up_state(struct xwos_tcb * tcb, unsigned int state);

void linux_thrd_fake_signal_wake_up(struct xwos_tcb * tcb);

void linux_thrd_clear_fake_signal(struct xwos_tcb * tcb);

xwer_t linux_thrd_ksym_load(void);

xwer_t linux_thrd_ksym_unload(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   inline function implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwos/core/thread.h */
