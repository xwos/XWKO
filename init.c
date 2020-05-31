/**
 * @file
 * @brief XuanWuKO的初始化
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
#include <xwos/init.h>
#include <xwmd/sysfs/core.h>
#include <xwmd/xwfs/fs.h>
#include <bdl/board.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
int XuanWuKo_init(void);

static
void XuanWuKo_exit(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
MODULE_LICENSE("Dual MPL/GPL");
MODULE_AUTHOR("隐星魂(Roy.Sun) <https://www.starsoul.tech>");
MODULE_DESCRIPTION("XuanWuKO");
module_init(XuanWuKo_init);
module_exit(XuanWuKo_exit);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
int XuanWuKo_init(void)
{
        xwer_t rc;

        xwlogf(INFO, "XuanWuKo-V%s\n", XWOS_VER_STR);

        rc = xwos_init();
        if (__unlikely(rc < 0)) {
                xwlogf(ERR, "Init xwos ... [FAILED], rc:%d\n", rc);
                goto err_xwos_init;
        }
        xwlogf(INFO, "Init xwos ... [OK]\n");

        rc = xwsys_init();
        if (__unlikely(rc < 0)) {
                xwlogf(ERR, "Init xwmd/xwsys ... [FAILED], rc:%d\n", rc);
                goto err_xwsys_init;
        }
        xwlogf(INFO, "Init xwmd/xwsys ... [OK]\n");

        rc = xwfs_init();
        if (__unlikely(rc < 0)) {
                xwlogf(ERR, "Init xwmd/xwfs ... [FAILED], rc:%d\n", rc);
                goto err_xwfs_init;
        }
        xwlogf(INFO, "Init xwmd/xwfs ... [OK]\n");

        rc = board_init();
        if (__unlikely(rc < 0)) {
                xwlogf(ERR, "Init board ... [FAILED], rc:%d\n", rc);
                goto err_board_init;
        }
        xwlogf(INFO, "Init board... [OK]\n");

        return OK;

err_board_init:
        xwfs_exit();
err_xwfs_init:
        xwsys_exit();
err_xwsys_init:
        xwos_exit();
err_xwos_init:
        return (int)rc;
}

static
void XuanWuKo_exit(void)
{
	board_exit();
        xwlogf(INFO, "clean board ... [OK]\n");
        xwfs_exit();
        xwlogf(INFO, "clean xwmd/xwfs ... [OK]\n");
        xwsys_exit();
        xwlogf(INFO, "clean xwmd/xwsys ... [OK]\n");
        xwos_exit();
        xwlogf(INFO, "clean xwos ... [OK]\n");
}
