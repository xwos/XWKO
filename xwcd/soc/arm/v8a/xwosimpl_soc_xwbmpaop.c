/**
 * @file
 * @brief 玄武OS移植实现层：SOC位图原子操作
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
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwaop.h>
#include <armv8a_isa.h>

__xwlib_code
bool xwbmpaop_t1i(xwbmp_a * bmp, xwsq_t idx)
{
        bool ret;
        xwbmp_t value;

        bmp = bmp + XWBOP_BMP(idx);
        value = xwaop_load(xwbmp, bmp, xwmb_modr_consume);
        ret = (bool)(!!(value & XWBOP_BMP_MASK(idx)));
        return ret;
}

__xwlib_code
void xwbmpaop_s1i(xwbmp_a * bmp, xwsq_t idx)
{
        xwsq_t i = XWBOP_BMP(idx);
        xwbmp_t m = XWBOP_BMP_MASK(idx);

        xwaop_s1m(xwbmp, &bmp[i], m, NULL, NULL);
}

__xwlib_code
void xwbmpaop_c0i(xwbmp_a * bmp, xwsq_t idx)
{
        xwsq_t i = XWBOP_BMP(idx);
        xwbmp_t m = XWBOP_BMP_MASK(idx);

        xwaop_c0m(xwbmp, &bmp[i], m, NULL, NULL);
}

__xwlib_code
void xwbmpaop_x1i(xwbmp_a * bmp, xwsq_t idx)
{
        xwsq_t i = XWBOP_BMP(idx);
        xwbmp_t m = XWBOP_BMP_MASK(idx);

        xwaop_x1m(xwbmp, &bmp[i], m, NULL, NULL);
}

__xwlib_code
xwer_t xwbmpaop_t0i_then_s1i(xwbmp_a * bmp, xwsq_t idx)
{
        xwsq_t i = XWBOP_BMP(idx);
        xwbmp_t m = XWBOP_BMP_MASK(idx);
        xwbmp_t ov, nv;
        xwer_t rc;

        do {
                ov = ldaxr(&bmp[i]);
                if (!(ov & m)) {
                        nv = ov | m;
                        rc = stlxr(&bmp[i], nv);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        return rc;
}

__xwlib_code
xwer_t xwbmpaop_t1i_then_c0i(xwbmp_a *bmp, xwsq_t idx)
{
        xwsq_t i = XWBOP_BMP(idx);
        xwbmp_t m = XWBOP_BMP_MASK(idx);
        xwbmp_t ov, nv;
        xwer_t rc;

        do {
                ov = ldaxr(&bmp[i]);
                if (ov & m) {
                        nv = ov & (~m);
                        rc = stlxr(&bmp[i], nv);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        return rc;
}

__xwlib_code
xwssq_t xwbmpaop_fls_then_c0i(xwbmp_a * bmp, xwsz_t num)
{
        xwsz_t total = BITS_TO_XWBMP_T(num);
        xwsq_t i;
        xwbmp_t msk;
        xwbmp_t ov, nv, m;
        xwssq_t pos;
        xwer_t rc;

        do {
                i = total;
                pos = -ENODATA;
                msk = ((xwbmp_t)XWBOP_BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
                if ((xwbmp_t)0 == msk) {
                        msk = ~(xwbmp_t)0;
                }
                do {
                        i--;
                        ov = ldaxr(&bmp[i]) & msk;
                        if (ov) {
                                pos = xwbop_fls(xwbmp_t, ov);
                                break;
                        }
                        msk = ~(xwbmp_t)0;
                } while (i > 0);
                if (pos < 0) {
                        pos = -ENODATA;
                        break;
                } else {
                        m = (1U << (xwsq_t)pos);
                        nv = ov & (xwbmp_t)(~m);
                        pos += (xwssq_t)(i * BITS_PER_XWBMP_T);
                        rc = stlxr(&bmp[i], nv);
                }
        } while (rc);
        return pos;
}

__xwlib_code
xwssq_t xwbmpaop_flz_then_s1i(xwbmp_a * bmp, xwsz_t num)
{
        xwsz_t total = BITS_TO_XWBMP_T(num);
        xwsq_t i;
        xwbmp_t msk;
        xwbmp_t tmp, ov, nv, m;
        xwssq_t pos;
        xwer_t rc;

        do {
                i = total;
                pos = -ENODATA;
                msk = ((xwbmp_t)XWBOP_BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
                if ((xwbmp_t)0 == msk) {
                        msk = ~(xwbmp_t)0;
                }
                do {
                        i--;
                        ov = ldaxr(&bmp[i]);
                        tmp = (xwbmp_t)(~ov) & msk;
                        if (tmp) {
                                pos = xwbop_flz(xwbmp_t, tmp);
                                break;
                        }
                        msk = ~(xwbmp_t)0;
                } while (i > 0);
                if (pos < 0) {
                        pos = -ENODATA;
                        break;
                } else {
                        m = (1U << (xwsq_t)pos);
                        nv = ov | m;
                        pos += (xwssq_t)(i * BITS_PER_XWBMP_T);
                        rc = stlxr(&bmp[i], nv);
                }
        } while (rc);
        return pos;
}

__xwlib_code
xwssq_t xwbmpaop_ffs_then_c0i(xwbmp_a * bmp, xwsz_t num)
{
        xwsz_t total = BITS_TO_XWBMP_T(num);
        xwsq_t i;
        xwbmp_t msk;
        xwbmp_t ov, nv, m;
        xwssq_t pos;
        xwer_t rc;

        do {
                i = 0;
                pos = -ENODATA;
                do {
                        if (i == total - 1) {
                                msk = ((xwbmp_t)XWBOP_BIT(num % BITS_PER_XWBMP_T) -
                                       (xwbmp_t)1);
                                if ((xwbmp_t)0 == msk) {
                                        msk = ~(xwbmp_t)0;
                                }
                        } else {
                                msk = ~(xwbmp_t)0;
                        }
                        ov = ldaxr(&bmp[i]) & msk;
                        if (ov) {
                                pos = xwbop_ffs(xwbmp_t, ov);
                                break;
                        }
                        i++;
                } while (i < total);
                if (pos < 0) {
                        pos = -ENODATA;
                        break;
                } else {
                        m = (1U << (xwsq_t)pos);
                        nv = ov & (xwbmp_t)(~m);
                        pos += (xwssq_t)(i * BITS_PER_XWBMP_T);
                        rc = stlxr(&bmp[i], nv);
                }
        } while (rc);
        return pos;
}

__xwlib_code
xwssq_t xwbmpaop_ffz_then_s1i(xwbmp_a * bmp, xwsz_t num)
{
        xwsz_t total = BITS_TO_XWBMP_T(num);
        xwsq_t i;
        xwbmp_t msk;
        xwbmp_t tmp, ov, nv, m;
        xwssq_t pos;
        xwer_t rc;

        do {
                i = 0;
                pos = -ENODATA;
                do {
                        if (i == (total - 1)) {
                                msk = ((xwbmp_t)XWBOP_BIT(num % BITS_PER_XWBMP_T) -
                                       (xwbmp_t)1);
                                if ((xwbmp_t)0 == msk) {
                                        msk = ~(xwbmp_t)0;
                                }
                        } else {
                                msk = ~(xwbmp_t)0;
                        }
                        ov = ldaxr(&bmp[i]);
                        tmp = (~ov) & msk;
                        if (tmp) {
                                pos = xwbop_ffs(xwbmp_t, tmp);
                                break;
                        }
                        i++;
                } while (i < total);
                if (pos < 0) {
                        pos = -ENODATA;
                        break;
                } else {
                        m = (1U << (xwsq_t)pos);
                        nv = ov | m;
                        pos += (xwssq_t)(i * BITS_PER_XWBMP_T);
                        rc = stlxr(&bmp[i], nv);
                }
        } while (rc);
        return pos;
}
