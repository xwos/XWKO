/**
 * @file
 * @brief 汇编库：无锁队列
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <armv7a_core.h>
#include <asmlib/lfq.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_code
void arch_lfq_push(__atomic xwlfq_t * h, __atomic xwlfq_t * n)
{
        xwlfq_t * next;

        do {
                next = (xwlfq_t *)ldrex(h);
                xwmb_smp_ddb();
                *n = (xwlfq_t)next;
                xwmb_smp_mb();
        } while (strex(h, (xwu32_t)n));
}

__xwbsp_code
xwlfq_t * arch_lfq_pop(__atomic xwlfq_t * h)
{
        xwlfq_t * top;
        xwlfq_t * next;
        xwer_t rc;

        do {
                top = (xwlfq_t *)ldrex(h);
                xwmb_smp_ddb();
                if (top) {
                        next = (xwlfq_t *)(*top);
                        xwmb_smp_mb();
                        rc = strex(h, (xwu32_t)next);
                } else {
                        break;
                }
        } while (rc);
        return top;
}
