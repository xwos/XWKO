/**
 * @file
 * @brief XWOS通用库：原子操作
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

#ifndef __xwos_lib_xwaop_h__
#define __xwos_lib_xwaop_h__

#include <xwos/standard.h>

/**
 * @defgroup xwaop 原子操作
 * @{
 */

#include <xwos/ospl/soc/xwaop8.h>
#include <xwos/ospl/soc/xwaop16.h>
#include <xwos/ospl/soc/xwaop32.h>
#include <xwos/ospl/soc/xwaop64.h>

/**
 * @brief 调用原子操作
 * @param[in] type: 原子变量的类型
 * @param[in] op: 操作函数
 * @param[in] memorder: 内存顺序
 * @param[in] a: 原子变量指针
 * @param[in,out] ...: 其他参数
 */
#define xwmoaop(type, op, a, memorder, ...) \
        xwaop__##type##__##op(a, memorder, ##__VA_ARGS__)

/**
 * @brief XWOS AOPLIB：加载原子变量
 * @param[in] type: 类型
 * @param[in] a: 数据地址的指针
 * @param[in] memorder: 内存顺序
 * @return 原子变量的值
 */
#define xwaop_load(type, a, memorder) \
        xwmoaop(type, load, (a), memorder)

/**
 * @brief XWOS AOPLIB：存储原子变量
 * @param[in] type: 类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] v: 新值
 * @param[in] memorder: 内存顺序
 * @return 写入后，原子变量的值
 */
#define xwaop_store(type, a, memorder, v) \
        xwmoaop(type, store, (a), memorder, (v))

/**
 * @brief 调用原子操作函数
 * @param[in] type: 原子变量的类型
 * @param[in] op: 操作函数
 * @param[in] a: 原子变量指针
 * @param[in,out] ...: 其他参数
 */
#define xwaop(type, op, a, ...) \
        xwaop__##type##__##op(a, ##__VA_ARGS__)

/**
 * @brief XWOS AOPLIB：读取原子变量的值
 * @param[in] type: 类型
 * @param[in] a: 数据地址的指针
 * @param[out] ov: 指向缓冲区的指针，此缓冲区用于返回原子变量的值
 * @note
 * - 内存序：acquire
 */
#define xwaop_read(type, a, ov) \
        xwaop(type, read, (a), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并将原子变量写为新值
 * @param[in] type: 类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] v: 新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @note
 * - 内存序：acq_rel
 */
#define xwaop_write(type, a, v, ov) \
        xwaop(type, write, (a), (v), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的值，并测试旧值是否等于测试值：
 *                     - 如果结果为真，将原子变量设置为新值，并返回原子变量的旧值；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume。
 */
#define xwaop_teq_then_write(type, a, t, v, ov) \
        xwaop(type, teq_then_write, (a), (t), (v), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的值，并测试旧值是否不等于测试值。
 *                     - 如果结果为真，将原子变量设置为新值，并返回原子变量的旧值；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume。
 */
#define xwaop_tne_then_write(type, a, t, v, ov) \
        xwaop(type, tne_then_write, (a), (t), (v), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的值，并测试旧值是否大于测试值。
 *                     - 如果结果为真，将原子变量设置为新值，并返回原子变量的旧值；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume。
 */
#define xwaop_tgt_then_write(type, a, t, v, ov) \
        xwaop(type, tgt_then_write, (a), (t), (v), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的值，并测试旧值是否大于等于测试值。
 *                     - 如果结果为真，将原子变量设置为新值，并返回原子变量的旧值；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume。
 */
#define xwaop_tge_then_write(type, a, t, v, ov) \
        xwaop(type, tge_then_write, (a), (t), (v), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的值，并测试旧值是否小于测试值。
 *                     - 如果结果为真，将原子变量设置为新值，并返回原子变量的旧值；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume。
 */
#define xwaop_tlt_then_write(type, a, t, v, ov) \
        xwaop(type, tlt_then_write, (a), (t), (v), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的值，并测试旧值是否小于等于测试值。
 *                     - 如果结果为真，将原子变量设置为新值，并返回原子变量的旧值；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume。
 */
#define xwaop_tle_then_write(type, a, t, v, ov) \
        xwaop(type, tle_then_write, (a), (t), (v), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的值，并测试旧值是否属于区间(l,r)。
 *                     - 如果结果为真，将原子变量设置为新值，并返回原子变量的旧值；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume。
 */
#define xwaop_tgtlt_then_write(type, a, l, r, v, ov) \
        xwaop(type, tgtlt_then_write, (a), (l), (r), (v), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的值，并测试旧值是否属于区间[l,r)。
 *                     - 如果结果为真，将原子变量设置为新值，并返回原子变量的旧值；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume。
 */
#define xwaop_tgelt_then_write(type, a, l, r, v, ov) \
        xwaop(type, tgelt_then_write, (a), (l), (r), (v), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的值，并测试旧值是否属于区间(l,r]。
 *                     - 如果结果为真，将原子变量设置为新值，并返回原子变量的旧值；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume。
 */
#define xwaop_tgtle_then_write(type, a, l, r, v, ov) \
        xwaop(type, tgtle_then_write, (a), (l), (r), (v), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的值，并测试旧值是否属于区间[l,r]。
 *                     - 如果结果为真，将原子变量设置为新值，并返回原子变量的旧值；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume。
 */
#define xwaop_tgele_then_write(type, a, l, r, v, ov) \
        xwaop(type, tgele_then_write, (a), (l), (r), (v), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的值，并增加加数v，结果回写到原子变量中
 * @param[in] type: 类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] v: 加数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @note
 * - 内存序：acq_rel
 */
#define xwaop_add(type, a, v, nv, ov) \
        xwaop(type, add, (a), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否等于测试值t：
 *                     - 如果结果为真，旧值增加加数v，结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 加数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_teq_then_add(type, a, t, v, nv, ov) \
        xwaop(type, teq_then_add, (a), (t), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否不等于测试值t：
 *                     - 如果结果为真，旧值增加加数v，结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 加数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tne_then_add(type, a, t, v, nv, ov) \
        xwaop(type, tne_then_add, (a), (t), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否大于测试值t：
 *                     - 如果结果为真，旧值增加加数v，结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 加数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tgt_then_add(type, a, t, v, nv, ov) \
        xwaop(type, tgt_then_add, (a), (t), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否大于等于测试值t：
 *                     - 如果结果为真，旧值增加加数v，结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 加数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tge_then_add(type, a, t, v, nv, ov) \
        xwaop(type, tge_then_add, (a), (t), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否小于测试值t：
 *                     - 如果结果为真，旧值增加加数v，结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 加数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tlt_then_add(type, a, t, v, nv, ov) \
        xwaop(type, tlt_then_add, (a), (t), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否小于等于测试值t：
 *                     - 如果结果为真，旧值增加加数v，结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 加数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tle_then_add(type, a, t, v, nv, ov) \
        xwaop(type, tle_then_add, (a), (t), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否属于区间(l,r)：
 *                     - 如果结果为真，旧值增加加数v，结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 加数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tgtlt_then_add(type, a, l, r, v, nv, ov) \
        xwaop(type, tgtlt_then_add, (a), (l), (r), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否属于区间[l,r)：
 *                     - 如果结果为真，旧值增加加数v，结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 加数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tgelt_then_add(type, a, l, r, v, nv, ov) \
        xwaop(type, tgelt_then_add, (a), (l), (r), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否属于区间(l,r]：
 *                     - 如果结果为真，旧值增加加数v，结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 加数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tgtle_then_add(type, a, l, r, v, nv, ov) \
        xwaop(type, tgtle_then_add, (a), (l), (r), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否属于区间[l,r]：
 *                     - 如果结果为真，旧值增加加数v，结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 加数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tgele_then_add(type, a, l, r, v, nv, ov) \
        xwaop(type, tgele_then_add, (a), (l), (r), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并减去减数v，结果回写到原子变量中
 * @param[in] type: 类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] v: 减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @note
 * - 内存序：acq_rel
 */
#define xwaop_sub(type, a, v, nv, ov) \
        xwaop(type, sub, (a), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否等于测试值t：
 *                     - 如果结果为真，旧值减去减数v，结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_teq_then_sub(type, a, t, v, nv, ov) \
        xwaop(type, teq_then_sub, (a), (t), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否不等于测试值t：
 *                     - 如果结果为真，旧值减去减数v，结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tne_then_sub(type, a, t, v, nv, ov) \
        xwaop(type, tne_then_sub, (a), (t), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否大于测试值t：
 *                     - 如果结果为真，旧值减去减数v，结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tgt_then_sub(type, a, t, v, nv, ov) \
        xwaop(type, tgt_then_sub, (a), (t), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否大于等于测试值t：
 *                     - 如果结果为真，旧值减去减数v，结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tge_then_sub(type, a, t, v, nv, ov) \
        xwaop(type, tge_then_sub, (a), (t), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否小于测试值t：
 *                     - 如果结果为真，旧值减去减数v，结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tlt_then_sub(type, a, t, v, nv, ov) \
        xwaop(type, tlt_then_sub, (a), (t), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否小于等于测试值t：
 *                     - 如果结果为真，旧值减去减数v，结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tle_then_sub(type, a, t, v, nv, ov) \
        xwaop(type, tle_then_sub, (a), (t), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否属于区间(l,r)：
 *                     - 如果结果为真，旧值减去减数v，结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tgtlt_then_sub(type, a, l, r, v, nv, ov) \
        xwaop(type, tgtlt_then_sub, (a), (l), (r), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否属于区间[l,r)：
 *                     - 如果结果为真，旧值减去减数v，结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tgelt_then_sub(type, a, l, r, v, nv, ov) \
        xwaop(type, tgelt_then_sub, (a), (l), (r), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否属于区间(l,r]：
 *                     - 如果结果为真，旧值减去减数v，结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tgtle_then_sub(type, a, l, r, v, nv, ov) \
        xwaop(type, tgtle_then_sub, (a), (l), (r), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否属于区间[l,r]：
 *                     - 如果结果为真，旧值减去减数v，结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tgele_then_sub(type, a, l, r, v, nv, ov) \
        xwaop(type, tgele_then_sub, (a), (l), (r), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并执行反向减法，从被减数v中减去旧值，
 *                     结果回写到原子变量中。
 * @param[in] type: 类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] v: 被减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @note
 * - 内存序：acq_rel
 */
#define xwaop_rsb(type, a, nv, ov) \
        xwaop(type, rsb, (a), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否等于测试值t：
 *                     - 如果结果为真，执行反向减法，从被减数v中减去旧值，
 *                       结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 被减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_teq_then_rsb(type, a, t, v, nv, ov) \
        xwaop(type, teq_then_rsb, (a), (t), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否不等于测试值t：
 *                     - 如果结果为真，执行反向减法，从被减数v中减去旧值，
 *                       结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 被减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tne_then_rsb(type, a, t, v, nv, ov) \
        xwaop(type, tne_then_rsb, (a), (t), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否大于测试值t：
 *                     - 如果结果为真，执行反向减法，从被减数v中减去旧值，
 *                       结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 被减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tgt_then_rsb(type, a, t, v, nv, ov) \
        xwaop(type, tgt_then_rsb, (a), (t), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否大于等于测试值t：
 *                     - 如果结果为真，执行反向减法，从被减数v中减去旧值，
 *                       结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 被减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tge_then_rsb(type, a, t, v, nv, ov) \
        xwaop(type, tge_then_rsb, (a), (t), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否小于测试值t：
 *                     - 如果结果为真，执行反向减法，从被减数v中减去旧值，
 *                       结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 被减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tlt_then_rsb(type, a, t, v, nv, ov) \
        xwaop(type, tlt_then_rsb, (a), (t), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否小于等于测试值t：
 *                     - 如果结果为真，执行反向减法，从被减数v中减去旧值，
 *                       结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 被减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tle_then_rsb(type, a, t, v, nv, ov) \
        xwaop(type, tle_then_rsb, (a), (t), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否属于区间(l,r)：
 *                     - 如果结果为真，执行反向减法，从被减数v中减去旧值，
 *                       结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 被减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tgtlt_then_rsb(type, a, l, r, v, nv, ov) \
        xwaop(type, tgtlt_then_rsb, (a), (l), (r), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否属于区间[l,r)：
 *                     - 如果结果为真，执行反向减法，从被减数v中减去旧值，
 *                       结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 被减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tgelt_then_rsb(type, a, l, r, v, nv, ov) \
        xwaop(type, tgelt_then_rsb, (a), (l), (r), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否属于区间(l,r]：
 *                     - 如果结果为真，执行反向减法，从被减数v中减去旧值，
 *                       结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 被减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tgtle_then_rsb(type, a, l, r, v, nv, ov) \
        xwaop(type, tgtle_then_rsb, (a), (l), (r), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值是否属于区间[l,r]：
 *                     - 如果结果为真，执行反向减法，从被减数v中减去旧值，
 *                       结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 被减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tgele_then_rsb(type, a, l, r, v, nv, ov) \
        xwaop(type, tgele_then_rsb, (a), (l), (r), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并和操作数v进行与操作，
 *                     结果回写到原子变量中
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] v: 操作数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @note
 * - 内存序：acq_rel
 */
#define xwaop_and(type, a, v, nv, ov) \
        xwaop(type, and, (a), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并和操作数v进行或操作，
 *                     结果回写到原子变量中
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] v: 操作数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @note
 * - 内存序：acq_rel
 */
#define xwaop_or(type, a, v, nv, ov) \
        xwaop(type, or, (a), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并和操作数v进行异或操作，
 *                     结果回写到原子变量中
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] v: 操作数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @note
 * - 内存序：acq_rel
 */
#define xwaop_xor(type, a, v, nv, ov) \
        xwaop(type, xor, (a), (v), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并按掩码m进行置1操作，
 *                     结果回写到原子变量中
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] m: 掩码位
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @note
 * - 内存序：acq_rel
 */
#define xwaop_s1m(type, a, m, nv, ov) \
        xwaop(type, or, (a), (m), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并按掩码m进行清零操作，
 *                     结果回写到原子变量中
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] m: 掩码位
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @note
 * - 内存序：acq_rel
 */
#define xwaop_c0m(type, a, m, nv, ov) \
        xwaop(type, c0m, (a), (m), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并按掩码m进行位翻转操作，
 *                     结果回写到原子变量中
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] m: 掩码位
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @note
 * - 内存序：acq_rel
 */
#define xwaop_x1m(type, a, m, nv, ov) \
        xwaop(type, xor, (a), (m), (nv), (ov))


/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值中的掩码m覆盖的部分是否都为1：
 *                     - 如果结果为真，将他们全部清零，结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] m: 掩码
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_t1ma_then_c0m(type, a, m, nv, ov) \
        xwaop(type, t1ma_then_c0m, (a), (m), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值中的掩码m覆盖的部分
                       是否至少有一位为1：
 *                     - 如果结果为真，将他们全部清零，结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] m: 掩码
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_t1mo_then_c0m(type, a, m, nv, ov) \
        xwaop(type, t1mo_then_c0m, (a), (m), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值中的掩码m覆盖的部分是否都为0：
 *                     - 如果结果为真，将他们全部置1，结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] m: 掩码
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_t0ma_then_s1m(type, a, m, nv, ov) \
        xwaop(type, t0ma_then_s1m, (a), (m), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并测试旧值中的掩码m覆盖的部分
                       是否至少有一位为0：
 *                     - 如果结果为真，将他们全部置1，结果回写到原子变量中；
 *                     - 如果结果为假，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] m: 掩码
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_t0mo_then_s1m(type, a, m, nv, ov) \
        xwaop(type, t0mo_then_s1m, (a), (m), (nv), (ov))

/**
 * @brief XWOS AOPLIB：读取原子变量的旧值，并使用测试函数测试：
 *                     - 如果结果为真或测试函数为空，使用操作函数操作修改此值，
 *                       结果回写到原子变量中；
 *                     - 如果结果为假或操作函数为空，返回原子变量的旧值。
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] tf: 测试函数的函数指针
 * @param[in] tfarg: 测试函数的参数
 * @param[in] of: 操作函数的函数指针
 * @param[in] ofarg: 操作函数的参数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval OK: 测试成功或测试函数为空
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 * - 测试成功时，nv为原子操作后的新值，ov为原子操作前的旧值；
 * - 测试失败时，nv与ov都为旧值。
 */
#define xwaop_tst_then_op(type, a, tf, tfarg, of, ofarg, nv, ov) \
        xwaop(type, tst_then_op, (a), (tf), (tfarg), (of), (ofarg), (nv), (ov))

/******** ******** load ******** ********/
/**
 * @brief 定义原子操作模板：load
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_LOAD(type, btype) \
static __xwlib_inline \
type##_t xwaop__##type##__load(type##_a * a, \
                               const enum xwmb_memory_order_em mo) \
{ \
        return (type##_t)xwaop__##btype##__load((btype##_a *)a, \
                                                mo); \
}

/******** ******** store ******** ********/
/**
 * @brief 定义原子操作模板：store
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_STORE(type, btype) \
static __xwlib_inline \
type##_t xwaop__##type##__store(type##_a * a, \
                                const enum xwmb_memory_order_em mo, \
                                type##_t v) \
{ \
        return (type##_t)xwaop__##btype##__store((btype##_a *)a, \
                                                 mo, \
                                                 (btype##_t)v); \
}

/******** ******** read ******** ********/
/**
 * @brief 定义原子操作模板：read
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_READ(type, btype) \
static __xwlib_inline \
void xwaop__##type##__read(type##_a * a, \
                           type##_t * ov) \
{ \
        xwaop__##btype##__read((btype##_a *)a, \
                               (btype##_t *)ov); \
}

/******** ******** write ******** ********/
/**
 * @brief 定义原子操作模板：write
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_WRITE(type, btype) \
static __xwlib_inline \
void xwaop__##type##__write(type##_a * a, \
                            type##_t v, \
                            type##_t * ov) \
{ \
        xwaop__##btype##__write((btype##_a *)a, \
                                (btype##_t)v, \
                                (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：teq_then_write
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TEQ_THEN_WRITE(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__teq_then_write(type##_a * a, \
                                       type##_t t, \
                                       type##_t v, \
                                       type##_t * ov) \
{ \
        return xwaop__##btype##__teq_then_write((btype##_a *)a, \
                                                (btype##_t)t, \
                                                (btype##_t)v, \
                                                (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tne_then_write
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TNE_THEN_WRITE(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tne_then_write(type##_a * a, \
                                       type##_t t, \
                                       type##_t v, \
                                       type##_t * ov) \
{ \
        return xwaop__##btype##__tne_then_write((btype##_a *)a, \
                                                (btype##_t)t, \
                                                (btype##_t)v, \
                                                (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tgt_then_write
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGT_THEN_WRITE(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgt_then_write(type##_a * a, \
                                       type##_t t, \
                                       type##_t v, \
                                       type##_t * ov) \
{ \
        return xwaop__##btype##__tgt_then_write((btype##_a *)a, \
                                                (btype##_t)t, \
                                                (btype##_t)v, \
                                                (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tge_then_write
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGE_THEN_WRITE(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tge_then_write(type##_a * a, \
                                       type##_t t, \
                                       type##_t v, \
                                       type##_t * ov) \
{ \
        return xwaop__##btype##__tge_then_write((btype##_a *)a, \
                                                (btype##_t)t, \
                                                (btype##_t)v, \
                                                (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tlt_then_write
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TLT_THEN_WRITE(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tlt_then_write(type##_a * a, \
                                       type##_t t, \
                                       type##_t v, \
                                       type##_t * ov) \
{ \
        return xwaop__##btype##__tlt_then_write((btype##_a *)a, \
                                                (btype##_t)t, \
                                                (btype##_t)v, \
                                                (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tle_then_write
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TLE_THEN_WRITE(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tle_then_write(type##_a * a, \
                                       type##_t t, \
                                       type##_t v, \
                                       type##_t * ov) \
{ \
        return xwaop__##btype##__tle_then_write((btype##_a *)a, \
                                                (btype##_t)t, \
                                                (btype##_t)v, \
                                                (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tgtlt_then_write
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGTLT_THEN_WRITE(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgtlt_then_write(type##_a * a, \
                                         type##_t l, type##_t r, \
                                         type##_t v, \
                                         type##_t * ov) \
{ \
        return xwaop__##btype##__tgtlt_then_write((btype##_a *)a, \
                                                  (btype##_t)l, (btype##_t)r, \
                                                  (btype##_t)v, \
                                                  (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tgelt_then_write
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGELT_THEN_WRITE(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgelt_then_write(type##_a * a, \
                                         type##_t l, type##_t r, \
                                         type##_t v, \
                                         type##_t * ov) \
{ \
        return xwaop__##btype##__tgelt_then_write((btype##_a *)a, \
                                                  (btype##_t)l, (btype##_t)r, \
                                                  (btype##_t)v, \
                                                  (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tgtle_then_write
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGTLE_THEN_WRITE(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgtle_then_write(type##_a * a, \
                                         type##_t l, type##_t r, \
                                         type##_t v, \
                                         type##_t * ov) \
{ \
        return xwaop__##btype##__tgtle_then_write((btype##_a *)a, \
                                                  (btype##_t)l, (btype##_t)r, \
                                                  (btype##_t)v, \
                                                  (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tgele_then_write
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGELE_THEN_WRITE(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgele_then_write(type##_a * a, \
                                         type##_t l, type##_t r, \
                                         type##_t v, \
                                         type##_t * ov) \
{ \
        return xwaop__##btype##__tgele_then_write((btype##_a *)a, \
                                                  (btype##_t)l, (btype##_t)r, \
                                                  (btype##_t)v, \
                                                  (btype##_t *)ov); \
}

/******** ******** add ******** ********/
/**
 * @brief 定义原子操作模板：add
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_ADD(type, btype) \
static __xwlib_inline \
void xwaop__##type##__add(type##_a * a, \
                          type##_t v, \
                          type##_t * nv, type##_t * ov) \
{ \
        xwaop__##btype##__add((btype##_a *)a, \
                              (btype##_t)v, \
                              (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：teq_then_add
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TEQ_THEN_ADD(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__teq_then_add(type##_a * a, \
                                     type##_t t, \
                                     type##_t v, \
                                     type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__teq_then_add((btype##_a *)a, \
                                              (btype##_t)t, (btype##_t)v, \
                                              (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tne_then_add
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TNE_THEN_ADD(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tne_then_add(type##_a * a, \
                                     type##_t t, \
                                     type##_t v, \
                                     type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tne_then_add((btype##_a *)a, \
                                              (btype##_t)t, \
                                              (btype##_t)v, \
                                              (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tgt_then_add
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGT_THEN_ADD(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgt_then_add(type##_a * a, \
                                     type##_t t, \
                                     type##_t v, \
                                     type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tgt_then_add((btype##_a *)a, \
                                              (btype##_t)t, \
                                              (btype##_t)v, \
                                              (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tge_then_add
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGE_THEN_ADD(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tge_then_add(type##_a * a, \
                                     type##_t t, \
                                     type##_t v, \
                                     type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tge_then_add((btype##_a *)a, \
                                              (btype##_t)t, \
                                              (btype##_t)v, \
                                              (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tlt_then_add
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TLT_THEN_ADD(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tlt_then_add(type##_a * a, \
                                     type##_t t, \
                                     type##_t v, \
                                     type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tlt_then_add((btype##_a *)a, \
                                              (btype##_t)t, \
                                              (btype##_t)v, \
                                              (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tle_then_add
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TLE_THEN_ADD(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tle_then_add(type##_a * a, \
                                     type##_t t, \
                                     type##_t v, \
                                     type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tle_then_add((btype##_a *)a, \
                                              (btype##_t)t, \
                                              (btype##_t)v, \
                                              (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tgtlt_then_add
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGTLT_THEN_ADD(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgtlt_then_add(type##_a * a, \
                                       type##_t l, type##_t r, \
                                       type##_t v, \
                                       type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tgtlt_then_add((btype##_a *)a, \
                                                (btype##_t)l, (btype##_t)r, \
                                                (btype##_t)v, \
                                                (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tgelt_then_add
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGELT_THEN_ADD(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgelt_then_add(type##_a * a, \
                                       type##_t l, type##_t r, \
                                       type##_t v, \
                                       type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tgelt_then_add((btype##_a *)a, \
                                                (btype##_t)l, (btype##_t)r, \
                                                (btype##_t)v, \
                                                (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tgtle_then_add
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGTLE_THEN_ADD(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgtle_then_add(type##_a * a, \
                                       type##_t l, type##_t r, \
                                       type##_t v, \
                                       type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tgtle_then_add((btype##_a *)a, \
                                                (btype##_t)l, (btype##_t)r, \
                                                (btype##_t)v, \
                                                (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tgele_then_add
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGELE_THEN_ADD(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgele_then_add(type##_a * a, \
                                       type##_t l, type##_t r, \
                                       type##_t v, \
                                       type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tgele_then_add((btype##_a *)a, \
                                                (btype##_t)l, (btype##_t)r, \
                                                (btype##_t)v, \
                                                (btype##_t *)nv, (btype##_t *)ov); \
}

/******** ******** subtract ******** ********/
/**
 * @brief 定义原子操作模板：sub
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_SUB(type, btype) \
static __xwlib_inline \
void xwaop__##type##__sub(type##_a * a, \
                          type##_t v, \
                          type##_t * nv, type##_t * ov) \
{ \
        xwaop__##btype##__sub((btype##_a *)a, \
                              (btype##_t)v, \
                              (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：teq_then_sub
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TEQ_THEN_SUB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__teq_then_sub(type##_a * a, \
                                     type##_t t, \
                                     type##_t v, \
                                     type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__teq_then_sub((btype##_a *)a, \
                                              (btype##_t)t, \
                                              (btype##_t)v, \
                                              (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tne_then_sub
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TNE_THEN_SUB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tne_then_sub(type##_a * a, \
                                     type##_t t, \
                                     type##_t v, \
                                     type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tne_then_sub((btype##_a *)a, \
                                              (btype##_t)t, \
                                              (btype##_t)v, \
                                              (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tge_then_sub
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGT_THEN_SUB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgt_then_sub(type##_a * a, \
                                     type##_t t, \
                                     type##_t v, \
                                     type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tgt_then_sub((btype##_a *)a, \
                                              (btype##_t)t, \
                                              (btype##_t)v, \
                                              (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tge_then_sub
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGE_THEN_SUB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tge_then_sub(type##_a * a, \
                                     type##_t t, \
                                     type##_t v, \
                                     type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tge_then_sub((btype##_a *)a, \
                                              (btype##_t)t, \
                                              (btype##_t)v, \
                                              (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tlt_then_sub
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TLT_THEN_SUB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tlt_then_sub(type##_a * a, \
                                     type##_t t, \
                                     type##_t v, \
                                     type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tlt_then_sub((btype##_a *)a, \
                                              (btype##_t)t, \
                                              (btype##_t)v, \
                                              (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tle_then_sub
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TLE_THEN_SUB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tle_then_sub(type##_a * a, \
                                     type##_t t, \
                                     type##_t v, \
                                     type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tle_then_sub((btype##_a *)a, \
                                              (btype##_t)t, \
                                              (btype##_t)v, \
                                              (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tgtlt_then_sub
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGTLT_THEN_SUB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgtlt_then_sub(type##_a * a, \
                                       type##_t l, type##_t r, \
                                       type##_t v, \
                                       type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tgtlt_then_sub((btype##_a *)a, \
                                                (btype##_t)l, (btype##_t)r, \
                                                (btype##_t)v, \
                                                (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tgelt_then_sub
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGELT_THEN_SUB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgelt_then_sub(type##_a * a, \
                                       type##_t l, type##_t r, \
                                       type##_t v, \
                                       type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tgelt_then_sub((btype##_a *)a, \
                                                (btype##_t)l, (btype##_t)r, \
                                                (btype##_t)v, \
                                                (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tgtle_then_sub
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGTLE_THEN_SUB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgtle_then_sub(type##_a * a, \
                                       type##_t l, type##_t r, \
                                       type##_t v, \
                                       type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tgtle_then_sub((btype##_a *)a, \
                                                (btype##_t)l, (btype##_t)r, \
                                                (btype##_t)v, \
                                                (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tgele_then_sub
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGELE_THEN_SUB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgele_then_sub(type##_a * a, \
                                       type##_t l, type##_t r, \
                                       type##_t v, \
                                       type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tgele_then_sub((btype##_a *)a, \
                                                (btype##_t)l, (btype##_t)r, \
                                                (btype##_t)v, \
                                                (btype##_t *)nv, (btype##_t *)ov); \
}

/******** ******** reverse subtract ******** ********/
/**
 * @brief 定义原子操作模板：rsb
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_RSB(type, btype) \
static __xwlib_inline \
void xwaop__##type##__rsb(type##_a * a, \
                          type##_t v, \
                          type##_t * nv, type##_t * ov) \
{ \
        xwaop__##btype##__rsb((btype##_a *)a, \
                              (btype##_t)v, \
                              (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：teq_then_rsb
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TEQ_THEN_RSB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__teq_then_rsb(type##_a * a, \
                                     type##_t t, \
                                     type##_t v, \
                                     type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__teq_then_rsb((btype##_a *)a, \
                                              (btype##_t)t, \
                                              (btype##_t)v, \
                                              (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tne_then_rsb
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TNE_THEN_RSB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tne_then_rsb(type##_a * a, \
                                     type##_t t, \
                                     type##_t v, \
                                     type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tne_then_rsb((btype##_a *)a, \
                                              (btype##_t)t, \
                                              (btype##_t)v, \
                                              (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tgt_then_rsb
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGT_THEN_RSB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgt_then_rsb(type##_a * a, \
                                     type##_t t, \
                                     type##_t v, \
                                     type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tgt_then_rsb((btype##_a *)a, \
                                              (btype##_t)t, \
                                              (btype##_t)v, \
                                              (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tge_then_rsb
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGE_THEN_RSB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tge_then_rsb(type##_a * a, \
                                     type##_t t, \
                                     type##_t v, \
                                     type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tge_then_rsb((btype##_a *)a, \
                                              (btype##_t)t, \
                                              (btype##_t)v, \
                                              (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tle_then_rsb
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TLT_THEN_RSB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tlt_then_rsb(type##_a * a, \
                                     type##_t t, \
                                     type##_t v, \
                                     type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tlt_then_rsb((btype##_a *)a, \
                                              (btype##_t)t, \
                                              (btype##_t)v, \
                                              (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tle_then_rsb
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TLE_THEN_RSB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tle_then_rsb(type##_a * a, \
                                     type##_t t, \
                                     type##_t v, \
                                     type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tle_then_rsb((btype##_a *)a, \
                                              (btype##_t)t, \
                                              (btype##_t)v, \
                                              (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tgtlt_then_rsb
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGTLT_THEN_RSB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgtlt_then_rsb(type##_a * a, \
                                       type##_t l, type##_t r, \
                                       type##_t v, \
                                       type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tgtlt_then_rsb((btype##_a *)a, \
                                                (btype##_t)l, (btype##_t)r, \
                                                (btype##_t)v, \
                                                (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tgelt_then_rsb
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGELT_THEN_RSB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgelt_then_rsb(type##_a * a, \
                                       type##_t l, type##_t r, \
                                       type##_t v, \
                                       type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tgelt_then_rsb((btype##_a *)a, \
                                                (btype##_t)l, (btype##_t)r, \
                                                (btype##_t)v, \
                                                (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tgtle_then_rsb
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGTLE_THEN_RSB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgtle_then_rsb(type##_a * a, \
                                       type##_t l, type##_t r, \
                                       type##_t v, \
                                       type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tgtle_then_rsb((btype##_a *)a, \
                                                (btype##_t)l, (btype##_t)r, \
                                                (btype##_t)v, \
                                                (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：tgele_then_rsb
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGELE_THEN_RSB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgele_then_rsb(type##_a * a, \
                                       type##_t l, type##_t r, \
                                       type##_t v, \
                                       type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tgele_then_rsb((btype##_a *)a, \
                                                (btype##_t)l, (btype##_t)r, \
                                                (btype##_t)v, \
                                                (btype##_t *)nv, (btype##_t *)ov); \
}

/******** ******** bit operations ******** ********/
/**
 * @brief 定义原子操作模板：or
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_OR(type, btype) \
static __xwlib_inline \
void xwaop__##type##__or(type##_a * a, \
                         type##_t v, \
                         type##_t * nv, type##_t * ov) \
{ \
        xwaop__##btype##__or((btype##_a *)a, \
                             (btype##_t)v, \
                             (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：and
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_AND(type, btype) \
static __xwlib_inline \
void xwaop__##type##__and(type##_a * a, \
                          type##_t v, \
                          type##_t * nv, type##_t * ov) \
{ \
        xwaop__##btype##__and((btype##_a *)a, \
                              (btype##_t)v, \
                              (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：xor
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_XOR(type, btype) \
static __xwlib_inline \
void xwaop__##type##__xor(type##_a * a, \
                          type##_t v, \
                          type##_t * nv, type##_t * ov) \
{ \
        xwaop__##btype##__xor((btype##_a *)a, \
                              (btype##_t)v, \
                              (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：c0m
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_C0M(type, btype) \
static __xwlib_inline \
void xwaop__##type##__c0m(type##_a * a, \
                          type##_t m, \
                          type##_t * nv, type##_t * ov) \
{ \
        xwaop__##btype##__c0m((btype##_a *)a, \
                              (btype##_t)m, \
                              (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：t1ma_then_c0m
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_T1MA_THEN_C0M(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__t1ma_then_c0m(type##_a * a, \
                                      type##_t m, \
                                      type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__t1ma_then_c0m((btype##_a *)a, \
                                               (btype##_t)m, \
                                               (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：t1mo_then_c0m
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_T1MO_THEN_C0M(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__t1mo_then_c0m(type##_a * a, \
                                      type##_t m, \
                                      type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__t1mo_then_c0m((btype##_a *)a, \
                                               (btype##_t)m, \
                                               (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：t0ma_then_s1m
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_T0MA_THEN_S1M(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__t0ma_then_s1m(type##_a * a, \
                                      type##_t m, \
                                      type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__t0ma_then_s1m((btype##_a *)a, \
                                               (btype##_t)m, \
                                               (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 定义原子操作模板：t0mo_then_s1m
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_T0MO_THEN_S1M(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__t0mo_then_s1m(type##_a * a, \
                                      type##_t m, \
                                      type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__t0mo_then_s1m((btype##_a *)a, \
                                               (btype##_t)m, \
                                               (btype##_t *)nv, (btype##_t *)ov); \
}

/******** ******** test and operation ******** ********/
/**
 * @brief 定义原子操作模板：tst_then_op
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TST_THEN_OP(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tst_then_op(type##_a * a, \
                                    xwaop_tst_f tst, void * tst_args, \
                                    xwaop_op_f op, void * op_args, \
                                    type##_t * nv, type##_t * ov) \
{ \
        return xwaop__##btype##__tst_then_op((btype##_a *)a, \
                                             tst, tst_args, \
                                             op, op_args, \
                                             (btype##_t *)nv, (btype##_t *)ov); \
}

/**
 * @brief 使用模板为某一类型定义原子操作函数集合
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP(type, btype) \
        DEFINE_XWAOP_LOAD(type, btype) \
        DEFINE_XWAOP_STORE(type, btype) \
        DEFINE_XWAOP_READ(type, btype) \
        DEFINE_XWAOP_WRITE(type, btype) \
        DEFINE_XWAOP_TEQ_THEN_WRITE(type, btype) \
        DEFINE_XWAOP_TNE_THEN_WRITE(type, btype) \
        DEFINE_XWAOP_TGT_THEN_WRITE(type, btype) \
        DEFINE_XWAOP_TGE_THEN_WRITE(type, btype) \
        DEFINE_XWAOP_TLT_THEN_WRITE(type, btype) \
        DEFINE_XWAOP_TLE_THEN_WRITE(type, btype) \
        DEFINE_XWAOP_TGTLT_THEN_WRITE(type, btype) \
        DEFINE_XWAOP_TGELT_THEN_WRITE(type, btype) \
        DEFINE_XWAOP_TGTLE_THEN_WRITE(type, btype) \
        DEFINE_XWAOP_TGELE_THEN_WRITE(type, btype) \
        DEFINE_XWAOP_ADD(type, btype) \
        DEFINE_XWAOP_TEQ_THEN_ADD(type, btype) \
        DEFINE_XWAOP_TNE_THEN_ADD(type, btype) \
        DEFINE_XWAOP_TGT_THEN_ADD(type, btype) \
        DEFINE_XWAOP_TGE_THEN_ADD(type, btype) \
        DEFINE_XWAOP_TLT_THEN_ADD(type, btype) \
        DEFINE_XWAOP_TLE_THEN_ADD(type, btype) \
        DEFINE_XWAOP_TGTLT_THEN_ADD(type, btype) \
        DEFINE_XWAOP_TGELT_THEN_ADD(type, btype) \
        DEFINE_XWAOP_TGTLE_THEN_ADD(type, btype) \
        DEFINE_XWAOP_TGELE_THEN_ADD(type, btype) \
        DEFINE_XWAOP_SUB(type, btype) \
        DEFINE_XWAOP_TEQ_THEN_SUB(type, btype) \
        DEFINE_XWAOP_TNE_THEN_SUB(type, btype) \
        DEFINE_XWAOP_TGT_THEN_SUB(type, btype) \
        DEFINE_XWAOP_TGE_THEN_SUB(type, btype) \
        DEFINE_XWAOP_TLT_THEN_SUB(type, btype) \
        DEFINE_XWAOP_TLE_THEN_SUB(type, btype) \
        DEFINE_XWAOP_TGTLT_THEN_SUB(type, btype) \
        DEFINE_XWAOP_TGELT_THEN_SUB(type, btype) \
        DEFINE_XWAOP_TGTLE_THEN_SUB(type, btype) \
        DEFINE_XWAOP_TGELE_THEN_SUB(type, btype) \
        DEFINE_XWAOP_RSB(type, btype) \
        DEFINE_XWAOP_TEQ_THEN_RSB(type, btype) \
        DEFINE_XWAOP_TNE_THEN_RSB(type, btype) \
        DEFINE_XWAOP_TGT_THEN_RSB(type, btype) \
        DEFINE_XWAOP_TGE_THEN_RSB(type, btype) \
        DEFINE_XWAOP_TLT_THEN_RSB(type, btype) \
        DEFINE_XWAOP_TLE_THEN_RSB(type, btype) \
        DEFINE_XWAOP_TGTLT_THEN_RSB(type, btype) \
        DEFINE_XWAOP_TGELT_THEN_RSB(type, btype) \
        DEFINE_XWAOP_TGTLE_THEN_RSB(type, btype) \
        DEFINE_XWAOP_TGELE_THEN_RSB(type, btype) \
        DEFINE_XWAOP_OR(type, btype) \
        DEFINE_XWAOP_AND(type, btype) \
        DEFINE_XWAOP_XOR(type, btype) \
        DEFINE_XWAOP_C0M(type, btype) \
        DEFINE_XWAOP_T1MA_THEN_C0M(type, btype) \
        DEFINE_XWAOP_T1MO_THEN_C0M(type, btype) \
        DEFINE_XWAOP_T0MA_THEN_S1M(type, btype) \
        DEFINE_XWAOP_T0MO_THEN_S1M(type, btype) \
        DEFINE_XWAOP_TST_THEN_OP(type, btype)


#if (16 == BITS_PER_XWSZ_T)
DEFINE_XWAOP(xwsz, xwu16)
#elif (32 == BITS_PER_XWSZ_T)
DEFINE_XWAOP(xwsz, xwu32)
#elif (64 == BITS_PER_XWSZ_T)
DEFINE_XWAOP(xwsz, xwu64)
#endif

#if (16 == BITS_PER_XWSSZ_T)
DEFINE_XWAOP(xwssz, xws16)
#elif (32 == BITS_PER_XWSSZ_T)
DEFINE_XWAOP(xwssz, xws32)
#elif (64 == BITS_PER_XWSSZ_T)
DEFINE_XWAOP(xwssz, xws64)
#endif

#if (16 == BITS_PER_XWPTR_T)
DEFINE_XWAOP(xwptr, xwu16)
DEFINE_XWAOP(xwlfq, xwu16)
#elif (32 == BITS_PER_XWPTR_T)
DEFINE_XWAOP(xwlfq, xwu32)
DEFINE_XWAOP(xwptr, xwu32)
#elif (64 == BITS_PER_XWPTR_T)
DEFINE_XWAOP(xwlfq, xwu64)
DEFINE_XWAOP(xwptr, xwu64)
#endif

#if (16 == BITS_PER_XWER_T)
DEFINE_XWAOP(xwer, xws16)
#elif (32 == BITS_PER_XWER_T)
DEFINE_XWAOP(xwer, xws32)
#elif (64 == BITS_PER_XWER_T)
DEFINE_XWAOP(xwer, xws64)
#endif

#if (8 == BITS_PER_XWID_T)
DEFINE_XWAOP(xwid, xwu8)
#elif (16 == BITS_PER_XWID_T)
DEFINE_XWAOP(xwid, xwu16)
#elif (32 == BITS_PER_XWID_T)
DEFINE_XWAOP(xwid, xwu32)
#elif (64 == BITS_PER_XWID_T)
DEFINE_XWAOP(xwid, xwu64)
#endif

#if (8 == BITS_PER_XWSID_T)
DEFINE_XWAOP(xwsid, xws8)
#elif (16 == BITS_PER_XWSID_T)
DEFINE_XWAOP(xwsid, xws16)
#elif (32 == BITS_PER_XWSID_T)
DEFINE_XWAOP(xwsid, xws32)
#elif (64 == BITS_PER_XWSID_T)
DEFINE_XWAOP(xwsid, xws64)
#endif

#if (16 == BITS_PER_XWREG_T)
DEFINE_XWAOP(xwreg, xwu16)
#elif (32 == BITS_PER_XWREG_T)
DEFINE_XWAOP(xwreg, xwu32)
#elif (64 == BITS_PER_XWREG_T)
DEFINE_XWAOP(xwreg, xwu64)
#endif

#if (16 == BITS_PER_XWSREG_T)
DEFINE_XWAOP(xwsreg, xws16)
#elif (32 == BITS_PER_XWSREG_T)
DEFINE_XWAOP(xwsreg, xws32)
#elif (64 == BITS_PER_XWSREG_T)
DEFINE_XWAOP(xwsreg, xws64)
#endif

#if (16 == BITS_PER_XWSQ_T)
DEFINE_XWAOP(xwsq, xwu16)
#elif (32 == BITS_PER_XWSQ_T)
DEFINE_XWAOP(xwsq, xwu32)
#elif (64 == BITS_PER_XWSQ_T)
DEFINE_XWAOP(xwsq, xwu64)
#endif

#if (16 == BITS_PER_XWSSQ_T)
DEFINE_XWAOP(xwssq, xws16)
#elif (32 == BITS_PER_XWSSQ_T)
DEFINE_XWAOP(xwssq, xws32)
#elif (64 == BITS_PER_XWSSQ_T)
DEFINE_XWAOP(xwssq, xws64)
#endif

#if (8 == BITS_PER_XWBMP_T)
DEFINE_XWAOP(xwbmp, xwu16)
#elif (16 == BITS_PER_XWBMP_T)
DEFINE_XWAOP(xwbmp, xwu16)
#elif (32 == BITS_PER_XWBMP_T)
DEFINE_XWAOP(xwbmp, xwu32)
#elif (64 == BITS_PER_XWBMP_T)
DEFINE_XWAOP(xwbmp, xwu64)
#endif

DEFINE_XWAOP(xwtm, xws64)

/**
 * @} xwaop
 */

#endif /* xwos/lib/xwaop.h */
