/**
 * @file
 * @brief XuanWuOS内核：中断
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

#ifndef __xwos_core_irq_h__
#define __xwos_core_irq_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <linux/irqflags.h>
#include <linux/preempt.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xw_inline
void xwos_cpuirq_enable_lc(void)
{
        local_irq_enable();
}

static __xw_inline
void xwos_cpuirq_disable_lc(void)
{
        local_irq_disable();
}

static __xw_inline
void xwos_cpuirq_restore_lc(xwreg_t flag)
{
        local_irq_restore(flag);
}

static __xw_inline
void xwos_cpuirq_save_lc(xwreg_t * flag)
{
        local_irq_save(*flag);
}

static __xw_inline
xwer_t xwos_irq_get_id(xwirq_t * irqnbuf)
{
        xwer_t rc;

        if (in_interrupt()) {
                if (in_irq()) {
                        rc = OK;
                } else {
                        rc = -EINBH;
                }
        } else {
                rc = -EINTHRD;
        }
        return rc;
}

#endif /* xwos/core/irq.h */
