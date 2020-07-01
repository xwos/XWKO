/**
 * @file
 * @brief 位图原子操作汇编库
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
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwaop.h>
#include <armv7a_core.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwlib_code
bool xwbmpaop_t1i(__atomic xwbmp_t * bmp, xwsq_t idx)
{
        bool ret;
        xwbmp_t value;

        bmp = bmp + BIT_BMP(idx);
        value = xwaop_load(xwbmp_t, bmp, xwmb_modr_consume);
        ret = (bool)(!!(value & BIT_BMP_MASK(idx)));
        return ret;
}

__xwlib_code
void xwbmpaop_s1i(__atomic xwbmp_t * bmp, xwsq_t idx)
{
        xwsq_t i = BIT_BMP(idx);
        xwbmp_t m = BIT_BMP_MASK(idx);

        xwaop_s1m(xwbmp_t, &bmp[i], m, NULL, NULL);
}

__xwlib_code
void xwbmpaop_c0i(__atomic xwbmp_t * bmp, xwsq_t idx)
{
        xwsq_t i = BIT_BMP(idx);
        xwbmp_t m = BIT_BMP_MASK(idx);

        xwaop_c0m(xwbmp_t, &bmp[i], m, NULL, NULL);
}

__xwlib_code
void xwbmpaop_x1i(__atomic xwbmp_t * bmp, xwsq_t idx)
{
        xwsq_t i = BIT_BMP(idx);
        xwbmp_t m = BIT_BMP_MASK(idx);

        xwaop_x1m(xwbmp_t, &bmp[i], m, NULL, NULL);
}

__xwos_code
xwer_t xwbmpaop_t0i_then_s1i(__atomic xwbmp_t * bmp, xwsq_t idx)
{
        xwsq_t i = BIT_BMP(idx);
        xwbmp_t m = BIT_BMP_MASK(idx);
        xwbmp_t ov, nv;
        xwer_t rc;

        do {
                ov = ldrex((xwu32_t *)&bmp[i]);
                xwmb_smp_ddb();
                if (!(ov & m)) {
                        nv = ov | m;
                        xwmb_smp_mb();
                        rc = strex((xwu32_t *)&bmp[i], nv)
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        return rc;
}

__xwos_code
xwer_t xwbmpaop_t1i_then_c0i(__atomic xwbmp_t *bmp, xwsq_t idx)
{
        xwsq_t i = BIT_BMP(idx);
        xwbmp_t m = BIT_BMP_MASK(idx);
        xwbmp_t ov, nv;
        xwer_t rc;

        do {
                ov = ldrex((xwu32_t *)&bmp[i]);
                xwmb_smp_ddb();
                if (ov & m) {
                        nv = ov & (~m);
                        xwmb_smp_mb();
                        rc = strex((xwu32_t *)&bmp[i], nv);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        return rc;
}

xwssq_t xwbmpaop_fls_then_c0i(__atomic xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t total = BITS_TO_BMPS(num);
        xwsq_t i;
        xwbmp_t msk;
        xwbmp_t ov, nv, m;
        xwssq_t pos;
        xwer_t rc;

        do {
                i = total;
                pos = -ENODATA;
                msk = ((xwbmp_t)BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
                if ((xwbmp_t)0 == msk) {
                        msk = ~(xwbmp_t)0;
                }
                do {
                        i--;
                        ov = ldrex((xwu32_t *)&bmp[i]) & msk;
                        xwmb_smp_ddb();
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
                        xwmb_smp_mb();
                        rc = strex((xwu32_t *)&bmp[i], nv);
                }
        } while (rc);
        return pos;
}

xwssq_t xwbmpaop_flz_then_s1i(__atomic xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t total = BITS_TO_BMPS(num);
        xwsq_t i;
        xwbmp_t msk;
        xwbmp_t tmp, ov, nv, m;
        xwssq_t pos;
        xwer_t rc;

        do {
                i = total;
                pos = -ENODATA;
                msk = ((xwbmp_t)BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
                if ((xwbmp_t)0 == msk) {
                        msk = ~(xwbmp_t)0;
                }
                do {
                        i--;
                        ov = ldrex((xwu32_t *)&bmp[i]);
                        xwmb_smp_ddb();
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
                        xwmb_smp_mb();
                        rc = strex((xwu32_t *)&bmp[i], nv);
                }
        } while (rc);
        return pos;
}

xwssq_t xwbmpaop_ffs_then_c0i(__atomic xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t total = BITS_TO_BMPS(num);
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
                                msk = ((xwbmp_t)BIT(num % BITS_PER_XWBMP_T) -
                                       (xwbmp_t)1);
                                if ((xwbmp_t)0 == msk) {
                                        msk = ~(xwbmp_t)0;
                                }
                        } else {
                                msk = ~(xwbmp_t)0;
                        }
                        ov = ldrex((xwu32_t *)&bmp[i]) & msk;
                        xwmb_smp_ddb();
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
                        xwmb_smp_mb();
                        rc = strex((xwu32_t *)&bmp[i], nv);
                }
        } while (rc);
        return pos;
}

xwssq_t xwbmpaop_ffz_then_s1i(__atomic xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t total = BITS_TO_BMPS(num);
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
                                msk = ((xwbmp_t)BIT(num % BITS_PER_XWBMP_T) -
                                       (xwbmp_t)1);
                                if ((xwbmp_t)0 == msk) {
                                        msk = ~(xwbmp_t)0;
                                }
                        } else {
                                msk = ~(xwbmp_t)0;
                        }
                        ov = ldrex((xwu32_t *)&bmp[i]);
                        xwmb_smp_ddb();
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
                        xwmb_smp_mb();
                        rc = strex((xwu32_t *)&bmp[i], nv);
                }
        } while (rc);
        return pos;
}
