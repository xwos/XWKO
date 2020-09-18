/**
 * @file
 * @brief 操作系统抽象层：互斥锁
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

#ifndef __xwos_osal_lock_mutex_h__
#define __xwos_osal_lock_mutex_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/osdl/os.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief [XWOSAL] 互斥锁
 */
struct xwosal_mtx {
        struct xwosdl_mtx osmtx; /**< 操作系统的互斥锁 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       APIs        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOSAL API：静态方式初始化互斥锁
 * @param mtx: (I) 互斥锁的指针
 * @param sprio: (I) 互斥锁的静态优先级
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的ID或空指针
 * @retval -EINVAL: 无效参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwosal_mtx_init(struct xwosal_mtx * mtx, xwpr_t sprio)
{
        return xwosdl_mtx_init(&mtx->osmtx, sprio);
}

/**
 * @brief XWOSAL API：销毁静态方式初始化的互斥锁
 * @param mtx: (I) 互斥锁的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的ID或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwosal_mtx_destroy(struct xwosal_mtx * mtx)
{
        return xwosdl_mtx_destroy(&mtx->osmtx);
}

/**
 * @brief XWOSAL API：动态方式创建互斥锁
 * @param midbuf: (O) 指向缓冲区的指针，通过此缓冲区返回互斥锁的ID
 * @param sprio: (I) 互斥锁的静态优先级
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的ID或空指针
 * @retval -EINVAL: 无效参数
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwosal_mtx_create(xwid_t * midbuf, xwpr_t sprio)
{
        return xwosdl_mtx_create(midbuf, sprio);
}

/**
 * @brief XWOSAL API：删除动态方式创建的互斥锁
 * @param mid: (I) 互斥锁ID
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的ID或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwosal_mtx_delete(xwid_t mid)
{
        return xwosdl_mtx_delete(mid);
}

/**
 * @brief XWOSAL API：从互斥锁对象指针获取互斥锁ID
 * @param mtx: (I) 互斥锁对象的指针
 * @return 互斥锁ID
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwid_t xwosal_mtx_get_id(struct xwosal_mtx * mtx)
{
        return xwosdl_mtx_get_id(&mtx->osmtx);
}

/**
 * @brief XWOSAL API：从互斥锁ID获取对象的指针
 * @param mid: (I) 互斥锁ID
 * @return 互斥锁对象的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
struct xwosal_mtx * xwosal_mtx_get_obj(xwid_t mid)
{
        return (struct xwosal_mtx *)xwosdl_mtx_get_obj(mid);
}

/**
 * @brief XWOSAL API：解锁互斥锁
 * @param mid: (I) 互斥锁ID
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的ID或空指针
 * @retval -EOWNER: 线程并没有锁定此互斥锁
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwosal_mtx_unlock(xwid_t mid)
{
        return xwosdl_mtx_unlock(mid);
}

/**
 * @brief XWOSAL API：等待并上锁互斥锁
 * @param mid: (I) 互斥锁ID
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的ID或空指针
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwosal_mtx_lock(xwid_t mid)
{
        return xwosdl_mtx_lock(mid);
}

/**
 * @brief XWOSAL API：尝试上锁互斥锁，不会阻塞调用者
 * @param mid: (I) 互斥锁ID
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的ID或空指针
 * @retval -ENODATA: 获取锁失败
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 此函数虽然不会阻塞调用者，但也不可在中断上下文中使用，因为在OS中
 *   互斥锁通常需要记录自己的拥有者（一般是线程对象结构体），
 *   中断上下文不存在对应的线程对象结构体。
 */
static __xwos_inline_api
xwer_t xwosal_mtx_trylock(xwid_t mid)
{
        return xwosdl_mtx_trylock(mid);
}

/**
 * @brief XWOSAL API：限时等待并上锁互斥锁
 * @param mid: (I) 互斥锁ID
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的ID或空指针
 * @retval -EINTR: 等待被中断
 * @retval -ETIMEDOUT: 超时
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 函数返回 *-ETIMEDOUT* 时， *xwtm* 指向的缓冲区内的值减为0。
 */
static __xwos_inline_api
xwer_t xwosal_mtx_timedlock(xwid_t mid, xwtm_t * xwtm)
{
        return xwosdl_mtx_timedlock(mid, xwtm);
}

/**
 * @brief XWOSAL API：等待并上锁互斥锁，且等待不可被中断
 * @param mid: (I) 互斥锁ID
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的ID或空指针
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwosal_mtx_lock_unintr(xwid_t mid)
{
        return xwosdl_mtx_lock_unintr(mid);
}

#endif /* xwos/osal/lock/mutex.h */
