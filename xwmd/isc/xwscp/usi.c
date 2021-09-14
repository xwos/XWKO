/**
 * @file
 * @brief 精简的点对点通讯协议：用户态接口
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
#include <linux/slab.h>
#include <linux/parser.h>
#include <linux/uaccess.h>
#include <xwos/lib/xwlog.h>
#include <xwos/mp/pm.h>
#include <xwos/osal/skd.h>
#include <xwmd/xwfs/fs.h>
#include <xwmd/xwfs/bs.h>
#include <xwmd/sysfs/core.h>
#include <xwmd/isc/xwscp/hwifal.h>
#include <xwmd/isc/xwscp/protocol.h>
#include <xwmd/isc/xwscp/usi.h>
#include <xwmd/isc/xwscp/hwif/uart.h>
#include <bdl/isc/uart.h>

XWSCP_DEF_MEMPOOL(usi_xwscp_mem);
struct xwscp usi_xwscp;
xwsq_t usi_xwscp_state = USI_XWSCP_STATE_STOP;

xwsq_t usi_xwscp_get_state(void)
{
        return usi_xwscp_state;
}

void usi_xwscp_pm_notify(unsigned long pmevt)
{
        switch (pmevt) {
        case PM_HIBERNATION_PREPARE:
        case PM_SUSPEND_PREPARE:
                if (USI_XWSCP_STATE_START == usi_xwscp_get_state()) {
                        xwos_thd_intr(usi_xwscp.rx.thd);
                }
                break;
        case PM_POST_RESTORE:
        case PM_POST_HIBERNATION:
        case PM_POST_SUSPEND:
                break;
        case PM_RESTORE_PREPARE:
        default:
                break;
        }
}

struct xwfs_dir * usi_xwscp_xwfs = NULL;

static
ssize_t usi_xwscp_xwfs_port_read(struct xwfs_node * xwfsnode,
                                 struct file * file,
                                 char __user * usrbuf,
                                 size_t count,
                                 loff_t * pos);

static
ssize_t usi_xwscp_xwfs_port_write(struct xwfs_node *xwfsnode,
                                  struct file *file,
                                  const char __user *data,
                                  size_t count,
                                  loff_t *pos);

struct xwfs_node * usi_xwscp_xwfs_port = NULL;

const struct xwfs_operations usi_xwscp_xwfs_port_xwfsops = {
        .read = usi_xwscp_xwfs_port_read,
        .write = usi_xwscp_xwfs_port_write,
};

static
ssize_t usi_xwscp_xwfs_port_read(struct xwfs_node * xwfsnode,
                                 struct file * file,
                                 char __user * usbuf,
                                 size_t count,
                                 loff_t * pos)
{
        ssize_t ret;
        xwsz_t bufsize;
        xwtm_t desired;
        xwer_t rc;
        uint8_t buf[count];

        desired = XWTM_MAX;
        bufsize = count;
        rc = xwscp_rx(&usi_xwscp, buf, &bufsize, &desired);
        if (XWOK == rc) {
                ret = bufsize;
                rc = copy_to_user(usbuf, buf, bufsize);
        } else {
                ret = (ssize_t)rc;
        }
        return ret;
}

static
ssize_t usi_xwscp_xwfs_port_write(struct xwfs_node *xwfsnode,
                                  struct file *file,
                                  const char __user *usdata,
                                  size_t count,
                                  loff_t *pos)
{
        ssize_t ret;
        xwsz_t datasz;
        xwtm_t desired;
        xwer_t rc;
        uint8_t data[count];

        if (usdata) {
                rc = copy_from_user(data, usdata, count);
                datasz = count;
                desired = XWTM_MAX;
                rc = xwscp_tx(&usi_xwscp, data, &datasz, XWSCP_MSG_QOS_3, &desired);
                if (rc < 0) {
                        ret = (ssize_t)rc;
                } else {
                        ret = datasz;
                }
        } else {
                desired = XWTM_MAX;
                rc = xwscp_connect(&usi_xwscp, &desired);
                ret = rc;
        }
        return ret;
}

struct xwsys_object * usi_xwscp_xwsys;

#define USI_XWSCP_XWSYS_ARGBUFSIZE  32

enum usi_xwscp_xwsys_cmd_em {
        USI_XWSCP_XWSYS_CMD_STOP = 0,
        USI_XWSCP_XWSYS_CMD_START,
        USI_XWSCP_XWSYS_CMD_UNEXPORT,
        USI_XWSCP_XWSYS_CMD_EXPORT,
        USI_XWSCP_XWSYS_CMD_NUM,
};

static
ssize_t usi_xwscp_xwsys_cmd_show(struct xwsys_object * xwobj,
                                 struct xwsys_attribute * xwattr,
                                 char * buf);

static
ssize_t usi_xwscp_xwsys_cmd_store(struct xwsys_object * xwobj,
                                  struct xwsys_attribute * xwattr,
                                  const char * buf,
                                  size_t count);

/**
 * @brief file entry: /sys/xwos/xwscp/cmd
 */
static XWSYS_ATTR(file_xwscp_cmd, cmd, 0644,
                  usi_xwscp_xwsys_cmd_show,
                  usi_xwscp_xwsys_cmd_store);

static const match_table_t xwscp_cmd_tokens = {
        {USI_XWSCP_XWSYS_CMD_STOP, "stop"},
        {USI_XWSCP_XWSYS_CMD_START, "start"},
        {USI_XWSCP_XWSYS_CMD_UNEXPORT, "unexport %s"},
        {USI_XWSCP_XWSYS_CMD_EXPORT, "export %s"},
        {USI_XWSCP_XWSYS_CMD_NUM, NULL},
};

xwer_t usi_xwscp_start(void)
{
        xwer_t rc;

        if (USI_XWSCP_STATE_START == usi_xwscp_state) {
                rc = -EALREADY;
                goto err_already;
        }

        rc = xwfs_holdon();
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EOWNERDEAD;
                goto err_xwfs_not_ready;
        }

        rc = xwfs_mkdir("xwscp", xwfs_dir_isc, &usi_xwscp_xwfs);
        if (__xwcc_unlikely(rc < 0)) {
                xwscplogf(ERR, "Fail to mkdir(\"xwscp\"), rc: %d\n", rc);
                goto err_mkdir_usi_xwscp_xwfs;
        }

        rc = xwscp_start(&usi_xwscp, "usi_xwscp",
                         &xwscpif_uart_ops, modparam_isc_uart,
                         usi_xwscp_mem, sizeof(usi_xwscp_mem));
        if (__xwcc_unlikely(rc < 0)) {
                xwscplogf(ERR, "Activate xwscp ... [Failed], errno: %d\n", rc);
                goto err_xwscp_start;
        }
        xwscplogf(INFO, "Activate xwscp ... [OK]\n");

        usi_xwscp_state = USI_XWSCP_STATE_START;
        return XWOK;

err_xwscp_start:
        xwfs_rmdir(usi_xwscp_xwfs);
        usi_xwscp_xwfs = NULL;
err_mkdir_usi_xwscp_xwfs:
        xwfs_giveup();
err_xwfs_not_ready:
err_already:
        return rc;
}

xwer_t usi_xwscp_stop(void)
{
        xwer_t rc;

        if (USI_XWSCP_STATE_START != usi_xwscp_state) {
                rc = -EPERM;
                goto err_notstart;
        }

        rc = xwscp_stop(&usi_xwscp);
        if (XWOK == rc) {
                xwscplogf(INFO, "stop xwscp ... [OK].\n");
        }

        xwscplogf(INFO, "kfree mempool ... [OK].\n");

        xwfs_rmdir(usi_xwscp_xwfs);
        usi_xwscp_xwfs = NULL;
        xwfs_giveup();
        usi_xwscp_state = USI_XWSCP_STATE_STOP;
        return XWOK;

err_notstart:
        return rc;
}

static
xwer_t usi_xwscp_xwsys_cmd_export_port(void)
{
        xwer_t rc;

        if (USI_XWSCP_STATE_START != usi_xwscp_state) {
                rc = -EPERM;
                goto err_notstart;
        }

        rc = xwfs_mknod("port", 0660, &usi_xwscp_xwfs_port_xwfsops,
                        NULL, usi_xwscp_xwfs, &usi_xwscp_xwfs_port);
        if (__xwcc_unlikely(rc < 0)) {
                xwscplogf(ERR, "Fail to mknod(\"port\"), rc: %d\n", rc);
                goto err_xwfs_mknod;
        }
        return XWOK;

err_xwfs_mknod:
err_notstart:
        return rc;
}

static
xwer_t usi_xwscp_xwsys_cmd_unexport_port(void)
{
        xwer_t rc;

        if (USI_XWSCP_STATE_START != usi_xwscp_state) {
                rc = -EPERM;
                goto err_notstart;
        }
        rc = xwfs_rmnod(usi_xwscp_xwfs_port);
        if (rc < 0) {
                goto err_xwfs_rmnod;
        }
        usi_xwscp_xwfs_port = NULL;
        return XWOK;

err_xwfs_rmnod:
err_notstart:
        return rc;
}

static
xwer_t usi_xwscp_xwsys_cmd_parse(const char * cmdstring, size_t cnt)
{
        int token;
        substring_t tmp[MAX_OPT_ARGS];
        char cmd[cnt + 1];
        char * p, * pos;
        xwer_t rc = -ENOSYS;

        memcpy(cmd, cmdstring, cnt);
        if ('\n' == cmd[cnt - 1]) {
                if ('\r' == cmd[cnt - 2]) {
                        cmd[cnt - 2] = '\0';
                } else {
                        cmd[cnt - 1] = '\0';
                }
        } else if ('\0' == cmd[cnt - 1]) {
        } else {
                cmd[cnt] = '\0';
        }

        xwscplogf(INFO, "cmd:\"%s\"\n", cmd);
        pos = (char *)cmd;
        while ((p = strsep(&pos, ";")) != NULL) {
                if (!*p) {
                        continue;
                }
                token = match_token(p, xwscp_cmd_tokens, tmp);
                switch (token) {
                case USI_XWSCP_XWSYS_CMD_STOP:
                        rc = usi_xwscp_stop();
                        break;
                case USI_XWSCP_XWSYS_CMD_START:
                        rc = usi_xwscp_start();
                        break;
                case USI_XWSCP_XWSYS_CMD_EXPORT:
                        rc = usi_xwscp_xwsys_cmd_export_port();
                        break;
                case USI_XWSCP_XWSYS_CMD_UNEXPORT:
                        rc = usi_xwscp_xwsys_cmd_unexport_port();
                        break;
                }
        }
        return rc;
}

static
ssize_t usi_xwscp_xwsys_cmd_show(struct xwsys_object * xwobj,
                                 struct xwsys_attribute * xwattr,
                                 char * buf)
{
        return -ENOSYS;
}

static
ssize_t usi_xwscp_xwsys_cmd_store(struct xwsys_object * xwobj,
                                  struct xwsys_attribute * xwattr,
                                  const char * buf,
                                  size_t cnt)
{
        xwer_t rc;

        rc = usi_xwscp_xwsys_cmd_parse(buf, cnt);
        if (__xwcc_unlikely(rc < 0)) {
                cnt = rc;
        }
        return cnt;
}

static
ssize_t usi_xwscp_xwsys_state_show(struct xwsys_object * xwobj,
                                   struct xwsys_attribute * xwattr,
                                   char * buf);

static
ssize_t usi_xwscp_xwsys_state_store(struct xwsys_object * xwobj,
                                   struct xwsys_attribute * xwattr,
                                   const char * buf,
                                   size_t count);

static XWSYS_ATTR(file_xwscp_state, state, 0644,
                  usi_xwscp_xwsys_state_show,
                  usi_xwscp_xwsys_state_store);

static
ssize_t usi_xwscp_xwsys_state_show(struct xwsys_object * xwobj,
                                   struct xwsys_attribute * xwattr,
                                   char * buf)
{
        ssize_t showcnt;
        xwssq_t semval;
        xwer_t rc;

        showcnt = 0;

        /* Title */
        showcnt += sprintf(&buf[showcnt], "[XWSCP state]\n");

        if (USI_XWSCP_STATE_STOP == usi_xwscp_state) {
                showcnt += sprintf(&buf[showcnt], "State: OFF\n");
        } else {
                showcnt += sprintf(&buf[showcnt], "State: ON\n");

                /* RXQ */
                rc = xwos_sem_getvalue(&usi_xwscp.rx.sem, &semval);
                if (XWOK == rc) {
                        showcnt += sprintf(&buf[showcnt],
                                           "RX Queue: 0x%lX\n", semval);
                }
        }

        /* End flag */
        buf[showcnt] = '\0';
        return showcnt;
}

static
ssize_t usi_xwscp_xwsys_state_store(struct xwsys_object * xwobj,
                                    struct xwsys_attribute * xwattr,
                                    const char * buf,
                                    size_t count)
{
        return -ENOSYS;
}

xwer_t usi_xwscp_init(void)
{
        xwer_t rc;

        usi_xwscp_xwsys = xwsys_register("xwscp", NULL, NULL);
        if (__xwcc_unlikely(is_err_or_null(usi_xwscp_xwsys))) {
                rc = PTR_ERR(usi_xwscp_xwsys);
                xwscplogf(ERR,
                          "Create \"/sys/xwos/xwscp\" ... [rc:%d]\n",
                          rc);
                goto err_usi_xwscp_xwsys_create;
        }
        xwscplogf(INFO, "Create \"/sys/xwos/xwscp\" ... [OK]\n");

        rc = xwsys_create_file(usi_xwscp_xwsys, &xwsys_attr_file_xwscp_cmd);
        if (__xwcc_unlikely(rc < 0)) {
                xwscplogf(ERR,
                          "Create \"/sys/xwos/xwscp/cmd\" ... [rc:%d]\n",
                          rc);
                goto err_usi_xwscp_xwsys_cmd_create;
        }
        xwscplogf(INFO, "Create \"/sys/xwos/xwscp/cmd\" ... [OK]\n");

        rc = xwsys_create_file(usi_xwscp_xwsys, &xwsys_attr_file_xwscp_state);
        if (__xwcc_unlikely(rc < 0)) {
                xwscplogf(ERR,
                          "Create \"/sys/xwos/xwscp/state\" ... [rc:%d]\n",
                          rc);
                goto err_usi_xwscp_xwsys_state_create;
        }
        xwscplogf(INFO, "Create \"/sys/xwos/xwscp/state\" ... [OK]\n");
        return XWOK;

err_usi_xwscp_xwsys_state_create:
        xwsys_remove_file(usi_xwscp_xwsys, &xwsys_attr_file_xwscp_cmd);
err_usi_xwscp_xwsys_cmd_create:
        xwsys_unregister(usi_xwscp_xwsys);
        usi_xwscp_xwsys = NULL;
err_usi_xwscp_xwsys_create:
        return rc;
}

void usi_xwscp_exit(void)
{
        xwscplogf(INFO, "Destory \"/sys/xwos/xwscp/port\" ... [OK]\n");
        xwsys_remove_file(usi_xwscp_xwsys, &xwsys_attr_file_xwscp_state);
        xwscplogf(INFO, "Destory \"/sys/xwos/xwscp/state\" ... [OK]\n");
        xwsys_remove_file(usi_xwscp_xwsys, &xwsys_attr_file_xwscp_cmd);
        xwscplogf(INFO, "Destory \"/sys/xwos/xwscp/cmd\" ... [OK]\n");
        xwsys_unregister(usi_xwscp_xwsys);
        usi_xwscp_xwsys = NULL;
        xwscplogf(INFO, "Destory \"/sys/xwos/xwscp\" ... [OK]\n");
}
