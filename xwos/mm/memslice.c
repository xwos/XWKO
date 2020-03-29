/**
 * @file
 * @brief XuanWuOS的内存管理机制：内存切片分配器
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwaop.h>
#include <xwos/lib/lfq.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#include <xwos/mm/memslice.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWMM API：静态方式初始化内存切片分配器。
 * @param msa: (I) 内存切片分配器对象的指针
 * @param origin: (I) 建立内存切片分配算法的内存区域首地址
 * @param total_size: (I) 建立内存切片分配算法的内存区域大小
 * @param card_size: (I) 切片大小
 * @param name: (I) 名字
 * @param ctor: (I) 切片的构造函数
 * @param dtor: (I) 切片的析构函数
 * @return 错误码
 * @retval OK: OK
 * @retval -E2SMALL: 内存区域太小
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwmm_memslice_init(struct xwmm_memslice * msa, xwptr_t origin,
                          xwsz_t total_size, xwsz_t card_size,
                          const char * name, ctor_f ctor, dtor_f dtor)
{
        xwsz_t num_max, n, nm;
        xwptr_t curr, next;
        xwer_t rc;

        XWOS_VALIDATE((msa), "nullptr", -EFAULT);

        card_size = ALIGN(card_size, XWMM_ALIGNMENT);
        num_max = total_size / card_size;
        if (0 == num_max) {
                rc = -E2SMALL;
                goto err_mem2small;
        }

        msa->zone.origin = origin;
        msa->zone.size = total_size;
        msa->name = name;
        msa->card_size = card_size;
        msa->num_max = num_max;
        msa->num_free = num_max;
        msa->ctor = ctor;
        msa->dtor = dtor;

        /* 构造所有“卡牌” */
        if (ctor) {
                curr = origin;
                next = curr;
                for (n = 0; n < num_max; n++) {
                        next += card_size;
                        ctor((void *)curr);
                        curr = next;
                }
                msa->backup = *((xwptr_t *)origin);
        } else {
                msa->backup = 0;
        }

        xwlib_lfq_init(&msa->free_list);
        curr = origin;
        next = origin;
        nm = num_max;
        for (n = 0; n < nm; n++) {
                next += card_size;
                xwlib_lfq_init((__atomic xwlfq_t *)curr);
                xwlib_lfq_push(&msa->free_list, (__atomic xwlfq_t *)curr);
                curr = next;
        }

        return OK;

err_mem2small:
        return rc;
}

/**
 * @brief XWMM API：销毁静态方式初始化的内存切片分配器。
 * @param msa: (I) 内存切片分配器对象的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwmm_memslice_destroy(struct xwmm_memslice * msa)
{
        XWOS_VALIDATE((msa), "nullptr", -EFAULT);

        XWOS_UNUSED(msa);
        return OK;
}

/**
 * @brief XWMM API：动态方式创建内存切片分配器。
 * @param ptrbuf: (O) 用于返回内存切片分配器对象指针的缓存
 * @param origin: (I) 建立内存切片分配算法的内存区域首地址
 * @param total_size: (I) 建立内存切片分配算法的内存区域大小
 * @param card_size: (I) 切片大小
 * @param ctor: (I) 切片的构造函数
 * @param dtor: (I) 切片的析构函数
 * @param name: (I) 名字
 * @return 错误码
 * @retval OK: OK
 * @retval -E2SMALL: 内存区域太小
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwmm_memslice_create(struct xwmm_memslice ** ptrbuf,
                            xwptr_t origin, xwsz_t total_size, xwsz_t card_size,
                            const char * name, ctor_f ctor, dtor_f dtor)
{
        void * mem;
        struct xwmm_memslice * msa;
        xwsz_t num_max;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);

        card_size = ALIGN(card_size, XWMM_ALIGNMENT);
        num_max = total_size / card_size;
        if (__unlikely(0 == num_max)) {
                rc = -E2SMALL;
                goto err_mem2small;
        }

        rc = xwmm_kma_alloc(sizeof(struct xwmm_memslice), XWMM_ALIGNMENT, &mem);
        if (__unlikely(rc < 0)) {
                goto err_msa_alloc;
        }
        msa = mem;

        rc = xwmm_memslice_init(msa, origin, total_size, card_size, name, ctor, dtor);
        if (__unlikely(rc < 0)) {
                goto err_msa_init;
        }

        *ptrbuf = msa;
        return OK;

err_msa_init:
        xwmm_kma_free(msa);
err_msa_alloc:
err_mem2small:
        return rc;
}

/**
 * @brief XWMM API：删除动态方式创建的内存切片分配器。
 * @param msa: (I) 内存切片分配器对象的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwmm_memslice_delete(struct xwmm_memslice * msa)
{
        XWOS_VALIDATE((msa), "nullptr", -EFAULT);
        xwmm_kma_free(msa);
        return OK;
}

/**
 * @brief XWMM API：申请一片内存切片。
 * @param msa: (I) 内存切片分配器对象的指针
 * @param membuf: (O) 指向指针缓存的指针，该指针缓存用于返回申请到的内存的首地址
 * @return 错误码
 * @retval -EFAULT: 空指针
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwmm_memslice_alloc(struct xwmm_memslice * msa, void ** membuf)
{
        xwlfq_t * card;
        xwer_t rc;

        XWOS_VALIDATE((msa), "nullptr", -EFAULT);
        XWOS_VALIDATE((membuf), "nullptr", -EFAULT);

        card = xwlib_lfq_pop(&msa->free_list);
        if (NULL == card) {
                rc = -ENOMEM;
                *membuf = NULL;
                goto err_lfq_pop;
        }
        xwaop_sub(xwsz_t, &msa->num_free, 1, NULL, NULL);
        *(xwptr_t *)card = msa->backup; /* restore original data */
        *membuf = card;
        return OK;

err_lfq_pop:
        return rc;
}

/**
 * @brief XWMM API：释放一片内存切片。
 * @param msa: (I) 内存切片分配器对象的指针
 * @param mem: (I) 内存切片的首地址
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwmm_memslice_free(struct xwmm_memslice * msa, void * mem)
{
        xwer_t rc;
        __atomic xwlfq_t * card;

        XWOS_VALIDATE((msa), "nullptr", -EFAULT);
        XWOS_VALIDATE((mem), "nullptr", -EFAULT);

        if (__unlikely((xwptr_t)mem < msa->zone.origin ||
                       ((xwptr_t)mem - msa->zone.origin) > msa->zone.size)) {
                rc = -EOWNER;
        } else {
                if (msa->dtor) {
                        msa->dtor(mem);
                }/* else {} */
                card = (__atomic xwlfq_t *)mem;
                xwlib_lfq_push(&msa->free_list, card);
                xwaop_add(xwsz_t, &msa->num_free, 1, NULL, NULL);
                rc = OK;
        }

        return rc;
}
