/**
 * @file
 * @brief XWOS错误码
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

#ifndef __xwos_lib_errno_h__
#define __xwos_lib_errno_h__

/* XuanWuOS扩展的错误码 */
#define OK              0
#define EEMPTY          180     /* is empty */
#define ETYPE           181     /* type error */
#define EINTHRD         182     /* in Thread mode */
#define ENOTINTHRD      183     /* not in Thread mode */
#define EINBH           184     /* in Bottom Half */
#define ENOTINBH        185     /* not in Bottom Half */
#define EINISR          186     /* in ISR */
#define ENOTINISR       187     /* not in ISR */
#define EBUG            188     /* is a BUG */
#define EEXIT           189     /* need to exit */
#define EBADID          190     /* bad ID */
#define EOBJDEAD        191     /* object dead */
#define EOBJACTIVE      192     /* object active */
#define ENEGATIVE       193     /* negative resource */
#define ESIZE           194     /* size error */
#define E2SMALL         195     /* too small */
#define EOOR            196     /* out of range */
#define EALIGN          197     /* not aligned */
#define EOWNER          198     /* owner error */

#endif /* xwos/lib/errno.h */
