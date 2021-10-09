/**
 * @file
 * @brief 玄武OS通用库：无符号8位原子操作函数库
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
#include <xwos/lib/xwaop.h>

#ifdef __GNUC__
__xwlib_code
xwu8_t xwaop__xwu8__load(xwu8_a * a,
                         const enum xwmb_memory_order_em mo)
{
        xwu8_t v;

        v = __atomic_load_n(a, (int)mo);
        return v;
}

__xwlib_code
xwu8_t xwaop__xwu8__store(xwu8_a * a,
                          const enum xwmb_memory_order_em mo,
                          xwu8_t v)
{
        __atomic_store_n(a, v, (int)mo);
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

        o = __atomic_exchange_n(a, v, __ATOMIC_ACQ_REL);
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

        o = t;
        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, v,
                                                 false,
                                                 __ATOMIC_ACQ_REL,
                                                 __ATOMIC_CONSUME);
        if ((bool)rc) {
                rc = XWOK;
        } else {
                rc = -EACCES;
        }
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
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o != t) {
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, v,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != XWOK);
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
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o > t) {
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, v,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != XWOK);
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
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o >= t) {
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, v,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != XWOK);
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
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o < t) {
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, v,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != XWOK);
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
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o <= t) {
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, v,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != XWOK);
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
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o > l) && (o < r)) {
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, v,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != XWOK);
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
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o >= l) && (o < r)) {
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, v,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != XWOK);
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
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o > l) && (o <= r)) {
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, v,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != XWOK);
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
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o >= l) && (o <= r)) {
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, v,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != XWOK);
        if (ov) {
                *ov = o;
        }
        return rc;
}

/******** ******** add ******** ********/
__xwlib_code
void xwaop__xwu8__add(xwu8_a * a,
                      xwu8_t v,
                      xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;

        o = __atomic_fetch_add(a, v, __ATOMIC_ACQ_REL);
        if (nv) {
                *nv = o + v;
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        o = t;
        n = o + v;
        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                 false,
                                                 __ATOMIC_ACQ_REL,
                                                 __ATOMIC_CONSUME);
        if ((bool)rc) {
                rc = XWOK;
        } else {
                rc = -EACCES;
                n = o;
        }
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o != t) {
                        n = o + v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o > t) {
                        n = o + v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o >= t) {
                        n = o + v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o < t) {
                        n = o + v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o <= t) {
                        n = o + v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o > l) && (o < r)) {
                        n = o + v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o >= l) && (o < r)) {
                        n = o + v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o > l) && (o <= r)) {
                        n = o + v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o >= l) && (o <= r)) {
                        n = o + v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

/******** ******** subtract ******** ********/
__xwlib_code
void xwaop__xwu8__sub(xwu8_a * a,
                      xwu8_t v,
                      xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;

        o = __atomic_fetch_sub(a, v, __ATOMIC_ACQ_REL);
        if (nv) {
                *nv = o - v;
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        o = t;
        n = o - v;
        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                 false,
                                                 __ATOMIC_ACQ_REL,
                                                 __ATOMIC_CONSUME);
        if ((bool)rc) {
                rc = XWOK;
        } else {
                rc = -EACCES;
                n = o;
        }
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o != t) {
                        n = o - v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o > t) {
                        n = o - v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o >= t) {
                        n = o - v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o < t) {
                        n = o - v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o <= t) {
                        n = o - v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o > l) && (o < r)) {
                        n = o - v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o >= l) && (o < r)) {
                        n = o - v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o > l) && (o <= r)) {
                        n = o - v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o >= l) && (o <= r)) {
                        n = o - v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

/******** ******** reverse subtract ******** ********/
__xwlib_code
void xwaop__xwu8__rsb(xwu8_a * a,
                      xwu8_t v,
                      xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
        bool r;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                n = v - o;
                r = __atomic_compare_exchange_n(a, &o, n,
                                                false,
                                                __ATOMIC_ACQ_REL,
                                                __ATOMIC_CONSUME);
        } while (!r);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o == t) {
                        n = v - o;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o != t) {
                        n = v - o;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o > t) {
                        n = v - o;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o >= t) {
                        n = v - o;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o < t) {
                        n = v - o;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o <= t) {
                        n = v - o;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o > l) && (o < r)) {
                        n = v - o;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o >= l) && (o < r)) {
                        n = v - o;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o > l) && (o <= r)) {
                        n = v - o;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o >= l) && (o <= r)) {
                        n = v - o;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

/******** ******** bit operations ******** ********/
__xwlib_code
void xwaop__xwu8__or(xwu8_a * a,
                     xwu8_t v,
                     xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;

        o = __atomic_fetch_or(a, v, __ATOMIC_ACQ_REL);
        if (nv) {
                *nv = o | v;
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
        xwu8_t o;

        o = __atomic_fetch_and(a, v, __ATOMIC_ACQ_REL);
        if (nv) {
                *nv = o & v;
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
        xwu8_t o;

        o = __atomic_fetch_xor(a, v, __ATOMIC_ACQ_REL);
        if (nv) {
                *nv = o ^ v;
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
        xwu8_t o;

        o = __atomic_fetch_and(a, ~m, __ATOMIC_ACQ_REL);
        if (nv) {
                *nv = o & (~m);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o & m) == m) {
                        n = o & (~m);
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o & m) {
                        n = o & (~m);
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (!(o & m)) {
                        n = o | m;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
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
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o & m) != m) {
                        n = o | m;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

/******** ******** test and operation ******** ********/
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
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (tst) {
                        rc = tst((const void *)&o, tst_args);
                        if (XWOK == rc) {
                                if (op) {
                                        op(&n, (const void *)&o, op_args);
                                        rc = (xwer_t)__atomic_compare_exchange_n(
                                                a, &o, n,
                                                false,
                                                __ATOMIC_ACQ_REL,
                                                __ATOMIC_CONSUME);
                                } else {
                                        n = o;
                                        break;
                                }
                        } else {
                                n = o;
                                break;
                        }
                } else {
                        if (op) {
                                op(&n, (const void *)&o, op_args);
                                rc = (xwer_t)__atomic_compare_exchange_n(
                                        a, &o, n,
                                        false,
                                        __ATOMIC_ACQ_REL,
                                        __ATOMIC_CONSUME);
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

#endif
