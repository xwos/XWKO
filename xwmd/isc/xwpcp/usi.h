/**
 * @file
 * @brief 点对点通讯协议：用户态接口
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

#ifndef __xwmd_isc_xwpcp_usi_h__
#define __xwmd_isc_xwpcp_usi_h__

#include <xwos/standard.h>
#include <xwos/osal/skd.h>
#include <xwmd/isc/xwpcp/protocol.h>
#include <linux/time.h>

struct xwpcp_usmsg {
        int16_t prio;
        size_t size;
        struct __kernel_timespec timeout;
        uint8_t * text;
};

enum xwpcp_usmsg_state_em {
        XWPCP_USMSG_OK,
};

#define USI_XWPCP_STATE_STOP    0
#define USI_XWPCP_STATE_START   1

#define UAPI_XWPCP_IOC_MAGIC    'a'
#define UAPI_XWPCP_IOC_TX       _IOC(_IOC_WRITE, UAPI_XWPCP_IOC_MAGIC, 0, \
                                     sizeof(struct xwpcp_usmsg))
#define UAPI_XWPCP_IOC_EQ       _IOC(_IOC_WRITE, UAPI_XWPCP_IOC_MAGIC, 1, \
                                     sizeof(struct xwpcp_usmsg))
#define UAPI_XWPCP_IOC_CK       _IOC(_IOC_WRITE, UAPI_XWPCP_IOC_MAGIC, 2, \
                                     sizeof(uint8_t))
#define UAPI_XWPCP_IOC_RX       _IOC(_IOC_READ, UAPI_XWPCP_IOC_MAGIC, 0, \
                                     sizeof(struct xwpcp_usmsg))

extern struct xwpcp usi_xwpcp;

xwer_t usi_xwpcp_init(void);
void usi_xwpcp_exit(void);
xwsq_t usi_xwpcp_get_state(void);
void usi_xwpcp_pm_notify(unsigned long pmevt);

#endif /* xwmd/isc/xwpcp/usi.h */
