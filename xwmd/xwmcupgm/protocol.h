/**
 * @file
 * @brief MCU firmware program protocol
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
 * @note
 * # frame:
 * - A authenticate command frame:
 *   + (xwu8_t)0x0, frame tag
 *   + (xwu8_t)seed[0]
 *   + (xwu8_t)seed[1]
 *   + (xwu8_t)seed[2]
 *   + (xwu8_t)seed[3]
 *   + (xwu8_t)seed[4]
 *   + (xwu8_t)seed[5]
 *   + (xwu8_t)seed[6]
 *   + (xwu8_t)seed[7]
 * - A authentication answering command frame:
 *   + (xwu8_t)0x1, frame tag
 *   + (xwu8_t)CRC32 of seed (MSB)
 *   + (xwu8_t)CRC32 of seed (2nd byte)
 *   + (xwu8_t)CRC32 of seed (3rd byte)
 *   + (xwu8_t)CRC32 of seed (LSB)
 * - A start command frame:
 *   + (xwu8_t)0x2, frame tag
 *   + (xwu8_t)image size (MSB)
 *   + (xwu8_t)image size (2nd byte)
 *   + (xwu8_t)image size (3rd byte)
 *   + (xwu8_t)image size (LSB)
 * - A finish command frame:
 *   + (xwu8_t)0x3, frame tag
 *   + (xwu8_t)CRC32 of image (MSB)
 *   + (xwu8_t)CRC32 of image (2nd byte)
 *   + (xwu8_t)CRC32 of image (3rd byte)
 *   + (xwu8_t)CRC32 of image (LSB)
 * - A data frame:
 *   + (xwu8_t)BIT(6) | sub-index, frame tag
 *   + (xwu8_t)size
 *   + (xwu8_t)text[XWMCUPGM_DFRM_TEXTSIZE]
 * - A ACK frame:
 *   + (xwu8_t)BIT(7) | frame tag
 *   + (xwu8_t)ACK
 * # protocol:
 *   client (MCU) =================================== server (上位机)
 *                Authenticate (seed)
 *                -------- -------- -------- ------>>
 *
 *                         Authenticate (CRC32<seed>)
 *                <<------ -------- -------- --------
 *
 *                Authenticate Ack
 *                -------- -------- -------- ------>>
 *
 *                                              Start
 *                <<------ -------- -------- --------
 *
 *                Start Ack
 *                -------- -------- -------- ------>>
 *
 *                                              DATA0
 *                <<------ -------- -------- --------
 *
 *                DATA0 Ack
 *                -------- -------- -------- ------>>
 *
 *                                              DATA1
 *                <<------ -------- -------- --------
 *
 *                DATA1 Ack
 *                -------- -------- -------- ------>>
 *
 *                ......
 *                (重复多次)
 *                ......
 *
 *                                              DATAn
 *                <<------ -------- -------- --------
 *
 *                DATAn Ack
 *                -------- -------- -------- ------>>
 *
 *                                             Finish
 *                <<------ -------- -------- --------
 *
 *                Finish Ack
 *                -------- -------- -------- ------>>
 *
 * # Todo
 *   + 认真过程中引入真正的加密算法替代CRC32
 */

#ifndef __xwmd_xwmcupgm_protocol_h__
#define __xwmd_xwmcupgm_protocol_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwlog.h>
#include <xwos/lib/xwbop.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWMCUPGM_DFRM_TEXTSIZE     XWMDCFG_xwmcupgm_DFRM_TEXTSIZE
#define XWMCUPGM_SEED_SIZE         8U

/**
 * @brief log function of mcupgm client
 */
#if (defined(XWMDCFG_xwmcupgm_LOG) && (1 == XWMDCFG_xwmcupgm_LOG))
  #define xwmcupgmlogf(lv, fmt, ...)    xwlogf(lv, fmt, ##__VA_ARGS__)
#else
  #define xwmcupgmlogf(lv, fmt, ...)
#endif

#if (defined(XWMDCFG_CHECK_PARAMETERS) && (1 == XWMDCFG_CHECK_PARAMETERS))
#define XWMCUPGM_VALIDATE(exp, errstr, ...)     \
        if (__unlikely(!(exp))) {               \
            return __VA_ARGS__;                 \
        }
#else
#define XWMCUPGM_VALIDATE(exp, errstr, ...)
#endif

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 帧类型枚举
 */
enum xwmcupgm_frame_type_em {
        XWMCUPGM_FRMT_DATA = BIT(6),
        XWMCUPGM_FRMT_ACK = BIT(7),
};

/**
 * @breif 数据帧
 */
struct __packed xwmcupgm_data_frame {
        xwu8_t tag; /**< tag of this frame:
                         bit7: 0
                         bit6: 1
                         bit0 ~ 5: 索引 */
        xwu8_t size; /**< 帧的大小 */
        xwu8_t text[XWMCUPGM_DFRM_TEXTSIZE]; /**< 数据 */
};

/**
 * @brief 数据应答枚举
 */
enum xwmcupgm_data_ack_em {
        XWMCUPGM_DACK_OK = 0,
        XWMCUPGM_DACK_IDXERR,
        XWMCUPGM_DACK_SZERR,
        XWMCUPGM_DACK_PGMERR,
};

/**
 * @brief 命令枚举
 */
enum xwmcupgm_cmd_em {
        XWMCUPGM_CMD_AUTH = 0,
        XWMCUPGM_CMD_AUTHAW,
        XWMCUPGM_CMD_START,
        XWMCUPGM_CMD_FINISH,
};

/**
 * @breif 命令帧
 */
struct __packed xwmcupgm_cmd_frame {
        xwu8_t tag; /**< tag of this frame:
                         bit7: 0
                         bit6: 0
                         bit0 ~ 5: 命令 */
        xwu8_t text[sizeof(struct xwmcupgm_data_frame) - 1]; /**< 数据 */
};

/**
 * @brief 命令应答枚举
 */
enum xwmcupgm_cmd_ack_em {
        XWMCUPGM_CMDACK_OK = 0,
        XWMCUPGM_CMDACK_ERR,
        XWMCUPGM_CMDACK_ERRAUTH,
        XWMCUPGM_CMDACK_ERRSIZE,
        XWMCUPGM_CMDACK_ERRCHKSUM,
};

/**
 * @breif 应答帧
 */
__packed struct xwmcupgm_ack_frame {
        xwu8_t tag; /**< tag of this frame:
                         bit7: 1
                         bit0 ~ bit6: 同被应答的消息 */
        xwu8_t code; /**< Ack code */
};

/**
 * @breif 消息帧
 */
__packed union xwmcupgm_frame {
        struct xwmcupgm_data_frame dfrm;
        struct xwmcupgm_cmd_frame cmdfrm;
        struct xwmcupgm_ack_frame ack;
        struct {
                xwu8_t tag; /**< tag */
                xwu8_t data[sizeof(struct xwmcupgm_data_frame) - 1]; /**< data */
        } raw;
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwmd/xwmcupgm/protocol.h */
