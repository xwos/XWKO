/**
 * @file
 * @brief 玄武OS的错误码
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

#ifndef __xwos_err_h__
#define __xwos_err_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/type.h>
#include <linux/err.h>
#include <xwos/lib/errno.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macro       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 将错误码转换为指针
 * @param errno: (I) 错误码
 * @return 值为错误码的指针
 */
static __xw_inline
void * __must_check err_ptr(xwer_t errno)
{
        return ERR_PTR(errno);
}

/**
 * @brief 将指针的值转换为错误码
 * @param ptr: (I) 指针
 * @return 错误码
 */
static __xw_inline
xwer_t __must_check ptr_err(__force const void *ptr)
{
        return (xwer_t)PTR_ERR(ptr);
}

/**
 * @brief 测试指针的值是否为错误码
 * @param ptr: (I) 指针
 * @retval true: 是
 * @retval false: 否
 */
static __xw_inline
bool __must_check is_err(__force const void *ptr)
{
        return IS_ERR(ptr);
}

/**
 * @brief 测试指针的值是否为错误码或空(指针是否为无效指针)
 * @param ptr: (I) 指针
 * @retval true: 是
 * @retval false: 否
 */
static __xw_inline
bool __must_check is_err_or_null(__force const void *ptr)
{
        return IS_ERR_OR_NULL(ptr);
}

#endif /* xwos/err.h */
