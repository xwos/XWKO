/**
 * @file
 * @brief 玄武OS内存管理：内核内存分配器
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

#include <xwos/standard.h>
#include <linux/slab.h>
#include <xwos/mm/common.h>
#include <xwos/mm/sma.h>
#include <xwos/mm/kma.h>

/**
 * @brief 使用简单内存分配器作为默认的内核内存分配器
 */
__xwos_data struct xwmm_sma xwmm_kma;

/**
 * @brief 默认的内核内存分配器的名字
 */
__xwos_rodata const char xwmm_kma_name[] = "xwmm_kma";

/**
 * @brief 初始化默认的内核内存分配器
 * @param[in] total: 总的大小
 */
__xwos_init_code
xwer_t xwmm_kma_init(xwsz_t total)
{
        void * origin;
        xwer_t rc;

        origin = kmalloc(total, GFP_KERNEL);
        rc = xwmm_sma_init(&xwmm_kma,
                           (xwptr_t)origin,
                           (xwsz_t)total,
                           (xwsq_t)0,
                           xwmm_kma_name);
        return rc;
}

/**
 * @brief 退出默认的内核内存分配器
 */
__xwos_init_code
xwer_t xwmm_kma_exit(void)
{
        kfree((void *)xwmm_kma.zone.origin);
        return XWOK;
}

/**
 * @brief XWMM API：申请内核内存
 * @param[in] size: 大小
 * @param[in] aligned: 申请到的内存的首地址需要对齐到的边界
 * @param[out] membuf: 指向指针缓存的指针，此指针缓存用于返回申请到的内存的首地址
 * @return 错误码
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 参数无效
 * @retval -ENOMEM: 内存不足
 */
__xwos_code
xwer_t xwmm_kma_alloc(xwsz_t size, xwsz_t aligned, void ** membuf)
{
        void * m;
        xwer_t rc;

        rc = xwmm_sma_alloc(&xwmm_kma, size, aligned, &m);
        if (rc < 0) {
                goto err_nomem;
        }
        *membuf = m;
        return XWOK;

err_nomem:
        return rc;
}

/**
 * @brief XWMM API：释放内核内存
 * @param[in] mem: 内存首地址指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 */
__xwos_code
xwer_t xwmm_kma_free(void * mem)
{
        return xwmm_sma_free(&xwmm_kma, mem);
}
