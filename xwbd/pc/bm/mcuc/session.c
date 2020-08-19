/**
 * @file
 * @brief MCU通讯模块：与MCU建立的会话
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
#include <xwos/lib/xwlog.h>
#include <xwos/lib/xwaop.h>
#include <xwmd/xwfs/fs.h>
#include <bm/mcuc/init.h>
#include <bm/mcuc/session.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      types        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********        function declarations        ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
int mcuc_session_open(struct xwfs_node * xwfsnode, struct file * file);

int mcuc_session_close(struct xwfs_node * xwfsnode, struct file * file);

static
ssize_t mcuc_session_read(struct xwfs_node * xwfsnode, struct file * file,
                          char __user * usbuf, size_t count, loff_t * pos);

static
ssize_t mcuc_session_write(struct xwfs_node * xwfsnode, struct file * file,
                           const char __user * usdata,
                           size_t count, loff_t * pos);

static
long mcuc_session_ioctl(struct xwfs_node * xwfsnode, struct file * file,
                       unsigned int cmd, unsigned long arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      .data        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct xwfs_operations mcuc_session_xwfsops = {
        .open = mcuc_session_open,
        .release = mcuc_session_close,
        .read = mcuc_session_read,
        .write = mcuc_session_write,
        .unlocked_ioctl = mcuc_session_ioctl,
};

struct xwfs_node * mcuc_session;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
int mcuc_session_open(struct xwfs_node * xwfsnode, struct file * file)
{
        return (int)mcuc_grab();
}

int mcuc_session_close(struct xwfs_node * xwfsnode, struct file * file)
{
        return (int)mcuc_put();
}

static
ssize_t mcuc_session_read(struct xwfs_node * xwfsnode, struct file * file,
                          char __user * usbuf, size_t count, loff_t * pos)
{
        return count;
}

static
ssize_t mcuc_session_write(struct xwfs_node * xwfsnode, struct file * file,
                           const char __user * usdata, size_t count, loff_t * pos)
{
        return count;
}

static
long mcuc_session_ioctl(struct xwfs_node * xwfsnode, struct file * file,
                        unsigned int cmd, unsigned long arg)
{
        return -ENOSYS;
}

xwer_t mcuc_session_init(void)
{
        xwer_t rc;
        struct xwfs_node * node;

        rc = xwfs_mknod("session", 0660, &mcuc_session_xwfsops, NULL,
                        dir_mcuc, &node);
        if (__xwcc_unlikely(rc < 0)) {
                mcuclogf(ERR, "Fail to mknod(\"session\"), rc: %d\n", rc);
                goto err_mknod_session;
        }
        mcuc_session = node;

        return XWOK;

err_mknod_session:
        return rc;
}

xwer_t mcuc_session_exit(void)
{
        xwfs_rmnod(mcuc_session);
        mcuc_session = NULL;
        return XWOK;
}
