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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/string.h>
#include <xwos/lib/crc32.h>
#include <xwmd/xwmcupgm/protocol.h>
#include <xwmd/xwmcupgm/server.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 初始化MCU PGM服务端
 * @param xwmcupgmsrv: (I) MCU PGM服务端控制块指针
 * @param ops: (I) 操作函数集合
 * @param ifcb: (I) 硬件设备指针
 */
__xwmd_code
void xwmcupgmsrv_init(struct xwmcupgmsrv * xwmcupgmsrv,
                      const struct xwmcupgmsrv_operations * ops,
                      void * ifcb)
{
        XWMCUPGM_VALIDATE(ops, "nullptr", -EINVAL);

        xwmcupgmsrv->ops = ops;
        xwmcupgmsrv->state = XWMCUPGMSRV_STATE_INIT;
        xwmcupgmsrv->idx = 0;
        xwmcupgmsrv->size = 0;
        xwmcupgmsrv->pos = 0;
        xwmcupgmsrv->ifcb = ifcb;
        memset(xwmcupgmsrv->seed, 0, XWMCUPGM_SEED_SIZE);
}

/**
 * @brief 认证Client编程对话
 * @param xwmcupgmsrv: (I) MCU PGM服务端控制块指针
 * @return 错误码
 * @retval OK: OK
 * @retval -ENOTRECOVERABLE: state error
 * @retval -ESIZE: size error
 * @retval -EBADMSG: message format error
 * @retval -EREMOTE: client error
 */
__xwmd_code
xwer_t xwmcupgmsrv_authenticate(struct xwmcupgmsrv * xwmcupgmsrv)
{
        union xwmcupgm_frame frm;
        xwu32_t crc32;
        xwer_t rc;

        if (XWMCUPGMSRV_STATE_INIT != xwmcupgmsrv->state) {
                rc = -ENOTRECOVERABLE;
                goto err_state;
        }
        xwmcupgmlogf(DEBUG, "wait for seed...");
        rc = xwmcupgmsrv->ops->rx(xwmcupgmsrv, &frm);
        if (__unlikely(rc < 0)) {
                goto err_ops_rx;
        }
        if (XWMCUPGM_CMD_AUTH == frm.raw.tag) {
                memcpy(xwmcupgmsrv->seed, frm.cmdfrm.text, XWMCUPGM_SEED_SIZE);
                crc32 = xwlib_crc32_calms((xwu8_t *)xwmcupgmsrv->seed,
                                          XWMCUPGM_SEED_SIZE);
                frm.cmdfrm.tag = XWMCUPGM_CMD_AUTHAW;
                frm.cmdfrm.text[0] = (xwu8_t)((crc32 >> 24) & 0xFF);
                frm.cmdfrm.text[1] = (xwu8_t)((crc32 >> 16) & 0xFF);
                frm.cmdfrm.text[2] = (xwu8_t)((crc32 >> 8) & 0xFF);
                frm.cmdfrm.text[3] = (xwu8_t)((crc32 >> 0) & 0xFF);
                xwmcupgmlogf(DEBUG, "Tx key: 0x%X", crc32);
                rc = xwmcupgmsrv->ops->tx(xwmcupgmsrv, &frm, 1 + 4);
                if (__unlikely(rc < 0)) {
                        goto err_ops_tx;
                }
                rc = xwmcupgmsrv->ops->rx(xwmcupgmsrv, &frm);
                if (__unlikely(rc < 0)) {
                        goto err_ops_rx;
                }
                if (__unlikely((XWMCUPGM_FRMT_ACK | XWMCUPGM_CMD_AUTHAW) !=
                               frm.raw.tag)) {
                        rc = -EBADMSG;
                        goto err_badmsg;
                }
                switch (frm.ack.code) {
                case XWMCUPGM_CMDACK_OK:
                        xwmcupgmsrv->state = XWMCUPGMSRV_STATE_AUTHENTIFICATION;
                        break;
                default:
                        rc = -EREMOTE;
                        goto err_ack;
                        break;
                }
        } else {
                rc = -EBADMSG;
                goto err_badmsg;
        }
        return OK;

err_ack:
err_badmsg:
err_ops_tx:
err_ops_rx:
        xwmcupgmsrv->state = XWMCUPGMSRV_STATE_ERROR;
err_state:
        return rc;
}

/**
 * @brief 启动编程对话
 * @param xwmcupgmsrv: (I) MCU PGM服务端控制块指针
 * @param imgsize: (I) 固件大小
 * @return 错误码
 * @retval -ENOTRECOVERABLE: state error
 * @retval -ESIZE: size error
 * @retval -EBADMSG: message format error
 * @retval -EREMOTE: client error
 */
__xwmd_code
xwer_t xwmcupgmsrv_start(struct xwmcupgmsrv * xwmcupgmsrv,
                         xwsz_t imgsize)
{
        union xwmcupgm_frame frm;
        xwer_t rc;

        if (XWMCUPGMSRV_STATE_AUTHENTIFICATION != xwmcupgmsrv->state) {
                rc = -ENOTRECOVERABLE;
                goto err_state;
        }
        xwmcupgmlogf(DEBUG, "Start the session <imgsize:%d>", imgsize);
        xwmcupgmsrv->size = imgsize;
        frm.cmdfrm.tag = XWMCUPGM_CMD_START;
        frm.cmdfrm.text[0] = (xwu8_t)((imgsize >> 24) & 0xFF);
        frm.cmdfrm.text[1] = (xwu8_t)((imgsize >> 16) & 0xFF);
        frm.cmdfrm.text[2] = (xwu8_t)((imgsize >> 8) & 0xFF);
        frm.cmdfrm.text[3] = (xwu8_t)((imgsize >> 0) & 0xFF);
        rc = xwmcupgmsrv->ops->tx(xwmcupgmsrv, &frm, 5);
        if (__unlikely(rc < 0)) {
                goto err_ops_tx;
        }
        rc = xwmcupgmsrv->ops->rx(xwmcupgmsrv, &frm);
        if (__unlikely(rc < 0)) {
                goto err_ops_rx;
        }
        if (__unlikely((XWMCUPGM_FRMT_ACK | XWMCUPGM_CMD_START) != frm.raw.tag)) {
                rc = -EBADMSG;
                goto err_badmsg;
        }
        switch (frm.ack.code) {
        case XWMCUPGM_CMDACK_OK:
                xwmcupgmsrv->state = XWMCUPGMSRV_STATE_SESSION;
                break;
        default:
                rc = -EREMOTE;
                goto err_ack;
                break;
        }
        return OK;

err_ack:
err_badmsg:
err_ops_rx:
err_ops_tx:
        xwmcupgmsrv->state = XWMCUPGMSRV_STATE_ERROR;
err_state:
        return rc;
}

/**
 * @brief 发送一帧数据
 * @param xwmcupgmsrv: (I) MCU PGM服务端控制块指针
 * @param data: (I) 数据缓冲区的指针
 * @param size: (I) 数据大小
 * @return 错误码
 * @retval -ENOTRECOVERABLE: state error
 * @retval -EBADMSG: message format error
 * @retval -EREMOTE: client error
 */
__xwmd_code
xwer_t xwmcupgmsrv_tx_dataframe_once(struct xwmcupgmsrv * xwmcupgmsrv,
                                     xwu8_t * data,
                                     xwsz_t size)
{
        union xwmcupgm_frame frm;
        xwu8_t subidx;
        xwer_t rc;

        if (XWMCUPGMSRV_STATE_SESSION != xwmcupgmsrv->state) {
                rc = -ENOTRECOVERABLE;
                goto err_state;
        }
        xwmcupgmlogf(DEBUG, "TX a data frame <size:%d>", size);
        subidx = (xwu8_t)(xwmcupgmsrv->idx & 0x3F);
        frm.dfrm.tag = subidx | XWMCUPGM_FRMT_DATA;
        frm.dfrm.size = (xwu8_t)size;
        memcpy(frm.dfrm.text, data, size);
        rc = xwmcupgmsrv->ops->tx(xwmcupgmsrv, &frm, 2 + size);
        if (__unlikely(rc < 0)) {
                goto err_ops_tx;
        }
        rc = xwmcupgmsrv->ops->rx(xwmcupgmsrv, &frm);
        if (__unlikely(rc < 0)) {
                goto err_ops_rx;
        }
        if (__unlikely((XWMCUPGM_FRMT_ACK | subidx | XWMCUPGM_FRMT_DATA) !=
                       frm.raw.tag)) {
                rc = -EBADMSG;
                goto err_badmsg;
        }
        switch (frm.ack.code) {
        case XWMCUPGM_DACK_OK:
                xwmcupgmsrv->idx ++;
                xwmcupgmsrv->pos += size;
                if (xwmcupgmsrv->pos == xwmcupgmsrv->size) {
                        xwmcupgmsrv->state = XWMCUPGMSRV_STATE_FINISHING;
                }
                break;
        case XWMCUPGM_DACK_IDXERR:
        case XWMCUPGM_DACK_SZERR:
        case XWMCUPGM_DACK_PGMERR:
                rc = -EREMOTE;
                goto err_ack;
        }
        return OK;

err_ack:
err_badmsg:
err_ops_rx:
err_ops_tx:
        xwmcupgmsrv->state = XWMCUPGMSRV_STATE_ERROR;
err_state:
        return rc;
}

/**
 * @brief 完成编程对话
 * @param xwmcupgmsrv: (I) MCU PGM服务端控制块指针
 * @param crc32: (I) firmware的CRC32校验值
 * @return 错误码
 * @retval -ENOTRECOVERABLE: state error
 * @retval -ESIZE: size error
 * @retval -EBADMSG: message format error
 * @retval -EREMOTE: client error
 */
__xwmd_code
xwer_t xwmcupgmsrv_finish(struct xwmcupgmsrv * xwmcupgmsrv,
                          xwu32_t crc32)
{
        union xwmcupgm_frame frm;
        xwer_t rc;

        if (XWMCUPGMSRV_STATE_FINISHING != xwmcupgmsrv->state) {
                rc = -ENOTRECOVERABLE;
                goto err_state;
        }
        frm.cmdfrm.tag = XWMCUPGM_CMD_FINISH;
        frm.cmdfrm.text[0] = (xwu8_t)((crc32 >> 24) & 0xFF);
        frm.cmdfrm.text[1] = (xwu8_t)((crc32 >> 16) & 0xFF);
        frm.cmdfrm.text[2] = (xwu8_t)((crc32 >> 8) & 0xFF);
        frm.cmdfrm.text[3] = (xwu8_t)((crc32 >> 0) & 0xFF);
        rc = xwmcupgmsrv->ops->tx(xwmcupgmsrv, &frm, 1 + 4);
        if (__unlikely(rc < 0)) {
                goto err_ops_tx;
        }
        rc = xwmcupgmsrv->ops->rx(xwmcupgmsrv, &frm);
        if (__unlikely(rc < 0)) {
                goto err_ops_rx;
        }
        if (__unlikely((XWMCUPGM_FRMT_ACK | XWMCUPGM_CMD_FINISH) != frm.raw.tag)) {
                rc = -EBADMSG;
                goto err_badmsg;
        }
        switch (frm.ack.code) {
        case XWMCUPGM_CMDACK_OK:
                xwmcupgmsrv->state = XWMCUPGMSRV_STATE_FINISHED;
                break;
        default:
                rc = -EREMOTE;
                goto err_ack;
                break;
        }
        return OK;

err_ack:
err_badmsg:
err_ops_rx:
err_ops_tx:
        xwmcupgmsrv->state = XWMCUPGMSRV_STATE_ERROR;
err_state:
        return rc;
}
