/**
 * @file
 * @brief 原子操作汇编库: xwu16
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

#include <xwos/standard.h>
#include <armv8a_isa.h>
#include <xwos/lib/xwaop.h>

__xwlib_code
xwu16_t xwaop__xwu16__load(xwu16_a * a,
                           const enum xwmb_memory_order_em mo)
{
        xwu16_t v;

        switch (mo) {
        case xwmb_modr_relaxed:
                v = *a;
                break;
        case xwmb_modr_consume:
                v = *a;
                xwmb_mp_ddb();
                break;
        case xwmb_modr_acquire:
                v = ldarh(a);
                break;
        case xwmb_modr_release:
                v = *a;
                break;
        case xwmb_modr_acq_rel:
                v = ldarh(a);
                break;
        case xwmb_modr_seq_cst:
                xwmb_mp_mb();
                v = ldarh(a);
                break;
        default:
                v = *a;
                break;
        }
        return v;
}

__xwlib_code
xwu16_t xwaop__xwu16__store(xwu16_a * a,
                            const enum xwmb_memory_order_em mo,
                            xwu16_t v)
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
                stlrh(a, v);
                break;
        case xwmb_modr_acq_rel:
                stlrh(a, v);
                break;
        case xwmb_modr_seq_cst:
                stlrh(a, v);
                xwmb_mp_mb();
                break;
        }
        return v;
}

__xwlib_code
void xwaop__xwu16__read(xwu16_a * a,
                        xwu16_t * ov)
{
        xwu16_t o;
        o = xwaop__xwu16__load(a, xwmb_modr_acquire);
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xwu16__write(xwu16_a * a,
                         xwu16_t v,
                         xwu16_t * ov)
{
        xwu16_t o;

        do {
                o = (xwu16_t)ldaxrh(a);
        } while (stlxrh(a, (xwu16_t)v));
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xwu16__teq_then_write(xwu16_a * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * ov)
{
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o == t) {
                        rc = stlxrh(a, (xwu16_t)v);
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
xwer_t xwaop__xwu16__tne_then_write(xwu16_a * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * ov)
{
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o != t) {
                        rc = stlxrh(a, (xwu16_t)v);
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
xwer_t xwaop__xwu16__tgt_then_write(xwu16_a * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * ov)
{
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o > t) {
                        rc = stlxrh(a, (xwu16_t)v);
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
xwer_t xwaop__xwu16__tge_then_write(xwu16_a * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * ov)
{
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o >= t) {
                        rc = stlxrh(a, (xwu16_t)v);
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
xwer_t xwaop__xwu16__tlt_then_write(xwu16_a * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * ov)
{
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o < t) {
                        rc = stlxrh(a, (xwu16_t)v);
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
xwer_t xwaop__xwu16__tle_then_write(xwu16_a * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * ov)
{
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o <= t) {
                        rc = stlxrh(a, (xwu16_t)v);
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
xwer_t xwaop__xwu16__tgtlt_then_write(xwu16_a * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * ov)
{
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if ((o > l) && (o < r)) {
                        rc = stlxrh(a, (xwu16_t)v);
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
xwer_t xwaop__xwu16__tgelt_then_write(xwu16_a * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * ov)
{
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if ((o >= l) && (o < r)) {
                        rc = stlxrh(a, (xwu16_t)v);
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
xwer_t xwaop__xwu16__tgtle_then_write(xwu16_a * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * ov)
{
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if ((o > l) && (o <= r)) {
                        rc = stlxrh(a, (xwu16_t)v);
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
xwer_t xwaop__xwu16__tgele_then_write(xwu16_a * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * ov)
{
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if ((o >= l) && (o <= r)) {
                        rc = stlxrh(a, (xwu16_t)v);
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
void xwaop__xwu16__add(xwu16_a * a,
                       xwu16_t v,
                       xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;

        do {
                o = (xwu16_t)ldaxrh(a);
                n = o + v;
        } while (stlxrh(a, (xwu16_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xwu16__teq_then_add(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o == t) {
                        n = o + v;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tne_then_add(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o != t) {
                        n = o + v;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tgt_then_add(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o > t) {
                        n = o + v;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tge_then_add(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o >= t) {
                        n = o + v;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tlt_then_add(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o < t) {
                        n = o + v;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tle_then_add(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o <= t) {
                        n = o + v;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tgtlt_then_add(xwu16_a * a,
                                    xwu16_t l, xwu16_t r,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if ((o > l) && (o < r)) {
                        n = o + v;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tgelt_then_add(xwu16_a * a,
                                    xwu16_t l, xwu16_t r,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if ((o >= l) && (o < r)) {
                        n = o + v;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tgtle_then_add(xwu16_a * a,
                                    xwu16_t l, xwu16_t r,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if ((o > l) && (o <= r)) {
                        n = o + v;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tgele_then_add(xwu16_a * a,
                                    xwu16_t l, xwu16_t r,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if ((o >= l) && (o <= r)) {
                        n = o + v;
                        rc = stlxrh(a, (xwu16_t)n);
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
void xwaop__xwu16__sub(xwu16_a * a,
                       xwu16_t v,
                       xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;

        do {
                o = (xwu16_t)ldaxrh(a);
                n = o - v;
        } while (stlxrh(a, (xwu16_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xwu16__teq_then_sub(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o == t) {
                        n = o - v;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tne_then_sub(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o != t) {
                        n = o - v;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tgt_then_sub(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o > t) {
                        n = o - v;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tge_then_sub(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o >= t) {
                        n = o - v;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tlt_then_sub(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o < t) {
                        n = o - v;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tle_then_sub(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o <= t) {
                        n = o - v;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tgtlt_then_sub(xwu16_a * a,
                                    xwu16_t l, xwu16_t r,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if ((o > l) && (o < r)) {
                        n = o - v;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tgelt_then_sub(xwu16_a * a,
                                    xwu16_t l, xwu16_t r,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if ((o >= l) && (l < r)) {
                        n = o - v;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tgtle_then_sub(xwu16_a * a,
                                    xwu16_t l, xwu16_t r,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if ((o > l) && (o <= r)) {
                        n = o - v;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tgele_then_sub(xwu16_a * a,
                                    xwu16_t l, xwu16_t r,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if ((o >= l) && (o <= r)) {
                        n = o - v;
                        rc = stlxrh(a, (xwu16_t)n);
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
void xwaop__xwu16__rsb(xwu16_a * a,
                       xwu16_t v,
                       xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;

        do {
                o = (xwu16_t)ldaxrh(a);
                n = v - o;
        } while (stlxrh(a, (xwu16_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xwu16__teq_then_rsb(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o == t) {
                        n = v - o;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tne_then_rsb(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o != t) {
                        n = v - o;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tgt_then_rsb(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o > t) {
                        n = v - o;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tge_then_rsb(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o != t) {
                        n = v - o;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tlt_then_rsb(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o < t) {
                        n = v - o;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tle_then_rsb(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o <= t) {
                        n = v - o;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tgtlt_then_rsb(xwu16_a * a,
                                    xwu16_t l, xwu16_t r,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if ((o > l) && (o < r)) {
                        n = v - o;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tgelt_then_rsb(xwu16_a * a,
                                    xwu16_t l, xwu16_t r,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if ((o >= l) && (o < r)) {
                        n = v - o;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tgtle_then_rsb(xwu16_a * a,
                                    xwu16_t l, xwu16_t r,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if ((o > l) && (o <= r)) {
                        n = v - o;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tgele_then_rsb(xwu16_a * a,
                                    xwu16_t l, xwu16_t r,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if ((o >= l) && (o <= r)) {
                        n = v - o;
                        rc = stlxrh(a, (xwu16_t)n);
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
void xwaop__xwu16__or(xwu16_a * a,
                      xwu16_t v,
                      xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;

        do {
                o = (xwu16_t)ldaxrh(a);
                n = o | v;
        } while (stlxrh(a, (xwu16_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xwu16__and(xwu16_a * a,
                       xwu16_t v,
                       xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;

        do {
                o = (xwu16_t)ldaxrh(a);
                n = o & v;
        } while (stlxrh(a, (xwu16_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xwu16__xor(xwu16_a * a,
                       xwu16_t v,
                       xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;

        do {
                o = (xwu16_t)ldaxrh(a);
                n = o ^ v;
        } while (stlxrh(a, (xwu16_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xwu16__c0m(xwu16_a * a,
                       xwu16_t m,
                       xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;

        do {
                o = (xwu16_t)ldaxrh(a);
                n = o & (~m);
        } while (stlxrh(a, (xwu16_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xwu16__t1ma_then_c0m(xwu16_a * a,
                                   xwu16_t m,
                                   xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if ((o & m) == m) {
                        n = o & (~m);
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__t1mo_then_c0m(xwu16_a * a,
                                   xwu16_t m,
                                   xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (o & m) {
                        n = o & (~m);
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__t0ma_then_s1m(xwu16_a * a,
                                   xwu16_t m,
                                   xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (!(o & m)) {
                        n = o | m;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__t0mo_then_s1m(xwu16_a * a,
                                   xwu16_t m,
                                   xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if ((o & m) != m) {
                        n = o | m;
                        rc = stlxrh(a, (xwu16_t)n);
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
xwer_t xwaop__xwu16__tst_then_op(xwu16_a * a,
                                 xwaop_tst_f tst, void * tst_args,
                                 xwaop_op_f op, void * op_args,
                                 xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t o;
        xwu16_t n;
        xwer_t rc;

        do {
                o = (xwu16_t)ldaxrh(a);
                if (tst) {
                        if (tst((const void *)&o, tst_args)) {
                                if (op) {
                                        op(&n, (const void *)&o, op_args);
                                        rc = stlxrh(a, (xwu16_t)n);
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
                                rc = stlxrh(a, (xwu16_t)n);
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
