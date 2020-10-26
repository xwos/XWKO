/**
 * @file
 * @brief 玄武OS内存管理：内核内存分配器
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

#ifndef __xwos_mm_kma_h__
#define __xwos_mm_kma_h__

#include <xwos/standard.h>

/**
 * @defgroup xwmm_kma 内核内存分配器
 * @{
 */

xwer_t xwmm_kma_init(xwsz_t total);
xwer_t xwmm_kma_exit(void);
xwer_t xwmm_kma_alloc(xwsz_t size, xwsz_t aligned, void ** membuf);
xwer_t xwmm_kma_free(void * mem);

/**
 * @} xwmm_kma
 */

#endif /* xwos/mm/kma.h */
