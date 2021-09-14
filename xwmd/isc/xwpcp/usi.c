/**
 * @file
 * @brief 点对点通讯协议：用户态接口
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
#include <xwos/mm/common.h>
#include <xwos/mp/pm.h>
#include <linux/parser.h>
#include <linux/uaccess.h>
#include <xwmd/xwfs/fs.h>
#include <xwmd/xwfs/bs.h>
#include <xwmd/sysfs/core.h>
#include <xwmd/isc/xwpcp/hwifal.h>
#include <xwmd/isc/xwpcp/protocol.h>
#include <xwmd/isc/xwpcp/usi.h>
#include <xwmd/isc/xwpcp/hwif/uart.h>
#include <bdl/isc/uart.h>

XWPCP_DEF_MEMPOOL(usi_xwpcp_mem);
struct xwpcp usi_xwpcp;
xwsq_t usi_xwpcp_state = USI_XWPCP_STATE_STOP;

xwsq_t usi_xwpcp_get_state(void)
{
        return usi_xwpcp_state;
}

void usi_xwpcp_pm_notify(unsigned long pmevt)
{
        switch (pmevt) {
        case PM_HIBERNATION_PREPARE:
        case PM_SUSPEND_PREPARE:
                if (USI_XWPCP_STATE_START == usi_xwpcp_get_state()) {
                        xwos_thd_intr(usi_xwpcp.txthd);
                        xwos_thd_intr(usi_xwpcp.rxthd);
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

union usi_xwpcp_xwfs_port_data {
        struct {
                uint8_t port;
                uint8_t prio;
        } attr;
        unsigned long word;
};

static
ssize_t usi_xwpcp_xwfs_port_read(struct xwfs_node * xwfsnode,
                                 struct file * file,
                                 char __user * usrbuf,
                                 size_t cnt,
                                 loff_t * pos);

static
ssize_t usi_xwpcp_xwfs_port_write(struct xwfs_node * xwfsnode,
                                  struct file * file,
                                  const char __user * data,
                                  size_t cnt,
                                  loff_t * pos);

struct xwfs_dir * usi_xwpcp_xwfs = NULL;
struct xwfs_dir * usi_xwpcp_xwfs_portdir = NULL;
struct xwfs_node * usi_xwpcp_xwfs_port[XWMDCFG_isc_xwpcp_PORT_NUM] = {
        [0 ... (XWMDCFG_isc_xwpcp_PORT_NUM - 1)] = NULL,
};

const struct xwfs_operations usi_xwpcp_xwfs_port_xwfsops = {
        .read = usi_xwpcp_xwfs_port_read,
        .write = usi_xwpcp_xwfs_port_write,
};

static
ssize_t usi_xwpcp_xwfs_port_read(struct xwfs_node * xwfsnode,
                                 struct file * file,
                                 char __user * usbuf,
                                 size_t cnt,
                                 loff_t * pos)
{
        union usi_xwpcp_xwfs_port_data portdata;
        ssize_t ret;
        size_t rxsize;
        xwtm_t desire;
        xwer_t rc;
        uint8_t buf[cnt];

        portdata.word = (unsigned long)xwfs_node_get_data(xwfsnode);
        rxsize = cnt;
        desire = XWTM_MAX;
        rc = xwpcp_rx(&usi_xwpcp, portdata.attr.port,
                      buf, &rxsize, NULL, &desire);
        if (XWOK == rc) {
                ret = rxsize;
                rc = copy_to_user(usbuf, buf, rxsize);
        } else {
                ret = (ssize_t)rc;
        }
        return ret;
}

static
ssize_t usi_xwpcp_xwfs_port_write(struct xwfs_node * xwfsnode,
                                 struct file * file,
                                 const char __user * usdata,
                                 size_t cnt,
                                 loff_t * pos)
{
        union usi_xwpcp_xwfs_port_data portdata;
        xwssz_t ret;
        xwsz_t txsize;
        xwtm_t desire;
        xwer_t rc;
        uint8_t data[cnt];

        rc = copy_from_user(data, usdata, cnt);
        portdata.word = (unsigned long)xwfs_node_get_data(xwfsnode);
        desire = XWTM_MAX;
        txsize = cnt;
        rc = xwpcp_tx(&usi_xwpcp, data, &txsize,
                      portdata.attr.prio, portdata.attr.port, XWPCP_MSG_QOS_3,
                      &desire);
        if (rc < 0) {
                ret = (ssize_t)rc;
        } else {
                ret = txsize;
        }
        return ret;
}

struct xwsys_object * usi_xwpcp_xwsys;

/******** /sys/xwos/xwpcp/cmd ********/
#define USI_XWPCP_XWSYS_ARGBUFSIZE  32

enum usi_xwpcp_xwsys_cmd_em {
        USI_XWPCP_XWSYS_CMD_STOP = 0,
        USI_XWPCP_XWSYS_CMD_START,
        USI_XWPCP_XWSYS_CMD_UNEXPORT,
        USI_XWPCP_XWSYS_CMD_EXPORT,
        USI_XWPCP_XWSYS_CMD_NUM,
};

enum usi_xwpcp_xwsys_cmd_arg_em {
        USI_XWPCP_XWSYS_CMD_ARG_PORT = 0,
        USI_XWPCP_XWSYS_CMD_ARG_PRIO,
        USI_XWPCP_XWSYS_CMD_ARG_PORTNAME,
        USI_XWPCP_XWSYS_CMD_ARG_NUM,
};

static
ssize_t usi_xwpcp_xwsys_cmd_show(struct xwsys_object * xwobj,
                                 struct xwsys_attribute * xwattr,
                                 char * buf);

static
ssize_t usi_xwpcp_xwsys_cmd_store(struct xwsys_object * xwobj,
                                 struct xwsys_attribute * xwattr,
                                 const char * buf,
                                 size_t cnt);

static XWSYS_ATTR(file_xwpcp_cmd, cmd, 0644,
                  usi_xwpcp_xwsys_cmd_show,
                  usi_xwpcp_xwsys_cmd_store);

static const match_table_t xwpcp_cmd_tokens = {
        {USI_XWPCP_XWSYS_CMD_STOP, "stop"},
        {USI_XWPCP_XWSYS_CMD_START, "start"},
        {USI_XWPCP_XWSYS_CMD_UNEXPORT, "unexport(%s)"},
        {USI_XWPCP_XWSYS_CMD_EXPORT, "export(%s)"},
        {USI_XWPCP_XWSYS_CMD_NUM, NULL},
};

static const match_table_t xwpcp_cmd_arg_tokens = {
        {USI_XWPCP_XWSYS_CMD_ARG_PORT, "port=%u"},
        {USI_XWPCP_XWSYS_CMD_ARG_PRIO, "prio=%u"},
        {USI_XWPCP_XWSYS_CMD_ARG_PORTNAME, "name=%s"},
        {USI_XWPCP_XWSYS_CMD_ARG_NUM, NULL},
};

static
xwer_t usi_xwpcp_start(void)
{
        xwer_t rc;

        if (USI_XWPCP_STATE_START == usi_xwpcp_state) {
                rc = -EALREADY;
                goto err_already;
        }

        rc = xwfs_holdon();
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EOWNERDEAD;
                goto err_xwfs_not_ready;
        }

        rc = xwfs_mkdir("xwpcp", xwfs_dir_isc, &usi_xwpcp_xwfs);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mkdir_usi_xwpcp_xwfs;
        }

        rc = xwfs_mkdir("port", usi_xwpcp_xwfs, &usi_xwpcp_xwfs_portdir);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mkdir_usi_xwpcp_xwfs_portdir;
        }

        rc = xwpcp_start(&usi_xwpcp, "usi_xwpcp",
                         &xwpcpif_uart_ops, modparam_isc_uart,
                         usi_xwpcp_mem, sizeof(usi_xwpcp_mem));
        if (__xwcc_unlikely(rc < 0)) {
                xwpcplogf(ERR, "Start XWPCP ... [rc:%d]\n", rc);
                goto err_xwpcp_start;
        }
        xwpcplogf(INFO, "Start XWPCP ... [OK]\n");

        usi_xwpcp_state = USI_XWPCP_STATE_START;
        return XWOK;

err_xwpcp_start:
        xwfs_rmdir(usi_xwpcp_xwfs_portdir);
        usi_xwpcp_xwfs_portdir = NULL;
err_mkdir_usi_xwpcp_xwfs_portdir:
        xwfs_rmdir(usi_xwpcp_xwfs);
        usi_xwpcp_xwfs = NULL;
err_mkdir_usi_xwpcp_xwfs:
        xwfs_giveup();
err_xwfs_not_ready:
err_already:
        return rc;
}

static
xwer_t usi_xwpcp_stop(void)
{
        xwer_t rc;

        if (USI_XWPCP_STATE_START != usi_xwpcp_state) {
                rc = -EPERM;
                goto err_notstart;
        }

        rc = xwpcp_stop(&usi_xwpcp);
        if (XWOK == rc) {
                xwpcplogf(INFO, "Stop XWPCP ... [OK]\n");
        }

        xwfs_rmdir(usi_xwpcp_xwfs_portdir);
        usi_xwpcp_xwfs_portdir = NULL;
        xwfs_rmdir(usi_xwpcp_xwfs);
        usi_xwpcp_xwfs = NULL;
        xwfs_giveup();
        usi_xwpcp_state = USI_XWPCP_STATE_STOP;

        return XWOK;

err_notstart:
        return rc;
}

static
xwer_t usi_xwpcp_xwsys_cmd_export_port(int port, int prio, char * portname)
{
        union usi_xwpcp_xwfs_port_data portdata;
        struct xwfs_node * node;
        xwer_t rc;

        if (USI_XWPCP_STATE_START != usi_xwpcp_state) {
                rc = -EPERM;
                goto err_notstart;
        }
        if ((port > XWMDCFG_isc_xwpcp_PORT_NUM) || (port < 0)) {
                xwpcplogf(INFO, "Port ID is out of range!\n");
                rc = -ECHRNG;
                goto err_port;
        }
        if ((prio > XWMDCFG_isc_xwpcp_PRI_NUM) || (prio < 0)) {
                xwpcplogf(ERR, "Port priority is out of range!\n");
                rc = -ERANGE;
                goto err_prio;
        }

        portdata.attr.port = (uint8_t)port;
        portdata.attr.prio = (uint8_t)prio;
        rc = xwfs_mknod(portname, 0660, &usi_xwpcp_xwfs_port_xwfsops,
                        (void *)portdata.word,
                        usi_xwpcp_xwfs_portdir, &node);
        if (__xwcc_unlikely(rc < 0)) {
                xwpcplogf(ERR, "Fail to export port(%d) ... [rc:%d]\n", port, rc);
                goto err_xwfs_mknod;
        }
        usi_xwpcp_xwfs_port[port] = node;
        return XWOK;

err_xwfs_mknod:
err_prio:
err_port:
err_notstart:
        return rc;
}

static
xwer_t usi_xwpcp_xwsys_cmd_unexport_port(int port)
{
        struct xwfs_node *node;
        xwer_t rc;

        if (USI_XWPCP_STATE_START != usi_xwpcp_state) {
                rc = -EPERM;
                goto err_notstart;
        }

        if ((port > XWMDCFG_isc_xwpcp_PORT_NUM) || (port < 0)) {
                xwpcplogf(ERR, "port port out of range\n");
                rc = -ECHRNG;
                goto err_port;
        }

        node = usi_xwpcp_xwfs_port[port];
        if (NULL == node) {
                xwpcplogf(ERR, "port is not exported\n");
                rc = -ENODEV;
                goto err_noport;
        }

        rc = xwfs_rmnod(node);
        if (__xwcc_unlikely(rc < 0)) {
                xwpcplogf(ERR, "Fail to rmnod(port[%d]) ... [rc:%d]\n", port, rc);
                goto err_rmnod;
        }

        usi_xwpcp_xwfs_port[port] = NULL;
        return XWOK;

err_rmnod:
err_noport:
err_port:
err_notstart:
        return rc;
}

static
ssize_t usi_xwpcp_xwsys_cmd_show(struct xwsys_object * xwobj,
                                 struct xwsys_attribute * xwattr,
                                 char * buf)
{
        return -ENOSYS;
}

static
xwer_t usi_xwpcp_xwsys_cmd_parse_arg(const char * argstring,
                                     int * port,
                                     int * prio,
                                     char portname[],
                                     size_t bufsize)
{
        int token;
        substring_t tmp[MAX_OPT_ARGS];
        char *p, *pos;
        xwer_t rc = -ENOSYS;

        pos = (char *)argstring;
        while ((p = strsep(&pos, ",")) != NULL) {
                if (!*p) {
                        continue;
                }
                token = match_token(p, xwpcp_cmd_arg_tokens, tmp);
                switch (token) {
                case USI_XWPCP_XWSYS_CMD_ARG_PORT:
                        rc = match_int(&tmp[0], port);
                        if (__xwcc_unlikely(rc < 0)) {
                                goto err_match;
                        }
                        break;
                case USI_XWPCP_XWSYS_CMD_ARG_PRIO:
                        rc = match_int(&tmp[0], prio);
                        if (__xwcc_unlikely(rc < 0)) {
                                goto err_match;
                        }
                        break;
                case USI_XWPCP_XWSYS_CMD_ARG_PORTNAME:
                        rc = XWOK;
                        match_strlcpy(portname, &tmp[0], bufsize);
                        break;
                }
        }

err_match:
        return rc;
}

static
xwer_t usi_xwpcp_xwsys_cmd_parse(const char * cmdstring, size_t cnt)
{
        int token;
        substring_t tmp[MAX_OPT_ARGS];
        char * p, * pos;
        char arg[USI_XWPCP_XWSYS_ARGBUFSIZE];
        size_t argsize;
        int port = -1;
        int prio = -1;
        char portname[USI_XWPCP_XWSYS_ARGBUFSIZE];
        char cmd[cnt + 1];
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

        xwpcplogf(INFO, "cmd:\"%s\"\n", cmd);
        pos = (char *)cmd;
        while ((p = strsep(&pos, ";")) != NULL) {
                if (!*p) {
                        continue;
                }
                token = match_token(p, xwpcp_cmd_tokens, tmp);
                switch (token) {
                case USI_XWPCP_XWSYS_CMD_STOP:
                        rc = usi_xwpcp_stop();
                        break;
                case USI_XWPCP_XWSYS_CMD_START:
                        rc = usi_xwpcp_start();
                        break;
                case USI_XWPCP_XWSYS_CMD_EXPORT:
                        argsize = match_strlcpy(arg, &tmp[0],
                                                USI_XWPCP_XWSYS_ARGBUFSIZE);
                        rc = usi_xwpcp_xwsys_cmd_parse_arg(arg, &port, &prio,
                                                           portname,
                                                           USI_XWPCP_XWSYS_ARGBUFSIZE);
                        if (XWOK == rc) {
                                xwpcplogf(INFO,
                                         "cmd:export; port:%d; prio:%d; portname:%s",
                                         port, prio, portname);
                                rc = usi_xwpcp_xwsys_cmd_export_port(port,
                                                                     prio,
                                                                     portname);
                        }
                        break;
                case USI_XWPCP_XWSYS_CMD_UNEXPORT:
                        rc = match_int(&tmp[0], &port);
                        if (XWOK == rc) {
                                xwpcplogf(INFO, "cmd:unexport; port:%d", port);
                                rc = usi_xwpcp_xwsys_cmd_unexport_port(port);
                        }
                        break;
                }
        }
        return rc;
}

static
ssize_t usi_xwpcp_xwsys_cmd_store(struct xwsys_object * xwobj,
                                  struct xwsys_attribute * xwattr,
                                  const char * buf,
                                  size_t cnt)
{
        xwer_t rc;

        rc = usi_xwpcp_xwsys_cmd_parse(buf, cnt);
        if (__xwcc_unlikely(rc < 0)) {
                cnt = rc;
        }
        return cnt;
}

/******** /sys/xwos/xwpcp/state ********/
static
ssize_t usi_xwpcp_xwsys_state_show(struct xwsys_object * xwobj,
                                   struct xwsys_attribute * xwattr,
                                   char * buf);

static
ssize_t usi_xwpcp_xwsys_state_store(struct xwsys_object * xwobj,
                                   struct xwsys_attribute * xwattr,
                                   const char * buf,
                                   size_t cnt);

static XWSYS_ATTR(file_xwpcp_state, state, 0644,
                  usi_xwpcp_xwsys_state_show,
                  usi_xwpcp_xwsys_state_store);

static
ssize_t usi_xwpcp_xwsys_state_show(struct xwsys_object * xwobj,
                                   struct xwsys_attribute * xwattr,
                                   char * buf)
{
        size_t i;
        ssize_t showcnt;
        xwssq_t semval;
        xwer_t rc;

        showcnt = 0;

        /* Title */
        showcnt += sprintf(&buf[showcnt], "[XWPCP state]\n");

        if (USI_XWPCP_STATE_STOP == usi_xwpcp_state) {
                showcnt += sprintf(&buf[showcnt], "State: OFF\n");
        } else {
                showcnt += sprintf(&buf[showcnt], "State: ON\n");

                /* TXQ */
                rc = xwos_sem_getvalue(&usi_xwpcp.txq.qsem, &semval);
                if (XWOK == rc) {
                        showcnt += sprintf(&buf[showcnt],
                                           "TX Queue: 0x%lX\n",
                                           semval);
                }

                /* RXQ */
                for (i = 0; i < XWPCP_PORT_NUM; i++) {
                        rc = xwos_sem_getvalue(&usi_xwpcp.rxq.sem[i], &semval);
                        if (XWOK == rc) {
                                showcnt += sprintf(&buf[showcnt],
                                                   "RX Queue[%ld]: 0x%lX\n",
                                                   i, semval);
                        }
                }
        }

        /* End flag */
        buf[showcnt] = '\0';
        return showcnt;
}

static
ssize_t usi_xwpcp_xwsys_state_store(struct xwsys_object * xwobj,
                                    struct xwsys_attribute * xwattr,
                                    const char * buf,
                                    size_t cnt)
{
        return -ENOSYS;
}

xwer_t usi_xwpcp_init(void)
{
        xwer_t rc;

        usi_xwpcp_xwsys = xwsys_register("xwpcp", NULL, NULL);
        if (__xwcc_unlikely(is_err_or_null(usi_xwpcp_xwsys))) {
                rc = PTR_ERR(usi_xwpcp_xwsys);
                xwpcplogf(ERR,
                          "Create \"/sys/xwos/xwpcp\" ... [rc:%d]\n",
                          rc);
                goto err_usi_xwpcp_xwsys_create;
        }
        xwpcplogf(INFO, "Create \"/sys/xwos/xwpcp\" ... [OK]\n");

        rc = xwsys_create_file(usi_xwpcp_xwsys, &xwsys_attr_file_xwpcp_cmd);
        if (__xwcc_unlikely(rc < 0)) {
                xwpcplogf(ERR,
                          "Create \"/sys/xwos/xwpcp/cmd\" ... [rc:%d]\n",
                          rc);
                goto err_usi_xwpcp_xwsys_cmd_create;
        }
        xwpcplogf(INFO, "Create \"/sys/xwos/xwpcp/cmd\" ... [OK]\n");

        rc = xwsys_create_file(usi_xwpcp_xwsys, &xwsys_attr_file_xwpcp_state);
        if (__xwcc_unlikely(rc < 0)) {
                xwpcplogf(ERR,
                          "Create \"/sys/xwos/xwpcp/state\" ... [rc:%d]\n",
                          rc);
                goto err_usi_xwpcp_xwsys_state_create;
        }
        xwpcplogf(INFO, "Create \"/sys/xwos/xwpcp/state\" ... [OK]\n");
        return XWOK;

err_usi_xwpcp_xwsys_state_create:
        xwsys_remove_file(usi_xwpcp_xwsys, &xwsys_attr_file_xwpcp_cmd);
err_usi_xwpcp_xwsys_cmd_create:
        xwsys_unregister(usi_xwpcp_xwsys);
        usi_xwpcp_xwsys = NULL;
err_usi_xwpcp_xwsys_create:
        return rc;
}

void usi_xwpcp_exit(void)
{
        xwsys_remove_file(usi_xwpcp_xwsys, &xwsys_attr_file_xwpcp_state);
        xwpcplogf(INFO, "Destory \"/sys/xwos/xwpcp/state\" ... [OK]\n");
        xwsys_remove_file(usi_xwpcp_xwsys, &xwsys_attr_file_xwpcp_cmd);
        xwpcplogf(INFO, "Destory \"/sys/xwos/xwpcp/cmd\" ... [OK]\n");
        xwsys_unregister(usi_xwpcp_xwsys);
        usi_xwpcp_xwsys = NULL;
        xwpcplogf(INFO, "Destory \"/sys/xwos/xwpcp\" ... [OK]\n");
}
