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

#ifndef __xwmd_xwfs_bs_h__
#define __xwmd_xwfs_bs_h__

#include <xwos/standard.h>

extern struct xwfs_dir * xwfs_dir_xwos;
extern struct xwfs_dir * xwfs_dir_mp;
extern struct xwfs_dir * xwfs_dir_sync;
extern struct xwfs_dir * xwfs_dir_lock;
extern struct xwfs_dir * xwfs_dir_xwmd;
extern struct xwfs_dir * xwfs_dir_isc;

xwer_t xwfs_create_skeleton(void);
void xwfs_delete_skeleton(void);

#endif /* xwmd/xwfs/bs.h */
