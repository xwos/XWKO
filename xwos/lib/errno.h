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
#define EOWNER          182     /* owner error */
#define EALIGN          183     /* not aligned */
#define E2SMALL         184     /* too small */
#define EDEAD           185     /* already dead */
#define ENEGATIVE       186     /* negative resource */
#define ENOTINISR       187     /* not in isr */
#define EBUG            188     /* is a BUG */
#define EEXIT           189     /* need to exit */
#define EBADID          190     /* bad ID */
#define ESIZE           191     /* size error */
#define EOOR            192     /* out of range */
#define EOBJDEAD        193     /* object dead */
#define EOBJACTIVE      194     /* object active */

#endif /* xwos/lib/errno.h */
