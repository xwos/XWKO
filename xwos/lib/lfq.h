/**
 * @file
 * @brief XWOS通用库：无锁队列
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

#ifndef __xwos_lib_lfq_h__
#define __xwos_lib_lfq_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <asmlib/lfq.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 初始化无锁队列节点
 * @param n: (I) 队列节点指针
 */
static __xwlib_inline
void xwlib_lfq_init(__xwcc_atomic xwlfq_t * n)
{
        *n = (xwlfq_t)0;
}

/**
 * @brief 将一个节点推入无锁队列（栈式队列）
 * @param h: (I) 无锁队列头的指针
 * @param n: (I) 入队节点的指针
 */
static __xwlib_inline
void xwlib_lfq_push(__xwcc_atomic xwlfq_t * h, __xwcc_atomic xwlfq_t * n)
{
        arch_lfq_push(h, n);
}

/**
 * @brief 弹出无锁队列中的第一个节点（栈式队列）
 * @param h: (I) 无锁队列头的指针
 * @return 被弹出的节点
 */
static __xwlib_inline
xwlfq_t * xwlib_lfq_pop(__xwcc_atomic xwlfq_t * h)
{
        return arch_lfq_pop(h);
}

#endif /* xwos/lib/lfq.h */
