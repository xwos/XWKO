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

#include <xwos/standard.h>
#include <xwos/lib/xwlog.h>
#include <xwos/lib/xwaop.h>
#include <linux/slab.h>
#include <xwmd/xwfs/fs.h>
#include <xwmd/isc/xwpcp/usi.h>
#include <bm/mcuc/init.h>
#include <bm/mcuc/msgnode.h>

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
        [MCUC_MSGNODE_DAT] = {
                .attr = {
                        .port = MCUC_MSGNODE_DAT,
                        .prio = 2,
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
        if (USI_XWPCP_STATE_START == usi_xwpcp_get_state()) {
                xwsz_t bufsz;
                xwtm_t time;
                uint8_t buf[count];

                bufsz = count;
                time = XWTM_MAX;
                rc = xwpcp_rx(&usi_xwpcp, portdata->attr.port,
                              buf, &bufsz, NULL, &time);
                if (XWOK == rc) {
                        rdcnt = bufsz;
                        rc = copy_to_user(usbuf, buf, bufsz);
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
	mcuclogf(INFO, "TX data count:%d, XWPCP state:%d\n", count, state);
        if (USI_XWPCP_STATE_START == state) {
                xwu8_t data[count];
                xwsz_t txsize;
                xwtm_t time;

                rc = copy_from_user(data, usdata, count);
                txsize = count;
                time = XWTM_MAX;
                rc = xwpcp_tx(&usi_xwpcp, data, &txsize,
                              portdata->attr.port, portdata->attr.prio,
                              XWPCP_MSG_QOS_3, &time);
                if (rc < 0) {
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
                        xwfs_dir_mcuc, &node);
        if (__xwcc_unlikely(rc < 0)) {
                mcuclogf(ERR, "Fail to mknod(\"sys\"), rc: %d\n", rc);
                goto err_mknod_sys;
        }
        mcuc_msgnode[MCUC_MSGNODE_SYS].xwfsnode = node;

        rc = xwfs_mknod("pwr", 0666, &mcuc_msgnode_xwfsops,
                        (void *)&mcuc_msgnode[MCUC_MSGNODE_PWR],
                        xwfs_dir_mcuc, &node);
        if (__xwcc_unlikely(rc < 0)) {
                mcuclogf(ERR, "Fail to mknod(\"pwr\"), rc: %d\n", rc);
                goto err_mknod_pwr;
        }
        mcuc_msgnode[MCUC_MSGNODE_PWR].xwfsnode = node;

        rc = xwfs_mknod("dat", 0666, &mcuc_msgnode_xwfsops,
                        (void *)&mcuc_msgnode[MCUC_MSGNODE_DAT],
                        xwfs_dir_mcuc, &node);
        if (__xwcc_unlikely(rc < 0)) {
                mcuclogf(ERR, "Fail to mknod(\"dat\"), rc: %d\n", rc);
                goto err_mknod_dat;
        }
        mcuc_msgnode[MCUC_MSGNODE_DAT].xwfsnode = node;

        rc = xwfs_mknod("log", 0666, &mcuc_msgnode_xwfsops,
                        (void *)&mcuc_msgnode[MCUC_MSGNODE_LOG],
                        xwfs_dir_mcuc, &node);
        if (__xwcc_unlikely(rc < 0)) {
                mcuclogf(ERR, "Fail to mknod(\"log\"), rc: %d\n", rc);
                goto err_mknod_log;
        }
        mcuc_msgnode[MCUC_MSGNODE_LOG].xwfsnode = node;

        rc = xwfs_mknod("mfg", 0666, &mcuc_msgnode_xwfsops,
                        (void *)&mcuc_msgnode[MCUC_MSGNODE_MFG],
                        xwfs_dir_mcuc, &node);
        if (__xwcc_unlikely(rc < 0)) {
                mcuclogf(ERR, "Fail to mknod(\"mfg\"), rc: %d\n", rc);
                goto err_mknod_mfg;
        }
        mcuc_msgnode[MCUC_MSGNODE_MFG].xwfsnode = node;

        return XWOK;

err_mknod_mfg:
        xwfs_rmnod(mcuc_msgnode[MCUC_MSGNODE_LOG].xwfsnode);
        mcuc_msgnode[MCUC_MSGNODE_LOG].xwfsnode = NULL;
err_mknod_log:
        xwfs_rmnod(mcuc_msgnode[MCUC_MSGNODE_DAT].xwfsnode);
        mcuc_msgnode[MCUC_MSGNODE_DAT].xwfsnode = NULL;
err_mknod_dat:
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

        xwfs_rmnod(mcuc_msgnode[MCUC_MSGNODE_DAT].xwfsnode);
        mcuc_msgnode[MCUC_MSGNODE_DAT].xwfsnode = NULL;

        xwfs_rmnod(mcuc_msgnode[MCUC_MSGNODE_PWR].xwfsnode);
        mcuc_msgnode[MCUC_MSGNODE_PWR].xwfsnode = NULL;

        xwfs_rmnod(mcuc_msgnode[MCUC_MSGNODE_SYS].xwfsnode);
        mcuc_msgnode[MCUC_MSGNODE_SYS].xwfsnode = NULL;

        return XWOK;
}
