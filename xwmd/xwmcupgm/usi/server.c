/**
 * @file
 * @brief MCU programmer server接口
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
#include <linux/ioctl.h>
#include <xwmd/xwfs/fs.h>
#include <xwmd/xwfs/bs.h>
#include <xwmd/sysfs/core.h>
#include <xwmd/isc/xwscp/api.h>
#include <xwmd/isc/xwscp/usi.h>
#include <xwmd/xwmcupgm/server.h>
#include <xwmd/xwmcupgm/usi/server.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********        xwmcupgmsrv resources        ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** .data ******** ********/
struct xwmcupgmsrv usi_xwmcupgmsrv;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         xwfs/xwmd/xwmcupgm          ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** .data ******** ********/
struct xwfs_dir * usi_xwmcupgmsrv_xwfs_dir = NULL;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      xwfs/xwmd/xwmcupgm/server      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** type ******** ********/

/******** ******** macros ******** ********/
#define UAPI_XWMCUPGMSRV_IOC_MAGIC              'p'

#define UAPI_XWMCUPGMSRV_IOC_AUTHENTICATE       \
        _IOC(_IOC_READ | _IOC_WRITE,            \
             UAPI_XWMCUPGMSRV_IOC_MAGIC,        \
             XWMCUPGM_CMD_AUTH,                 \
             0)

#define UAPI_XWMCUPGMSRV_IOC_START              \
        _IOC(_IOC_READ | _IOC_WRITE,            \
             UAPI_XWMCUPGMSRV_IOC_MAGIC,        \
             XWMCUPGM_CMD_START,                \
             sizeof(xwsz_t))

#define UAPI_XWMCUPGMSRV_IOC_DATA               \
        _IOC(_IOC_READ | _IOC_WRITE,            \
             UAPI_XWMCUPGMSRV_IOC_MAGIC,        \
             XWMCUPGM_FRMT_DATA,                \
             sizeof(struct uapi_xwmcupgmsrv_ioc_data))

#define UAPI_XWMCUPGMSRV_IOC_FINISH             \
        _IOC(_IOC_READ | _IOC_WRITE,            \
             UAPI_XWMCUPGMSRV_IOC_MAGIC,        \
             XWMCUPGM_CMD_FINISH,               \
             sizeof(xwu32_t))

/******** ******** static function prototypes ******** ********/
static
int usi_xwmcupgmsrv_xwfs_open(struct xwfs_node * xwfsnode,
                              struct file * file);

static
long usi_xwmcupgmsrv_xwfs_ioctl(struct xwfs_node * xwfsnode,
                                struct file * file,
                                unsigned int cmd,
                                unsigned long arg);

/******** ******** .data ******** ********/
struct xwfs_node *usi_xwmcupgmsrv_xwfs_node = NULL;

const struct xwfs_operations usi_xwmcupgmsrv_xwfsops = {
        .open = usi_xwmcupgmsrv_xwfs_open,
        .unlocked_ioctl = usi_xwmcupgmsrv_xwfs_ioctl,
};

/******** ******** function implementations ******** ********/
static
int usi_xwmcupgmsrv_xwfs_open(struct xwfs_node * xwfsnode,
                              struct file * file)
{
        struct xwmcupgmsrv *pgmsrv;
        struct xwscp *xwscpcb;
        xwtm_t xwtm;
        xwer_t rc;

        pgmsrv = xwfs_node_get_data(xwfsnode);
        xwscpcb = pgmsrv->ifcb;
        xwtm = XWTM_MAX;
        rc = xwscp_connect(xwscpcb, &xwtm);
        return (int)rc;
}

static
long usi_xwmcupgmsrv_xwfs_ioctl(struct xwfs_node * xwfsnode,
                                struct file * file,
                                unsigned int cmd,
                                unsigned long arg)
{
        xwer_t rc;
        struct uapi_xwmcupgmsrv_ioc_data pgmdata;
        xwu8_t data[XWMDCFG_isc_xwscp_SDU_MAX_SIZE];

        switch (cmd) {
        case UAPI_XWMCUPGMSRV_IOC_AUTHENTICATE:
                rc = xwmcupgmsrv_authenticate(&usi_xwmcupgmsrv);
                break;
        case UAPI_XWMCUPGMSRV_IOC_START:
                rc = xwmcupgmsrv_start(&usi_xwmcupgmsrv, (xwsz_t)arg);
                break;
        case UAPI_XWMCUPGMSRV_IOC_DATA:
                rc = copy_from_user(&pgmdata, (void __user *)arg,
                                    sizeof(struct uapi_xwmcupgmsrv_ioc_data));
                if (pgmdata.size < XWMDCFG_isc_xwscp_SDU_MAX_SIZE) {
                        rc = copy_from_user(data, pgmdata.frm, pgmdata.size);
                        rc = xwmcupgmsrv_tx_dataframe_once(&usi_xwmcupgmsrv,
                                                        data,
                                                        pgmdata.size);
                } else {
                        rc = -E2BIG;
                }
                break;
        case UAPI_XWMCUPGMSRV_IOC_FINISH:
                rc = xwmcupgmsrv_finish(&usi_xwmcupgmsrv, (xwu32_t)arg);
                break;
        default:
                rc = -ENOSYS;
                break;
        }
        return rc;
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********        /sys/xwos/xwmcupgmsrv        ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** .data ******** ********/
struct xwsys_object * usi_xwmcupgmsrv_sysfs_kobj;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     /sys/xwos/xwmcupgmsrv/state     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
ssize_t usi_xwmcupgmsrv_sysfs_state_show(struct xwsys_object * xwobj,
                                         struct xwsys_attribute * soattr,
                                         char * buf);

static
ssize_t usi_xwmcupgmsrv_sysfs_state_store(struct xwsys_object * xwobj,
                                          struct xwsys_attribute * soattr,
                                          const char * buf,
                                          size_t count);

/******** ******** .data ******** ********/
/**
 * @brief entry:/sys/xwosal/xwmcupgmsrv/state
 */
static XWSYS_ATTR(state, 0644,
                  usi_xwmcupgmsrv_sysfs_state_show,
                  usi_xwmcupgmsrv_sysfs_state_store);

/******** ******** function implementations ******** ********/
static
ssize_t usi_xwmcupgmsrv_sysfs_state_show(struct xwsys_object * xwobj,
                                         struct xwsys_attribute * soattr,
                                         char * buf)
{
        return 0;
}

static
ssize_t usi_xwmcupgmsrv_sysfs_state_store(struct xwsys_object * xwobj,
                                          struct xwsys_attribute * soattr,
                                          const char * buf,
                                          size_t count)
{
        return count;
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********     operations    ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** static function prototypes ******** ********/
static
xwer_t usi_xwmcupgmsrv_ops_tx(struct xwmcupgmsrv * xwmcupgmsrv,
                              const void * data,
                              xwsz_t size);

static
xwer_t usi_xwmcupgmsrv_ops_rx(struct xwmcupgmsrv * xwmcupgmsrv,
                              union xwmcupgm_frame * buf);

/******** ******** .data ******** ********/
const struct xwmcupgmsrv_operations usi_xwmcupgmsrv_ops = {
        .tx = usi_xwmcupgmsrv_ops_tx,
        .rx = usi_xwmcupgmsrv_ops_rx,
};

/******** ******** function implementations ******** ********/
static
xwer_t usi_xwmcupgmsrv_ops_tx(struct xwmcupgmsrv * xwmcupgmsrv,
                              const void * data,
                              xwsz_t size)
{
        xwtm_t xwtm;

        xwtm = XWTM_MAX;
        return xwscp_tx(xwmcupgmsrv->ifcb, data, &size, &xwtm);
}

static
xwer_t usi_xwmcupgmsrv_ops_rx(struct xwmcupgmsrv * xwmcupgmsrv,
                              union xwmcupgm_frame * buf)
{
        xwtm_t xwtm;
        xwsz_t size;

        xwtm = XWTM_MAX;
        size = sizeof(union xwmcupgm_frame);
        return xwscp_rx(xwmcupgmsrv->ifcb, (void *)buf, &size, &xwtm);
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********    start & stop   ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** macros ******** ********/

/******** ******** .data ******** ********/
xwsq_t usi_xwmcupgmsrv_state = USI_XWMCUPGMSRV_STATE_STOP;

xwer_t usi_xwmcupgmsrv_start(void)
{
        xwer_t rc;

        if (USI_XWMCUPGMSRV_STATE_START == usi_xwmcupgmsrv_state) {
                rc = -EALREADY;
                goto err_already;
        }

        xwmcupgmsrv_init(&usi_xwmcupgmsrv, &usi_xwmcupgmsrv_ops, &usi_xwscp);

        rc = xwfs_holdon();
        if (__unlikely(rc < 0)) {
                rc = -EOWNERDEAD;
                goto err_xwfs_not_ready;
        }

        rc = xwfs_mkdir("xwmcupgm", dir_xwmd, &usi_xwmcupgmsrv_xwfs_dir);
        if (__unlikely(rc < 0)) {
                xwlogf(ERR, "Fail to mkdir(\"xwmcupgm\"), "
                       "rc: %d\n", rc);
                goto err_mkdir;
        }

        rc = xwfs_mknod("server", 0660, &usi_xwmcupgmsrv_xwfsops, &usi_xwmcupgmsrv,
                        usi_xwmcupgmsrv_xwfs_dir, &usi_xwmcupgmsrv_xwfs_node);
        if (__unlikely(rc < 0)) {
                xwlogf(ERR, "Fail to mknod(\"server\"), rc: %d\n", rc);
                goto err_mknod;
        }

        usi_xwmcupgmsrv_sysfs_kobj = xwsys_register("xwmcupgmsrv", NULL, NULL);
        if (__unlikely(is_err_or_null(usi_xwmcupgmsrv_sysfs_kobj))) {
                rc = PTR_ERR(usi_xwmcupgmsrv_sysfs_kobj);
                xwlogf(ERR,
                       "Create \"/sys/xwosal/xwmcupgmsrv\" ... [Failed], "
                       "rc: %d\n",
                       rc);
                goto err_xwsys_reg;
        }
        xwlogf(INFO, "Create \"/sys/xwosal/xwmcupgmsrv\" ... [OK]\n");

        rc = xwsys_create_file(usi_xwmcupgmsrv_sysfs_kobj, &xwsys_attr_state);
        if (__unlikely(rc < 0)) {
                xwlogf(ERR,
                       "Create \"/sys/xwosal/xwmcupgmsrv/state\" ... [Failed], "
                       "rc: %d\n",
                       rc);
                goto err_xwsys_create;
        }
        xwlogf(INFO, "Create \"/sys/xwosal/xwmcupgmsrv/state\" ... [OK]\n");

        usi_xwmcupgmsrv_state = USI_XWMCUPGMSRV_STATE_START;

        return OK;

err_xwsys_create:
        xwsys_unregister(usi_xwmcupgmsrv_sysfs_kobj);
        usi_xwmcupgmsrv_sysfs_kobj = NULL;
err_xwsys_reg:
        xwfs_rmnod(usi_xwmcupgmsrv_xwfs_node);
        usi_xwmcupgmsrv_xwfs_node = NULL;
err_mknod:
        xwfs_rmdir(usi_xwmcupgmsrv_xwfs_dir);
        usi_xwmcupgmsrv_xwfs_dir = NULL;
err_mkdir:
        xwfs_giveup();
err_xwfs_not_ready:
err_already:
        return rc;
}

xwer_t usi_xwmcupgmsrv_stop(void)
{
        xwer_t rc;

        if (USI_XWMCUPGMSRV_STATE_START != usi_xwmcupgmsrv_state) {
                rc = -EPERM;
                goto err_notstart;
        }

        xwsys_remove_file(usi_xwmcupgmsrv_sysfs_kobj, &xwsys_attr_state);
        xwsys_unregister(usi_xwmcupgmsrv_sysfs_kobj);
        usi_xwmcupgmsrv_sysfs_kobj = NULL;
        xwfs_rmnod(usi_xwmcupgmsrv_xwfs_node);
        usi_xwmcupgmsrv_xwfs_node = NULL;
        xwfs_rmdir(usi_xwmcupgmsrv_xwfs_dir);
        usi_xwmcupgmsrv_xwfs_dir = NULL;
        xwfs_giveup();
        usi_xwmcupgmsrv_state = USI_XWMCUPGMSRV_STATE_STOP;

        return OK;

err_notstart:
        return rc;
}

xwsq_t usi_xwmcupgmsrv_get_state(void)
{
        return usi_xwmcupgmsrv_state;
}
