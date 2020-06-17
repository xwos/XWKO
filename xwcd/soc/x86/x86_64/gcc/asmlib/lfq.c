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
#include <asmlib/lfq.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brife X86_64的无锁队列
 */
union arch_lfq {
        xwlfq_t u128;
        struct {
                xwptr_t link;
                xwptr_t ticket;
        } s;
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_code
void arch_lfq_push(__atomic xwlfq_t * h, __atomic xwlfq_t * n)
{
        union arch_lfq *top;
        union arch_lfq ov, nv;
        bool rc;

        top = (union arch_lfq *)n;
        do {
                ov.u128 = *h;
                top->s.link = ov.s.link;
                top->s.ticket = 0;
                nv.s.link = (xwptr_t)n;
                nv.s.ticket = ov.s.ticket + 1;
                xwmb_smp_mb();
                /* To generate instruction: cmpxchg16b */
                rc = __sync_bool_compare_and_swap(h, ov.u128, nv.u128);
        } while (!rc);
}

__xwbsp_code
xwlfq_t * arch_lfq_pop(__atomic xwlfq_t * h)
{
        union {
                xwlfq_t * lfq;
                union arch_lfq * arch_lfq;
        } top;
        union arch_lfq ov, nv;
        bool rc;

        do {
                ov.u128 = *h;
                if (ov.s.link) {
                        top.lfq = (xwlfq_t *)ov.s.link;
                        nv.s.link = top.arch_lfq->s.link;
                        nv.s.ticket = ov.s.ticket + 1;
                } else {
                        top.lfq = NULL;
                        break;
                }
                xwmb_smp_mb();
                /* To generate instruction: cmpxchg16b */
                rc = __sync_bool_compare_and_swap(h, ov.u128, nv.u128);
        } while (!rc);
        return top.lfq;
}
