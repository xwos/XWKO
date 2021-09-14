/**
 * @file
 * @brief XWFS的基本结构
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
#include <xwos/lib/xwlog.h>
#include <xwmd/xwfs/fs.h>

struct xwfs_dir * xwfs_dir_xwos = NULL;
struct xwfs_dir * xwfs_dir_mp = NULL;
struct xwfs_dir * xwfs_dir_sync = NULL;
struct xwfs_dir * xwfs_dir_lock = NULL;
struct xwfs_dir * xwfs_dir_xwmd = NULL;
struct xwfs_dir * xwfs_dir_isc = NULL;

xwer_t xwfs_create_skeleton(void)
{
        xwer_t rc;

        rc = xwfs_mkdir("xwos", NULL, &xwfs_dir_xwos);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mkxwfs_dir_xwos;
        }

        rc = xwfs_mkdir("mp", xwfs_dir_xwos, &xwfs_dir_mp);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mkxwfs_dir_mp;
        }

        rc = xwfs_mkdir("sync", xwfs_dir_mp, &xwfs_dir_sync);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mkxwfs_dir_sync;
        }

        rc = xwfs_mkdir("lock", xwfs_dir_mp, &xwfs_dir_lock);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mkxwfs_dir_lock;
        }

        rc = xwfs_mkdir("xwmd", NULL, &xwfs_dir_xwmd);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mkxwfs_dir_xwmd;
        }

        rc = xwfs_mkdir("isc", xwfs_dir_xwmd, &xwfs_dir_isc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mkxwfs_dir_isc;
        }

        return XWOK;

err_mkxwfs_dir_isc:
        xwfs_rmdir(xwfs_dir_xwmd);
        xwfs_dir_xwmd = NULL;
err_mkxwfs_dir_xwmd:
        xwfs_rmdir(xwfs_dir_lock);
        xwfs_dir_lock = NULL;
err_mkxwfs_dir_lock:
        xwfs_rmdir(xwfs_dir_sync);
        xwfs_dir_sync = NULL;
err_mkxwfs_dir_sync:
        xwfs_rmdir(xwfs_dir_mp);
        xwfs_dir_mp = NULL;
err_mkxwfs_dir_mp:
        xwfs_rmdir(xwfs_dir_xwos);
        xwfs_dir_xwos = NULL;
err_mkxwfs_dir_xwos:
        return rc;
}

void xwfs_delete_skeleton(void)
{
        xwfs_rmdir(xwfs_dir_isc);
        xwfs_dir_isc = NULL;

        xwfs_rmdir(xwfs_dir_xwmd);
        xwfs_dir_xwmd = NULL;

        xwfs_rmdir(xwfs_dir_lock);
        xwfs_dir_lock = NULL;

        xwfs_rmdir(xwfs_dir_sync);
        xwfs_dir_sync = NULL;

        xwfs_rmdir(xwfs_dir_mp);
        xwfs_dir_mp = NULL;

        xwfs_rmdir(xwfs_dir_xwos);
        xwfs_dir_xwos = NULL;
}
