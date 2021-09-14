/**
 * @file
 * @brief 原子操作汇编库: xwu64
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
xwu64_t xwaop__xwu64__load(xwu64_a * a,
                           const enum xwmb_memory_order_em mo)
{
        xwu64_t v;

        switch (mo) {
        case xwmb_modr_relaxed:
                v = *a;
                break;
        case xwmb_modr_consume:
                v = *a;
                xwmb_mp_ddb();
                break;
        case xwmb_modr_acquire:
                v = ldar(a);
                break;
        case xwmb_modr_release:
                v = *a;
                break;
        case xwmb_modr_acq_rel:
                v = ldar(a);
                break;
        case xwmb_modr_seq_cst:
                xwmb_mp_mb();
                v = ldar(a);
                break;
        default:
                v = *a;
                break;
        }
        return v;
}

__xwlib_code
xwu64_t xwaop__xwu64__store(xwu64_a * a,
                            const enum xwmb_memory_order_em mo,
                            xwu64_t v)
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
                stlr(a, v);
                break;
        case xwmb_modr_acq_rel:
                stlr(a, v);
                break;
        case xwmb_modr_seq_cst:
                stlr(a, v);
                xwmb_mp_mb();
                break;
        }
        return v;
}

__xwlib_code
void xwaop__xwu64__read(xwu64_a * a,
                        xwu64_t * ov)
{
        xwu64_t o;
        o = xwaop__xwu64__load(a, xwmb_modr_acquire);
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xwu64__write(xwu64_a * a,
                         xwu64_t v,
                         xwu64_t * ov)
{
        xwu64_t o;

        do {
                o = (xwu64_t)ldaxr(a);
        } while (stlxr(a, (xwu64_t)v));
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xwu64__teq_then_write(xwu64_a * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * ov)
{
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o == t) {
                        rc = stlxr(a, (xwu64_t)v);
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
xwer_t xwaop__xwu64__tne_then_write(xwu64_a * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * ov)
{
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o != t) {
                        rc = stlxr(a, (xwu64_t)v);
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
xwer_t xwaop__xwu64__tgt_then_write(xwu64_a * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * ov)
{
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o > t) {
                        rc = stlxr(a, (xwu64_t)v);
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
xwer_t xwaop__xwu64__tge_then_write(xwu64_a * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * ov)
{
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o >= t) {
                        rc = stlxr(a, (xwu64_t)v);
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
xwer_t xwaop__xwu64__tlt_then_write(xwu64_a * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * ov)
{
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o < t) {
                        rc = stlxr(a, (xwu64_t)v);
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
xwer_t xwaop__xwu64__tle_then_write(xwu64_a * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * ov)
{
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o <= t) {
                        rc = stlxr(a, (xwu64_t)v);
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
xwer_t xwaop__xwu64__tgtlt_then_write(xwu64_a * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * ov)
{
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if ((o > l) && (o < r)) {
                        rc = stlxr(a, (xwu64_t)v);
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
xwer_t xwaop__xwu64__tgelt_then_write(xwu64_a * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * ov)
{
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if ((o >= l) && (o < r)) {
                        rc = stlxr(a, (xwu64_t)v);
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
xwer_t xwaop__xwu64__tgtle_then_write(xwu64_a * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * ov)
{
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if ((o > l) && (o <= r)) {
                        rc = stlxr(a, (xwu64_t)v);
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
xwer_t xwaop__xwu64__tgele_then_write(xwu64_a * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * ov)
{
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if ((o >= l) && (o <= r)) {
                        rc = stlxr(a, (xwu64_t)v);
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
void xwaop__xwu64__add(xwu64_a * a,
                       xwu64_t v,
                       xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;

        do {
                o = (xwu64_t)ldaxr(a);
                n = o + v;
        } while (stlxr(a, (xwu64_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xwu64__teq_then_add(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o == t) {
                        n = o + v;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tne_then_add(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o != t) {
                        n = o + v;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tgt_then_add(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o > t) {
                        n = o + v;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tge_then_add(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o >= t) {
                        n = o + v;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tlt_then_add(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o < t) {
                        n = o + v;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tle_then_add(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o <= t) {
                        n = o + v;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tgtlt_then_add(xwu64_a * a,
                                    xwu64_t l, xwu64_t r,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if ((o > l) && (o < r)) {
                        n = o + v;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tgelt_then_add(xwu64_a * a,
                                    xwu64_t l, xwu64_t r,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if ((o >= l) && (o < r)) {
                        n = o + v;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tgtle_then_add(xwu64_a * a,
                                    xwu64_t l, xwu64_t r,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if ((o > l) && (o <= r)) {
                        n = o + v;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tgele_then_add(xwu64_a * a,
                                    xwu64_t l, xwu64_t r,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if ((o >= l) && (o <= r)) {
                        n = o + v;
                        rc = stlxr(a, (xwu64_t)n);
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
void xwaop__xwu64__sub(xwu64_a * a,
                       xwu64_t v,
                       xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;

        do {
                o = (xwu64_t)ldaxr(a);
                n = o - v;
        } while (stlxr(a, (xwu64_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xwu64__teq_then_sub(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o == t) {
                        n = o - v;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tne_then_sub(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o != t) {
                        n = o - v;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tgt_then_sub(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o > t) {
                        n = o - v;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tge_then_sub(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o >= t) {
                        n = o - v;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tlt_then_sub(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o < t) {
                        n = o - v;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tle_then_sub(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o <= t) {
                        n = o - v;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tgtlt_then_sub(xwu64_a * a,
                                    xwu64_t l, xwu64_t r,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if ((o > l) && (o < r)) {
                        n = o - v;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tgelt_then_sub(xwu64_a * a,
                                    xwu64_t l, xwu64_t r,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if ((o >= l) && (l < r)) {
                        n = o - v;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tgtle_then_sub(xwu64_a * a,
                                    xwu64_t l, xwu64_t r,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if ((o > l) && (o <= r)) {
                        n = o - v;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tgele_then_sub(xwu64_a * a,
                                    xwu64_t l, xwu64_t r,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if ((o >= l) && (o <= r)) {
                        n = o - v;
                        rc = stlxr(a, (xwu64_t)n);
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
void xwaop__xwu64__rsb(xwu64_a * a,
                       xwu64_t v,
                       xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;

        do {
                o = (xwu64_t)ldaxr(a);
                n = v - o;
        } while (stlxr(a, (xwu64_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xwu64__teq_then_rsb(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o == t) {
                        n = v - o;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tne_then_rsb(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o != t) {
                        n = v - o;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tgt_then_rsb(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o > t) {
                        n = v - o;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tge_then_rsb(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o != t) {
                        n = v - o;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tlt_then_rsb(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o < t) {
                        n = v - o;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tle_then_rsb(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o <= t) {
                        n = v - o;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tgtlt_then_rsb(xwu64_a * a,
                                    xwu64_t l, xwu64_t r,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if ((o > l) && (o < r)) {
                        n = v - o;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tgelt_then_rsb(xwu64_a * a,
                                    xwu64_t l, xwu64_t r,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if ((o >= l) && (o < r)) {
                        n = v - o;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tgtle_then_rsb(xwu64_a * a,
                                    xwu64_t l, xwu64_t r,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if ((o > l) && (o <= r)) {
                        n = v - o;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tgele_then_rsb(xwu64_a * a,
                                    xwu64_t l, xwu64_t r,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if ((o >= l) && (o <= r)) {
                        n = v - o;
                        rc = stlxr(a, (xwu64_t)n);
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
void xwaop__xwu64__or(xwu64_a * a,
                      xwu64_t v,
                      xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;

        do {
                o = (xwu64_t)ldaxr(a);
                n = o | v;
        } while (stlxr(a, (xwu64_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xwu64__and(xwu64_a * a,
                       xwu64_t v,
                       xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;

        do {
                o = (xwu64_t)ldaxr(a);
                n = o & v;
        } while (stlxr(a, (xwu64_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xwu64__xor(xwu64_a * a,
                       xwu64_t v,
                       xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;

        do {
                o = (xwu64_t)ldaxr(a);
                n = o ^ v;
        } while (stlxr(a, (xwu64_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xwu64__c0m(xwu64_a * a,
                       xwu64_t m,
                       xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;

        do {
                o = (xwu64_t)ldaxr(a);
                n = o & (~m);
        } while (stlxr(a, (xwu64_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xwu64__t1ma_then_c0m(xwu64_a * a,
                                   xwu64_t m,
                                   xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if ((o & m) == m) {
                        n = o & (~m);
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__t1mo_then_c0m(xwu64_a * a,
                                   xwu64_t m,
                                   xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (o & m) {
                        n = o & (~m);
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__t0ma_then_s1m(xwu64_a * a,
                                   xwu64_t m,
                                   xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (!(o & m)) {
                        n = o | m;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__t0mo_then_s1m(xwu64_a * a,
                                   xwu64_t m,
                                   xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t n;
        xwu64_t o;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if ((o & m) != m) {
                        n = o | m;
                        rc = stlxr(a, (xwu64_t)n);
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
xwer_t xwaop__xwu64__tst_then_op(xwu64_a * a,
                                 xwaop_tst_f tst, void * tst_args,
                                 xwaop_op_f op, void * op_args,
                                 xwu64_t * nv, xwu64_t * ov)
{
        xwu64_t o;
        xwu64_t n;
        xwer_t rc;

        do {
                o = (xwu64_t)ldaxr(a);
                if (tst) {
                        if (tst((const void *)&o, tst_args)) {
                                if (op) {
                                        op(&n, (const void *)&o, op_args);
                                        rc = stlxr(a, (xwu64_t)n);
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
                                rc = stlxr(a, (xwu64_t)n);
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
