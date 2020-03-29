/**
 * @file
 * @brief 点对点通讯协议：硬件接口抽象层
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

#ifndef __xwmd_isc_xwpcp_hwifal_h__
#define __xwmd_isc_xwpcp_hwifal_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWPCP_HWIFAL_SOF                        ((xwu8_t)'P')
#define XWPCP_HWIFAL_EOF                        ((xwu8_t)(XWPCP_HWIFAL_SOF ^ 0xFF))
#define XWPCP_HWIFAL_SOF_SIZE                   (1U)
#define XWPCP_HWIFAL_EOF_SIZE                   (1U)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwpcp;
struct xwpcp_frmslot;
struct xwpcp_frame;

/**
 * @breif 硬件接口抽象层操作函数集
 */
struct xwpcp_hwifal_operations {
        xwer_t (*open)(struct xwpcp *); /**< 打开硬件接口 */
        xwer_t (*close)(struct xwpcp *); /**< 关闭硬件接口 */
        xwer_t (*tx)(struct xwpcp *, const xwu8_t *, xwsz_t); /**< 发送数据 */
        xwer_t (*rx)(struct xwpcp *, xwu8_t *, xwsz_t *); /**< 接收数据 */
        void (*notify)(struct xwpcp *, xwsq_t); /**< 通知事件 */
};

/**
 * @breif 硬件接口抽象层状态枚举
 */
enum xwpcp_hwifal_state_em {
        XWPCP_HWIFST_CLOSED, /**< 硬件接口已经关闭 */
        XWPCP_HWIFST_RX = BIT(0), /**< 硬件接口正在接收数据 */
        XWPCP_HWIFST_TX = BIT(1), /**< 硬件接口正在发送一帧 */
};

/**
 * @breif 硬件接口层事件枚举
 */
enum xwpcp_hwifal_notification_em {
        XWPCP_HWIFNTF_CONNECT = 0, /**< 与远程端链接 */
        XWPCP_HWIFNTF_NETUNREACH = 1, /**< 远程端无响应 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwmd_code
xwer_t xwpcp_hwifal_open(struct xwpcp * xwpcp);

__xwmd_code
xwer_t xwpcp_hwifal_close(struct xwpcp * xwpcp);

__xwmd_code
xwer_t xwpcp_hwifal_tx(struct xwpcp * xwpcp, struct xwpcp_frame * frm);

__xwmd_code
xwer_t xwpcp_hwifal_rx(struct xwpcp * xwpcp, struct xwpcp_frmslot ** frmslotbuf);

__xwmd_code
void xwpcp_hwifal_notify(struct xwpcp * xwpcp, xwsq_t evt);

#endif /* xwmd/isc/xwpcp/hwifal.h */
