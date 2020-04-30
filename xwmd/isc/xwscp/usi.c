/**
 * @file
 * @brief Linux内核中XWSCP的适配层
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
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <linux/slab.h>
#include <linux/parser.h>
#include <linux/uaccess.h>
#include <xwmd/xwfs/fs.h>
#include <xwmd/xwfs/bs.h>
#include <xwmd/sysfs/core.h>
#include <xwmd/isc/xwscp/hwifal.h>
#include <xwmd/isc/xwscp/protocol.h>
#include <bdl/isc/xwscpif.h>
#include <xwmd/isc/xwscp/usi.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********            XWSCP resrouces            ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** macros ******** ********/
#define USI_XWSCP_THRD_PRIORITY          \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 20)

/******** ******** .data ******** ********/
struct xwscp usi_xwscp;

xwid_t usi_xwscp_thrd = 0;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********          xwfs/xwmd/isc/xwscp          ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** .data ******** ********/
struct xwfs_dir * usi_xwscp_xwfs = NULL;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********        xwfs/xwmd/isc/xwscp/port       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** static function prototypes ******** ********/
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

/******** ******** .data ******** ********/
struct xwfs_node * usi_xwscp_xwfs_port = NULL;

const struct xwfs_operations usi_xwscp_xwfs_port_xwfsops = {
        .read = usi_xwscp_xwfs_port_read,
        .write = usi_xwscp_xwfs_port_write,
};

/******** ******** function implementations ******** ********/
static
ssize_t usi_xwscp_xwfs_port_read(struct xwfs_node * xwfsnode,
                                 struct file * file,
                                 char __user * usbuf,
                                 size_t count,
                                 loff_t * pos)
{
        ssize_t rdcnt;
        xwtm_t desired;
        xwsz_t bufcnt;
        xwer_t rc;
        uint8_t buf[count];

        desired = XWTM_MAX;
        bufcnt = count;
        rc = xwscp_rx(&usi_xwscp, buf, &bufcnt, &desired);
        if (OK == rc) {
                rdcnt = bufcnt;
                rc = copy_to_user(usbuf, buf, rdcnt);
        } else {
                rdcnt = (ssize_t)rc;
        }
        return rdcnt;
}

static
ssize_t usi_xwscp_xwfs_port_write(struct xwfs_node *xwfsnode,
                                  struct file *file,
                                  const char __user *usdata,
                                  size_t count,
                                  loff_t *pos)
{
        xwtm_t desired;
        xwer_t rc;
        xwsz_t datasz;
        uint8_t data[count];

        if (usdata) {
                rc = copy_from_user(data, usdata, count);
                datasz = count;
                desired = XWTM_MAX;
                rc = xwscp_tx(&usi_xwscp, data, &datasz, &desired);
                if (__unlikely(rc < 0)) {
                        count = (ssize_t)rc;
                }
        } else {
                desired = XWTM_MAX;
                rc = xwscp_connect(&usi_xwscp, &desired);
                count = rc;
        }
        return count;
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********           /sys/xwosal/xwscp           ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** .data ******** ********/
struct xwsys_object * usi_xwscp_sysfs;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         /sys/xwosal/xwscp/cmd         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
ssize_t usi_xwscp_sysfs_cmd_show(struct xwsys_object *xwobj,
                                 struct xwsys_attribute *soattr,
                                 char *buf);

static
ssize_t usi_xwscp_sysfs_cmd_store(struct xwsys_object *xwobj,
                                  struct xwsys_attribute *soattr,
                                  const char *buf,
                                  size_t count);

/******** ******** .data ******** ********/
/**
 * @brief /sys/xwos/xwscp/cmd entry
 */
static XWSYS_ATTR(cmd, 0644,
                  usi_xwscp_sysfs_cmd_show,
                  usi_xwscp_sysfs_cmd_store);

/******** ******** function implementations ******** ********/

static
ssize_t usi_xwscp_sysfs_cmd_show(struct xwsys_object *xwobj,
                                 struct xwsys_attribute *soattr,
                                 char *buf)
{
        return 0;
}

static
ssize_t usi_xwscp_sysfs_cmd_store(struct xwsys_object *xwobj,
                                  struct xwsys_attribute *soattr,
                                  const char *buf,
                                  size_t count)
{
        xwer_t rc = -ENOSYS;

        if (0 == strcmp(buf, "export")) {
                rc = xwfs_mknod("port", 0660, &usi_xwscp_xwfs_port_xwfsops,
                                NULL, usi_xwscp_xwfs, &usi_xwscp_xwfs_port);
                if (__unlikely(rc < 0)) {
                        xwscplogf(ERR, "Fail to mknod(\"port\"), rc: %d\n", rc);
                }
        } else if (0 == strcmp(buf, "unexport")) {
                rc = xwfs_rmnod(usi_xwscp_xwfs_port);
                usi_xwscp_xwfs_port = NULL;
        }
        if (rc < 0) {
                count = rc;
        }
        return count;
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********    start & stop   ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** macros ******** ********/

/******** ******** .data ******** ********/
xwsq_t usi_xwscp_state = USI_XWSCP_STATE_STOP;

/**
 * @brief Get XWSCP state
 */
xwsq_t usi_xwscp_get_state(void)
{
        return usi_xwscp_state;
}

xwer_t usi_xwscp_start(void)
{
        xwer_t rc;
        xwid_t tcbd;

        tcbd = 0;
        if (USI_XWSCP_STATE_START == usi_xwscp_state) {
                rc = -EALREADY;
                goto err_already;
        }

        rc = xwfs_holdon();
        if (__unlikely(rc < 0)) {
                rc = -EOWNERDEAD;
                goto err_xwfs_not_ready;
        }

        rc = xwfs_mkdir("xwscp", dir_isc, &usi_xwscp_xwfs);
        if (__unlikely(rc < 0)) {
                xwscplogf(ERR, "Fail to mkdir(\"xwscp\"), rc: %d\n", rc);
                goto err_mkdir_usi_xwscp_xwfs;
        }

        usi_xwscp_sysfs = xwsys_register("xwscp", NULL, NULL);
        if (__unlikely(is_err_or_null(usi_xwscp_sysfs))) {
                rc = PTR_ERR(usi_xwscp_sysfs);
                xwscplogf(ERR,
                        "Create \"/sys/xwosal/xwscp\" ... [Failed], errno: %d\n",
                        rc);
                goto err_usi_xwscp_sysfs_create;
        }
        xwscplogf(INFO, "Create \"/sys/xwosal/xwscp\" ... [OK]\n");

        rc = xwsys_create_file(usi_xwscp_sysfs, &xwsys_attr_cmd);
        if (__unlikely(rc < 0)) {
                xwscplogf(ERR,
                        "Create \"/sys/xwosal/xwscp/cmd\" ... [Failed], errno: %d\n",
                        rc);
                goto err_usi_xwscp_sysfs_cmd_create;
        }
        xwscplogf(INFO, "Create \"/sys/xwosal/xwscp/cmd\" ... [OK]\n");

        xwscp_init(&usi_xwscp);
        rc = xwscp_start(&usi_xwscp, "usi_xwscp", &bdl_xwscpif_ops);
        if (__unlikely(rc < 0)) {
                xwscplogf(ERR, "Activate xwscp ... [Failed], errno: %d\n", rc);
                goto err_xwscp_start;
        }
        xwscplogf(INFO, "Activate xwscp ... [OK]\n");

        rc = xwscp_hwifal_open(&usi_xwscp);
        if (__unlikely(rc < 0)) {
                xwscplogf(ERR, "Open xwscp hwifal ... [Failed], errno: %d\n", rc);
                goto err_xwscp_ifopen;
        }
        xwscplogf(INFO, "Open xwscp hwifal ... [OK]\n");

        rc = xwosal_thrd_create(&tcbd, "xwscp_thread",
                                (xwosal_thrd_f)xwscp_thrd,
                                &usi_xwscp, XWOS_UNUSED_ARGUMENT,
                                USI_XWSCP_THRD_PRIORITY,
                                XWOS_UNUSED_ARGUMENT);
        if (__unlikely(rc < 0)) {
                xwscplogf(ERR, "Create xwscp tx thread ... [Failed], errno %d\n", rc);
                goto err_xwscp_thread_create;
        }
        usi_xwscp_thrd = tcbd;
        xwscplogf(INFO, "Create xwscp daemon thread ... [OK]\n");

        usi_xwscp_state = USI_XWSCP_STATE_START;

        return OK;

err_xwscp_thread_create:
        xwscp_hwifal_close(&usi_xwscp);
err_xwscp_ifopen:
        xwscp_stop(&usi_xwscp);
err_xwscp_start:
        xwsys_remove_file(usi_xwscp_sysfs, &xwsys_attr_cmd);
err_usi_xwscp_sysfs_cmd_create:
        xwsys_unregister(usi_xwscp_sysfs);
        usi_xwscp_sysfs = NULL;
err_usi_xwscp_sysfs_create:
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

        rc = xwosal_thrd_terminate(usi_xwscp_thrd, &rc);
        if (OK == rc) {
                rc = xwosal_thrd_delete(usi_xwscp_thrd);
                if (OK == rc) {
                        usi_xwscp_thrd = 0;
                        xwscplogf(INFO, "Terminate xwscp thread... [OK]\n");
                }
        }

        rc = xwscp_hwifal_close(&usi_xwscp);
        if (OK == rc) {
                xwscplogf(INFO, "close xwscp hwifal ... [OK]\n");
        }

        rc = xwscp_stop(&usi_xwscp);
        if (OK == rc) {
                xwscplogf(INFO, "stop xwscp ... [OK]\n");
        }

        xwscplogf(INFO, "kfree mempool ... [OK]\n");

        xwsys_remove_file(usi_xwscp_sysfs, &xwsys_attr_cmd);
        xwsys_unregister(usi_xwscp_sysfs);
        usi_xwscp_sysfs = NULL;
        xwscplogf(INFO, "destory \"/sys/xwosal/xwscp\" ... [OK]\n");

        xwfs_rmdir(usi_xwscp_xwfs);
        usi_xwscp_xwfs = NULL;
        xwfs_giveup();
        usi_xwscp_state = USI_XWSCP_STATE_STOP;

        return OK;

err_notstart:
        return rc;
}
