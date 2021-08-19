/**
 * @file
 * @brief 原子操作汇编库: xwu8
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
xwu8_t xwaop__xwu8__load(xwu8_a * a,
                         const enum xwmb_memory_order_em mo)
{
        xwu8_t v;

        switch (mo) {
        case xwmb_modr_relaxed:
                v = *a;
                break;
        case xwmb_modr_consume:
                v = *a;
                xwmb_mp_ddb();
                break;
        case xwmb_modr_acquire:
                v = ldarb(a);
                break;
        case xwmb_modr_release:
                v = *a;
                break;
        case xwmb_modr_acq_rel:
                v = ldarb(a);
                break;
        case xwmb_modr_seq_cst:
                xwmb_mp_mb();
                v = ldarb(a);
                break;
        default:
                v = *a;
                break;
        }
        return v;
}

__xwlib_code
xwu8_t xwaop__xwu8__store(xwu8_a * a,
                          const enum xwmb_memory_order_em mo,
                          xwu8_t v)
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
                stlrb(a, v);
                break;
        case xwmb_modr_acq_rel:
                stlrb(a, v);
                break;
        case xwmb_modr_seq_cst:
                stlrb(a, v);
                xwmb_mp_mb();
                break;
        }
        return v;
}

__xwlib_code
void xwaop__xwu8__read(xwu8_a * a,
                       xwu8_t * ov)
{
        xwu8_t o;
        o = xwaop__xwu8__load(a, xwmb_modr_acquire);
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xwu8__write(xwu8_a * a,
                        xwu8_t v,
                        xwu8_t * ov)
{
        xwu8_t o;

        do {
                o = (xwu8_t)ldaxrb(a);
        } while (stlxrb(a, (xwu8_t)v));
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xwu8__teq_then_write(xwu8_a * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * ov)
{
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o == t) {
                        rc = stlxrb(a, (xwu8_t)v);
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
xwer_t xwaop__xwu8__tne_then_write(xwu8_a * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * ov)
{
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o != t) {
                        rc = stlxrb(a, (xwu8_t)v);
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
xwer_t xwaop__xwu8__tgt_then_write(xwu8_a * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * ov)
{
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o > t) {
                        rc = stlxrb(a, (xwu8_t)v);
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
xwer_t xwaop__xwu8__tge_then_write(xwu8_a * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * ov)
{
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o >= t) {
                        rc = stlxrb(a, (xwu8_t)v);
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
xwer_t xwaop__xwu8__tlt_then_write(xwu8_a * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * ov)
{
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o < t) {
                        rc = stlxrb(a, (xwu8_t)v);
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
xwer_t xwaop__xwu8__tle_then_write(xwu8_a * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * ov)
{
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o <= t) {
                        rc = stlxrb(a, (xwu8_t)v);
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
xwer_t xwaop__xwu8__tgtlt_then_write(xwu8_a * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * ov)
{
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if ((o > l) && (o < r)) {
                        rc = stlxrb(a, (xwu8_t)v);
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
xwer_t xwaop__xwu8__tgelt_then_write(xwu8_a * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * ov)
{
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if ((o >= l) && (o < r)) {
                        rc = stlxrb(a, (xwu8_t)v);
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
xwer_t xwaop__xwu8__tgtle_then_write(xwu8_a * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * ov)
{
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if ((o > l) && (o <= r)) {
                        rc = stlxrb(a, (xwu8_t)v);
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
xwer_t xwaop__xwu8__tgele_then_write(xwu8_a * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * ov)
{
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if ((o >= l) && (o <= r)) {
                        rc = stlxrb(a, (xwu8_t)v);
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
void xwaop__xwu8__add(xwu8_a * a,
                      xwu8_t v,
                      xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;

        do {
                o = (xwu8_t)ldaxrb(a);
                n = o + v;
        } while (stlxrb(a, (xwu8_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xwu8__teq_then_add(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o == t) {
                        n = o + v;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tne_then_add(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o != t) {
                        n = o + v;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tgt_then_add(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o > t) {
                        n = o + v;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tge_then_add(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o >= t) {
                        n = o + v;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tlt_then_add(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o < t) {
                        n = o + v;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tle_then_add(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o <= t) {
                        n = o + v;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tgtlt_then_add(xwu8_a * a,
                                   xwu8_t l, xwu8_t r,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if ((o > l) && (o < r)) {
                        n = o + v;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tgelt_then_add(xwu8_a * a,
                                   xwu8_t l, xwu8_t r,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if ((o >= l) && (o < r)) {
                        n = o + v;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tgtle_then_add(xwu8_a * a,
                                   xwu8_t l, xwu8_t r,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if ((o > l) && (o <= r)) {
                        n = o + v;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tgele_then_add(xwu8_a * a,
                                   xwu8_t l, xwu8_t r,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if ((o >= l) && (o <= r)) {
                        n = o + v;
                        rc = stlxrb(a, (xwu8_t)n);
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
void xwaop__xwu8__sub(xwu8_a * a,
                      xwu8_t v,
                      xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;

        do {
                o = (xwu8_t)ldaxrb(a);
                n = o - v;
        } while (stlxrb(a, (xwu8_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xwu8__teq_then_sub(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o == t) {
                        n = o - v;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tne_then_sub(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o != t) {
                        n = o - v;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tgt_then_sub(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o > t) {
                        n = o - v;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tge_then_sub(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o >= t) {
                        n = o - v;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tlt_then_sub(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o < t) {
                        n = o - v;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tle_then_sub(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o <= t) {
                        n = o - v;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tgtlt_then_sub(xwu8_a * a,
                                   xwu8_t l, xwu8_t r,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if ((o > l) && (o < r)) {
                        n = o - v;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tgelt_then_sub(xwu8_a * a,
                                   xwu8_t l, xwu8_t r,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if ((o >= l) && (l < r)) {
                        n = o - v;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tgtle_then_sub(xwu8_a * a,
                                   xwu8_t l, xwu8_t r,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if ((o > l) && (o <= r)) {
                        n = o - v;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tgele_then_sub(xwu8_a * a,
                                   xwu8_t l, xwu8_t r,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if ((o >= l) && (o <= r)) {
                        n = o - v;
                        rc = stlxrb(a, (xwu8_t)n);
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
void xwaop__xwu8__rsb(xwu8_a * a,
                      xwu8_t v,
                      xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;

        do {
                o = (xwu8_t)ldaxrb(a);
                n = v - o;
        } while (stlxrb(a, (xwu8_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xwu8__teq_then_rsb(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o == t) {
                        n = v - o;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tne_then_rsb(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o != t) {
                        n = v - o;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tgt_then_rsb(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o > t) {
                        n = v - o;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tge_then_rsb(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o != t) {
                        n = v - o;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tlt_then_rsb(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o < t) {
                        n = v - o;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tle_then_rsb(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o <= t) {
                        n = v - o;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tgtlt_then_rsb(xwu8_a * a,
                                   xwu8_t l, xwu8_t r,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if ((o > l) && (o < r)) {
                        n = v - o;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tgelt_then_rsb(xwu8_a * a,
                                   xwu8_t l, xwu8_t r,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if ((o >= l) && (o < r)) {
                        n = v - o;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tgtle_then_rsb(xwu8_a * a,
                                   xwu8_t l, xwu8_t r,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if ((o > l) && (o <= r)) {
                        n = v - o;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tgele_then_rsb(xwu8_a * a,
                                   xwu8_t l, xwu8_t r,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if ((o >= l) && (o <= r)) {
                        n = v - o;
                        rc = stlxrb(a, (xwu8_t)n);
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
void xwaop__xwu8__or(xwu8_a * a,
                     xwu8_t v,
                     xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;

        do {
                o = (xwu8_t)ldaxrb(a);
                n = o | v;
        } while (stlxrb(a, (xwu8_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xwu8__and(xwu8_a * a,
                      xwu8_t v,
                      xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;

        do {
                o = (xwu8_t)ldaxrb(a);
                n = o & v;
        } while (stlxrb(a, (xwu8_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xwu8__xor(xwu8_a * a,
                      xwu8_t v,
                      xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;

        do {
                o = (xwu8_t)ldaxrb(a);
                n = o ^ v;
        } while (stlxrb(a, (xwu8_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xwu8__c0m(xwu8_a * a,
                      xwu8_t m,
                      xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;

        do {
                o = (xwu8_t)ldaxrb(a);
                n = o & (~m);
        } while (stlxrb(a, (xwu8_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xwu8__t1ma_then_c0m(xwu8_a * a,
                                  xwu8_t m,
                                  xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if ((o & m) == m) {
                        n = o & (~m);
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__t1mo_then_c0m(xwu8_a * a,
                                  xwu8_t m,
                                  xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (o & m) {
                        n = o & (~m);
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__t0ma_then_s1m(xwu8_a * a,
                                  xwu8_t m,
                                  xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (!(o & m)) {
                        n = o | m;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__t0mo_then_s1m(xwu8_a * a,
                                  xwu8_t m,
                                  xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t n;
        xwu8_t o;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if ((o & m) != m) {
                        n = o | m;
                        rc = stlxrb(a, (xwu8_t)n);
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
xwer_t xwaop__xwu8__tst_then_op(xwu8_a * a,
                                xwaop_tst_f tst, void * tst_args,
                                xwaop_op_f op, void * op_args,
                                xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = (xwu8_t)ldaxrb(a);
                if (tst) {
                        if (tst((const void *)&o, tst_args)) {
                                if (op) {
                                        op(&n, (const void *)&o, op_args);
                                        rc = stlxrb(a, (xwu8_t)n);
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
                                rc = stlxrb(a, (xwu8_t)n);
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
