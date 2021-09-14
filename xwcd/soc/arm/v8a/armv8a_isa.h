/**
 * @file
 * @brief ARMv8-A Instruction Set Architecture
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

#ifndef __armv8a_isa_h__
#define __armv8a_isa_h__

#include <xwos/standard.h>

static __xwcc_inline
xwu8_t ldarb(volatile xwu8_t * addr)
{
        register xwu32_t tmp;

        __asm__ volatile(
        "       ldarb   %w[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return (xwu8_t)tmp;
}

static __xwcc_inline
void stlrb(volatile xwu8_t * addr, xwu8_t value)
{
        __asm__ volatile(
        "       stlrb  %w[__value], [%[__addr]]\n"
        :
        : [__value] "r" ((xwu32_t)value),
          [__addr] "r" (addr)
        : "memory"
        );
}

static __xwcc_inline
xwu16_t ldarh(volatile xwu16_t * addr)
{
        register xwu32_t tmp;

        __asm__ volatile(
        "       ldarh   %w[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return (xwu16_t)tmp;
}

static __xwcc_inline
void stlrh(volatile xwu16_t * addr, xwu16_t value)
{
        __asm__ volatile(
        "       stlrh  %w[__value], [%[__addr]]\n"
        :
        : [__value] "r" ((xwu32_t)value),
          [__addr] "r" (addr)
        : "memory"
        );
}

static __xwcc_inline
xwu32_t ldarw(volatile xwu32_t * addr)
{
        register xwu32_t tmp;

        __asm__ volatile(
        "       ldar    %w[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return tmp;
}

static __xwcc_inline
void stlrw(volatile xwu32_t * addr, xwu32_t value)
{
        __asm__ volatile(
        "       stlr    %w[__value], [%[__addr]]\n"
        :
        : [__value] "r" (value),
          [__addr] "r" (addr)
        : "memory"
        );
}

static __xwcc_inline
xwu64_t ldar(volatile xwu64_t * addr)
{
        register xwu64_t tmp;

        __asm__ volatile(
        "       ldar    %[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return tmp;
}

static __xwcc_inline
void stlr(volatile xwu64_t * addr, xwu64_t value)
{
        __asm__ volatile(
        "       stlr    %[__value], [%[__addr]]\n"
        :
        : [__value] "r" (value),
          [__addr] "r" (addr)
        : "memory"
        );
}

static __xwcc_inline
void clrex(void)
{
        __asm__ volatile(
        "       clrex\n"
        :
        :
        : "memory"
        );
}

static __xwcc_inline
xwu8_t ldaxrb(volatile xwu8_t * addr)
{
        register xwu32_t tmp;

        __asm__ volatile(
        "       prfm    pstl1strm, [%[__addr]]\n"
        "       ldaxrb  %w[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return (xwu8_t)tmp;
}

static __xwcc_inline
xwer_t stlxrb(volatile xwu8_t * addr, xwu8_t value)
{
        register int32_t rc;

        __asm__ volatile(
        "       stlxrb  %w[__rc], %w[__value], [%[__addr]]\n"
        : [__rc] "=&r" (rc)
        : [__value] "r" ((xwu32_t)value),
          [__addr] "r" (addr)
        : "memory"
        );
        return (xwer_t)rc;
}

static __xwcc_inline
xwu16_t ldaxrh(volatile xwu16_t * addr)
{
        register xwu32_t tmp;

        __asm__ volatile(
        "       prfm    pstl1strm, [%[__addr]]\n"
        "       ldaxrh  %w[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return (xwu16_t)tmp;
}

static __xwcc_inline
xwer_t stlxrh(volatile xwu16_t * addr, xwu16_t value)
{
        register int32_t rc;

        __asm__ volatile(
        "       stlxrh  %w[__rc], %w[__value], [%[__addr]]\n"
        : [__rc] "=&r" (rc)
        : [__value] "r" ((xwu32_t)value),
          [__addr] "r" (addr)
        : "memory"
        );
        return (xwer_t)rc;
}

static __xwcc_inline
xwu32_t ldaxrw(volatile xwu32_t * addr)
{
        register xwu32_t tmp;

        __asm__ volatile(
        "       prfm    pstl1strm, [%[__addr]]\n"
        "       ldaxr   %w[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return tmp;
}

static __xwcc_inline
xwer_t stlxrw(volatile xwu32_t * addr, xwu32_t value)
{
        register int32_t rc;

        __asm__ volatile(
        "       stlxr   %w[__rc], %w[__value], [%[__addr]]\n"
        : [__rc] "=&r" (rc)
        : [__value] "r" (value),
          [__addr] "r" (addr)
        : "memory"
        );
        return (xwer_t)rc;
}

static __xwcc_inline
xwu64_t ldaxr(volatile xwu64_t * addr)
{
        register xwu64_t tmp;

        __asm__ volatile(
        "       prfm    pstl1strm, [%[__addr]]\n"
        "       ldaxr   %[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return tmp;
}

static __xwcc_inline
xwer_t stlxr(volatile xwu64_t * addr, xwu64_t value)
{
        register int32_t rc;

        __asm__ volatile(
        "       stlxr   %w[__rc], %[__value], [%[__addr]]\n"
        : [__rc] "=&r" (rc)
        : [__value] "r" (value),
          [__addr] "r" (addr)
        : "memory"
        );
        return (xwer_t)rc;
}

#endif /* armv8a_isa.h */
