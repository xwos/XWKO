/**
 * @file
 * @brief 玄武OS通用库：CRC32
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
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

#ifndef __xwos_lib_crc32_h__
#define __xwos_lib_crc32_h__

#include <xwos/standard.h>

/**
 * @defgroup CRC32 CRC32
 * @{
 */

/**
 * @brief 计算CRC32的移位方向枚举
 */
enum xwlib_crc32_shift_direction_em {
        XWLIB_CRC32_LEFT_SHIFT, /**< 左移 */
        XWLIB_CRC32_RIGHT_SHIFT, /**< 右移 */
};

xwer_t xwlib_crc32_cal(xwu32_t * crc32, xwu32_t xorout,
                       bool refin, bool refout,
                       xwu32_t plynml, xwu32_t direction,
                       const xwu8_t stream[], xwsz_t * size);
xwu32_t xwlib_crc32_calms(const xwu8_t stream[], xwsz_t * size);

/**
 * @} CRC32
 */

#endif /* xwos/lib/crc32.h */
