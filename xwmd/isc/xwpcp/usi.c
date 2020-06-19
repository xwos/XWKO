/**
 * @file
 * @brief XWPCP在sysfs和xwfs中接口
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

/******** ******** ******** ******** ******** ***<***** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwlog.h>
#include <xwos/mm/common.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <linux/slab.h>
#include <linux/parser.h>
#include <linux/uaccess.h>
#include <bdl/isc/xwpcpif.h>
#include <xwmd/xwfs/fs.h>
#include <xwmd/xwfs/bs.h>
#include <xwmd/sysfs/core.h>
#include <xwmd/isc/xwpcp/hwifal.h>
#include <xwmd/isc/xwpcp/protocol.h>
#include <xwmd/isc/xwpcp/usi.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********            XWPCP resources            ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define USI_XWPCP_TXTHRD_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 20)
#define USI_XWPCP_RXTHRD_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 20)

struct xwpcp usi_xwpcp;
xwid_t usi_xwpcp_txthrd;
xwid_t usi_xwpcp_rxthrd;
xwu8_t __aligned(XWMMCFG_ALIGNMENT) usi_xwpcp_mempool[XWPCP_MEMPOOL_SIZE];
xwsq_t usi_xwpcp_state = USI_XWPCP_STATE_STOP;

xwsq_t usi_xwpcp_get_state(void)
{
        return usi_xwpcp_state;
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********          xwfs/xwmd/isc/xwpcp          ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
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
                                 size_t count,
                                 loff_t * pos);

static
ssize_t usi_xwpcp_xwfs_port_write(struct xwfs_node * xwfsnode,
                                  struct file * file,
                                  const char __user * data,
                                  size_t count,
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
                                 size_t count,
                                 loff_t * pos)
{
        union usi_xwpcp_xwfs_port_data portdata;
        struct xwpcp_msg rxmsg;
        ssize_t rdcnt;
        xwtm_t desire;
        xwer_t rc;
        uint8_t buf[count];

        portdata.word = (unsigned long)xwfs_node_get_data(xwfsnode);
        rxmsg.port = portdata.attr.port;
        rxmsg.size = count;
        rxmsg.text = buf;
        desire = XWTM_MAX;
        rc = xwpcp_rx(&usi_xwpcp, &rxmsg, &desire);
        if (OK == rc) {
                rdcnt = rxmsg.size;
                rc = copy_to_user(usbuf, buf, rdcnt);
        } else {
                rdcnt = (ssize_t)rc;
        }
        return rdcnt;
}

static
ssize_t usi_xwpcp_xwfs_port_write(struct xwfs_node * xwfsnode,
                                 struct file * file,
                                 const char __user * usdata,
                                 size_t count,
                                 loff_t * pos)
{
        union usi_xwpcp_xwfs_port_data portdata;
        struct xwpcp_msg msg;
        xwtm_t desire;
        xwer_t rc;
        uint8_t data[count];

        rc = copy_from_user(data, usdata, count);
        portdata.word = (unsigned long)xwfs_node_get_data(xwfsnode);
        msg.port = portdata.attr.port;
        msg.size = count;
        msg.text = data;
        desire = XWTM_MAX;
        rc = xwpcp_tx(&usi_xwpcp, &msg, portdata.attr.prio, &desire);
        if (__unlikely(rc < 0)) {
                count = (ssize_t)rc;
        }
        return count;
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********           /sys/xwos/xwpcp           ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwsys_object * usi_xwpcp_sysfs;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         /sys/xwos/xwpcp/cmd         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define USI_XWPCP_SYSFS_ARGBUFSIZE  32

enum usi_xwpcp_sysfs_cmd_em {
        USI_XWPCP_SYSFS_CMD_STOP = 0,
        USI_XWPCP_SYSFS_CMD_START,
        USI_XWPCP_SYSFS_CMD_UNEXPORT,
        USI_XWPCP_SYSFS_CMD_EXPORT,
        USI_XWPCP_SYSFS_CMD_NUM,
};

enum usi_xwpcp_sysfs_cmd_arg_em {
        USI_XWPCP_SYSFS_CMD_ARG_PORT = 0,
        USI_XWPCP_SYSFS_CMD_ARG_PRIO,
        USI_XWPCP_SYSFS_CMD_ARG_PORTNAME,
        USI_XWPCP_SYSFS_CMD_ARG_NUM,
};

static
ssize_t usi_xwpcp_sysfs_cmd_show(struct xwsys_object * xwobj,
                                 struct xwsys_attribute * soattr,
                                 char * buf);

static
ssize_t usi_xwpcp_sysfs_cmd_store(struct xwsys_object * xwobj,
                                 struct xwsys_attribute * soattr,
                                 const char * buf,
                                 size_t count);

static XWSYS_ATTR(file_xwpcp_cmd, cmd, 0644,
                  usi_xwpcp_sysfs_cmd_show,
                  usi_xwpcp_sysfs_cmd_store);

static const match_table_t xwpcp_cmd_tokens = {
        {USI_XWPCP_SYSFS_CMD_STOP, "stop"},
        {USI_XWPCP_SYSFS_CMD_START, "start"},
        {USI_XWPCP_SYSFS_CMD_UNEXPORT, "unexport(%s)"},
        {USI_XWPCP_SYSFS_CMD_EXPORT, "export(%s)"},
        {USI_XWPCP_SYSFS_CMD_NUM, NULL},
};

static const match_table_t xwpcp_cmd_arg_tokens = {
        {USI_XWPCP_SYSFS_CMD_ARG_PORT, "port=%u"},
        {USI_XWPCP_SYSFS_CMD_ARG_PRIO, "prio=%u"},
        {USI_XWPCP_SYSFS_CMD_ARG_PORTNAME, "name=%s"},
        {USI_XWPCP_SYSFS_CMD_ARG_NUM, NULL},
};

static
xwer_t usi_xwpcp_start(void)
{
        xwer_t rc;
        xwer_t trc;
        xwid_t tid;

        tid = 0;
        if (USI_XWPCP_STATE_START == usi_xwpcp_state) {
                rc = -EALREADY;
                goto err_already;
        }

        rc = xwfs_holdon();
        if (__unlikely(rc < 0)) {
                rc = -EOWNERDEAD;
                goto err_xwfs_not_ready;
        }

        rc = xwfs_mkdir("xwpcp", dir_isc, &usi_xwpcp_xwfs);
        if (__unlikely(rc < 0)) {
                goto err_mkdir_usi_xwpcp_xwfs;
        }

        rc = xwfs_mkdir("port", usi_xwpcp_xwfs, &usi_xwpcp_xwfs_portdir);
        if (__unlikely(rc < 0)) {
                goto err_mkdir_usi_xwpcp_xwfs_portdir;
        }

        xwpcp_init(&usi_xwpcp);
        rc = xwpcp_start(&usi_xwpcp, "usi_xwpcp", &bdl_xwpcpif_ops,
                         (xwptr_t)usi_xwpcp_mempool, XWPCP_MEMPOOL_SIZE);
        if (__unlikely(rc < 0)) {
                xwpcplogf(ERR, "Start XWPCP ... [rc:%d]\n", rc);
                goto err_xwpcp_start;
        }
        xwpcplogf(INFO, "Start XWPCP ... [OK]\n");

        rc = xwosal_thrd_create(&tid, "xwpcp_tx_thrd",
                                (xwosal_thrd_f)xwpcp_txthrd,
                                &usi_xwpcp, XWOS_UNUSED_ARGUMENT,
                                USI_XWPCP_TXTHRD_PRIORITY,
                                XWOS_UNUSED_ARGUMENT);
        if (__unlikely(rc < 0)) {
                xwpcplogf(ERR, "Create XWPCP TX thread ... [rc:%d]\n", rc);
                goto err_xwpcp_txthrd_create;
        }
        usi_xwpcp_txthrd = tid;
        xwpcplogf(INFO, "Create XWPCP TX thread ... [OK]\n");

        rc = xwosal_thrd_create(&tid, "xwpcp_rx_thrd",
                                (xwosal_thrd_f)xwpcp_rxthrd,
                                &usi_xwpcp, XWOS_UNUSED_ARGUMENT,
                                USI_XWPCP_RXTHRD_PRIORITY,
                                XWOS_UNUSED_ARGUMENT);
        if (__unlikely(rc < 0)) {
                xwpcplogf(ERR, "Create XWPCP RX thread ... [rc:%d]\n", rc);
                goto err_xwpcp_rxthrd_create;
        }
        usi_xwpcp_rxthrd = tid;
        xwpcplogf(INFO, "Create XWPCP RX thread ... [OK]\n");

        usi_xwpcp_state = USI_XWPCP_STATE_START;
        return OK;

err_xwpcp_rxthrd_create:
        xwosal_thrd_terminate(usi_xwpcp_txthrd, &trc);
        xwosal_thrd_delete(usi_xwpcp_txthrd);
        usi_xwpcp_txthrd = 0;
err_xwpcp_txthrd_create:
        xwpcp_stop(&usi_xwpcp);
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
        xwer_t rc, trc;

        if (USI_XWPCP_STATE_START != usi_xwpcp_state) {
                rc = -EPERM;
                goto err_notstart;
        }

        rc = xwosal_thrd_terminate(usi_xwpcp_rxthrd, &trc);
        if (OK == rc) {
                rc = xwosal_thrd_delete(usi_xwpcp_rxthrd);
                if (OK == rc) {
                        usi_xwpcp_rxthrd = 0;
                        xwpcplogf(INFO, "Terminate XWPCP RX thread... [OK]\n");
                }
        }

        rc = xwosal_thrd_terminate(usi_xwpcp_txthrd, &trc);
        if (OK == rc) {
                rc = xwosal_thrd_delete(usi_xwpcp_txthrd);
                if (OK == rc) {
                        usi_xwpcp_txthrd = 0;
                        xwpcplogf(INFO, "Terminate XWPCP TX thread... [OK]\n");
                }
        }

        rc = xwpcp_stop(&usi_xwpcp);
        if (OK == rc) {
                xwpcplogf(INFO, "Stop XWPCP ... [OK]\n");
        }

        xwfs_rmdir(usi_xwpcp_xwfs_portdir);
        usi_xwpcp_xwfs_portdir = NULL;
        xwfs_rmdir(usi_xwpcp_xwfs);
        usi_xwpcp_xwfs = NULL;
        xwfs_giveup();
        usi_xwpcp_state = USI_XWPCP_STATE_STOP;

        return OK;

err_notstart:
        return rc;
}

static
xwer_t usi_xwpcp_sysfs_cmd_export_port(int port, int prio, char * portname)
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
        if ((prio > XWMDCFG_isc_xwpcp_PRIORITY_NUM) || (prio < 0)) {
                xwpcplogf(ERR, "Port priority is out of range!\n");
                rc = -ERANGE;
                goto err_prio;
        }

        portdata.attr.port = (uint8_t)port;
        portdata.attr.prio = (uint8_t)prio;
        rc = xwfs_mknod(portname, 0660, &usi_xwpcp_xwfs_port_xwfsops,
                        (void *)portdata.word,
                        usi_xwpcp_xwfs_portdir, &node);
        if (__unlikely(rc < 0)) {
                xwpcplogf(ERR, "Fail to export port(%d) ... [rc:%d]\n", port, rc);
                goto err_xwfs_mknod;
        }
        usi_xwpcp_xwfs_port[port] = node;
        return OK;

err_xwfs_mknod:
err_prio:
err_port:
err_notstart:
        return rc;
}

static
xwer_t usi_xwpcp_sysfs_cmd_unexport_port(int port)
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
        if (__unlikely(rc < 0)) {
                xwpcplogf(ERR, "Fail to rmnod(port[%d]) ... [rc:%d]\n", port, rc);
                goto err_rmnod;
        }

        usi_xwpcp_xwfs_port[port] = NULL;
        return OK;

err_rmnod:
err_noport:
err_port:
err_notstart:
        return rc;
}

static
ssize_t usi_xwpcp_sysfs_cmd_show(struct xwsys_object * xwobj,
                                 struct xwsys_attribute * soattr,
                                 char * buf)
{
        return 0;
}

static
xwer_t usi_xwpcp_sysfs_cmd_parse_arg(const char * argstring,
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
		case USI_XWPCP_SYSFS_CMD_ARG_PORT:
                        rc = match_int(&tmp[0], port);
                        if (__unlikely(rc < 0)) {
                                goto err_match;
                        }
			break;
                case USI_XWPCP_SYSFS_CMD_ARG_PRIO:
                        rc = match_int(&tmp[0], prio);
                        if (__unlikely(rc < 0)) {
                                goto err_match;
                        }
                        break;
		case USI_XWPCP_SYSFS_CMD_ARG_PORTNAME:
                        rc = OK;
                        match_strlcpy(portname, &tmp[0], bufsize);
			break;
		}
	}

err_match:
        return rc;
}

static
xwer_t usi_xwpcp_sysfs_cmd_parse(const char * cmdstring)
{
	int token;
	substring_t tmp[MAX_OPT_ARGS];
	char * p, * pos;
        char arg[USI_XWPCP_SYSFS_ARGBUFSIZE];
        size_t argsize;
        int port = -1;
        int prio = -1;
        char portname[USI_XWPCP_SYSFS_ARGBUFSIZE];
        xwer_t rc = -ENOSYS;

	xwpcplogf(INFO, "cmd:\"%s\"\n", cmdstring);
	pos = (char *)cmdstring;
	while ((p = strsep(&pos, ";")) != NULL) {
		if (!*p) {
			continue;
                }
		token = match_token(p, xwpcp_cmd_tokens, tmp);
		switch (token) {
                case USI_XWPCP_SYSFS_CMD_STOP:
                        rc = usi_xwpcp_stop();
                        break;
                case USI_XWPCP_SYSFS_CMD_START:
                        rc = usi_xwpcp_start();
                        break;
		case USI_XWPCP_SYSFS_CMD_EXPORT:
                        argsize = match_strlcpy(arg, &tmp[0],
                                                USI_XWPCP_SYSFS_ARGBUFSIZE);
                        rc = usi_xwpcp_sysfs_cmd_parse_arg(arg, &port, &prio,
                                                           portname,
                                                           USI_XWPCP_SYSFS_ARGBUFSIZE);
                        if (OK == rc) {
                                xwpcplogf(INFO,
                                         "cmd:export; port:%d; prio:%d; portname:%s",
                                         port, prio, portname);
                                rc = usi_xwpcp_sysfs_cmd_export_port(port,
                                                                     prio,
                                                                     portname);
                        }
			break;
		case USI_XWPCP_SYSFS_CMD_UNEXPORT:
                        rc = match_int(&tmp[0], &port);
                        if (OK == rc) {
                                xwpcplogf(INFO, "cmd:unexport; port:%d", port);
                                rc = usi_xwpcp_sysfs_cmd_unexport_port(port);
                        }
			break;
		}
	}
        return rc;
}

static
ssize_t usi_xwpcp_sysfs_cmd_store(struct xwsys_object * xwobj,
                                  struct xwsys_attribute * soattr,
                                  const char * buf,
                                  size_t count)
{
        xwer_t rc;

        if ('\0' != buf[count - 1]) {
                count = -EINVAL;
        } else {
                rc = usi_xwpcp_sysfs_cmd_parse(buf);
                if (__unlikely(rc < 0)) {
                        count = rc;
                }
        }
        return count;
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********        /sys/xwos/xwpcp/state        ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
ssize_t usi_xwpcp_sysfs_state_show(struct xwsys_object * xwobj,
                                   struct xwsys_attribute * soattr,
                                   char * buf);

static
ssize_t usi_xwpcp_sysfs_state_store(struct xwsys_object * xwobj,
                                   struct xwsys_attribute * soattr,
                                   const char * buf,
                                   size_t count);

static XWSYS_ATTR(file_xwpcp_state, state, 0644,
                  usi_xwpcp_sysfs_state_show,
                  usi_xwpcp_sysfs_state_store);

static
ssize_t usi_xwpcp_sysfs_state_show(struct xwsys_object * xwobj,
                                   struct xwsys_attribute * soattr,
                                   char * buf)
{
        size_t i;
        ssize_t showcnt;
        size_t blkcnt;
        xwssq_t smrval;
        xwer_t rc;

        showcnt = 0;

        /* Title */
        showcnt += sprintf(&buf[showcnt], "[XWPCP state]\n");

        if (USI_XWPCP_STATE_STOP == usi_xwpcp_state) {
                showcnt += sprintf(&buf[showcnt], "State: OFF\n");
        } else {
                showcnt += sprintf(&buf[showcnt], "State: ON\n");
                /* slot */
                showcnt += sprintf(&buf[showcnt], "Slot Pool:\n");
                blkcnt = usi_xwpcp.slot.pool->zone.size / usi_xwpcp.slot.pool->blksize;
                for (i = 0; i < blkcnt; i++) {
                        showcnt += sprintf(&buf[showcnt], "0x%X,",
                                           usi_xwpcp.slot.pool->bcbs[i].order);
                        if (i % 8 == 0) {
                                showcnt += sprintf(&buf[showcnt], "\n");
                        }
                }
                showcnt += sprintf(&buf[showcnt], "\n");

                /* TXQ */
                rc = xwosal_smr_getvalue(xwosal_smr_get_id(&usi_xwpcp.txq.smr),
                                         &smrval);
                if (OK == rc) {
                        showcnt += sprintf(&buf[showcnt],
                                           "TX Queue: 0x%lX\n",
                                           smrval);
                }

                /* RXQ */
                for (i = 0; i < XWPCP_PORT_NUM; i++) {
                        rc = xwosal_smr_getvalue(xwosal_smr_get_id(&usi_xwpcp.rxq.smr[i]),
                                                 &smrval);
                        if (OK == rc) {
                                showcnt += sprintf(&buf[showcnt],
                                                   "RX Queue[%ld]: 0x%lX\n",
                                                   i, smrval);
                        }
                }
        }

        /* End flag */
        buf[showcnt] = '\0';
        return showcnt;
}

static
ssize_t usi_xwpcp_sysfs_state_store(struct xwsys_object * xwobj,
                                    struct xwsys_attribute * soattr,
                                    const char * buf,
                                    size_t count)
{
        return -ENOSYS;
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********    init & exit    ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t usi_xwpcp_init(void)
{
        xwer_t rc;

        usi_xwpcp_sysfs = xwsys_register("xwpcp", NULL, NULL);
        if (__unlikely(is_err_or_null(usi_xwpcp_sysfs))) {
                rc = PTR_ERR(usi_xwpcp_sysfs);
                xwpcplogf(ERR,
                          "Create \"/sys/xwos/xwpcp\" ... [rc:%d]\n",
                          rc);
                goto err_usi_xwpcp_sysfs_create;
        }
        xwpcplogf(INFO, "Create \"/sys/xwos/xwpcp\" ... [OK]\n");

        rc = xwsys_create_file(usi_xwpcp_sysfs, &xwsys_attr_file_xwpcp_cmd);
        if (__unlikely(rc < 0)) {
                xwpcplogf(ERR,
                          "Create \"/sys/xwos/xwpcp/cmd\" ... [rc:%d]\n",
                          rc);
                goto err_usi_xwpcp_sysfs_cmd_create;
        }
        xwpcplogf(INFO, "Create \"/sys/xwos/xwpcp/cmd\" ... [OK]\n");

        rc = xwsys_create_file(usi_xwpcp_sysfs, &xwsys_attr_file_xwpcp_state);
        if (__unlikely(rc < 0)) {
                xwpcplogf(ERR,
                          "Create \"/sys/xwos/xwpcp/state\" ... [rc:%d]\n",
                          rc);
                goto err_usi_xwpcp_sysfs_state_create;
        }
        xwpcplogf(INFO, "Create \"/sys/xwos/xwpcp/state\" ... [OK]\n");
        return OK;

err_usi_xwpcp_sysfs_state_create:
        xwsys_remove_file(usi_xwpcp_sysfs, &xwsys_attr_file_xwpcp_cmd);
err_usi_xwpcp_sysfs_cmd_create:
        xwsys_unregister(usi_xwpcp_sysfs);
        usi_xwpcp_sysfs = NULL;
err_usi_xwpcp_sysfs_create:
        return rc;
}

void usi_xwpcp_exit(void)
{
        xwsys_remove_file(usi_xwpcp_sysfs, &xwsys_attr_file_xwpcp_state);
        xwpcplogf(INFO, "Destory \"/sys/xwos/xwpcp/state\" ... [OK]\n");
        xwsys_remove_file(usi_xwpcp_sysfs, &xwsys_attr_file_xwpcp_cmd);
        xwpcplogf(INFO, "Destory \"/sys/xwos/xwpcp/cmd\" ... [OK]\n");
        xwsys_unregister(usi_xwpcp_sysfs);
        usi_xwpcp_sysfs = NULL;
        xwpcplogf(INFO, "Destory \"/sys/xwos/xwpcp\" ... [OK]\n");
}
