/**
 * @file
 * @brief 板级描述层：板级初始化
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
#include <xwos/lib/xwlog.h>
#include <xwmd/isc/xwpcp/usi.h>
#include <xwmd/isc/xwscp/usi.h>
#include <bm/mcuc/init.h>
#include <bdl/board.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function declarations    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      .data        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t board_init(void)
{
        xwer_t rc;

        rc = usi_xwpcp_init();
        if (__unlikely(rc < 0)) {
                xwbdlogf(ERR, "Init XWPCP ... [FAILED], rc:%d\n",
                         rc);
                goto err_xwpcp_init;
        }
        xwbdlogf(INFO, "Init XWPCP ... [OK]\n");

        rc = usi_xwscp_init();
        if (__unlikely(rc < 0)) {
                xwbdlogf(ERR, "Init XWSCP ... [FAILED], rc:%d\n",
                         rc);
                goto err_xwscp_init;
        }
        xwbdlogf(INFO, "Init XWSCP ... [OK]\n");

        rc = mcuc_init();
        if (__unlikely(rc < 0)) {
                xwbdlogf(ERR, "Init MCU Controller ... [FAILED], rc:%d\n",
                         rc);
                goto err_mcuc_init;
        }
        xwbdlogf(INFO, "Init MCU Controller ... [OK]\n");
        return OK;

err_mcuc_init:
        usi_xwscp_exit();
err_xwscp_init:
        usi_xwpcp_exit();
err_xwpcp_init:
        return rc;
}

void board_exit(void)
{
        mcuc_exit();
        usi_xwscp_exit();
        usi_xwpcp_exit();
}
