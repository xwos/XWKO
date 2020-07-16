/**
 * @file
 * @brief MCU通讯模块：用户态文件节点
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > Licensed under the MIT License (the "License");
 * > You may obtain a copy of the License at
 * >
 * >         https://mit-license.org
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
#include <linux/slab.h>
#include <xwmd/xwfs/fs.h>
#include <xwmd/isc/xwpcp/usi.h>
#include <bm/mcuc/imitator.h>
#include <bm/mcuc/init.h>
#include <bm/mcuc/msgnode.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      types        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      types        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********        function declarations        ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
int mcuc_msgnode_open(struct xwfs_node * xwfsnode, struct file * file);

int mcuc_msgnode_close(struct xwfs_node * xwfsnode, struct file * file);

static
ssize_t mcuc_msgnode_read(struct xwfs_node * xwfsnode,
                          struct file * file,
                          char __user * usbuf,
                          size_t count,
                          loff_t * pos);

static
ssize_t mcuc_msgnode_write(struct xwfs_node * xwfsnode,
                           struct file * file,
                           const char __user * usdata,
                           size_t count,
                           loff_t * pos);

static
long mcuc_msgnode_ioctl(struct xwfs_node * xwfsnode, struct file * file,
                        unsigned int cmd, unsigned long arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      .data        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct xwfs_operations mcuc_msgnode_xwfsops = {
        .open = mcuc_msgnode_open,
        .release = mcuc_msgnode_close,
        .read = mcuc_msgnode_read,
        .write = mcuc_msgnode_write,
        .unlocked_ioctl = mcuc_msgnode_ioctl,
};

struct mcuc_msgnode_info mcuc_msgnode[MCUC_MSGNODE_NUM] = {
        [MCUC_MSGNODE_SYS] = {
                .attr = {
                        .port = MCUC_MSGNODE_SYS,
                        .prio = 2,
                },
                .xwfsnode = NULL,
        },
        [MCUC_MSGNODE_PWR] = {
                .attr = {
                        .port = MCUC_MSGNODE_PWR,
                        .prio = 3,
                },
                .xwfsnode = NULL,
        },
        [MCUC_MSGNODE_VHC] = {
                .attr = {
                        .port = MCUC_MSGNODE_VHC,
                        .prio = 2,
                },
                .xwfsnode = NULL,
        },
        [MCUC_MSGNODE_DIAG] = {
                .attr = {
                        .port = MCUC_MSGNODE_DIAG,
                        .prio = 1,
                },
                .xwfsnode = NULL,
        },
        [MCUC_MSGNODE_MISC] = {
                .attr = {
                        .port = MCUC_MSGNODE_MISC,
                        .prio = 3,
                },
                .xwfsnode = NULL,
        },
        [MCUC_MSGNODE_SENSOR] = {
                .attr = {
                        .port = MCUC_MSGNODE_SENSOR,
                        .prio = 1,
                },
                .xwfsnode = NULL,
        },
        [MCUC_MSGNODE_LOG] = {
                .attr = {
                        .port = MCUC_MSGNODE_LOG,
                        .prio = 0,
                },
                .xwfsnode = NULL,
        },
        [MCUC_MSGNODE_MFG] = {
                .attr = {
                        .port = MCUC_MSGNODE_MFG,
                        .prio = 1,
                },
                .xwfsnode = NULL,
        },
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
int mcuc_msgnode_open(struct xwfs_node * xwfsnode, struct file * file)
{
        return (int)mcuc_grab();
}

int mcuc_msgnode_close(struct xwfs_node * xwfsnode, struct file * file)
{
        return (int)mcuc_put();
}

static
ssize_t mcuc_msgnode_read(struct xwfs_node * xwfsnode,
                          struct file * file,
                          char __user * usbuf,
                          size_t count,
                          loff_t * pos)
{
        struct mcuc_msgnode_info * portdata;
        ssize_t rdcnt;
        xwer_t rc;

        portdata = xwfs_node_get_data(xwfsnode);
        if (mcuc_imitator) {
                struct mcuc_imitator_msg * imsg;

                rc = mcuc_imitator_get_txmsg(&portdata->imitator, &imsg);
                if (XWOK == rc) {
                        rdcnt = count > imsg->msg.size ? imsg->msg.size : count;
                        rc = copy_to_user(usbuf, imsg->msg.text, rdcnt);
                        mcuc_imitator_free_msg(imsg);
                } else {
                        rdcnt = rc;
                }
        } else if (USI_XWPCP_STATE_START == usi_xwpcp_get_state()) {
                struct xwpcp_msg msg;
                xwtm_t time;
                uint8_t buf[count];

                msg.port = portdata->attr.port;
                msg.size = count;
                msg.text = buf;
                time = XWTM_MAX;
                rc = xwpcp_rx(&usi_xwpcp, &msg, &time);
                if (XWOK == rc) {
                        rdcnt = msg.size;
                        rc = copy_to_user(usbuf, buf, rdcnt);
                } else {
                        rdcnt = (ssize_t)rc;
                }
        } else {
                rdcnt = -ENOTCONN;
        }
        return rdcnt;
}

static
ssize_t mcuc_msgnode_write(struct xwfs_node * xwfsnode,
                           struct file * file,
                           const char __user * usdata,
                           size_t count,
                           loff_t * pos)
{
        struct mcuc_msgnode_info * portdata;
	xwsq_t state;
        xwer_t rc;

        portdata = xwfs_node_get_data(xwfsnode);
	state = usi_xwpcp_get_state();
	mcuclogf(INFO, "TX data count:%d, imitator:%s, XWPCP state:%d\n",
                 count, mcuc_imitator? "true" : "false", state);
        if (mcuc_imitator) {
                struct mcuc_imitator_msg * imsg;

                imsg = kmalloc(sizeof(struct mcuc_imitator_msg) + count, GFP_KERNEL);
                if (is_err_or_null(imsg)) {
                        count = -ENOMEM;
                } else {
                        xwlib_bclst_init_node(&imsg->cln);
                        imsg->msg.port = portdata->attr.port;
                        imsg->msg.size = count;
                        imsg->msg.text = (void *)&imsg[1];
                        rc = copy_from_user(imsg->msg.text, usdata, count);
                        mcuc_imitator_pulish_rxmsg(&portdata->imitator, imsg);
                }
        } else if (USI_XWPCP_STATE_START == state) {
                xwu8_t data[count];
                struct xwpcp_msg msg;
                xwtm_t time;

                rc = copy_from_user(data, usdata, count);
                msg.port = portdata->attr.port;
                msg.size = count;
                msg.text = data;
                time = XWTM_MAX;
                rc = xwpcp_tx(&usi_xwpcp, &msg, portdata->attr.prio, &time);
                if (__unlikely(rc < 0)) {
                        count = (ssize_t)rc;
                }
        } else {
                count = -ENOTCONN;
        }
        return count;
}

static
long mcuc_msgnode_ioctl(struct xwfs_node * xwfsnode, struct file * file,
                        unsigned int cmd, unsigned long arg)
{
        return -ENOSYS;
}

xwer_t mcuc_msgnode_init(void)
{
        xwer_t rc;
        struct xwfs_node * node;

        rc = xwfs_mknod("sys", 0666, &mcuc_msgnode_xwfsops,
                        (void *)&mcuc_msgnode[MCUC_MSGNODE_SYS],
                        dir_mcuc, &node);
        if (__unlikely(rc < 0)) {
                mcuclogf(ERR, "Fail to mknod(\"sys\"), rc: %d\n", rc);
                goto err_mknod_sys;
        }
        mcuc_msgnode[MCUC_MSGNODE_SYS].xwfsnode = node;
        xwosal_smr_init(&mcuc_msgnode[MCUC_MSGNODE_SYS].imitator.txq.smr,
                        0, XWSSQ_MAX);
        xwlib_bclst_init_head(&mcuc_msgnode[MCUC_MSGNODE_SYS].imitator.txq.head);
        xwosal_splk_init(&mcuc_msgnode[MCUC_MSGNODE_SYS].imitator.txq.lock);
        xwosal_smr_init(&mcuc_msgnode[MCUC_MSGNODE_SYS].imitator.rxq.smr,
                        0, XWSSQ_MAX);
        xwlib_bclst_init_head(&mcuc_msgnode[MCUC_MSGNODE_SYS].imitator.rxq.head);
        xwosal_splk_init(&mcuc_msgnode[MCUC_MSGNODE_SYS].imitator.rxq.lock);

        rc = xwfs_mknod("pwr", 0666, &mcuc_msgnode_xwfsops,
                        (void *)&mcuc_msgnode[MCUC_MSGNODE_PWR],
                        dir_mcuc, &node);
        if (__unlikely(rc < 0)) {
                mcuclogf(ERR, "Fail to mknod(\"pwr\"), rc: %d\n", rc);
                goto err_mknod_pwr;
        }
        mcuc_msgnode[MCUC_MSGNODE_PWR].xwfsnode = node;
        xwosal_smr_init(&mcuc_msgnode[MCUC_MSGNODE_PWR].imitator.txq.smr,
                        0, XWSSQ_MAX);
        xwlib_bclst_init_head(&mcuc_msgnode[MCUC_MSGNODE_PWR].imitator.txq.head);
        xwosal_splk_init(&mcuc_msgnode[MCUC_MSGNODE_PWR].imitator.txq.lock);
        xwosal_smr_init(&mcuc_msgnode[MCUC_MSGNODE_PWR].imitator.rxq.smr,
                        0, XWSSQ_MAX);
        xwlib_bclst_init_head(&mcuc_msgnode[MCUC_MSGNODE_PWR].imitator.rxq.head);
        xwosal_splk_init(&mcuc_msgnode[MCUC_MSGNODE_PWR].imitator.rxq.lock);

        rc = xwfs_mknod("vhc", 0666, &mcuc_msgnode_xwfsops,
                        (void *)&mcuc_msgnode[MCUC_MSGNODE_VHC],
                        dir_mcuc, &node);
        if (__unlikely(rc < 0)) {
                mcuclogf(ERR, "Fail to mknod(\"vhc\"), rc: %d\n", rc);
                goto err_mknod_vhc;
        }
        mcuc_msgnode[MCUC_MSGNODE_VHC].xwfsnode = node;
        xwosal_smr_init(&mcuc_msgnode[MCUC_MSGNODE_VHC].imitator.txq.smr,
                        0, XWSSQ_MAX);
        xwlib_bclst_init_head(&mcuc_msgnode[MCUC_MSGNODE_VHC].imitator.txq.head);
        xwosal_splk_init(&mcuc_msgnode[MCUC_MSGNODE_VHC].imitator.txq.lock);
        xwosal_smr_init(&mcuc_msgnode[MCUC_MSGNODE_VHC].imitator.rxq.smr,
                        0, XWSSQ_MAX);
        xwlib_bclst_init_head(&mcuc_msgnode[MCUC_MSGNODE_VHC].imitator.rxq.head);
        xwosal_splk_init(&mcuc_msgnode[MCUC_MSGNODE_VHC].imitator.rxq.lock);

        rc = xwfs_mknod("diag", 0666, &mcuc_msgnode_xwfsops,
                        (void *)&mcuc_msgnode[MCUC_MSGNODE_DIAG],
                        dir_mcuc, &node);
        if (__unlikely(rc < 0)) {
                mcuclogf(ERR, "Fail to mknod(\"diag\"), rc: %d\n", rc);
                goto err_mknod_diag;
        }
        mcuc_msgnode[MCUC_MSGNODE_DIAG].xwfsnode = node;
        xwosal_smr_init(&mcuc_msgnode[MCUC_MSGNODE_DIAG].imitator.txq.smr,
                        0, XWSSQ_MAX);
        xwlib_bclst_init_head(&mcuc_msgnode[MCUC_MSGNODE_DIAG].imitator.txq.head);
        xwosal_splk_init(&mcuc_msgnode[MCUC_MSGNODE_DIAG].imitator.txq.lock);
        xwosal_smr_init(&mcuc_msgnode[MCUC_MSGNODE_DIAG].imitator.rxq.smr,
                        0, XWSSQ_MAX);
        xwlib_bclst_init_head(&mcuc_msgnode[MCUC_MSGNODE_DIAG].imitator.rxq.head);
        xwosal_splk_init(&mcuc_msgnode[MCUC_MSGNODE_DIAG].imitator.rxq.lock);

        rc = xwfs_mknod("misc", 0666, &mcuc_msgnode_xwfsops,
                        (void *)&mcuc_msgnode[MCUC_MSGNODE_MISC],
                        dir_mcuc, &node);
        if (__unlikely(rc < 0)) {
                mcuclogf(ERR, "Fail to mknod(\"button\"), rc: %d\n", rc);
                goto err_mknod_button;
        }
        mcuc_msgnode[MCUC_MSGNODE_MISC].xwfsnode = node;
        xwosal_smr_init(&mcuc_msgnode[MCUC_MSGNODE_MISC].imitator.txq.smr,
                        0, XWSSQ_MAX);
        xwlib_bclst_init_head(&mcuc_msgnode[MCUC_MSGNODE_MISC].imitator.txq.head);
        xwosal_splk_init(&mcuc_msgnode[MCUC_MSGNODE_MISC].imitator.txq.lock);
        xwosal_smr_init(&mcuc_msgnode[MCUC_MSGNODE_MISC].imitator.rxq.smr,
                        0, XWSSQ_MAX);
        xwlib_bclst_init_head(&mcuc_msgnode[MCUC_MSGNODE_MISC].imitator.rxq.head);
        xwosal_splk_init(&mcuc_msgnode[MCUC_MSGNODE_MISC].imitator.rxq.lock);

        rc = xwfs_mknod("sensor", 0666, &mcuc_msgnode_xwfsops,
                        (void *)&mcuc_msgnode[MCUC_MSGNODE_SENSOR],
                        dir_mcuc, &node);
        if (__unlikely(rc < 0)) {
                mcuclogf(ERR, "Fail to mknod(\"sensor\"), rc: %d\n", rc);
                goto err_mknod_sensor;
        }
        mcuc_msgnode[MCUC_MSGNODE_SENSOR].xwfsnode = node;
        xwosal_smr_init(&mcuc_msgnode[MCUC_MSGNODE_SENSOR].imitator.txq.smr,
                        0, XWSSQ_MAX);
        xwlib_bclst_init_head(&mcuc_msgnode[MCUC_MSGNODE_SENSOR].imitator.txq.head);
        xwosal_splk_init(&mcuc_msgnode[MCUC_MSGNODE_SENSOR].imitator.txq.lock);
        xwosal_smr_init(&mcuc_msgnode[MCUC_MSGNODE_SENSOR].imitator.rxq.smr,
                        0, XWSSQ_MAX);
        xwlib_bclst_init_head(&mcuc_msgnode[MCUC_MSGNODE_SENSOR].imitator.rxq.head);
        xwosal_splk_init(&mcuc_msgnode[MCUC_MSGNODE_SENSOR].imitator.rxq.lock);

        rc = xwfs_mknod("log", 0666, &mcuc_msgnode_xwfsops,
                        (void *)&mcuc_msgnode[MCUC_MSGNODE_LOG],
                        dir_mcuc, &node);
        if (__unlikely(rc < 0)) {
                mcuclogf(ERR, "Fail to mknod(\"log\"), rc: %d\n", rc);
                goto err_mknod_log;
        }
        mcuc_msgnode[MCUC_MSGNODE_LOG].xwfsnode = node;
        xwosal_smr_init(&mcuc_msgnode[MCUC_MSGNODE_LOG].imitator.txq.smr,
                        0, XWSSQ_MAX);
        xwlib_bclst_init_head(&mcuc_msgnode[MCUC_MSGNODE_LOG].imitator.txq.head);
        xwosal_splk_init(&mcuc_msgnode[MCUC_MSGNODE_LOG].imitator.txq.lock);
        xwosal_smr_init(&mcuc_msgnode[MCUC_MSGNODE_LOG].imitator.rxq.smr,
                        0, XWSSQ_MAX);
        xwlib_bclst_init_head(&mcuc_msgnode[MCUC_MSGNODE_LOG].imitator.rxq.head);
        xwosal_splk_init(&mcuc_msgnode[MCUC_MSGNODE_LOG].imitator.rxq.lock);

        rc = xwfs_mknod("mfg", 0666, &mcuc_msgnode_xwfsops,
                        (void *)&mcuc_msgnode[MCUC_MSGNODE_MFG],
                        dir_mcuc, &node);
        if (__unlikely(rc < 0)) {
                mcuclogf(ERR, "Fail to mknod(\"mfg\"), rc: %d\n", rc);
                goto err_mknod_mfg;
        }
        mcuc_msgnode[MCUC_MSGNODE_MFG].xwfsnode = node;
        xwosal_smr_init(&mcuc_msgnode[MCUC_MSGNODE_MFG].imitator.txq.smr,
                        0, XWSSQ_MAX);
        xwlib_bclst_init_head(&mcuc_msgnode[MCUC_MSGNODE_MFG].imitator.txq.head);
        xwosal_splk_init(&mcuc_msgnode[MCUC_MSGNODE_MFG].imitator.txq.lock);
        xwosal_smr_init(&mcuc_msgnode[MCUC_MSGNODE_MFG].imitator.rxq.smr,
                        0, XWSSQ_MAX);
        xwlib_bclst_init_head(&mcuc_msgnode[MCUC_MSGNODE_MFG].imitator.rxq.head);
        xwosal_splk_init(&mcuc_msgnode[MCUC_MSGNODE_MFG].imitator.rxq.lock);

        return XWOK;

err_mknod_mfg:
        xwfs_rmnod(mcuc_msgnode[MCUC_MSGNODE_LOG].xwfsnode);
        mcuc_msgnode[MCUC_MSGNODE_LOG].xwfsnode = NULL;
err_mknod_log:
        xwfs_rmnod(mcuc_msgnode[MCUC_MSGNODE_SENSOR].xwfsnode);
        mcuc_msgnode[MCUC_MSGNODE_SENSOR].xwfsnode = NULL;
err_mknod_sensor:
        xwfs_rmnod(mcuc_msgnode[MCUC_MSGNODE_MISC].xwfsnode);
        mcuc_msgnode[MCUC_MSGNODE_MISC].xwfsnode = NULL;
err_mknod_button:
        xwfs_rmnod(mcuc_msgnode[MCUC_MSGNODE_DIAG].xwfsnode);
        mcuc_msgnode[MCUC_MSGNODE_DIAG].xwfsnode = NULL;
err_mknod_diag:
        xwfs_rmnod(mcuc_msgnode[MCUC_MSGNODE_VHC].xwfsnode);
        mcuc_msgnode[MCUC_MSGNODE_VHC].xwfsnode = NULL;
err_mknod_vhc:
        xwfs_rmnod(mcuc_msgnode[MCUC_MSGNODE_PWR].xwfsnode);
        mcuc_msgnode[MCUC_MSGNODE_PWR].xwfsnode = NULL;
err_mknod_pwr:
        xwfs_rmnod(mcuc_msgnode[MCUC_MSGNODE_SYS].xwfsnode);
        mcuc_msgnode[MCUC_MSGNODE_SYS].xwfsnode = NULL;
err_mknod_sys:
        return rc;
}

xwer_t mcuc_msgnode_exit(void)
{
        xwfs_rmnod(mcuc_msgnode[MCUC_MSGNODE_MFG].xwfsnode);
        mcuc_msgnode[MCUC_MSGNODE_MFG].xwfsnode = NULL;

        xwfs_rmnod(mcuc_msgnode[MCUC_MSGNODE_LOG].xwfsnode);
        mcuc_msgnode[MCUC_MSGNODE_LOG].xwfsnode = NULL;

        xwfs_rmnod(mcuc_msgnode[MCUC_MSGNODE_SENSOR].xwfsnode);
        mcuc_msgnode[MCUC_MSGNODE_SENSOR].xwfsnode = NULL;

        xwfs_rmnod(mcuc_msgnode[MCUC_MSGNODE_MISC].xwfsnode);
        mcuc_msgnode[MCUC_MSGNODE_MISC].xwfsnode = NULL;

        xwfs_rmnod(mcuc_msgnode[MCUC_MSGNODE_DIAG].xwfsnode);
        mcuc_msgnode[MCUC_MSGNODE_DIAG].xwfsnode = NULL;

        xwfs_rmnod(mcuc_msgnode[MCUC_MSGNODE_VHC].xwfsnode);
        mcuc_msgnode[MCUC_MSGNODE_VHC].xwfsnode = NULL;

        xwfs_rmnod(mcuc_msgnode[MCUC_MSGNODE_PWR].xwfsnode);
        mcuc_msgnode[MCUC_MSGNODE_PWR].xwfsnode = NULL;

        xwfs_rmnod(mcuc_msgnode[MCUC_MSGNODE_SYS].xwfsnode);
        mcuc_msgnode[MCUC_MSGNODE_SYS].xwfsnode = NULL;

        return XWOK;
}
