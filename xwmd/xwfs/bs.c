/**
 * @file
 * @brief XWFS的基本结构
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
#include <xwos/lib/xwlog.h>
#include <xwmd/xwfs/fs.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macro       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       type        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function declarations    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      .data        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwfs_dir * dir_xwos = NULL;

struct xwfs_dir * dir_core = NULL;

struct xwfs_dir * dir_sync = NULL;

struct xwfs_dir * dir_locks = NULL;

struct xwfs_dir * dir_xwmd = NULL;

struct xwfs_dir * dir_isc = NULL;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t xwfs_create_skeleton(void)
{
        xwer_t rc;

        rc = xwfs_mkdir("xwos", NULL, &dir_xwos);
        if (__unlikely(rc < 0)) {
                goto err_mkdir_xwos;
        }

        rc = xwfs_mkdir("core", dir_xwos, &dir_core);
        if (__unlikely(rc < 0)) {
                goto err_mkdir_core;
        }

        rc = xwfs_mkdir("sync", dir_core, &dir_sync);
        if (__unlikely(rc < 0)) {
                goto err_mkdir_sync;
        }

        rc = xwfs_mkdir("locks", dir_core, &dir_locks);
        if (__unlikely(rc < 0)) {
                goto err_mkdir_locks;
        }

        rc = xwfs_mkdir("xwmd", NULL, &dir_xwmd);
        if (__unlikely(rc < 0)) {
                goto err_mkdir_xwmd;
        }

        rc = xwfs_mkdir("isc", dir_xwmd, &dir_isc);
        if (__unlikely(rc < 0)) {
                goto err_mkdir_isc;
        }

        return OK;

err_mkdir_isc:
        xwfs_rmdir(dir_xwmd);
        dir_xwmd = NULL;
err_mkdir_xwmd:
        xwfs_rmdir(dir_locks);
        dir_locks = NULL;
err_mkdir_locks:
        xwfs_rmdir(dir_sync);
        dir_sync = NULL;
err_mkdir_sync:
        xwfs_rmdir(dir_core);
        dir_core = NULL;
err_mkdir_core:
        xwfs_rmdir(dir_xwos);
        dir_xwos = NULL;
err_mkdir_xwos:
        return rc;
}

void xwfs_delete_skeleton(void)
{
        xwfs_rmdir(dir_isc);
        dir_isc = NULL;

        xwfs_rmdir(dir_xwmd);
        dir_xwmd = NULL;

        xwfs_rmdir(dir_locks);
        dir_locks = NULL;

        xwfs_rmdir(dir_sync);
        dir_sync = NULL;

        xwfs_rmdir(dir_core);
        dir_core = NULL;

        xwfs_rmdir(dir_xwos);
        dir_xwos = NULL;
}
