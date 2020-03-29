/**
 * @file
 * @brief 板级描述层：MCU的唤醒
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
#include <xwos/lib/xwaop.h>
#include <xwmd/xwfs/fs.h>
#include <bm/mcuc/init.h>
#include <bm/mcuc/wkup.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      types        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********        function declarations        ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
int mcuc_wkup_open(struct xwfs_node * xwfsnode, struct file * file);

int mcuc_wkup_close(struct xwfs_node * xwfsnode, struct file * file);

static
ssize_t mcuc_wkup_read(struct xwfs_node * xwfsnode,
                       struct file * file,
                       char __user * usbuf,
                       size_t count,
                       loff_t * pos);

static
ssize_t mcuc_wkup_write(struct xwfs_node * xwfsnode,
                        struct file * file,
                        const char __user * usdata,
                        size_t count,
                        loff_t * pos);

static
long mcuc_wkup_ioctl(struct xwfs_node * xwfsnode,
                     struct file * file,
                     unsigned int cmd,
                     unsigned long arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      .data        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct xwfs_operations mcuc_wkup_xwfsops = {
        .open = mcuc_wkup_open,
        .release = mcuc_wkup_close,
        .read = mcuc_wkup_read,
        .write = mcuc_wkup_write,
        .unlocked_ioctl = mcuc_wkup_ioctl,
};

struct xwfs_node * mcuc_wkup;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
int mcuc_wkup_open(struct xwfs_node * xwfsnode, struct file * file)
{
        return (int)mcuc_grab();
}

int mcuc_wkup_close(struct xwfs_node * xwfsnode, struct file * file)
{
        return (int)mcuc_put();
}

static
ssize_t mcuc_wkup_read(struct xwfs_node * xwfsnode,
                       struct file * file,
                       char __user * usbuf,
                       size_t count,
                       loff_t * pos)
{
        return count;
}

static
ssize_t mcuc_wkup_write(struct xwfs_node * xwfsnode,
                        struct file * file,
                        const char __user * usdata,
                        size_t count,
                        loff_t * pos)
{
        return count;
}

static
long mcuc_wkup_ioctl(struct xwfs_node * xwfsnode,
                     struct file * file,
                     unsigned int cmd,
                     unsigned long arg)
{
        return -ENOSYS;
}

xwer_t mcuc_wkup_init(void)
{
        xwer_t rc;
        struct xwfs_node * node;

        rc = xwfs_mknod("wkup", 0660, &mcuc_wkup_xwfsops, NULL,
                        dir_mcuc, &node);
        if (__unlikely(rc < 0)) {
                mcuclogf(ERR, "Fail to mknod(\"wkup\"), rc: %d\n", rc);
                goto err_mknod_wkup;
        }
        mcuc_wkup = node;

        return OK;

err_mknod_wkup:
        return rc;
}

xwer_t mcuc_wkup_exit(void)
{
        xwfs_rmnod(mcuc_wkup);
        mcuc_wkup = NULL;
        return OK;
}
