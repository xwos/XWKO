/**
 * @file
 * @brief MCU firmware program server (上位机)
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

#ifndef __xwmd_xwmcupgm_server_h__
#define __xwmd_xwmcupgm_server_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwlog.h>
#include <xwos/lib/xwbop.h>
#include <xwmd/xwmcupgm/protocol.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @breif MCU PGM 服务状态枚举
 */
enum xwmcupgmsrv_state_em {
        XWMCUPGMSRV_STATE_INIT = 0,
        XWMCUPGMSRV_STATE_AUTHENTIFICATION,
        XWMCUPGMSRV_STATE_SESSION,
        XWMCUPGMSRV_STATE_FINISHING,
        XWMCUPGMSRV_STATE_FINISHED,
        XWMCUPGMSRV_STATE_ERROR,
};

struct xwmcupgmsrv;

/**
 * @breif MCU PGM 服务操作函数集合
 */
struct xwmcupgmsrv_operations {
        xwer_t (*tx)(struct xwmcupgmsrv *, const void *, xwsz_t); /**< 发送数据 */
        xwer_t (*rx)(struct xwmcupgmsrv *, union xwmcupgm_frame *); /**< 接收数据 */
};

/**
 * @breif MCU PGM 服务控制块
 */
struct xwmcupgmsrv {
        xwsq_t state; /**< 状态 */
        const struct xwmcupgmsrv_operations * ops; /**< 操作函数集合 */
        xwsq_t idx; /**< 当前数据帧索引 */
        xwsz_t size; /**< firmware总大小 */
        xwsz_t pos; /**< 当前正在编程的位置 */
        char seed[XWMCUPGM_SEED_SIZE]; /* 与client端握手的秘钥 */
        void * ifcb; /**< 硬件设备指针 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwmd_code
void xwmcupgmsrv_init(struct xwmcupgmsrv * xwmcupgmsrv,
                      const struct xwmcupgmsrv_operations * ops,
                      void * ifcb);

__xwmd_code
xwer_t xwmcupgmsrv_authenticate(struct xwmcupgmsrv * xwmcupgmsrv);

__xwmd_code
xwer_t xwmcupgmsrv_start(struct xwmcupgmsrv * xwmcupgmsrv,
                         xwsz_t imgsize);

__xwmd_code
xwer_t xwmcupgmsrv_tx_dataframe_once(struct xwmcupgmsrv * xwmcupgmsrv,
                                     xwu8_t * frm,
                                     xwsz_t size);

__xwmd_code
xwer_t xwmcupgmsrv_finish(struct xwmcupgmsrv * xwmcupgmsrv,
                          xwu32_t crc32);

#endif /* xwmd/xwmcupgm/server.h */
