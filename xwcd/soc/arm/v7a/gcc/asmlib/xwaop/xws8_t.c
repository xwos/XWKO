/**
 * @file
 * @brief 原子操作汇编库: xws8_t
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
#include <armv7a_core.h>
#include <xwos/lib/xwaop8.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwlib_code
xws8_t xwaop__xws8_t__load(__atomic xws8_t * a,
                           const enum xwmb_memory_order_em mo)
{
        xws8_t v;

        switch (mo) {
        case xwmb_modr_relaxed:
                v = *a;
                break;
        case xwmb_modr_consume:
                v = *a;
                xwmb_smp_ddb();
                break;
        case xwmb_modr_acquire:
                v = *a;
                xwmb_smp_mb();
                break;
        case xwmb_modr_release:
                v = *a;
                break;
        case xwmb_modr_acq_rel:
                v = *a;
                xwmb_smp_mb();
                break;
        case xwmb_modr_seq_cst:
                xwmb_smp_mb();
                v = *a;
                xwmb_smp_mb();
                break;
        default:
                v = *a;
                break;
        }
        return v;
}

__xwlib_code
xws8_t xwaop__xws8_t__store(__atomic xws8_t * a,
                            const enum xwmb_memory_order_em mo,
                            xws8_t v)
{
        switch (mo) {
        case xwmb_modr_relaxed:
                *a = v;
                break;
        case xwmb_modr_consume:
                *a = v;
                break;
        case xwmb_modr_acquire:
                *a = v;
                break;
        case xwmb_modr_release:
                xwmb_smp_mb();
                *a = v;
                break;
        case xwmb_modr_acq_rel:
                xwmb_smp_mb();
                *a = v;
                break;
        case xwmb_modr_seq_cst:
                xwmb_smp_mb();
                *a = v;
                xwmb_smp_mb();
                break;
        }
        return v;
}

__xwlib_code
void xwaop__xws8_t__read(__atomic xws8_t * a,
                         xws8_t * ov)
{
        xws8_t o;
        o = xwaop__xws8_t__load(a, xwmb_modr_acquire);
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xws8_t__write(__atomic xws8_t * a,
                          xws8_t v,
                          xws8_t * ov)
{
        xws8_t o;

        do {
                o = (xws8_t)ldrexb(a);
                xwmb_smp_mb();
        } while (strexb(a, (xwu8_t)v));
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xws8_t__teq_then_write(__atomic xws8_t * a,
                                     xws8_t t,
                                     xws8_t v,
                                     xws8_t * ov)
{
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (o == t) {
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)v);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tne_then_write(__atomic xws8_t * a,
                                     xws8_t t,
                                     xws8_t v,
                                     xws8_t * ov)
{
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (o != t) {
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)v);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgt_then_write(__atomic xws8_t * a,
                                     xws8_t t,
                                     xws8_t v,
                                     xws8_t * ov)
{
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (o > t) {
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)v);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tge_then_write(__atomic xws8_t * a,
                                     xws8_t t,
                                     xws8_t v,
                                     xws8_t * ov)
{
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (o >= t) {
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)v);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tlt_then_write(__atomic xws8_t * a,
                                     xws8_t t,
                                     xws8_t v,
                                     xws8_t * ov)
{
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (o < t) {
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)v);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tle_then_write(__atomic xws8_t * a,
                                     xws8_t t,
                                     xws8_t v,
                                     xws8_t * ov)
{
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (o <= t) {
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)v);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgtlt_then_write(__atomic xws8_t * a,
                                       xws8_t l, xws8_t r,
                                       xws8_t v,
                                       xws8_t * ov)
{
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if ((o > l) && (o < r)) {
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)v);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgelt_then_write(__atomic xws8_t * a,
                                       xws8_t l, xws8_t r,
                                       xws8_t v,
                                       xws8_t * ov)
{
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if ((o >= l) && (o < r)) {
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)v);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgtle_then_write(__atomic xws8_t * a,
                                       xws8_t l, xws8_t r,
                                       xws8_t v,
                                       xws8_t * ov)
{
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if ((o > l) && (o <= r)) {
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)v);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgele_then_write(__atomic xws8_t * a,
                                       xws8_t l, xws8_t r,
                                       xws8_t v,
                                       xws8_t * ov)
{
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if ((o >= l) && (o <= r)) {
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)v);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
void xwaop__xws8_t__add(__atomic xws8_t * a,
                        xws8_t v,
                        xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;

        do {
                o = (xws8_t)ldrexb(a);
                n = o + v;
                xwmb_smp_mb();
        } while (strexb(a, (xwu8_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xws8_t__teq_then_add(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (o == t) {
                        n = o + v;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tne_then_add(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (o != t) {
                        n = o + v;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgt_then_add(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (o > t) {
                        n = o + v;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tge_then_add(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (o >= t) {
                        n = o + v;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tlt_then_add(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (o < t) {
                        n = o + v;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tle_then_add(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (o <= t) {
                        n = o + v;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgtlt_then_add(__atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if ((o > l) && (o < r)) {
                        n = o + v;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgelt_then_add(__atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if ((o >= l) && (o < r)) {
                        n = o + v;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgtle_then_add(__atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if ((o > l) && (o <= r)) {
                        n = o + v;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgele_then_add(__atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if ((o >= l) && (o <= r)) {
                        n = o + v;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
void xwaop__xws8_t__sub(__atomic xws8_t * a,
                        xws8_t v,
                        xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;

        do {
                o = (xws8_t)ldrexb(a);
                n = o - v;
                xwmb_smp_mb();
        } while (strexb(a, (xwu8_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xws8_t__teq_then_sub(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (o == t) {
                        n = o - v;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tne_then_sub(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (o != t) {
                        n = o - v;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgt_then_sub(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (o > t) {
                        n = o - v;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tge_then_sub(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (o >= t) {
                        n = o - v;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tlt_then_sub(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (o < t) {
                        n = o - v;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tle_then_sub(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (o <= t) {
                        n = o - v;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgtlt_then_sub(__atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if ((o > l) && (o < r)) {
                        n = o - v;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgelt_then_sub(__atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if ((o >= l) && (l < r)) {
                        n = o - v;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgtle_then_sub(__atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if ((o > l) && (o <= r)) {
                        n = o - v;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgele_then_sub(__atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if ((o >= l) && (o <= r)) {
                        n = o - v;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
void xwaop__xws8_t__rsb(__atomic xws8_t * a,
                        xws8_t v,
                        xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;

        do {
                o = (xws8_t)ldrexb(a);
                n = v - o;
                xwmb_smp_mb();
        } while (strexb(a, (xwu8_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xws8_t__teq_then_rsb(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (o == t) {
                        n = v - o;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tne_then_rsb(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (o != t) {
                        n = v - o;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgt_then_rsb(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (o > t) {
                        n = v - o;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tge_then_rsb(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (o != t) {
                        n = v - o;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tlt_then_rsb(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (o < t) {
                        n = v - o;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tle_then_rsb(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (o <= t) {
                        n = v - o;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgtlt_then_rsb(__atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if ((o > l) && (o < r)) {
                        n = v - o;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgelt_then_rsb(__atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if ((o >= l) && (o < r)) {
                        n = v - o;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgtle_then_rsb(__atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if ((o > l) && (o <= r)) {
                        n = v - o;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgele_then_rsb(__atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if ((o >= l) && (o <= r)) {
                        n = v - o;
                        xwmb_smp_mb();
                        rc = strexb(a, (xwu8_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
void xwaop__xws8_t__or(__atomic xws8_t * a,
                       xws8_t v,
                       xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;

        do {
                o = (xws8_t)ldrexb(a);
                n = o | v;
                xwmb_smp_mb();
        } while (strexb(a, (xwu8_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xws8_t__and(__atomic xws8_t * a,
                        xws8_t v,
                        xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;

        do {
                o = (xws8_t)ldrexb(a);
                n = o & v;
                xwmb_smp_mb();
        } while (strexb(a, (xwu8_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        };
}

__xwlib_code
void xwaop__xws8_t__xor(__atomic xws8_t * a,
                        xws8_t v,
                        xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;

        do {
                o = (xws8_t)ldrexb(a);
                n = o ^ v;
                xwmb_smp_mb();
        } while (strexb(a, (xwu8_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xws8_t__tst_then_op(__atomic xws8_t * a,
                                  xwaop_tst_f tst, void * tst_args,
                                  xwaop_op_f op, void * op_args,
                                  xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = (xws8_t)ldrexb(a);
                if (tst) {
                        if (tst((const void *)&o, tst_args)) {
                                if (op) {
                                        op(&n, (const void *)&o, op_args);
                                        xwmb_smp_mb();
                                        rc = strexb(a, (xwu8_t)n);
                                } else {
                                        rc = XWOK;
                                        n = o;
                                        xwmb_smp_mb();
                                        break;
                                }
                        } else {
                                rc = -EACCES;
                                n = o;
                                xwmb_smp_ddb();
                                break;
                        }
                } else {
                        if (op) {
                                op(&n, (const void *)&o, op_args);
                                xwmb_smp_mb();
                                rc = strexb(a, (xwu8_t)n);
                        } else {
                                rc = XWOK;
                                n = o;
                                xwmb_smp_mb();
                                break;
                        }
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}
