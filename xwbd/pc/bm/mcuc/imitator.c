/**
 * @file
 * @brief MCU通讯模块：MCU模拟器
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
#include <xwos/lib/bclst.h>
#include <linux/slab.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/semaphore.h>
#include <xwmd/xwfs/fs.h>
#include <bm/mcuc/msgnode.h>
#include <bm/mcuc/init.h>
#include <bm/mcuc/imitator.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      types        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********        function declarations        ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
int mcuc_imitator_open(struct xwfs_node * xwfsnode, struct file * file);

static
int mcuc_imitator_close(struct xwfs_node * xwfsnode, struct file * file);

static
ssize_t mcuc_imitator_write(struct xwfs_node * xwfsnode, struct file * file,
                            const char __user * usdata, size_t count,
                            loff_t * pos);

static
ssize_t mcuc_imitator_read(struct xwfs_node * xwfsnode, struct file * file,
                           char __user * usbuf, size_t count, loff_t * pos);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      .data        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
bool mcuc_imitator = false;
struct xwfs_node * mcuc_imitator_node;

const struct xwfs_operations mcuc_imitator_xwfsops = {
        .open = mcuc_imitator_open,
        .release = mcuc_imitator_close,
        .write = mcuc_imitator_write,
        .read = mcuc_imitator_read,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
void mcuc_imitator_pulish_txmsg(struct mcuc_imitator_info * imi,
                                struct mcuc_imitator_msg * imsg)
{
        xwid_t smrid;

        smrid = xwosal_smr_get_id(&imi->txq.smr);
        xwosal_splk_lock(&imi->txq.lock);
        xwlib_bclst_add_tail(&imi->txq.head, &imsg->cln);
        xwosal_splk_unlock(&imi->txq.lock);
        xwosal_smr_post(smrid);
}

xwer_t mcuc_imitator_get_txmsg(struct mcuc_imitator_info * imi,
                               struct mcuc_imitator_msg ** imsgbuf)
{
        struct mcuc_imitator_msg * imsg;
        xwid_t smrid;
        xwer_t rc;

        smrid = xwosal_smr_get_id(&imi->txq.smr);
        rc = xwosal_smr_wait(smrid);
        if (XWOK == rc) {
                xwosal_splk_lock(&imi->txq.lock);
                imsg = xwlib_bclst_first_entry(&imi->txq.head,
                                               struct mcuc_imitator_msg,
                                               cln);
                xwlib_bclst_del_init(&imsg->cln);
                xwosal_splk_unlock(&imi->txq.lock);
                *imsgbuf = imsg;
        }
        return rc;
}

void mcuc_imitator_pulish_rxmsg(struct mcuc_imitator_info * imi,
                                struct mcuc_imitator_msg * imsg)
{
        xwid_t smrid;

        smrid = xwosal_smr_get_id(&imi->rxq.smr);
        xwosal_splk_lock(&imi->rxq.lock);
        xwlib_bclst_add_tail(&imi->rxq.head, &imsg->cln);
        xwosal_splk_unlock(&imi->rxq.lock);
        xwosal_smr_post(smrid);
}

xwer_t mcuc_imitator_get_rxmsg(struct mcuc_imitator_info * imi,
                               struct mcuc_imitator_msg ** imsgbuf)
{
        struct mcuc_imitator_msg * imsg;
        xwid_t smrid;
        xwer_t rc;

        smrid = xwosal_smr_get_id(&imi->rxq.smr);
        rc = xwosal_smr_wait(smrid);
        if (XWOK == rc) {
                xwosal_splk_lock(&imi->rxq.lock);
                imsg = xwlib_bclst_first_entry(&imi->rxq.head,
                                               struct mcuc_imitator_msg,
                                               cln);
                xwlib_bclst_del_init(&imsg->cln);
                xwosal_splk_unlock(&imi->rxq.lock);
                *imsgbuf = imsg;
        }
        return rc;
}

void mcuc_imitator_free_msg(struct mcuc_imitator_msg * imsg)
{
        kfree(imsg);
}

static
int mcuc_imitator_open(struct xwfs_node * xwfsnode, struct file * file)
{
        return (int)mcuc_grab();
}

static
int mcuc_imitator_close(struct xwfs_node * xwfsnode, struct file * file)
{
        return (int)mcuc_put();
}

static
ssize_t mcuc_imitator_write(struct xwfs_node * xwfsnode,
                            struct file * file,
                            const char __user * usdata,
                            size_t count,
                            loff_t * pos)
{
        struct xwpcp_msg usrmsg;
        struct mcuc_imitator_msg * krnmsg;
        struct mcuc_msgnode_info * portdata;
        xwer_t rc;

        rc = copy_from_user(&usrmsg, usdata, sizeof(struct xwpcp_msg));
        krnmsg = kmalloc(sizeof(struct mcuc_imitator_msg) + usrmsg.size, GFP_KERNEL);
        if (is_err_or_null(krnmsg)) {
                count = -ENOMEM;
                goto err_nomem;
        }
        xwlib_bclst_init_node(&krnmsg->cln);
        krnmsg->msg.port = usrmsg.port;
        krnmsg->msg.size = usrmsg.size;
        krnmsg->msg.text = (void *)&krnmsg[1];
        rc = copy_from_user(krnmsg->msg.text,
                            (const char __user *)usrmsg.text,
                            usrmsg.size);
        portdata = xwfs_node_get_data(mcuc_msgnode[usrmsg.port].xwfsnode);
        mcuc_imitator_pulish_txmsg(&portdata->imitator, krnmsg);
        return count;

err_nomem:
        return count;
}

static
ssize_t mcuc_imitator_read(struct xwfs_node * xwfsnode,
                          struct file * file,
                          char __user * usbuf,
                          size_t count,
                          loff_t * pos)
{
        struct xwpcp_msg usrmsg;
        struct mcuc_msgnode_info * portdata;
        struct mcuc_imitator_msg * imsg;
        ssize_t rdcnt;
        xwer_t rc;

        rc = copy_from_user(&usrmsg, usbuf, sizeof(struct xwpcp_msg));
        portdata = xwfs_node_get_data(mcuc_msgnode[usrmsg.port].xwfsnode);
        rc = mcuc_imitator_get_rxmsg(&portdata->imitator, &imsg);
        if (XWOK == rc) {
                rdcnt = usrmsg.size > imsg->msg.size ? imsg->msg.size : usrmsg.size;
                rc = copy_to_user(usrmsg.text, imsg->msg.text, rdcnt);
                mcuc_imitator_free_msg(imsg);
        } else {
                rdcnt = rc;
        }
        return rdcnt;
}

xwer_t mcuc_imitator_init(void)
{
        xwer_t rc;
        struct xwfs_node * node;

        rc = xwfs_mknod("imitator", 0660, &mcuc_imitator_xwfsops, NULL,
                        dir_mcuc, &node);
        if (__xwcc_unlikely(rc < 0)) {
                mcuclogf(ERR, "Fail to mknod(\"imitator\"), rc: %d\n", rc);
                goto err_mknod_imitator;
        }
        mcuc_imitator_node = node;

        return XWOK;

err_mknod_imitator:
        return rc;
}

xwer_t mcuc_imitator_exit(void)
{
        xwfs_rmnod(mcuc_imitator_node);
        mcuc_imitator_node = NULL;

        return XWOK;
}
