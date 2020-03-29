/**
 * @file
 * @brief XWOS通用库：位图原子操作
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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
#include <xwos/lib/xwbmpaop.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 原子操作：测试位图中的某位是否被置位
 * @param bmp: (I) 位图的起始地址指针
 * @param n: (I) 被测试的位的序号
 */
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

/**
 * @brief 原子操作：将位图中某位置1
 * @param bmp: (I) 位图的起始地址指针
 * @param n: (I) 被置1的位的序号
 */
__xwlib_code
void xwbmpaop_s1i(__atomic xwbmp_t * bmp, xwsq_t idx)
{
        xwsq_t i = BIT_BMP(idx);
        xwbmp_t m = BIT_BMP_MASK(idx);

        xwaop_s1m(xwbmp_t, &bmp[i], m, NULL, NULL);
}

/**
 * @brief 原子操作：将位图中某位清0
 * @param bmp: (I) 位图的起始地址指针
 * @param n: (I) 被清0的位的序号
 */
__xwlib_code
void xwbmpaop_c0i(__atomic xwbmp_t * bmp, xwsq_t idx)
{
        xwsq_t i = BIT_BMP(idx);
        xwbmp_t m = BIT_BMP_MASK(idx);

        xwaop_c0m(xwbmp_t, &bmp[i], m, NULL, NULL);
}

/**
 * @brief 原子操作：将位图中某位翻转
 * @param bmp: (I) 位图的起始地址指针
 * @param n: (I) 被翻转的位的序号
 */
__xwlib_code
void xwbmpaop_x1i(__atomic xwbmp_t * bmp, xwsq_t idx)
{
        xwsq_t i = BIT_BMP(idx);
        xwbmp_t m = BIT_BMP_MASK(idx);

        xwaop_x1m(xwbmp_t, &bmp[i], m, NULL, NULL);
}

/**
 * @brief 原子操作：测试位图中某位是否为0。如果是，就将它置1。
 * @param bmp: (I) 位图的起始地址指针
 * @param i: (I) 被测试位的序号
 * @retval OK: OK
 * @retval -EACCES: 测试失败
 */
__xwlib_code
xwer_t xwbmpaop_t0i_then_s1i(__atomic xwbmp_t * bmp, xwsq_t idx)
{
        xwsq_t i = BIT_BMP(idx);
        xwbmp_t m = BIT_BMP_MASK(idx);
        xwbmp_t o, n;
        xwer_t rc;

        do {
                o = xwaop_load(xwbmp_t, &bmp[i], xwmb_modr_consume);
                if (!(o & m)) {
                        n = o | m;
                        rc = OK;
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (xwaop_teq_then_write(xwbmp_t, &bmp[i], o, n, NULL));
        return rc;
}

/**
 * @brief 原子操作：测试位图中某位是否为1。如果是，就将它清0。
 * @param bmp: (I) 位图的起始地址指针
 * @param n: (I) 被测试位的序号
 * @retval OK: OK
 * @retval -EACCES: 测试失败
 */
__xwlib_code
xwer_t xwbmpaop_t1i_then_c0i(__atomic xwbmp_t * bmp, xwsq_t idx)
{
        xwsq_t i = BIT_BMP(idx);
        xwbmp_t m = BIT_BMP_MASK(idx);
        xwbmp_t o, n;
        xwer_t rc;

        do {
                o = xwaop_load(xwbmp_t, &bmp[i], xwmb_modr_consume);
                if (o & m) {
                        n = o & (xwbmp_t)(~m);
                        rc = OK;
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (xwaop_teq_then_write(xwbmp_t, &bmp[i], o, n, NULL));
        return rc;
}

/**
 * @brief 原子操作：从最高位起找到位图中第一个为1的位并将它清0
 * @param bmp: (I) 位图的起始地址指针
 * @param num: (I) 位图中总的位数
 * @retval >=0: 位的序号
 * @retval -ENODATA: 没有任何一个位为1
 */
__xwlib_code
xwssq_t xwbmpaop_fls_then_c0i(__atomic xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t total = BITS_TO_BMPS(num);
        xwsq_t i;
        xwbmp_t msk;
        xwbmp_t o, n, m;
        xwssq_t pos;

        do {
                i = total;
                pos = -ENODATA;
                msk = ((xwbmp_t)BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
                if ((xwbmp_t)0 == msk) {
                        msk = (xwbmp_t)(~(xwbmp_t)0);
                }
                do {
                        i--;
                        o = xwaop_load(xwbmp_t, &bmp[i], xwmb_modr_consume);
                        o &= msk;
                        if (o) {
                                pos = xwbop_fls(xwbmp_t, o);
                                break;
                        }
                        msk = (xwbmp_t)(~(xwbmp_t)0);
                } while (i > 0);
                if (pos < 0) {
                        pos = -ENODATA;
                        break;
                } else {
                        m = ((xwbmp_t)1 << pos);
                        n = o & (xwbmp_t)(~m);
                        pos += (xwssq_t)(i * BITS_PER_XWBMP_T);
                }
        } while (xwaop_teq_then_write(xwbmp_t, &bmp[i], o, n, NULL));
        return pos;
}

/**
 * @brief 原子操作：从最高位起找到位图中第一个为0的位并将它置1
 * @param bmp: (I) 位图的起始地址指针
 * @param num: (I) 位图中总的位数
 * @retval >=0: 位的序号
 * @retval -ENODATA: 没有任何一个位为0
 */
__xwlib_code
xwssq_t xwbmpaop_flz_then_s1i(__atomic xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t total = BITS_TO_BMPS(num);
        xwsq_t i;
        xwbmp_t msk;
        xwbmp_t tmp, o, n, m;
        xwssq_t pos;

        do {
                i = total;
                pos = -ENODATA;
                msk = ((xwbmp_t)BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
                if ((xwbmp_t)0 == msk) {
                        msk = (xwbmp_t)(~(xwbmp_t)0);
                }
                do {
                        i--;
                        o = xwaop_load(xwbmp_t, &bmp[i], xwmb_modr_consume);
                        tmp = (xwbmp_t)(~o) & msk;
                        if (tmp) {
                                pos = xwbop_fls(xwbmp_t, tmp);
                                break;
                        }
                        msk = (xwbmp_t)(~(xwbmp_t)0);
                } while (i > 0);
                if (pos < 0) {
                        pos = -ENODATA;
                        break;
                } else {
                        m = ((xwbmp_t)1 << pos);
                        n = o | m;
                        pos += (xwssq_t)(i * BITS_PER_XWBMP_T);
                }
        } while (xwaop_teq_then_write(xwbmp_t, &bmp[i], o, n, NULL));
        return pos;
}

/**
 * @brief 原子操作：从最低位起找到位图中第一个为1的位并将它清0
 * @param bmp: (I) 位图的起始地址指针
 * @param num: (I) 位图中总的位数
 * @retval >=0: 位的序号
 * @retval -ENODATA: 没有任何一个位为1
 */
__xwlib_code
xwssq_t xwbmpaop_ffs_then_c0i(__atomic xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t total = BITS_TO_BMPS(num);
        xwsq_t i;
        xwbmp_t msk;
        xwbmp_t o, n, m;
        xwssq_t pos;

        do {
                i = 0;
                pos = -ENODATA;
                do {
                        if (i == total - 1) {
                                msk = ((xwbmp_t)BIT(num % BITS_PER_XWBMP_T) -
                                       (xwbmp_t)1);
                                if ((xwbmp_t)0 == msk) {
                                        msk = (xwbmp_t)(~(xwbmp_t)0);
                                }
                        } else {
                                msk = (xwbmp_t)(~(xwbmp_t)0);
                        }
                        o = xwaop_load(xwbmp_t, &bmp[i], xwmb_modr_consume);
                        o &= msk;
                        if (o) {
                                pos = xwbop_ffs(xwbmp_t, o);
                                break;
                        }
                        i++;
                } while (i < total);
                if (pos < 0) {
                        pos = -ENODATA;
                        break;
                } else {
                        m = ((xwbmp_t)1 << pos);
                        n = o & (xwbmp_t)(~m);
                        pos += (xwssq_t)(i * BITS_PER_XWBMP_T);
                }
        } while (xwaop_teq_then_write(xwbmp_t, &bmp[i], o, n, NULL));
        return pos;
}

/**
 * @brief 原子操作：从最低位起找到位图中第一个为0的位并将它置1
 * @param bmp: (I) 位图的起始地址指针
 * @param num: (I) 位图中总的位数
 * @retval >=0: 位的序号
 * @retval -ENODATA: 没有任何一个位为0
 */
__xwlib_code
xwssq_t xwbmpaop_ffz_then_s1i(__atomic xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t total = BITS_TO_BMPS(num);
        xwsq_t i;
        xwbmp_t msk;
        xwbmp_t tmp, n, o, m;
        xwssq_t pos;

        do {
                i = 0;
                pos = -ENODATA;
                do {
                        if (i == (total - 1)) {
                                msk = ((xwbmp_t)BIT(num % BITS_PER_XWBMP_T) -
                                       (xwbmp_t)1);
                                if ((xwbmp_t)0 == msk) {
                                        msk = (xwbmp_t)(~(xwbmp_t)0);
                                }
                        } else {
                                msk = (xwbmp_t)(~(xwbmp_t)0);
                        }
                        o = xwaop_load(xwbmp_t, &bmp[i], xwmb_modr_consume);
                        tmp = (xwbmp_t)(~o) & msk;
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
                        m = ((xwbmp_t)1 << pos);
                        n = o | m;
                        pos += (xwssq_t)(i * BITS_PER_XWBMP_T);
                }
        } while (xwaop_teq_then_write(xwbmp_t, &bmp[i], o, n, NULL));
        return pos;
}
