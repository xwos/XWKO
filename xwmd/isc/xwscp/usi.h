/**
 * @file
 * @brief Linux内核中SOSCP的适配层
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

#ifndef __xwmd_isc_xwscp_usi_h__
#define __xwmd_isc_xwscp_usi_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/osal/thread.h>
#include <xwmd/isc/xwscp/protocol.h>
#include <linux/time.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwscp_usmsg {
        size_t size;
        struct timeval timeout;
        uint8_t *text;
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define USI_SOSCP_STATE_STOP             0
#define USI_SOSCP_STATE_START            1

#define UAPI_SOSCP_IOC_MAGIC             's'
#define UAPI_SOSCP_IOC_TX                _IOC(_IOC_WRITE, UAPI_SOSCP_IOC_MAGIC, 0, \
                                              sizeof(struct xwscp_usmsg))
#define UAPI_SOSCP_IOC_RX                _IOC(_IOC_READ, UAPI_SOSCP_IOC_MAGIC, 0, \
                                              sizeof(struct xwscp_usmsg))

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern struct xwscp usi_xwscp;
extern xwid_t usi_xwscp_thrd;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern
xwsq_t usi_xwscp_get_state(void);

extern
xwer_t usi_xwscp_start(void);

extern
xwer_t usi_xwscp_stop(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********  inline functions ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwmd/isc/xwscp/usi.h */
