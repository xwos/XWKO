/**
 * @file
 * @brief 原子操作汇编库: xws32
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
#include <armv8a_isa.h>
#include <xwos/lib/xwaop.h>

__xwlib_code
xws32_t xwaop__xws32__load(xws32_a * a,
                           const enum xwmb_memory_order_em mo)
{
        xws32_t v;

        switch (mo) {
        case xwmb_modr_relaxed:
                v = *a;
                break;
        case xwmb_modr_consume:
                v = *a;
                xwmb_mp_ddb();
                break;
        case xwmb_modr_acquire:
                v = (xws32_t)ldarw(a);
                break;
        case xwmb_modr_release:
                v = *a;
                break;
        case xwmb_modr_acq_rel:
                v = (xws32_t)ldarw(a);
                break;
        case xwmb_modr_seq_cst:
                xwmb_mp_mb();
                v = (xws32_t)ldarw(a);
                break;
        default:
                v = *a;
                break;
        }
        return v;
}

__xwlib_code
xws32_t xwaop__xws32__store(xws32_a * a,
                            const enum xwmb_memory_order_em mo,
                            xws32_t v)
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
                stlrw(a, (xwu32_t)v);
                break;
        case xwmb_modr_acq_rel:
                stlrw(a, (xwu32_t)v);
                break;
        case xwmb_modr_seq_cst:
                stlrw(a, (xwu32_t)v);
                xwmb_mp_mb();
                break;
        }
        return v;
}

__xwlib_code
void xwaop__xws32__read(xws32_a * a,
                        xws32_t * ov)
{
        xws32_t o;
        o = xwaop__xws32__load(a, xwmb_modr_acquire);
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xws32__write(xws32_a * a,
                         xws32_t v,
                         xws32_t * ov)
{
        xws32_t o;

        do {
                o = (xws32_t)ldaxrw(a);
        } while (stlxrw(a, (xwu32_t)v));
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xws32__teq_then_write(xws32_a * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * ov)
{
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (o == t) {
                        rc = stlxrw(a, (xwu32_t)v);
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
xwer_t xwaop__xws32__tne_then_write(xws32_a * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * ov)
{
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (o != t) {
                        rc = stlxrw(a, (xwu32_t)v);
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
xwer_t xwaop__xws32__tgt_then_write(xws32_a * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * ov)
{
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (o > t) {
                        rc = stlxrw(a, (xwu32_t)v);
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
xwer_t xwaop__xws32__tge_then_write(xws32_a * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * ov)
{
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (o >= t) {
                        rc = stlxrw(a, (xwu32_t)v);
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
xwer_t xwaop__xws32__tlt_then_write(xws32_a * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * ov)
{
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (o < t) {
                        rc = stlxrw(a, (xwu32_t)v);
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
xwer_t xwaop__xws32__tle_then_write(xws32_a * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * ov)
{
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (o <= t) {
                        rc = stlxrw(a, (xwu32_t)v);
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
xwer_t xwaop__xws32__tgtlt_then_write(xws32_a * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * ov)
{
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if ((o > l) && (o < r)) {
                        rc = stlxrw(a, (xwu32_t)v);
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
xwer_t xwaop__xws32__tgelt_then_write(xws32_a * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * ov)
{
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if ((o >= l) && (o < r)) {
                        rc = stlxrw(a, (xwu32_t)v);
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
xwer_t xwaop__xws32__tgtle_then_write(xws32_a * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * ov)
{
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if ((o > l) && (o <= r)) {
                        rc = stlxrw(a, (xwu32_t)v);
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
xwer_t xwaop__xws32__tgele_then_write(xws32_a * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * ov)
{
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if ((o >= l) && (o <= r)) {
                        rc = stlxrw(a, (xwu32_t)v);
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
void xwaop__xws32__add(xws32_a * a,
                       xws32_t v,
                       xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;

        do {
                o = (xws32_t)ldaxrw(a);
                n = o + v;
        } while (stlxrw(a, (xwu32_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xws32__teq_then_add(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (o == t) {
                        n = o + v;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tne_then_add(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (o != t) {
                        n = o + v;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tgt_then_add(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (o > t) {
                        n = o + v;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tge_then_add(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (o >= t) {
                        n = o + v;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tlt_then_add(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (o < t) {
                        n = o + v;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tle_then_add(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (o <= t) {
                        n = o + v;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tgtlt_then_add(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if ((o > l) && (o < r)) {
                        n = o + v;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tgelt_then_add(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if ((o >= l) && (o < r)) {
                        n = o + v;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tgtle_then_add(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if ((o > l) && (o <= r)) {
                        n = o + v;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tgele_then_add(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if ((o >= l) && (o <= r)) {
                        n = o + v;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
void xwaop__xws32__sub(xws32_a * a,
                       xws32_t v,
                       xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;

        do {
                o = (xws32_t)ldaxrw(a);
                n = o - v;
        } while (stlxrw(a, (xwu32_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xws32__teq_then_sub(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (o == t) {
                        n = o - v;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tne_then_sub(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (o != t) {
                        n = o - v;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tgt_then_sub(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (o > t) {
                        n = o - v;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tge_then_sub(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (o >= t) {
                        n = o - v;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tlt_then_sub(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (o < t) {
                        n = o - v;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tle_then_sub(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (o <= t) {
                        n = o - v;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tgtlt_then_sub(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if ((o > l) && (o < r)) {
                        n = o - v;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tgelt_then_sub(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if ((o >= l) && (l < r)) {
                        n = o - v;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tgtle_then_sub(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if ((o > l) && (o <= r)) {
                        n = o - v;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tgele_then_sub(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if ((o >= l) && (o <= r)) {
                        n = o - v;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
void xwaop__xws32__rsb(xws32_a * a,
                       xws32_t v,
                       xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;

        do {
                o = (xws32_t)ldaxrw(a);
                n = v - o;
        } while (stlxrw(a, (xwu32_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xws32__teq_then_rsb(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (o == t) {
                        n = v - o;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tne_then_rsb(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (o != t) {
                        n = v - o;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tgt_then_rsb(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (o > t) {
                        n = v - o;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tge_then_rsb(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (o != t) {
                        n = v - o;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tlt_then_rsb(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (o < t) {
                        n = v - o;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tle_then_rsb(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (o <= t) {
                        n = v - o;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tgtlt_then_rsb(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if ((o > l) && (o < r)) {
                        n = v - o;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tgelt_then_rsb(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if ((o >= l) && (o < r)) {
                        n = v - o;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tgtle_then_rsb(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if ((o > l) && (o <= r)) {
                        n = v - o;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
xwer_t xwaop__xws32__tgele_then_rsb(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if ((o >= l) && (o <= r)) {
                        n = v - o;
                        rc = stlxrw(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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
void xwaop__xws32__or(xws32_a * a,
                      xws32_t v,
                      xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;

        do {
                o = (xws32_t)ldaxrw(a);
                n = o | v;
        } while (stlxrw(a, (xwu32_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xws32__and(xws32_a * a,
                       xws32_t v,
                       xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;

        do {
                o = (xws32_t)ldaxrw(a);
                n = o & v;
        } while (stlxrw(a, (xwu32_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        };
}

__xwlib_code
void xwaop__xws32__xor(xws32_a * a,
                       xws32_t v,
                       xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;

        do {
                o = (xws32_t)ldaxrw(a);
                n = o ^ v;
        } while (stlxrw(a, (xwu32_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xws32__tst_then_op(xws32_a * a,
                                 xwaop_tst_f tst, void * tst_args,
                                 xwaop_op_f op, void * op_args,
                                 xws32_t * nv, xws32_t * ov)
{
        xws32_t o;
        xws32_t n;
        xwer_t rc;

        do {
                o = (xws32_t)ldaxrw(a);
                if (tst) {
                        if (tst((const void *)&o, tst_args)) {
                                if (op) {
                                        op(&n, (const void *)&o, op_args);
                                        rc = stlxrw(a, (xwu32_t)n);
                                } else {
                                        rc = XWOK;
                                        n = o;
                                        break;
                                }
                        } else {
                                rc = -EACCES;
                                n = o;
                                break;
                        }
                } else {
                        if (op) {
                                op(&n, (const void *)&o, op_args);
                                rc = stlxrw(a, (xwu32_t)n);
                        } else {
                                rc = XWOK;
                                n = o;
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
