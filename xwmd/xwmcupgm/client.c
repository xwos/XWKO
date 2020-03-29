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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/string.h>
#include <xwos/lib/crc32.h>
#include <xwos/osal/thread.h>
#include <xwmd/xwmcupgm/protocol.h>
#include <xwmd/xwmcupgm/client.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwmd_code
xwer_t xwmcupgmc_tx_ack(struct xwmcupgmc * xwmcupgmc, xwu8_t tag, xwu8_t ack);

static __xwmd_code
xwer_t xwmcupgmc_authenticate(struct xwmcupgmc * xwmcupgmc);

static __xwmd_code
xwer_t xwmcupgmc_start(struct xwmcupgmc * xwmcupgmc);

static __xwmd_code
xwer_t xwmcupgmc_pgm_once(struct xwmcupgmc * xwmcupgmc);

static __xwmd_code
xwer_t xwmcupgmc_finish(struct xwmcupgmc * xwmcupgmc);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 初始化MCU PGM客服端
 * @param xwmcupgmc: (I) MCU PGM客服端控制块指针
 * @param ops: (I) 操作函数集合
 * @param ifcb: (I) 硬件设备指针
 */
__xwmd_code
xwer_t xwmcupgmc_init(struct xwmcupgmc * xwmcupgmc,
                      const struct xwmcupgmc_operations * ops,
                      void * ifcb)
{
        xwer_t rc;
        char seed[XWMCUPGM_SEED_SIZE];

        XWMCUPGM_VALIDATE(ops, "nullptr", -EINVAL);

        xwmcupgmc->ops = ops;
        xwmcupgmc->state = XWMCUPGMC_STATE_INIT;
        xwmcupgmc->subidx = 0;
        xwmcupgmc->idx = 0;
        xwmcupgmc->size = 0;
        xwmcupgmc->pos = 0;
        xwmcupgmc->ifcb = ifcb;
        if (ops->init) {
                rc = ops->init(xwmcupgmc, seed);
                memcpy(xwmcupgmc->seed, seed, XWMCUPGM_SEED_SIZE);
        } else {
                /* Use the random data in stack as seed */
                memcpy(xwmcupgmc->seed, seed, XWMCUPGM_SEED_SIZE);
                rc = OK;
        }
        return rc;
}

/**
 * @brief 发送一帧应答帧
 * @param xwmcupgmc: (I) MCU PGM客服端控制块指针
 * @param tag: (I) 应答的消息的tag
 * @param ack: (I) 应答值
 * @return 错误码
 */
static __xwmd_code
xwer_t xwmcupgmc_tx_ack(struct xwmcupgmc * xwmcupgmc, xwu8_t tag, xwu8_t ack)
{
        union xwmcupgm_frame frm;

        frm.ack.tag = tag | XWMCUPGM_FRMT_ACK;
        frm.ack.code = ack;
        return xwmcupgmc->ops->tx(xwmcupgmc, &frm, sizeof(struct xwmcupgm_ack_frame));
}

/**
 * @brief 认证Server编程对话
 * @param xwmcupgmc: (I) MCU PGM客服端控制块指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwmcupgmc_authenticate(struct xwmcupgmc * xwmcupgmc)
{
        union xwmcupgm_frame frm;
        xwu32_t crc32;
        xwer_t rc;

        frm.cmdfrm.tag = XWMCUPGM_CMD_AUTH;
        memcpy(frm.cmdfrm.text, xwmcupgmc->seed, XWMCUPGM_SEED_SIZE);
        rc = xwmcupgmc->ops->tx(xwmcupgmc, &frm, 1 + XWMCUPGM_SEED_SIZE);
        if (__unlikely(rc < 0)) {
                goto err_ops_tx;
        }
        rc = xwmcupgmc->ops->rx(xwmcupgmc, &frm);
        if (__unlikely(rc < 0)) {
                goto err_ops_rx;
        }
        if (XWMCUPGM_CMD_AUTHAW == frm.raw.tag) {
                crc32 = xwlib_crc32_calms((xwu8_t *)xwmcupgmc->seed, XWMCUPGM_SEED_SIZE);
                if ((((crc32 >> 24) & 0xFF) == (xwu32_t)frm.cmdfrm.text[0]) ||
                    (((crc32 >> 16) & 0xFF) == (xwu32_t)frm.cmdfrm.text[1]) ||
                    (((crc32 >> 8) & 0xFF) == (xwu32_t)frm.cmdfrm.text[2]) ||
                    (((crc32 >> 0) & 0xFF) == (xwu32_t)frm.cmdfrm.text[3])) {
                        xwmcupgmc->state = XWMCUPGMC_STATE_AUTHENTIFICATION;
                        rc = xwmcupgmc_tx_ack(xwmcupgmc,
                                              frm.cmdfrm.tag,
                                              XWMCUPGM_CMDACK_OK);
                        if (__unlikely(rc < 0)) {
                                goto err_ops_tx;
                        }
                } else {
                        rc = xwmcupgmc_tx_ack(xwmcupgmc,
                                              frm.cmdfrm.tag,
                                              XWMCUPGM_CMDACK_ERRAUTH);
                        if (__unlikely(rc < 0)) {
                                goto err_ops_tx;
                        }
                }
        } else {
                rc = -EBADMSG;
                goto err_badmsg;
        }
        return OK;

err_ops_tx:
err_badmsg:
err_ops_rx:
        return rc;
}

/**
 * @brief 启动编程对话
 * @param xwmcupgmc: (I) MCU PGM客服端控制块指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwmcupgmc_start(struct xwmcupgmc * xwmcupgmc)
{
        union xwmcupgm_frame frm;
        xwer_t rc;

        rc = xwmcupgmc->ops->rx(xwmcupgmc, &frm);
        if (__unlikely(rc < 0)) {
                goto err_ops_rx;
        }
        if (XWMCUPGM_CMD_START == frm.raw.tag) {
                xwmcupgmc->subidx = 0;
                xwmcupgmc->idx = 0;
                xwmcupgmc->size = (xwsz_t)frm.cmdfrm.text[0] << 24U |
                               (xwsz_t)frm.cmdfrm.text[1] << 16U |
                               (xwsz_t)frm.cmdfrm.text[2] << 8U |
                               (xwsz_t)frm.cmdfrm.text[3] << 0U;
                xwmcupgmc->pos = 0;
                rc = xwmcupgmc->ops->start(xwmcupgmc);
                if (__unlikely(rc < 0)) {
                        xwmcupgmc_tx_ack(xwmcupgmc, frm.cmdfrm.tag, XWMCUPGM_CMDACK_ERR);
                        goto err_ops_start;
                }
                xwmcupgmc->state = XWMCUPGMC_STATE_SESSION;
                rc = xwmcupgmc_tx_ack(xwmcupgmc, frm.cmdfrm.tag, XWMCUPGM_CMDACK_OK);
                if (__unlikely(rc < 0)) {
                        goto err_ops_tx;
                }
        } else {
                rc = -EBADMSG;
                goto err_badmsg;
        }
        return OK;

err_ops_tx:
err_ops_start:
err_badmsg:
err_ops_rx:
        return rc;
}

/**
 * @brief 接收并编程一帧固件数据
 * @param xwmcupgmc: (I) MCU PGM客服端控制块指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwmcupgmc_pgm_once(struct xwmcupgmc *xwmcupgmc)
{
        union xwmcupgm_frame frm;
        xwu8_t subidx;
        xwer_t rc;

        rc = xwmcupgmc->ops->rx(xwmcupgmc, &frm);
        if (__unlikely(rc < 0)) {
                goto err_ops_rx;
        }
        if (XWMCUPGM_FRMT_DATA & frm.raw.tag) {
                subidx = frm.dfrm.tag & 0x3F;
                if (subidx == xwmcupgmc->subidx) {
                        if (xwmcupgmc->pos + frm.dfrm.size <= xwmcupgmc->size) {
                                rc = xwmcupgmc->ops->pgm(xwmcupgmc, &frm);
                                if (__unlikely(rc < 0)) {
                                        xwmcupgmc_tx_ack(xwmcupgmc, frm.dfrm.tag,
                                                      XWMCUPGM_DACK_PGMERR);
                                        goto err_ops_pgm;
                                }
                                xwmcupgmc->idx ++;
                                xwmcupgmc->subidx = (xwu8_t)(xwmcupgmc->idx & 0x3F);
                                xwmcupgmc->pos += frm.dfrm.size;
                                if (xwmcupgmc->pos == xwmcupgmc->size) {
                                        xwmcupgmc->state = XWMCUPGMC_STATE_FINISHING;
                                }/* else {} */
                                rc = xwmcupgmc_tx_ack(xwmcupgmc, frm.dfrm.tag,
                                                   XWMCUPGM_DACK_OK);
                                if (__unlikely(rc < 0)) {
                                        goto err_ops_tx;
                                }
                        } else {
                                rc = xwmcupgmc_tx_ack(xwmcupgmc, frm.dfrm.tag,
                                                   XWMCUPGM_DACK_SZERR);
                                if (__unlikely(rc < 0)) {
                                        goto err_ops_tx;
                                }
                        }
                } else {
                        rc = xwmcupgmc_tx_ack(xwmcupgmc, frm.dfrm.tag,
                                           XWMCUPGM_DACK_IDXERR);
                        if (__unlikely(rc < 0)) {
                                goto err_ops_tx;
                        }
                }
        } else {
                rc = xwmcupgmc_tx_ack(xwmcupgmc, frm.dfrm.tag, XWMCUPGM_CMDACK_ERR);
                if (__unlikely(rc < 0)) {
                        goto err_ops_rx;
                }
        }
        return OK;

err_ops_tx:
err_ops_pgm:
err_ops_rx:
        return rc;
}

/**
 * @brief 完成编程对话
 * @param xwmcupgmc: (I) MCU PGM客服端控制块指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwmcupgmc_finish(struct xwmcupgmc *xwmcupgmc)
{
        union xwmcupgm_frame frm;
        xwu32_t crc32;
        xwer_t rc;

        rc = xwmcupgmc->ops->rx(xwmcupgmc, &frm);
        if (__unlikely(rc < 0)) {
                goto err_ops_rx;
        }
        if (XWMCUPGM_CMD_FINISH == frm.raw.tag) {
                crc32 = (xwu32_t)frm.cmdfrm.text[0] << 24U;
                crc32 |= (xwu32_t)frm.cmdfrm.text[1] << 16U;
                crc32 |= (xwu32_t)frm.cmdfrm.text[2] << 8U;
                crc32 |= (xwu32_t)frm.cmdfrm.text[3] << 0U;
                rc = xwmcupgmc->ops->finish(xwmcupgmc, crc32);
                if (__unlikely(rc < 0)) {
                        xwmcupgmc_tx_ack(xwmcupgmc, frm.cmdfrm.tag, XWMCUPGM_CMDACK_ERRCHKSUM);
                        goto err_ops_finish;
                }
                xwmcupgmc->state = XWMCUPGMC_STATE_FINISHED;
                rc = xwmcupgmc_tx_ack(xwmcupgmc, frm.cmdfrm.tag, XWMCUPGM_CMDACK_OK);
                if (__unlikely(rc < 0)) {
                        goto err_ops_tx;
                }
        } else {
                rc = xwmcupgmc_tx_ack(xwmcupgmc, frm.cmdfrm.tag, XWMCUPGM_CMDACK_ERR);
                if (__unlikely(rc < 0)) {
                        goto err_ops_tx;
                }
        }
        return OK;

err_ops_tx:
err_ops_finish:
err_ops_rx:
        return rc;
}

/**
 * @brief 编程对话状态机
 * @param xwmcupgmc: (I) MCU PGM客服端控制块指针
 * @return 错误码
 */
__xwmd_code
xwer_t xwmcupgmc_fsm(struct xwmcupgmc *xwmcupgmc)
{
        xwer_t rc;
        xwsq_t cnt;

        cnt = 0;
        do {
                cnt ++;
                switch (xwmcupgmc->state) {
                case XWMCUPGMC_STATE_INIT:
                        rc = xwmcupgmc_authenticate(xwmcupgmc);
                        break;
                case XWMCUPGMC_STATE_AUTHENTIFICATION:
                        rc = xwmcupgmc_start(xwmcupgmc);
                        break;
                case XWMCUPGMC_STATE_SESSION:
                        rc = xwmcupgmc_pgm_once(xwmcupgmc);
                        break;
                case XWMCUPGMC_STATE_FINISHING:
                        rc = xwmcupgmc_finish(xwmcupgmc);
                        break;
                default:
                        rc = -EBUG;
                        break;
                }
        } while ((cnt < 5) && (rc < 0));
        if (rc < 0) {
                xwmcupgmc->state = XWMCUPGMC_STATE_ERROR;
        }/* else {} */
        return rc;
}
