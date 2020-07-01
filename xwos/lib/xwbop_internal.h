/**
 * @file
 * @brief XWOS通用库：位操作内部头文件（不可直接引用）
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

#ifndef __xwos_lib_xwbop_internal_h__
#define __xwos_lib_xwbop_internal_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <asmlib/xwbop.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         macros & functions          ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#ifndef BIT
  #define BIT(n)                (1UL << (n))
#endif
#ifndef BIT_BMP_MASK
  #define BIT_BMP_MASK(n)       ((xwbmp_t)1 << (xwbmp_t)((n) % BITS_PER_XWBMP_T))
#endif
#ifndef BIT_BMP
  #define BIT_BMP(n)            ((n) / BITS_PER_XWBMP_T)
#endif
#ifndef BITS_PER_BYTE
  #define BITS_PER_BYTE         8
#endif
#ifndef DIV_ROUND
  #define DIV_ROUND(n, d)       ((n) / (d))
#endif
#ifndef DIV_ROUND_UP
  #define DIV_ROUND_UP(n, d)    (((n) + (d) - 1U) / (d))
#endif
#ifndef SHIFT_ROUND
  #define SHIFT_ROUND(n, s)     ((n) >> (s))
#endif
#ifndef SHIFT_ROUND_UP
  #define SHIFT_ROUND_UP(n, s)  (((n) + (1U << (s)) - 1U) >> (s))
#endif
#ifndef BITS_TO_BYTES
  #define BITS_TO_BYTES(n)      DIV_ROUND_UP(n, BITS_PER_BYTE)
#endif
#ifndef BITS_TO_BMPS
  #define BITS_TO_BMPS(n)       DIV_ROUND_UP(n, BITS_PER_BYTE * sizeof(xwbmp_t))
#endif
#ifndef ROUND
  #define ROUND(x, n)           ((x) & (~((n) - 1U)))
#endif
#ifndef ALIGN
  #define ALIGN(x, n)           (((x) + ((n) - 1U)) & (~((n) - 1U)))
#endif
#ifndef GETBIT
  #define GETBIT(x, bit)        (((x) >> (bit)) & 1U)
#endif

/******** ******** 8-bit data bit-operations ******** ********/
#if (defined(ARCHCFG_LIB_XWBOP_S1M8) && (1 == ARCHCFG_LIB_XWBOP_S1M8))
#define xwbop_s1m8(a, mask)     arch_xwbop_s1m8(a, mask)
#else
#define xwbop_s1m8(a, mask)     *(a) |= (mask)
#endif

#if (defined(ARCHCFG_LIB_XWBOP_C0M8) && (1 == ARCHCFG_LIB_XWBOP_C0M8))
#define xwbop_c0m8(a, mask)     arch_xwbop_c0m8(a, mask)
#else
#define xwbop_c0m8(a, mask)     *(a) &= (~(mask))
#endif

#if (defined(ARCHCFG_LIB_XWBOP_X1M8) && (1 == ARCHCFG_LIB_XWBOP_X1M8))
#define xwbop_x1m8(a, mask)     arch_xwbop_x1m8(a, mask)
#else
#define xwbop_x1m8(a, mask)     *(a) ^= (mask)
#endif

#if (defined(ARCHCFG_LIB_XWBOP_FFS8) && (1 == ARCHCFG_LIB_XWBOP_FFS8))
#define xwbop_ffs8(x)           arch_xwbop_ffs8(x)
#else
__xwlib_code
xwssq_t xwbop_ffs8(xwu8_t x);
#endif

#if (defined(ARCHCFG_LIB_XWBOP_FLS8) && (1 == ARCHCFG_LIB_XWBOP_FLS8))
#define xwbop_fls8(x)           arch_xwbop_fls8(x)
#else
__xwlib_code
xwssq_t xwbop_fls8(xwu8_t x);
#endif

static __xwlib_inline
xwssq_t xwbop_ffz8(xwu8_t x)
{
        return xwbop_ffs8((xwu8_t)(~x));
}

static __xwlib_inline
xwssq_t xwbop_flz8(xwu8_t x)
{
        return xwbop_fls8((xwu8_t)(~x));
}

#if (defined(ARCHCFG_LIB_XWBOP_RBIT8) && (1 == ARCHCFG_LIB_XWBOP_RBIT8))
  #define xwbop_rbit8(x)        arch_xwbop_rbit8(x)
#else
__xwlib_code
xwu8_t xwbop_rbit8(xwu8_t x);
#endif

/**
 * @brief 翻转8位数据的大小端
 * @param x: (I) 8位数据
 * @return 结果
 * @note
 * - 这个函数不做任何操作，是为了位操作模板的统一接口而实现的dummy函数。
 *   因为大小端是以字节为单位的。
 */
static __xwlib_inline
xwu8_t xwbop_re8(xwu8_t x)
{
        return x;
}

/******** ******** 16-bit data bit operations ******** ********/
#if (defined(ARCHCFG_LIB_XWBOP_S1M16) && (1 == ARCHCFG_LIB_XWBOP_S1M16))
#define xwbop_s1m16(a, mask)    arch_s1m16(a, mask)
#else
#define xwbop_s1m16(a, mask)    *(a) |= (mask)
#endif

#if (defined(ARCHCFG_LIB_XWBOP_C0M16) && (1 == ARCHCFG_LIB_XWBOP_C0M16))
#define xwbop_c0m16(a, mask)    arch_xwbop_c0m16(a, mask)
#else
#define xwbop_c0m16(a, mask)    *(a) &= (~(mask))
#endif

#if (defined(ARCHCFG_LIB_XWBOP_X1M16) && (1 == ARCHCFG_LIB_XWBOP_X1M16))
#define xwbop_x1m16(a, mask)    arch_xwbop_x1m16(a, mask)
#else
#define xwbop_x1m16(a, mask)    *(a) ^= (mask)
#endif

#if (defined(ARCHCFG_LIB_XWBOP_FFS16) && (1 == ARCHCFG_LIB_XWBOP_FFS16))
#define xwbop_ffs16(x)          arch_xwbop_ffs16(x)
#else
__xwlib_code
xwssq_t xwbop_ffs16(xwu16_t x);
#endif

#if (defined(ARCHCFG_LIB_XWBOP_FLS16) && (1 == ARCHCFG_LIB_XWBOP_FLS16))
#define xwbop_fls16(x)          arch_xwbop_fls16(x)
#else
__xwlib_code
xwssq_t xwbop_fls16(xwu16_t x);
#endif

static __xwlib_inline
xwssq_t xwbop_ffz16(xwu16_t x)
{
        return xwbop_ffs16((xwu16_t)(~x));
}

static __xwlib_inline
xwssq_t xwbop_flz16(xwu16_t x)
{
        return xwbop_fls16((xwu16_t)(~x));
}

#if (defined(ARCHCFG_LIB_XWBOP_RBIT16) && (1 == ARCHCFG_LIB_XWBOP_RBIT16))
#define xwbop_rbit16(x)         arch_xwbop_rbit16(x)
#else
__xwlib_code
xwu16_t xwbop_rbit16(xwu16_t x);
#endif

#if (defined(ARCHCFG_LIB_XWBOP_RE16) && (1 == ARCHCFG_LIB_XWBOP_RE16))
#define xwbop_re16(x)           arch_xwbop_re16(x)
#else
__xwlib_code
xwu16_t xwbop_re16(xwu16_t a);
#endif

/**
 * @brief 将16位数据的大小端翻转，并将符号位扩展到32位
 * @param x: (I) 数据（注：非指针）
 * @return 结果
 */
#if (defined(ARCHCFG_LIB_XWBOP_RE16S32) && (1 == ARCHCFG_LIB_XWBOP_RE16S32))
#define xwbop_re16s32(x)        arch_xwbop_re16s32(x)
#else
__xwlib_code
xws32_t xwbop_re16s32(xwu16_t x);
#endif

/******** ******** 32-bit data bit operations ******** ********/
#if (defined(ARCHCFG_LIB_XWBOP_S1M32) && (1 == ARCHCFG_LIB_XWBOP_S1M32))
#define xwbop_s1m32(a, mask)    arch_xwbop_s1m32(a, mask)
#else
#define xwbop_s1m32(a, mask)    *(a) |= (mask)
#endif

#if (defined(ARCHCFG_LIB_XWBOP_C0M32) && (1 == ARCHCFG_LIB_XWBOP_C0M32))
#define xwbop_c0m32(a, mask)    arch_xwbop_c0m32(a, mask)
#else
#define xwbop_c0m32(a, mask)    *(a) &= (~(mask))
#endif

#if (defined(ARCHCFG_LIB_XWBOP_X1M32) && (1 == ARCHCFG_LIB_XWBOP_X1M32))
#define xwbop_x1m32(a, mask)    arch_xwbop_x1m32(a, mask)
#else
#define xwbop_x1m32(a, mask)    *(a) ^= (mask)
#endif

#if (defined(ARCHCFG_LIB_XWBOP_FFS32) && (1 == ARCHCFG_LIB_XWBOP_FFS32))
#define xwbop_ffs32(x)                  arch_xwbop_ffs32(x)
#else
__xwlib_code
xwssq_t xwbop_ffs32(xwu32_t x);
#endif

#if (defined(ARCHCFG_LIB_XWBOP_FLS32) && (1 == ARCHCFG_LIB_XWBOP_FLS32))
#define xwbop_fls32(x)                  arch_xwbop_fls32(x)
#else
__xwlib_code
xwssq_t xwbop_fls32(xwu32_t x);
#endif

static __xwlib_inline
xwssq_t xwbop_ffz32(xwu32_t x)
{
        return xwbop_ffs32(~x);
}

static __xwlib_inline
xwssq_t xwbop_flz32(xwu32_t x)
{
        return xwbop_fls32(~x);
}

#if (defined(ARCHCFG_LIB_XWBOP_RBIT32) && (1 == ARCHCFG_LIB_XWBOP_RBIT32))
#define xwbop_rbit32(x)         arch_xwbop_rbit32(x)
#else
__xwlib_code
xwu32_t xwbop_rbit32(xwu32_t x);
#endif

#if (defined(ARCHCFG_LIB_XWBOP_RE32) && (1 == ARCHCFG_LIB_XWBOP_RE32))
#define xwbop_re32(x)           arch_xwbop_re32(x)
#else
__xwlib_code
xwu32_t xwbop_re32(xwu32_t x);
#endif

/**
 * @brief 将32位数据的大小端翻转，并将符号位扩展到64位
 * @param x: (I) 数据（注：非指针）
 * @return 结果
 */
#if (defined(ARCHCFG_LIB_XWBOP_RE32S64) && (1 == ARCHCFG_LIB_XWBOP_RE32S64))
#define xwbop_re32s64(x)        arch_xwbop_re32s64(x)
#else
__xwlib_code
xws64_t xwbop_re32s64(xwu32_t x);
#endif

/******** ******** 64-bit data bit operations ******** ********/
#if (defined(ARCHCFG_LIB_XWBOP_S1M64) && (1 == ARCHCFG_LIB_XWBOP_S1M64))
#define xwbop_s1m64(a64, mask64)        arch_xwbop_s1m64(a64, mask64)
#else
#define xwbop_s1m64(a64, mask64)        *((xwu64_t *)a64) |= (mask64)
#endif

#if (defined(ARCHCFG_LIB_XWBOP_C0M64) && (1 == ARCHCFG_LIB_XWBOP_C0M64))
#define xwbop_c0m64(a64, mask64)        arch_xwbop_c0m64(a64, mask64)
#else
#define xwbop_c0m64(a64, mask64)        *((xwu64_t *)a64) &= (~(mask64))
#endif

#if (defined(ARCHCFG_LIB_XWBOP_X1M64) && (1 == ARCHCFG_LIB_XWBOP_X1M64))
#define xwbop_x1m64(a64, mask64)        arch_xwbop_x1m64(a64, mask64)
#else
#define xwbop_x1m64(a64, mask64)        *((xwu64_t *)a64) ^= (mask64)
#endif

#if (defined(ARCHCFG_LIB_XWBOP_FFS64) && (1 == ARCHCFG_LIB_XWBOP_FFS64))
#define xwbop_ffs64(x)                  arch_xwbop_ffs64(x)
#else
__xwlib_code
xwssq_t xwbop_ffs64(xwu64_t x);
#endif

#if (defined(ARCHCFG_LIB_XWBOP_FLS64) && (1 == ARCHCFG_LIB_XWBOP_FLS64))
#define xwbop_fls64(x)                  arch_xwbop_fls64(x)
#else
__xwlib_code
xwssq_t xwbop_fls64(xwu64_t x);
#endif

static __xwlib_inline
xwssq_t xwbop_ffz64(xwu64_t x)
{
        return xwbop_ffs64(~x);
}

static __xwlib_inline
xwssq_t xwbop_flz64(xwu64_t x)
{
        return xwbop_ffs64(~x);
}

#if (defined(ARCHCFG_LIB_XWBOP_RE64) && (1 == ARCHCFG_LIB_XWBOP_RE64))
#define xwbop_re64(x)           arch_xwbop_re64(x)
#else
__xwlib_code
xwu64_t xwbop_re64(xwu64_t x);
#endif

#if (defined(ARCHCFG_LIB_XWBOP_RBIT64) && (1 == ARCHCFG_LIB_XWBOP_RBIT64))
#define xwbop_rbit64(x)         arch_xwbop_rbit64(x)
#else
__xwlib_code
xwu64_t xwbop_rbit64(xwu64_t x);
#endif

#endif /* xwos/lib/xwbop_internal.h */
