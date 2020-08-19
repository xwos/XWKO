/**
 * @file
 * @brief XuanWuOS的内存管理机制：内核内存分配器
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <linux/slab.h>
#include <xwos/mm/common.h>
#include <xwos/mm/sma.h>
#include <xwos/mm/kma.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macro       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwos_code
xwer_t xwmm_dkma_alloc(xwsz_t size, xwsz_t aligned, void ** membuf);

static __xwos_code
xwer_t xwmm_dkma_free(void * mem);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 使用简单内存分配器作为默认的内核内存分配器
 */
__xwos_data struct xwmm_sma xwmm_dkma;

/**
 * @brief 默认的内核内存分配器的名字
 */
__xwos_rodata const char xwmm_dkma_name[] = "xwmm_dkma";

/**
 * @brief 申请内核内存虚函数
 */
__xwos_data xwmm_kma_alloc_f xwmm_kma_alloc_vf = xwmm_dkma_alloc;

/**
 * @brief 释放内核内存虚函数
 */
__xwos_data xwmm_kma_free_f xwmm_kma_free_vf = xwmm_dkma_free;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 初始化默认的内核内存分配器
 * @param total: (I) 总的大小
 */
__xwos_init_code
xwer_t xwmm_dkma_init(xwsz_t total)
{
        void *origin;
        xwer_t rc;

        origin = kmalloc(total, GFP_KERNEL);
        rc = xwmm_sma_init(&xwmm_dkma,
                           (xwptr_t)origin,
                           (xwsz_t)total,
                           (xwsq_t)0,
                           xwmm_dkma_name);
        return rc;
}

/**
 * @brief 退出默认的内核内存分配器
 */
__xwos_init_code
xwer_t xwmm_dkma_exit(void)
{
        kfree((void *)xwmm_dkma.zone.origin);
        return XWOK;
}

/**
 * @brief 从默认的内核内存分配器中申请内存
 * @param size: (I) 大小
 * @param aligned: (I) 申请到的内存的首地址需要对齐到的边界
 * @param membuf: (O) 指向指针缓存的指针，此指针缓存用于返回申请到的内存的首地址
 * @return 错误码
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 参数无效
 * @retval -ENOMEM: 内存不足
 */
static __xwos_code
xwer_t xwmm_dkma_alloc(xwsz_t size, xwsz_t aligned, void ** membuf)
{
        bool ret;
        void *m;
        xwer_t rc;

        ret = try_module_get(THIS_MODULE);
        if (__xwcc_unlikely(!ret)) {
                rc = -EOWNERDEAD;
                goto err_modget;
        }
        rc = xwmm_sma_alloc(&xwmm_dkma, size, aligned, &m);
        if (rc < 0) {
                goto err_nomem;
        }
        *membuf = m;
        return XWOK;

err_nomem:
        module_put(THIS_MODULE);
err_modget:
        return rc;
}

/**
 * @brief 释放内存到默认的内核内存分配器
 * @param mem: (I) 内存首地址指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 */
static __xwos_code
xwer_t xwmm_dkma_free(void * mem)
{
        xwer_t rc;

        rc = xwmm_sma_free(&xwmm_dkma, mem);
        if (XWOK == rc) {
                module_put(THIS_MODULE);
        }
        return rc;
}

/**
 * @brief XWMM API：申请内核内存
 * @param size: (I) 大小
 * @param aligned: (I) 申请到的内存的首地址需要对齐到的边界
 * @param membuf: (O) 指向指针缓存的指针，此指针缓存用于返回申请到的内存的首地址
 * @return 错误码
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 参数无效
 * @retval -ENOMEM: 内存不足
 */
__xwos_api
xwer_t xwmm_kma_alloc(xwsz_t size, xwsz_t aligned, void ** membuf)
{
        return xwmm_kma_alloc_vf(size, aligned, membuf);
}

/**
 * @brief XWMM API：释放内核内存
 * @param mem: (I) 内存首地址指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 */
__xwos_api
xwer_t xwmm_kma_free(void * mem)
{
        return xwmm_kma_free_vf(mem);
}
