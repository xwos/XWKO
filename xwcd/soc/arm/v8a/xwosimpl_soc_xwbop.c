/**
 * @file
 * @brief 玄武OS移植实现层：SOC位操作
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
#include <xwos/lib/xwlog.h>
#include <xwos/lib/xwbop.h>

xwssq_t xwbop_ffs8(xwu8_t x)
{
        xwu32_t tmp = x & (xwu32_t)0x000000FF;
        return xwbop_ffs32(tmp);
}
EXPORT_SYMBOL(xwbop_ffs8);

xwssq_t xwbop_fls8(xwu8_t x)
{
        xwu32_t tmp = x & (xwu32_t)0x000000FF;
        return xwbop_fls32(tmp);
}
EXPORT_SYMBOL(xwbop_fls8);

xwu8_t xwbop_rbit8(xwu8_t x)
{
        xwu32_t tmp = x & (xwu32_t)0x000000FF;
        __asm__ volatile(
        "       rbit            %w[__tmp], %w[__tmp]\n"
        : [__tmp] "+r" (tmp)
        :
        :
        );
        return (xwu8_t)(tmp >> 24);
}
EXPORT_SYMBOL(xwbop_rbit8);

xwssq_t xwbop_ffs16(xwu16_t x)
{
        xwu32_t tmp = x & (xwu32_t)0x0000FFFF;
        return xwbop_ffs32(tmp);
}
EXPORT_SYMBOL(xwbop_ffs16);

xwssq_t xwbop_fls16(xwu16_t x)
{
        xwu32_t tmp = x & (xwu32_t)0x0000FFFF;
        return xwbop_fls32(tmp);
}
EXPORT_SYMBOL(xwbop_fls16);

xwu16_t xwbop_rbit16(xwu16_t x)
{
        xwu32_t tmp = x & (xwu32_t)0x0000FFFF;
        __asm__ volatile(
        "       rbit            %w[__tmp], %w[__tmp]\n"
        : [__tmp] "+r" (tmp)
        :
        :
        );
        return (xwu16_t)(tmp >> 16);
}
EXPORT_SYMBOL(xwbop_rbit16);

xwu16_t xwbop_re16(xwu16_t x)
{
        xwu32_t res;
        xwu32_t tmp = x & (xwu32_t)0x0000FFFF;
        __asm__ volatile(
        "       rev16           %w[__res], %w[__tmp]\n"
        : [__res] "=&r" (res)
        : [__tmp] "r" (tmp)
        :
        );
        return (xwu16_t)res;
}
EXPORT_SYMBOL(xwbop_re16);

xwssq_t xwbop_ffs32(xwu32_t x)
{
        xwssq_t pos;
        xwu64_t tmp = x & (xwu64_t)0x00000000FFFFFFFF;
        __asm__ volatile(
        "       rbit            %[__pos], %[__tmp]\n"
        "       clz             %[__pos], %[__pos]\n"
        : [__pos] "=&r" (pos)
        : [__tmp] "r" (tmp)
        :
        );
        if (pos >= 32) {
                pos = -1;
        }
        return pos;
}
EXPORT_SYMBOL(xwbop_ffs32);

xwssq_t xwbop_fls32(xwu32_t x)
{
        xwssq_t pos;
        xwu64_t tmp = x & (xwu64_t)0x00000000FFFFFFFF;
        __asm__ volatile(
        "       clz             %[__pos], %[__tmp]\n"
        : [__pos] "=&r" (pos)
        : [__tmp] "r" (tmp)
        :
        );
        pos = 63 - pos;
        return pos;
}
EXPORT_SYMBOL(xwbop_fls32);

xwu32_t xwbop_rbit32(xwu32_t x)
{
        __asm__ volatile(
        "       rbit            %w[__x], %w[__x]\n"
        : [__x] "+r" (x)
        :
        :
        );
        return x;
}
EXPORT_SYMBOL(xwbop_rbit32);

xwu32_t xwbop_re32(xwu32_t x)
{
        xwu32_t res;
        __asm__ volatile(
        "       rev             %w[__res], %w[__x]\n"
        : [__res] "=&r" (res)
        : [__x] "r" (x)
        :
        );
        return res;
}
EXPORT_SYMBOL(xwbop_re32);

xwssq_t xwbop_ffs64(xwu64_t x)
{
        xwssq_t pos;
        __asm__ volatile(
        "       rbit            %[__pos], %[__x]\n"
        "       clz             %[__pos], %[__pos]\n"
        : [__pos] "=&r" (pos)
        : [__x] "r" (x)
        :
        );
        if (64 == pos) {
                pos = -1;
        }
        return pos;
}
EXPORT_SYMBOL(xwbop_ffs64);

xwssq_t xwbop_fls64(xwu64_t x)
{
        xwssq_t pos;
        __asm__ volatile(
        "       clz             %[__pos], %[__x]\n"
        : [__pos] "=&r" (pos)
        : [__x] "r" (x)
        :
        );
        pos = 63 - pos;
        return pos;
}
EXPORT_SYMBOL(xwbop_fls64);

xwu64_t xwbop_rbit64(xwu64_t x)
{
        __asm__ volatile(
        "       rbit            %[__x], %[__x]\n"
        : [__x] "+r" (x)
        :
        :
        );
        return x;
}
EXPORT_SYMBOL(xwbop_rbit64);

xwu64_t xwbop_re64(xwu64_t x)
{
        xwu64_t res;
        __asm__ volatile(
        "       rev             %[__res], %[__x]\n"
        : [__res] "=&r" (res)
        : [__x] "r" (x)
        :
        );
        return res;
}
EXPORT_SYMBOL(xwbop_re64);

xwssq_t xwbmpop_ffs(xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t m;
        xwsz_t i;
        xwbmp_t msk, tmp;
        xwssq_t p;

        m = BITS_TO_XWBMP_T(num);
        p = -1;
        for (i = 0; i < m; i++) {
                if (i == (m - 1)) {
                        msk = (XWBOP_BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
                        if ((xwbmp_t)0 == msk) {
                                msk = (~(xwbmp_t)0);
                        }
                        tmp = bmp[i] & msk;
                } else {
                        tmp = bmp[i];
                }
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

xwssq_t xwbmpop_ffz(xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t m;
        xwsz_t i;
        xwbmp_t msk;
        xwssq_t p;
        xwbmp_t tmp;

        m = BITS_TO_XWBMP_T(num);
        p = -1;
        for (i = 0; i < m; i++) {
                if (i == (m - 1)) {
                        msk = ~(XWBOP_BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
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

        i = BITS_TO_XWBMP_T(num);
        msk = (XWBOP_BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
        if ((xwbmp_t)0 == msk) {
                msk = (~(xwbmp_t)0);
        }
        while (i > 0) {
                i--;
                tmp = bmp[i] & msk;
                if (tmp) {
                        break;
                }
                msk = (~(xwbmp_t)0);
        }
        __asm__ volatile(
        "       clz             %[__p], %[__tmp]\n"
        : [__p] "=&r" (p)
        : [__tmp] "r" (tmp)
        :
        );
        p = (BITS_PER_XWBMP_T - 1) - p;
        if (p >= 0) {
                p += (xwssq_t)(i << XWBMP_T_SHIFT); /* p += i * BITS_PER_XWBMP_T; */
        }
        return p;
}
EXPORT_SYMBOL(xwbmpop_fls);

xwssq_t xwbmpop_flz(xwbmp_t *bmp, xwsz_t num)
{
        xwsz_t i;
        xwbmp_t msk;
        xwssq_t p;
        xwbmp_t tmp;

        i = BITS_TO_XWBMP_T(num);
        msk = ~(XWBOP_BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
        if ((~(xwbmp_t)0) == msk) {
                msk = (xwbmp_t)0;
        }
        while (i > 0) {
                i--;
                tmp = ~(bmp[i] | msk);
                if (tmp) {
                        break;
                }
                msk = (xwbmp_t)0;
        }
        __asm__ volatile(
        "       clz             %[__p], %[__tmp]\n"
        : [__p] "=&r" (p)
        : [__tmp] "r" (tmp)
        :
        );
        p = (BITS_PER_XWBMP_T - 1) - p;
        if (p >= 0) {
                p += (xwssq_t)(i << XWBMP_T_SHIFT);
        }
        return p;
}
EXPORT_SYMBOL(xwbmpop_flz);
