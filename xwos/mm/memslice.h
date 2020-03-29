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
 * @note
 * - 此算法是在所有上下文（线程、中断、中断底半部）都是安全的。
 */

#ifndef __xwos_mm_memslice_h__
#define __xwos_mm_memslice_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/object.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/lfq.h>
#include <xwos/mm/common.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 内存切片分配器
 */
struct xwmm_memslice {
        struct xwmm_zone zone; /**< 内存区域 */
        const char * name; /**< 名字 */
        xwsz_t card_size; /**< “卡片”大小 */
        __atomic xwsz_t num_free; /**< 当前分配器中还剩余多少张“卡片” */
        __atomic xwsz_t num_max; /**< 当前分配器中总共有多少张“卡片” */
        __atomic xwlfq_t free_list; /**< 空闲的内存切片链表 */
        xwptr_t backup; /**< 备份值：
                             - 每个对象的第一段内存字(word)用作无锁队列的
                               链表指针，当分配卡片时，用备份值恢复；
                             - 所有对象初始化后的第一个字(word)必须相同，
                               以保证备份值唯一。
                             */
        ctor_f ctor; /**< “卡片”的构造函数 */
        dtor_f dtor; /**< “卡片”的析构函数 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     internal function prototypes    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ********       internal inline function implementations        ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       API function prototypes       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_api
xwer_t xwmm_memslice_init(struct xwmm_memslice * msa,
                          xwptr_t origin, xwsz_t total_size,
                          xwsz_t card_size, const char * name,
                          ctor_f ctor, dtor_f dtor);

__xwos_api
xwer_t xwmm_memslice_destroy(struct xwmm_memslice * msa);

__xwos_api
xwer_t xwmm_memslice_create(struct xwmm_memslice ** ptrbuf,
                            xwptr_t origin, xwsz_t total_size, xwsz_t card_size,
                            const char * name, ctor_f ctor, dtor_f dtor);

__xwos_api
xwer_t xwmm_memslice_delete(struct xwmm_memslice * msa);

__xwos_api
xwer_t xwmm_memslice_alloc(struct xwmm_memslice * msa, void ** membuf);

__xwos_api
xwer_t xwmm_memslice_free(struct xwmm_memslice * msa, void * mem);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      inline API implementations     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwos/mm/memslice.h */
