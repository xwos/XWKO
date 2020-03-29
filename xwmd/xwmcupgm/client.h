/**
 * @file
 * @brief MCU firmware program client (MCU端)
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

#ifndef __xwmd_xwmcupgm_client_h__
#define __xwmd_xwmcupgm_client_h__

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
 * @breif MCU PGM 客服端状态枚举
 */
enum xwmcupgmc_state_em {
        XWMCUPGMC_STATE_INIT = 0,
        XWMCUPGMC_STATE_AUTHENTIFICATION,
        XWMCUPGMC_STATE_SESSION,
        XWMCUPGMC_STATE_FINISHING,
        XWMCUPGMC_STATE_FINISHED,
        XWMCUPGMC_STATE_ERROR,
};

struct xwmcupgmc;

/**
 * @breif MCU PGM 客服端操作函数
 */
struct xwmcupgmc_operations {
        xwer_t (*init)(struct xwmcupgmc *, char *);
        xwer_t (*start)(struct xwmcupgmc *);
        xwer_t (*tx)(struct xwmcupgmc *, const void *, xwsz_t);
        xwer_t (*rx)(struct xwmcupgmc *, union xwmcupgm_frame *);
        xwer_t (*pgm)(struct xwmcupgmc *, const union xwmcupgm_frame *);
        xwer_t (*finish)(struct xwmcupgmc *, xwu32_t);
};

/**
 * @breif MCU PGM 客服端控制块
 */
struct xwmcupgmc {
        xwsq_t state; /**< 状态 */
        const struct xwmcupgmc_operations * ops; /**< 操作函数 */
        xwu8_t subidx; /**< index的低6位 */
        xwsq_t idx; /**< 索引 */
        xwsz_t size; /**< 固件大小 */
        xwsz_t pos; /**< 当前正在编程的位置 */
        char seed[XWMCUPGM_SEED_SIZE]; /* 与client端握手的秘钥 */
        void *ifcb; /**< 硬件设备指针 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwmd_code
xwer_t xwmcupgmc_init(struct xwmcupgmc * xwmcupgmc,
                      const struct xwmcupgmc_operations * ops,
                      void *ifcb);

__xwmd_code
xwer_t xwmcupgmc_fsm(struct xwmcupgmc * xwmcupgmc);

#endif /* xwmd/sysm/mcupgm/client.h */
