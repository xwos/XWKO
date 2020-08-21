/**
 * @file
 * @brief XWOS通用库：32位原子操作函数库
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

#ifndef __xwos_lib_xwaop32_h__
#define __xwos_lib_xwaop32_h__

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
xwu32_t xwaop__xwu32_t__load(__xwcc_atomic xwu32_t * a,
                             const enum xwmb_memory_order_em mo);

__xwlib_code
xwu32_t xwaop__xwu32_t__store(__xwcc_atomic xwu32_t * a,
                              const enum xwmb_memory_order_em mo,
                              xwu32_t v);

__xwlib_code
void xwaop__xwu32_t__read(__xwcc_atomic xwu32_t * a,
                          xwu32_t * ov);

__xwlib_code
void xwaop__xwu32_t__write(__xwcc_atomic xwu32_t * a,
                           xwu32_t v,
                           xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__teq_then_write(__xwcc_atomic xwu32_t * a,
                                      xwu32_t t,
                                      xwu32_t v,
                                      xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tne_then_write(__xwcc_atomic xwu32_t * a,
                                      xwu32_t t,
                                      xwu32_t v,
                                      xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tgt_then_write(__xwcc_atomic xwu32_t * a,
                                      xwu32_t t,
                                      xwu32_t v,
                                      xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tge_then_write(__xwcc_atomic xwu32_t * a,
                                      xwu32_t t,
                                      xwu32_t v,
                                      xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tlt_then_write(__xwcc_atomic xwu32_t * a,
                                      xwu32_t t,
                                      xwu32_t v,
                                      xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tle_then_write(__xwcc_atomic xwu32_t * a,
                                      xwu32_t t,
                                      xwu32_t v,
                                      xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tgtlt_then_write(__xwcc_atomic xwu32_t * a,
                                        xwu32_t l, xwu32_t r,
                                        xwu32_t v,
                                        xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tgelt_then_write(__xwcc_atomic xwu32_t * a,
                                        xwu32_t l, xwu32_t r,
                                        xwu32_t v,
                                        xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tgtle_then_write(__xwcc_atomic xwu32_t * a,
                                        xwu32_t l, xwu32_t r,
                                        xwu32_t v,
                                        xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tgele_then_write(__xwcc_atomic xwu32_t * a,
                                        xwu32_t l, xwu32_t r,
                                        xwu32_t v,
                                        xwu32_t * ov);

__xwlib_code
void xwaop__xwu32_t__add(__xwcc_atomic xwu32_t * a,
                         xwu32_t v,
                         xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__teq_then_add(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tne_then_add(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tgt_then_add(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tge_then_add(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tlt_then_add(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tle_then_add(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tgtlt_then_add(__xwcc_atomic xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tgelt_then_add(__xwcc_atomic xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tgtle_then_add(__xwcc_atomic xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tgele_then_add(__xwcc_atomic xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov);

__xwlib_code
void xwaop__xwu32_t__sub(__xwcc_atomic xwu32_t * a,
                         xwu32_t v,
                         xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__teq_then_sub(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t, xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tne_then_sub(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tgt_then_sub(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tge_then_sub(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tlt_then_sub(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tle_then_sub(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tgtlt_then_sub(__xwcc_atomic xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tgelt_then_sub(__xwcc_atomic xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tgtle_then_sub(__xwcc_atomic xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tgele_then_sub(__xwcc_atomic xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov);

__xwlib_code
void xwaop__xwu32_t__rsb(__xwcc_atomic xwu32_t * a,
                         xwu32_t v,
                         xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__teq_then_rsb(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tne_then_rsb(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tgt_then_rsb(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tge_then_rsb(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tlt_then_rsb(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tle_then_rsb(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tgtlt_then_rsb(__xwcc_atomic xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tgelt_then_rsb(__xwcc_atomic xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tgtle_then_rsb(__xwcc_atomic xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tgele_then_rsb(__xwcc_atomic xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov);

__xwlib_code
void xwaop__xwu32_t__or(__xwcc_atomic xwu32_t * a,
                        xwu32_t v,
                        xwu32_t * nv, xwu32_t * ov);

__xwlib_code
void xwaop__xwu32_t__and(__xwcc_atomic xwu32_t * a,
                         xwu32_t v,
                         xwu32_t * nv, xwu32_t * ov);

__xwlib_code
void xwaop__xwu32_t__xor(__xwcc_atomic xwu32_t * a,
                         xwu32_t v,
                         xwu32_t * nv, xwu32_t * ov);

__xwlib_code
void xwaop__xwu32_t__c0m(__xwcc_atomic xwu32_t * a,
                         xwu32_t v,
                         xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__t1ma_then_c0m(__xwcc_atomic xwu32_t * a,
                                     xwu32_t m,
                                     xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__t1mo_then_c0m(__xwcc_atomic xwu32_t * a,
                                     xwu32_t m,
                                     xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__t0ma_then_s1m(__xwcc_atomic xwu32_t * a,
                                     xwu32_t m,
                                     xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__t0mo_then_s1m(__xwcc_atomic xwu32_t * a,
                                     xwu32_t m,
                                     xwu32_t * nv, xwu32_t * ov);

__xwlib_code
xwer_t xwaop__xwu32_t__tst_then_op(__xwcc_atomic xwu32_t * a,
                                   xwaop_tst_f tst, void * tst_args,
                                   xwaop_op_f op, void * op_args,
                                   xwu32_t * nv, xwu32_t * ov);

/******** ******** ******** ******** signed ******** ******** ******** ********/
__xwlib_code
xws32_t xwaop__xws32_t__load(__xwcc_atomic xws32_t * a,
                             const enum xwmb_memory_order_em mo);

__xwlib_code
xws32_t xwaop__xws32_t__store(__xwcc_atomic xws32_t * a,
                              const enum xwmb_memory_order_em mo,
                              xws32_t v);

__xwlib_code
void xwaop__xws32_t__read(__xwcc_atomic xws32_t * a,
                          xws32_t * ov);

__xwlib_code
void xwaop__xws32_t__write(__xwcc_atomic xws32_t * a,
                           xws32_t v,
                           xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__teq_then_write(__xwcc_atomic xws32_t * a,
                                      xws32_t t,
                                      xws32_t v,
                                      xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tne_then_write(__xwcc_atomic xws32_t * a,
                                      xws32_t t,
                                      xws32_t v,
                                      xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tgt_then_write(__xwcc_atomic xws32_t * a,
                                      xws32_t t,
                                      xws32_t v,
                                      xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tge_then_write(__xwcc_atomic xws32_t * a,
                                      xws32_t t,
                                      xws32_t v,
                                      xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tlt_then_write(__xwcc_atomic xws32_t * a,
                                      xws32_t t,
                                      xws32_t v,
                                      xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tle_then_write(__xwcc_atomic xws32_t * a,
                                      xws32_t t,
                                      xws32_t v,
                                      xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tgtlt_then_write(__xwcc_atomic xws32_t * a,
                                        xws32_t l, xws32_t r,
                                        xws32_t v,
                                        xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tgelt_then_write(__xwcc_atomic xws32_t * a,
                                        xws32_t l, xws32_t r,
                                        xws32_t v,
                                        xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tgtle_then_write(__xwcc_atomic xws32_t * a,
                                        xws32_t l, xws32_t r,
                                        xws32_t v,
                                        xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tgele_then_write(__xwcc_atomic xws32_t * a,
                                        xws32_t l, xws32_t r,
                                        xws32_t v,
                                        xws32_t * ov);

__xwlib_code
void xwaop__xws32_t__add(__xwcc_atomic xws32_t * a,
                         xws32_t v,
                         xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__teq_then_add(__xwcc_atomic xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tne_then_add(__xwcc_atomic xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tgt_then_add(__xwcc_atomic xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tge_then_add(__xwcc_atomic xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tlt_then_add(__xwcc_atomic xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tle_then_add(__xwcc_atomic xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tgtlt_then_add(__xwcc_atomic xws32_t * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tgelt_then_add(__xwcc_atomic xws32_t * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tgtle_then_add(__xwcc_atomic xws32_t * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tgele_then_add(__xwcc_atomic xws32_t * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * nv, xws32_t * ov);

__xwlib_code
void xwaop__xws32_t__sub(__xwcc_atomic xws32_t * a,
                         xws32_t v,
                         xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__teq_then_sub(__xwcc_atomic xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tne_then_sub(__xwcc_atomic xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tgt_then_sub(__xwcc_atomic xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tge_then_sub(__xwcc_atomic xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tlt_then_sub(__xwcc_atomic xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tle_then_sub(__xwcc_atomic xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tgtlt_then_sub(__xwcc_atomic xws32_t * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tgelt_then_sub(__xwcc_atomic xws32_t * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tgtle_then_sub(__xwcc_atomic xws32_t * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tgele_then_sub(__xwcc_atomic xws32_t * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * nv, xws32_t * ov);

__xwlib_code
void xwaop__xws32_t__rsb(__xwcc_atomic xws32_t * a,
                         xws32_t v,
                         xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__teq_then_rsb(__xwcc_atomic xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tne_then_rsb(__xwcc_atomic xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tgt_then_rsb(__xwcc_atomic xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tge_then_rsb(__xwcc_atomic xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tlt_then_rsb(__xwcc_atomic xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tle_then_rsb(__xwcc_atomic xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tgtlt_then_rsb(__xwcc_atomic xws32_t * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tgelt_then_rsb(__xwcc_atomic xws32_t * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tgtle_then_rsb(__xwcc_atomic xws32_t * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * nv, xws32_t * ov);

__xwlib_code
xwer_t xwaop__xws32_t__tgele_then_rsb(__xwcc_atomic xws32_t * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * nv, xws32_t * ov);

__xwlib_code
void xwaop__xws32_t__or(__xwcc_atomic xws32_t * a,
                        xws32_t v,
                        xws32_t * nv, xws32_t * ov);

__xwlib_code
void xwaop__xws32_t__and(__xwcc_atomic xws32_t * a,
                         xws32_t v,
                         xws32_t * nv, xws32_t * ov);

__xwlib_code
void xwaop__xws32_t__xor(__xwcc_atomic xws32_t * a,
                         xws32_t v,
                         xws32_t * nv, xws32_t * ov);

static __xwlib_inline
void xwaop__xws32_t__c0m(__xwcc_atomic xws32_t * a,
                         xws32_t m,
                         xws32_t * nv, xws32_t * ov)
{
        xwaop__xwu32_t__c0m((__xwcc_atomic xwu32_t *)a,
                            (xwu32_t)m,
                            (xwu32_t *)nv, (xwu32_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws32_t__t1ma_then_c0m(__xwcc_atomic xws32_t * a,
                                     xws32_t m,
                                     xws32_t * nv, xws32_t * ov)
{
        return xwaop__xwu32_t__t1ma_then_c0m((__xwcc_atomic xwu32_t *)a,
                                             (xwu32_t)m,
                                             (xwu32_t *)nv, (xwu32_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws32_t__t1mo_then_c0m(__xwcc_atomic xws32_t * a,
                                     xws32_t m,
                                     xws32_t * nv, xws32_t * ov)
{
        return xwaop__xwu32_t__t1mo_then_c0m((__xwcc_atomic xwu32_t *)a,
                                             (xwu32_t)m,
                                             (xwu32_t *)nv, (xwu32_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws32_t__t0ma_then_s1m(__xwcc_atomic xws32_t * a,
                                     xws32_t m,
                                     xws32_t * nv, xws32_t * ov)
{
        return xwaop__xwu32_t__t0ma_then_s1m((__xwcc_atomic xwu32_t *)a,
                                             (xwu32_t)m,
                                             (xwu32_t *)nv, (xwu32_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws32_t__t0mo_then_s1m(__xwcc_atomic xws32_t * a,
                                     xws32_t m,
                                     xws32_t * nv, xws32_t * ov)
{
        return xwaop__xwu32_t__t0mo_then_s1m((__xwcc_atomic xwu32_t *)a,
                                             (xwu32_t)m,
                                             (xwu32_t *)nv, (xwu32_t *)ov);
}

__xwlib_code
xwer_t xwaop__xws32_t__tst_then_op(__xwcc_atomic xws32_t * a,
                                   xwaop_tst_f tst, void * tst_args,
                                   xwaop_op_f op, void * op_args,
                                   xws32_t * nv, xws32_t * ov);

#endif /* xwos/lib/xwaop32.h */
