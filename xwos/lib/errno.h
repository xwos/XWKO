/**
 * @file
 * @brief XWOS通用库：错误码
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

#ifndef __xwos_lib_errno_h__
#define __xwos_lib_errno_h__

/* XuanWuOS扩展的错误码 */
#define XWOK            0 /* no error */
#define EEMPTY          1080 /* is empty */
#define ETYPE           1081 /* type error */
#define EINTHD          1082 /* in Thread mode */
#define ENOTINTHD       1083 /* not in Thread mode */
#define EINBH           1084 /* in Bottom Half */
#define ENOTINBH        1085 /* not in Bottom Half */
#define EINISR          1086 /* in ISR */
#define ENOTINISR       1087 /* not in ISR */
#define EBUG            1088 /* is a BUG */
#define EEXIT           1089 /* need to exit */
#define EBADID          1090 /* bad ID */
#define EOBJDEAD        1091 /* object dead */
#define EOBJACTIVE      1092 /* object active */
#define ENEGATIVE       1093 /* negative resource */
#define ESIZE           1094 /* size error */
#define E2SMALL         1095 /* too small */
#define EOOR            1096 /* out of range */
#define EALIGN          1097 /* not aligned */
#define EOWNER          1098 /* owner error */

#endif /* xwos/lib/errno.h */
