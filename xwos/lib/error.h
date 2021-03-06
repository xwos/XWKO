/**
 * @file
 * @brief 玄武OS通用库：错误
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

#ifndef __xwos_lib_error_h__
#define __xwos_lib_error_h__

#include <linux/err.h>
#include <xwos/lib/type.h>

/**
 * @defgroup error 错误
 * @{
 */

#include <xwos/lib/errno.h>

/**
 * @brief 将错误码转换为指针
 * @param[in] err: 错误码
 * @return 值为错误码的指针
 */
static __xwcc_inline
void * __xwcc_must_check err_ptr(xwer_t err)
{
        return ERR_PTR(err);
}

/**
 * @brief 将指针的值转换为错误码
 * @param[in] ptr: 指针
 * @return 错误码
 */
static __xwcc_inline
xwer_t __xwcc_must_check ptr_err(const void *ptr)
{
        return (xwer_t)PTR_ERR(ptr);
}

/**
 * @brief 测试指针的值是否为错误码
 * @param[in] ptr: 指针
 * @retval true: 是
 * @retval false: 否
 */
static __xwcc_inline
bool __xwcc_must_check is_err(const void *ptr)
{
        return IS_ERR(ptr);
}

/**
 * @brief 测试指针的值是否为错误码或空(指针是否为无效指针)
 * @param[in] ptr: 指针
 * @retval true: 是
 * @retval false: 否
 */
static __xwcc_inline
bool __xwcc_must_check is_err_or_null(const void *ptr)
{
        return IS_ERR_OR_NULL(ptr);
}

/**
 * @} error
 */

#endif /* xwos/lib/error.h */
