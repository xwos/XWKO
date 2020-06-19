/**
 * @file
 * @brief MCU通讯模块：MCU模拟器
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

#ifndef __bm_mcuc_imitator_h__
#define __bm_mcuc_imitator_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/semaphore.h>
#include <xwmd/isc/xwpcp/api.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct mcuc_imitator_msg {
        struct xwlib_bclst_node cln;
        struct xwpcp_msg msg;
};

struct mcuc_imitator_info {
        struct {
                struct xwosal_smr smr;
                struct xwlib_bclst_head head;
                struct xwosal_splk lock;
        } txq; /**< TX info */

        struct {
                struct xwosal_smr smr;
                struct xwlib_bclst_head head;
                struct xwosal_splk lock;
        } rxq; /**< RX info */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern bool mcuc_imitator;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t mcuc_imitator_init(void);

xwer_t mcuc_imitator_exit(void);

void mcuc_imitator_pulish_txmsg(struct mcuc_imitator_info * imi,
                                struct mcuc_imitator_msg * imsg);

xwer_t mcuc_imitator_get_txmsg(struct mcuc_imitator_info * imi,
                               struct mcuc_imitator_msg ** imsgbuf);

void mcuc_imitator_pulish_rxmsg(struct mcuc_imitator_info * imi,
                                struct mcuc_imitator_msg * imsg);

xwer_t mcuc_imitator_get_rxmsg(struct mcuc_imitator_info * imi,
                               struct mcuc_imitator_msg ** imsgbuf);

void mcuc_imitator_free_msg(struct mcuc_imitator_msg * imsg);

#endif /* bm/mcuc/imitator.h */
