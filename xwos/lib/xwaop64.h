/**
 * @file
 * @brief XWOS通用库：64位原子操作函数库
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

#ifndef __xwos_lib_xwaop64_h__
#define __xwos_lib_xwaop64_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** ******** unsigned ******** ******** ******** ********/
__xwlib_code
xwu64_t xwaop__xwu64_t__load(__atomic xwu64_t * a,
                             const enum xwmb_memory_order_em mo);

__xwlib_code
xwu64_t xwaop__xwu64_t__store(__atomic xwu64_t * a,
                              const enum xwmb_memory_order_em mo,
                              xwu64_t v);

__xwlib_code
void xwaop__xwu64_t__read(__atomic xwu64_t * a,
                          xwu64_t * ov);

__xwlib_code
void xwaop__xwu64_t__write(__atomic xwu64_t * a,
                           xwu64_t v,
                           xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__teq_then_write(__atomic xwu64_t * a,
                                      xwu64_t t,
                                      xwu64_t v,
                                      xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tne_then_write(__atomic xwu64_t * a,
                                      xwu64_t t,
                                      xwu64_t v,
                                      xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tgt_then_write(__atomic xwu64_t * a,
                                      xwu64_t t,
                                      xwu64_t v,
                                      xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tge_then_write(__atomic xwu64_t * a,
                                      xwu64_t t,
                                      xwu64_t v,
                                      xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tlt_then_write(__atomic xwu64_t * a,
                                      xwu64_t t,
                                      xwu64_t v,
                                      xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tle_then_write(__atomic xwu64_t * a,
                                      xwu64_t t,
                                      xwu64_t v,
                                      xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tgtlt_then_write(__atomic xwu64_t * a,
                                        xwu64_t l, xwu64_t r,
                                        xwu64_t v,
                                        xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tgelt_then_write(__atomic xwu64_t * a,
                                        xwu64_t l, xwu64_t r,
                                        xwu64_t v,
                                        xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tgtle_then_write(__atomic xwu64_t * a,
                                        xwu64_t l, xwu64_t r,
                                        xwu64_t v,
                                        xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tgele_then_write(__atomic xwu64_t * a,
                                        xwu64_t l, xwu64_t r,
                                        xwu64_t v,
                                        xwu64_t * ov);

__xwlib_code
void xwaop__xwu64_t__add(__atomic xwu64_t * a,
                         xwu64_t v,
                         xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__teq_then_add(__atomic xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tne_then_add(__atomic xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tgt_then_add(__atomic xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tge_then_add(__atomic xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tlt_then_add(__atomic xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tle_then_add(__atomic xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tgtlt_then_add(__atomic xwu64_t * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tgelt_then_add(__atomic xwu64_t * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tgtle_then_add(__atomic xwu64_t * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tgele_then_add(__atomic xwu64_t * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * nv, xwu64_t * ov);

__xwlib_code
void xwaop__xwu64_t__sub(__atomic xwu64_t * a,
                         xwu64_t v,
                         xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__teq_then_sub(__atomic xwu64_t * a,
                                    xwu64_t t, xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tne_then_sub(__atomic xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tgt_then_sub(__atomic xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tge_then_sub(__atomic xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tlt_then_sub(__atomic xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tle_then_sub(__atomic xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tgtlt_then_sub(__atomic xwu64_t * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tgelt_then_sub(__atomic xwu64_t * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tgtle_then_sub(__atomic xwu64_t * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tgele_then_sub(__atomic xwu64_t * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * nv, xwu64_t * ov);

__xwlib_code
void xwaop__xwu64_t__rsb(__atomic xwu64_t * a,
                         xwu64_t v,
                         xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__teq_then_rsb(__atomic xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tne_then_rsb(__atomic xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tgt_then_rsb(__atomic xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tge_then_rsb(__atomic xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tlt_then_rsb(__atomic xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tle_then_rsb(__atomic xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tgtlt_then_rsb(__atomic xwu64_t * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tgelt_then_rsb(__atomic xwu64_t * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tgtle_then_rsb(__atomic xwu64_t * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tgele_then_rsb(__atomic xwu64_t * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * nv, xwu64_t * ov);

__xwlib_code
void xwaop__xwu64_t__or(__atomic xwu64_t * a,
                        xwu64_t v,
                        xwu64_t * nv, xwu64_t * ov);

__xwlib_code
void xwaop__xwu64_t__and(__atomic xwu64_t * a,
                         xwu64_t v,
                         xwu64_t * nv, xwu64_t * ov);

__xwlib_code
void xwaop__xwu64_t__xor(__atomic xwu64_t * a,
                         xwu64_t v,
                         xwu64_t * nv, xwu64_t * ov);

__xwlib_code
void xwaop__xwu64_t__c0m(__atomic xwu64_t * a,
                         xwu64_t v,
                         xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__t1ma_then_c0m(__atomic xwu64_t * a,
                                     xwu64_t m,
                                     xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__t1mo_then_c0m(__atomic xwu64_t * a,
                                     xwu64_t m,
                                     xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__t0ma_then_s1m(__atomic xwu64_t * a,
                                     xwu64_t m,
                                     xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__t0mo_then_s1m(__atomic xwu64_t * a,
                                     xwu64_t m,
                                     xwu64_t * nv, xwu64_t * ov);

__xwlib_code
xwer_t xwaop__xwu64_t__tst_then_op(__atomic xwu64_t * a,
                                   xwaop_tst_f tst, void * tst_args,
                                   xwaop_op_f op, void * op_args,
                                   xwu64_t * nv, xwu64_t * ov);

/******** ******** ******** ******** signed ******** ******** ******** ********/
__xwlib_code
xws64_t xwaop__xws64_t__load(__atomic xws64_t * a,
                             const enum xwmb_memory_order_em mo);

__xwlib_code
xws64_t xwaop__xws64_t__store(__atomic xws64_t * a,
                              const enum xwmb_memory_order_em mo,
                              xws64_t v);

__xwlib_code
void xwaop__xws64_t__read(__atomic xws64_t * a,
                          xws64_t * ov);

__xwlib_code
void xwaop__xws64_t__write(__atomic xws64_t * a,
                           xws64_t v,
                           xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__teq_then_write(__atomic xws64_t * a,
                                      xws64_t t,
                                      xws64_t v,
                                      xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tne_then_write(__atomic xws64_t * a,
                                      xws64_t t,
                                      xws64_t v,
                                      xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tgt_then_write(__atomic xws64_t * a,
                                      xws64_t t,
                                      xws64_t v,
                                      xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tge_then_write(__atomic xws64_t * a,
                                      xws64_t t,
                                      xws64_t v,
                                      xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tlt_then_write(__atomic xws64_t * a,
                                      xws64_t t,
                                      xws64_t v,
                                      xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tle_then_write(__atomic xws64_t * a,
                                      xws64_t t,
                                      xws64_t v,
                                      xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tgtlt_then_write(__atomic xws64_t * a,
                                        xws64_t l, xws64_t r,
                                        xws64_t v,
                                        xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tgelt_then_write(__atomic xws64_t * a,
                                        xws64_t l, xws64_t r,
                                        xws64_t v,
                                        xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tgtle_then_write(__atomic xws64_t * a,
                                        xws64_t l, xws64_t r,
                                        xws64_t v,
                                        xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tgele_then_write(__atomic xws64_t * a,
                                        xws64_t l, xws64_t r,
                                        xws64_t v,
                                        xws64_t * ov);

__xwlib_code
void xwaop__xws64_t__add(__atomic xws64_t * a,
                         xws64_t v,
                         xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__teq_then_add(__atomic xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tne_then_add(__atomic xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tgt_then_add(__atomic xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tge_then_add(__atomic xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tlt_then_add(__atomic xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tle_then_add(__atomic xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tgtlt_then_add(__atomic xws64_t * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tgelt_then_add(__atomic xws64_t * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tgtle_then_add(__atomic xws64_t * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tgele_then_add(__atomic xws64_t * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * nv, xws64_t * ov);

__xwlib_code
void xwaop__xws64_t__sub(__atomic xws64_t * a,
                         xws64_t v,
                         xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__teq_then_sub(__atomic xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tne_then_sub(__atomic xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tgt_then_sub(__atomic xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tge_then_sub(__atomic xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tlt_then_sub(__atomic xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tle_then_sub(__atomic xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tgtlt_then_sub(__atomic xws64_t * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tgelt_then_sub(__atomic xws64_t * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tgtle_then_sub(__atomic xws64_t * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tgele_then_sub(__atomic xws64_t * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * nv, xws64_t * ov);

__xwlib_code
void xwaop__xws64_t__rsb(__atomic xws64_t * a,
                         xws64_t v,
                         xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__teq_then_rsb(__atomic xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tne_then_rsb(__atomic xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tgt_then_rsb(__atomic xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tge_then_rsb(__atomic xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tlt_then_rsb(__atomic xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tle_then_rsb(__atomic xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tgtlt_then_rsb(__atomic xws64_t * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tgelt_then_rsb(__atomic xws64_t * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tgtle_then_rsb(__atomic xws64_t * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * nv, xws64_t * ov);

__xwlib_code
xwer_t xwaop__xws64_t__tgele_then_rsb(__atomic xws64_t * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * nv, xws64_t * ov);

__xwlib_code
void xwaop__xws64_t__or(__atomic xws64_t * a,
                        xws64_t v,
                        xws64_t * nv, xws64_t * ov);

__xwlib_code
void xwaop__xws64_t__and(__atomic xws64_t * a,
                         xws64_t v,
                         xws64_t * nv, xws64_t * ov);

__xwlib_code
void xwaop__xws64_t__xor(__atomic xws64_t * a,
                         xws64_t v,
                         xws64_t * nv, xws64_t * ov);

static __xwlib_inline
void xwaop__xws64_t__c0m(__atomic xws64_t * a,
                         xws64_t m,
                         xws64_t * nv, xws64_t * ov)
{
        xwaop__xwu64_t__c0m((__atomic xwu64_t *)a,
                            (xwu64_t)m,
                            (xwu64_t *)nv, (xwu64_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws64_t__t1ma_then_c0m(__atomic xws64_t * a,
                                     xws64_t m,
                                     xws64_t * nv, xws64_t * ov)
{
        return xwaop__xwu64_t__t1ma_then_c0m((__atomic xwu64_t *)a,
                                             (xwu64_t)m,
                                             (xwu64_t *)nv, (xwu64_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws64_t__t1mo_then_c0m(__atomic xws64_t * a,
                                     xws64_t m,
                                     xws64_t * nv, xws64_t * ov)
{
        return xwaop__xwu64_t__t1mo_then_c0m((__atomic xwu64_t *)a,
                                             (xwu64_t)m,
                                             (xwu64_t *)nv, (xwu64_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws64_t__t0ma_then_s1m(__atomic xws64_t * a,
                                     xws64_t m,
                                     xws64_t * nv, xws64_t * ov)
{
        return xwaop__xwu64_t__t0ma_then_s1m((__atomic xwu64_t *)a,
                                             (xwu64_t)m,
                                             (xwu64_t *)nv, (xwu64_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws64_t__t0mo_then_s1m(__atomic xws64_t * a,
                                     xws64_t m,
                                     xws64_t * nv, xws64_t * ov)
{
        return xwaop__xwu64_t__t0mo_then_s1m((__atomic xwu64_t *)a,
                                             (xwu64_t)m,
                                             (xwu64_t *)nv, (xwu64_t *)ov);
}

__xwlib_code
xwer_t xwaop__xws64_t__tst_then_op(__atomic xws64_t * a,
                                   xwaop_tst_f tst, void * tst_args,
                                   xwaop_op_f op, void * op_args,
                                   xws64_t * nv, xws64_t * ov);

#endif /* xwos/lib/xwaop64.h */
