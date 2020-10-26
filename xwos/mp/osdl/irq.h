/**
 * @file
 * @brief 玄武OS内核操作系统接口描述层：中断
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

#ifndef __xwos_mp_osdl_irq_h__
#define __xwos_mp_osdl_irq_h__

#include <xwos/mp/irq.h>

static __xwcc_inline
xwer_t xwosdl_irq_get_id(xwirq_t * irqnbuf)
{
        return xwmp_irq_get_id(irqnbuf);
}

static __xwcc_inline
void xwosdl_cpuirq_enable_lc(void)
{
        xwmp_cpuirq_enable_lc();
}

static __xwcc_inline
void xwosdl_cpuirq_disable_lc(void)
{
        xwmp_cpuirq_disable_lc();
}

static __xwcc_inline
void xwosdl_cpuirq_restore_lc(xwreg_t cpuirq)
{
        xwmp_cpuirq_restore_lc(cpuirq);
}

static __xwcc_inline
void xwosdl_cpuirq_save_lc(xwreg_t * cpuirq)
{
        xwmp_cpuirq_save_lc(cpuirq);
}

#endif /* xwos/mp/osdl/irq.h */
