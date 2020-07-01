/**
 * @file
 * @brief cortex-a core definations
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

#ifndef __arch_core_ca_h__
#define __arch_core_ca_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      register     ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********          inline functions           ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xw_inline
void clrex(void)
{
        __asm__ volatile(
        "       clrex\n"
        :
        :
        : "memory"
        );
}

static __xw_inline
xwu8_t ldrexb(volatile xwu8_t *addr)
{
        xwu8_t tmp;

        __asm__ volatile(
        "       ldrexb  %[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return tmp;
}

static __xw_inline
xwer_t strexb(volatile xwu8_t *addr, xwu8_t value)
{
        xwer_t rc;

        __asm__ volatile(
        "       strexb  %[__rc], %[__value], [%[__addr]]\n"
        : [__rc] "=&r" (rc)
        : [__value] "r" (value),
          [__addr] "r" (addr)
        : "memory"
        );
        return rc;
}

static __xw_inline
xwu16_t ldrexh(volatile xwu16_t *addr)
{
        xwu16_t tmp;

        __asm__ volatile(
        "       ldrexh  %[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return tmp;
}

static __xw_inline
xwer_t strexh(volatile xwu16_t *addr, xwu16_t value)
{
        xwer_t rc;

        __asm__ volatile(
        "       strexh  %[__rc], %[__value], [%[__addr]]\n"
        : [__rc] "=&r" (rc)
        : [__value] "r" (value),
          [__addr] "r" (addr)
        : "memory"
        );
        return rc;
}

static __xw_inline
xwu32_t ldrex(volatile xwu32_t *addr)
{
        xwu32_t tmp;

        __asm__ volatile(
        "       ldrex   %[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return tmp;
}

static __xw_inline
xwer_t strex(volatile xwu32_t *addr, xwu32_t value)
{
        xwer_t rc;

        __asm__ volatile(
        "       strex   %[__rc], %[__value], [%[__addr]]\n"
        : [__rc] "=&r" (rc)
        : [__value] "r" (value),
          [__addr] "r" (addr)
        : "memory"
        );
        return rc;
}

static __xw_inline
xwu64_t ldrexd(volatile xwu64_t *addr)
{
        xwu64_t tmp;

        __asm__ volatile(
        "       ldrexd  %[__tmp], %H[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return tmp;
}

static __xw_inline
xwer_t strexd(volatile xwu64_t *addr, xwu64_t value)
{
        xwer_t rc;

        __asm__ volatile(
        "       strexd  %[__rc], %[__newval], %H[__newval], [%[__addr]]\n"
        : [__rc] "=&r" (rc),
          [__oldval] "+Qo" (*addr)
        : [__newval]"r" (value),
          [__addr] "r" (addr)
        : "memory"
        );
        return rc;
}

#endif /* armv7a_core.h */
