/**
 * @file
 * @brief 汇编库：位操作
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

#ifndef __asmlib_xwbop_h__
#define __asmlib_xwbop_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_code
xwssq_t arch_xwbmpop_ffs(xwbmp_t *bmp, xwsz_t len);

__xwos_code
xwssq_t arch_xwbmpop_ffz(xwbmp_t *bmp, xwsz_t len);

__xwos_code
xwssq_t arch_xwbmpop_fls(xwbmp_t *bmp, xwsz_t len);

__xwos_code
xwssq_t arch_xwbmpop_flz(xwbmp_t *bmp, xwsz_t len);

__xwos_code
void arch_xwbmpop_s1i(xwbmp_t *addr, xwsq_t n);

__xwos_code
void arch_xwbmpop_c0i(xwbmp_t *addr, xwsq_t n);

__xwos_code
bool arch_xwbmpop_t1i(xwbmp_t *addr, xwsq_t n);

__xwos_code
void arch_xwbmpop_x1i(xwbmp_t *addr, xwsq_t n);

__xwos_code
xwu8_t arch_xwbop_rbit8(xwu8_t x);

__xwos_code
xwu16_t arch_xwbop_rbit16(xwu16_t x);

__xwos_code
xwu32_t arch_xwbop_rbit32(xwu32_t x);

__xwos_code
xwu64_t arch_xwbop_rbit64(xwu64_t x);

__xwos_code
xwu16_t arch_xwbop_re16(xwu16_t x);

__xwos_code
xws32_t arch_xwbop_re16s32(xwu16_t x);

__xwos_code
xwu32_t arch_xwbop_re32(xwu32_t x);

__xwos_code
xws64_t arch_xwbop_re32s64(xwu32_t x);

__xwos_code
xwu64_t arch_xwbop_re64(xwu64_t x);

__xwos_code
xwssq_t arch_xwbop_ffs8(xwu8_t x);

__xwos_code
xwssq_t arch_xwbop_fls8(xwu8_t x);

__xwos_code
xwssq_t arch_xwbop_ffs16(xwu16_t x);

__xwos_code
xwssq_t arch_xwbop_fls16(xwu16_t x);

__xwos_code
xwssq_t arch_xwbop_ffs32(xwu32_t x);

__xwos_code
xwssq_t arch_xwbop_fls32(xwu32_t x);

__xwos_code
xwssq_t arch_xwbop_ffs64(xwu64_t x);

__xwos_code
xwssq_t arch_xwbop_fls64(xwu64_t x);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  macro functions & inline functions ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* asmlib/xwbop.h */
