/**
 * @file
 * @brief 汇编库：位操作
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
#include <xwos/lib/xwlog.h>
#include <xwos/lib/xwbop.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwssq_t arch_xwbop_ffs8(xwu8_t x)
{
        return arch_xwbop_ffs32((xwu32_t)(x & 0xFFU));
}
EXPORT_SYMBOL(arch_xwbop_ffs8);

xwssq_t arch_xwbop_fls8(xwu8_t x)
{
        return arch_xwbop_fls32((xwu32_t)(x & 0xFFU));
}
EXPORT_SYMBOL(arch_xwbop_fls8);

xwu8_t arch_xwbop_rbit8(xwu8_t x)
{
        __asm__ volatile(
        "       rbit            %[__x], %[__x]\n"
        "       lsr             %[__x], %[__x], #24\n"
        : [__x] "+r" (x)
        :
        :
        );
        return x;
}
EXPORT_SYMBOL(arch_xwbop_rbit8);

xwssq_t arch_xwbop_ffs16(xwu16_t x)
{
        return arch_xwbop_ffs32((xwu32_t)(x & 0xFFFFU));
}
EXPORT_SYMBOL(arch_xwbop_ffs16);

xwssq_t arch_xwbop_fls16(xwu16_t x)
{
        return arch_xwbop_fls32((xwu32_t)(x & 0xFFFFU));
}
EXPORT_SYMBOL(arch_xwbop_fls16);

xwu16_t arch_xwbop_rbit16(xwu16_t x)
{
        __asm__ volatile(
        "       rbit            %[__x], %[__x]\n"
        "       lsr             %[__x], %[__x], #16\n"
        : [__x] "+r" (x)
        :
        :
        );
        return x;
}
EXPORT_SYMBOL(arch_xwbop_rbit16);

xwu16_t arch_xwbop_re16(__maybe_unused xwu16_t x)
{
        xwu16_t r;

        __asm__ volatile(
        "       rev16           %[__r], %[__x]\n"
        : [__r] "=&r" (r)
        : [__x] "r" (x)
        : "memory"
        );
        return r;
}
EXPORT_SYMBOL(arch_xwbop_re16);

xws32_t arch_xwbop_re16s32(xwu16_t x)
{
        xws32_t r;

        __asm__ volatile(
        "       revsh           %[__r], %[__x]\n"
        : [__r] "=&r" (r)
        : [__x] "r" (x)
        : "memory"
        );
        return r;
}
EXPORT_SYMBOL(arch_xwbop_re16s32);

xwssq_t arch_xwbop_ffs32(xwu32_t x)
{
        xwssq_t pos;

        __asm__ volatile(
        "       rbit            %[__pos], %[__x]\n"
        "       clz             %[__pos], %[__pos]\n"
        "       teq             %[__pos], #32\n"
        "       mvneq           %[__pos], #0\n"
        : [__pos] "=&r" (pos)
        : [__x] "r" (x)
        : "cc"
        );
        return pos;
}
EXPORT_SYMBOL(arch_xwbop_ffs32);

xwssq_t arch_xwbop_fls32(xwu32_t x)
{
        xwssq_t pos;

        __asm__ volatile(
        "       clz             %[__pos], %[__x]\n"
        "       rsb             %[__pos], %[__pos], #31\n"
        : [__pos] "=&r" (pos)
        : [__x] "r" (x)
        :
        );
        return pos;
}
EXPORT_SYMBOL(arch_xwbop_fls32);

xwu32_t arch_xwbop_rbit32(xwu32_t x)
{
        __asm__ volatile(
        "       rbit            %[__x], %[__x]\n"
        : [__x] "+r" (x)
        :
        :
        );
        return x;
}
EXPORT_SYMBOL(arch_xwbop_rbit32);

xwu32_t arch_xwbop_re32(xwu32_t x)
{
        xwu32_t r;

        __asm__ volatile(
        "       rev             %[__r], %[__x]\n"
        : [__r] "=&r" (r)
        : [__x] "r" (x)
        : "memory"
        );
        return r;
}
EXPORT_SYMBOL(arch_xwbop_re32);

xws64_t arch_xwbop_re32s64(xwu32_t x)
{
        union {
                xws64_t q;
                xwu32_t w[2];
        } r;

        __asm__ volatile(
        "       rev             %[__r], %[__x]\n"
        : [__r] "=&r" (r.w[0])
        : [__x] "r" (x)
        : "memory"
        );
        if ((xws32_t)r.w[0] & 0x80000000UL)
                r.w[1] = (~0UL);
        return r.q;
}
EXPORT_SYMBOL(arch_xwbop_re32s64);

xwssq_t arch_xwbop_ffs64(xwu64_t x)
{
        union {
                xwu64_t v64;
                struct {
#if defined(ARCHCFG_LITTLE_ENDIAN) && (1 == ARCHCFG_LITTLE_ENDIAN)
                        xwu32_t low;
                        xwu32_t high;
#else
                        xwu32_t high;
                        xwu32_t low;
#endif
                } v32;
        } in, out;
        xwssq_t pos;

        in.v64 = x;
        __asm__ volatile(
        "rbit           %[__olow], %[__ilow]\n"
        "rbit           %[__ohigh], %[__ihigh]\n"
        "cmp            %[__olow], #0\n"
        "clzne          %[__pos], %[__olow]\n"
        "bne            1f\n"
        "clz            %[__pos], %[__ohigh]\n"
        "cmp            %[__pos], #32\n"
        "mvneq          %[__pos], #0\n"
        "addne          %[__pos], #32\n"
        "1:\n"
        : [__olow] "=&r" (out.v32.low),
          [__ohigh] "=&r" (out.v32.high),
          [__pos] "=&r" (pos)
        : [__ilow] "r" (in.v32.low),
          [__ihigh] "r" (in.v32.high)
        : "cc"
        );
        return pos;
}
EXPORT_SYMBOL(arch_xwbop_ffs64);

xwssq_t arch_xwbop_fls64(xwu64_t x)
{
        union {
                xwu64_t v64;
                struct {
#if defined(ARCHCFG_LITTLE_ENDIAN) && (1 == ARCHCFG_LITTLE_ENDIAN)
                        xwu32_t low;
                        xwu32_t high;
#else
                        xwu32_t high;
                        xwu32_t low;
#endif
                } v32;
        } in;
        xwssq_t pos;

        in.v64 = x;
        __asm__ volatile(
        "cmp            %[__ihigh], #0\n"
        "clzne          %[__pos], %[__ihigh]\n"
        "rsbne          %[__pos], %[__pos], #63\n"
        "bne            1f\n"
        "clz            %[__pos], %[__ilow]\n"
        "rsb            %[__pos], %[__pos], #31\n"
        "1:\n"
        : [__pos] "=&r" (pos)
        : [__ilow] "r" (in.v32.low),
          [__ihigh] "r" (in.v32.high)
        : "cc"
        );
        return pos;
}
EXPORT_SYMBOL(arch_xwbop_fls64);

xwu64_t arch_xwbop_rbit64(xwu64_t x)
{
        xwu32_t tmp;
        union {
                xwu64_t v64;
                struct {
                        xwu32_t w0;
                        xwu32_t w1;
                } v32;
        } val;

        val.v64 = x;
        __asm__ volatile(
        "       mov             %[__tmp], %[__w1]\n"
        "       rbit            %[__w1], %[__w0]\n"
        "       rbit            %[__w0], %[__tmp]\n"
        : [__w0] "+r" (val.v32.w0),
          [__w1] "+r" (val.v32.w1),
          [__tmp] "=&r" (tmp)
        :
        :
        );
        return val.v64;
}
EXPORT_SYMBOL(arch_xwbop_rbit64);

xwu64_t arch_xwbop_re64(xwu64_t x)
{
        xwu32_t *addrl;
        xwu32_t *addrh;
        union {
                xwu64_t q;
                xwu32_t w[2];
        } result;

        addrl = (xwu32_t *)&x;
        addrh = addrl + 1;
        __asm__ volatile(
        "       rev             %[__r_l32], %[__a_h32]\n"
        "       rev             %[__r_h32], %[__a_l32]\n"
        : [__r_l32] "=&r" (result.w[0]),
          [__r_h32] "=&r" (result.w[1])
        : [__a_l32] "r" (*addrl),
          [__a_h32] "r" (*addrh)
        : "memory"
        );
        return result.q;
}
EXPORT_SYMBOL(arch_xwbop_re64);

xwssq_t xwbmpop_ffs(xwbmp_t *bmp, xwsz_t num)
{
        xwsz_t m;
        xwsz_t i;
        xwbmp_t msk, tmp;
        xwssq_t p;

        m = BITS_TO_BMPS(num);
        p = -1;
        for (i = 0; i < m; i ++) {
                if (i == (m - 1)) {
                        msk = (BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
                        if ((xwbmp_t)0 == msk) {
                                msk = (~(xwbmp_t)0);
                        }
                } else {
                        msk = (~(xwbmp_t)0);
                }
                tmp = bmp[i] & msk;
                if (tmp) {
                        __asm__ volatile(
                        "       rbit            %[__p], %[__tmp]\n"
                        "       clz             %[__p], %[__p]\n"
                        : [__p] "=&r" (p)
                        : [__tmp] "r" (tmp)
                        :
                        );
                        p += (xwssq_t)(i << XWBMP_T_SHIFT);
                        break;
                }
        }
        return p;
}
EXPORT_SYMBOL(xwbmpop_ffs);

xwssq_t xwbmpop_ffz(xwbmp_t *bmp, xwsz_t num)
{
        xwsz_t m;
        xwsz_t i;
        xwbmp_t msk;
        xwssq_t p;
        xwbmp_t tmp;

        m = BITS_TO_BMPS(num);
        p = -1;
        for (i = 0; i < m; i ++) {
                if (i == (m - 1)) {
                        msk = ~(BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
                        if ((~(xwbmp_t)0) == msk) {
                                msk = (xwbmp_t)0;
                        }
                } else {
                        msk = (xwbmp_t)0;
                }
                tmp = ~(bmp[i] | msk);
                if (tmp) {
                        __asm__ volatile(
                        "       rbit            %[__p], %[__tmp]\n"
                        "       clz             %[__p], %[__p]\n"
                        : [__p] "=&r" (p)
                        : [__tmp] "r" (tmp)
                        :
                        );
                        p += (xwssq_t)(i << XWBMP_T_SHIFT);
                        break;
                }
        }
        return p;
}
EXPORT_SYMBOL(xwbmpop_ffz);

xwssq_t xwbmpop_fls(xwbmp_t *bmp, xwsz_t num)
{
        xwsz_t i;
        xwbmp_t msk, tmp;
        xwssq_t p;

        i = BITS_TO_BMPS(num);
        msk = (BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
        if ((xwbmp_t)0 == msk) {
                msk = (~(xwbmp_t)0);
        }
        while (i > 0) {
                i --;
                tmp = bmp[i] & msk;
                if (tmp)
                        break;
                msk = (~(xwbmp_t)0);
        }
        __asm__ volatile(
        "       clz             %[__p], %[__tmp]\n"
        : [__p] "=&r" (p)
        : [__tmp] "r" (tmp)
        :
        );
        p = (BITS_PER_XWBMP_T - 1) - p;
        if (p >= 0)
                p += (xwssq_t)(i << XWBMP_T_SHIFT); /* p += i * BITS_PER_XWBMP_T; */
        return p;
}
EXPORT_SYMBOL(xwbmpop_fls);

xwssq_t xwbmpop_flz(xwbmp_t *bmp, xwsz_t num)
{
        xwsz_t i;
        xwbmp_t msk;
        xwssq_t p;
        xwbmp_t tmp;

        i = BITS_TO_BMPS(num);
        msk = ~(BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
        if ((~(xwbmp_t)0) == msk) {
                msk = (xwbmp_t)0;
        }
        while (i > 0) {
                i --;
                tmp = ~(bmp[i] | msk);
                if (tmp)
                        break;
                msk = (xwbmp_t)0;
        }
        __asm__ volatile(
        "       clz             %[__p], %[__tmp]\n"
        : [__p] "=&r" (p)
        : [__tmp] "r" (tmp)
        :
        );
        p = (BITS_PER_XWBMP_T - 1) - p;
        if (p >= 0)
                p += (xwssq_t)(i << XWBMP_T_SHIFT);
        return p;
}
EXPORT_SYMBOL(xwbmpop_flz);
