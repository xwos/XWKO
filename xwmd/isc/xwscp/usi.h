/**
 * @file
 * @brief 精简的点对点通讯协议：用户态接口
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
#define USI_XWSCP_STATE_STOP             0
#define USI_XWSCP_STATE_START            1

#define UAPI_XWSCP_IOC_MAGIC             's'
#define UAPI_XWSCP_IOC_TX                _IOC(_IOC_WRITE, UAPI_XWSCP_IOC_MAGIC, 0, \
                                              sizeof(struct xwscp_usmsg))
#define UAPI_XWSCP_IOC_RX                _IOC(_IOC_READ, UAPI_XWSCP_IOC_MAGIC, 0, \
                                              sizeof(struct xwscp_usmsg))

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern struct xwscp usi_xwscp;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t usi_xwscp_init(void);

void usi_xwscp_exit(void);

xwsq_t usi_xwscp_get_state(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********  inline functions ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwmd/isc/xwscp/usi.h */
