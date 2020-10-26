/**
 * @file
 * @brief 操作系统抽象层：中断
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

#ifndef __xwos_osal_irq_h__
#define __xwos_osal_irq_h__

#include <xwos/standard.h>
#include <xwos/osal/jack/irq.h>

/**
 * @defgroup xwos_irq 中断
 * @{
 */

/**
 * @brief XWOS API：判断当前的上下文是否为中断上下文，并取得当前中断的中断号
 * @param[out] irqnbuf: 指向缓冲区的指针，通过此缓冲区返回当前中断号：
 * + 返回结果仅当返回值为OK时有效
 * + 可为NULL，表示不需要返回中断号
 * @return 错误码
 * @retval OK: 当前上下文为中断
 * @retval -EINTHD: 当前上下文为线程
 * @retval -EINBH: 当前上下文为中断底半部
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_irq_get_id(xwirq_t * irqnbuf)
{
        return xwosdl_irq_get_id(irqnbuf);
}

/**
 * @brief XWOS API：开启本地CPU的中断
 */
static __xwos_inline_api
void xwos_cpuirq_enable_lc(void)
{
        xwosdl_cpuirq_enable_lc();
}

/**
 * @brief XWOS API：关闭本地CPU的中断
 */
static __xwos_inline_api
void xwos_cpuirq_disable_lc(void)
{
        xwosdl_cpuirq_disable_lc();
}

/**
 * @brief XWOS API：恢复本地CPU的中断
 * @param[in] flag: 本地CPU的中断开关
 */
static __xwos_inline_api
void xwos_cpuirq_restore_lc(xwreg_t cpuirq)
{
        xwosdl_cpuirq_restore_lc(cpuirq);
}

/**
 * @brief XWOS API：保存然后关闭本地CPU的中断
 * @param[out] flag: 指向缓冲区的指针，此缓冲区用于返回本地CPU的中断开关
 */
static __xwos_inline_api
void xwos_cpuirq_save_lc(xwreg_t * cpuirq)
{
        xwosdl_cpuirq_save_lc(cpuirq);
}

/**
 * @} xwos_irq
 */

#endif /* xwos/osal/irq.h */
