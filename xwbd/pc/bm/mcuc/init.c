/**
 * @file
 * @brief MCU通讯模块：初始化
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
#include <xwos/osal/thread.h>
#include <xwmd/sysfs/core.h>
#include <xwmd/xwfs/fs.h>
#include <bdl/board.h>
#include <bm/mcuc/imitator.h>
#include <bm/mcuc/msgnode.h>
#include <bm/mcuc/session.h>
#include <bm/mcuc/wkup.h>
#include <bm/mcuc/init.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********     xwfs/mcuc/     ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** .data ******** ********/
struct xwfs_dir * dir_mcuc = NULL;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********    start & stop   ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** .data ******** ********/
__xwcc_atomic xwsq_t mcuc_state = MCUC_STATE_STOP;

/******** ******** function implementations ******** ********/
xwer_t mcuc_start(const char * cmdstring)
{
        xwer_t rc;
        char * identifier, * arg;

        rc = xwaop_teq_then_add(xwsq_t, &mcuc_state, MCUC_STATE_STOP, 1,
                                NULL, NULL);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_perm;
        }

        arg = (char *)cmdstring;
        identifier = strsep(&arg, ",");
        if (arg) {
                if (0 == strcmp(arg, "imitator")) {
                        mcuc_imitator = true;
                }
        }

        rc = xwfs_holdon();
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EOWNERDEAD;
                goto err_xwfs_not_ready;
        }

        rc = xwfs_mkdir("mcuc", NULL, &dir_mcuc);
        if (__xwcc_unlikely(rc < 0))
                goto err_mkdir_mcuc;

        rc = mcuc_msgnode_init();
        if (__xwcc_unlikely(rc < 0))
                goto err_msgnode_init;

        rc = mcuc_session_init();
        if (__xwcc_unlikely(rc < 0))
                goto err_session_init;

        rc = mcuc_wkup_init();
        if (__xwcc_unlikely(rc < 0))
                goto err_wkup_init;

        if (mcuc_imitator) {
                rc = mcuc_imitator_init();
                if (__xwcc_unlikely(rc < 0))
                        goto err_mcuc_imitator_start;
        }
        return XWOK;

err_mcuc_imitator_start:
        mcuc_wkup_exit();
err_wkup_init:
        mcuc_session_exit();
err_session_init:
        mcuc_msgnode_exit();
err_msgnode_init:
        xwfs_rmdir(dir_mcuc);
        dir_mcuc = NULL;
err_mkdir_mcuc:
        xwfs_giveup();
err_xwfs_not_ready:
        mcuc_imitator = false;
err_perm:
        return rc;
}

xwer_t mcuc_stop(void)
{
        xwer_t rc;

        rc = xwaop_teq_then_sub(xwsq_t, &mcuc_state, MCUC_STATE_START, 1,
                                NULL, NULL);
        if (rc < 0) {
                rc = -EPERM;
                goto err_state;
        }

        if (mcuc_imitator) {
                mcuc_imitator_exit();
                mcuc_imitator = false;
        }

        mcuc_wkup_exit();

        mcuc_session_exit();

        mcuc_msgnode_exit();

        xwfs_rmdir(dir_mcuc);
        dir_mcuc = NULL;

        xwfs_giveup();

        return XWOK;

err_state:
        return rc;
}

xwer_t mcuc_grab(void)
{
        xwer_t rc;

        rc = xwaop_tge_then_add(xwsq_t, &mcuc_state, MCUC_STATE_START, 1,
                                NULL, NULL);
        return rc;
}

xwer_t mcuc_put(void)
{
        xwer_t rc;

        rc = xwaop_tge_then_sub(xwsq_t, &mcuc_state, MCUC_STATE_START, 1,
                                NULL, NULL);
        return rc;
}

xwsq_t mcuc_get_state(void)
{
        xwsq_t st;

        xwaop_read(xwsq_t, &mcuc_state, &st);
        return st;
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********   /sys/mcuc/state  ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwsys_object * xwsys_mcuc = NULL;

/******** ******** static function declarations ******** ********/
static
ssize_t xwsys_mcuc_state_show(struct xwsys_object * xwobj,
                              struct xwsys_attribute * soattr,
                              char * buf);

static
ssize_t xwsys_mcuc_state_store(struct xwsys_object * xwobj,
                               struct xwsys_attribute * soattr,
                               const char * buf,
                               size_t count);

/******** ******** .data ******** ********/
XWSYS_ATTR(file_mcuc_state, state, 0644,
           xwsys_mcuc_state_show,
           xwsys_mcuc_state_store);

/******** ******** function implementations ******** ********/
static
ssize_t xwsys_mcuc_state_show(struct xwsys_object * xwobj,
                              struct xwsys_attribute * soattr,
                              char * buf)
{
        ssize_t count;

        if (MCUC_STATE_START == mcuc_get_state()) {
                count = sprintf(buf, "start\n");
        } else {
                count = sprintf(buf, "stop\n");
        }
        return count;
}

static
ssize_t xwsys_mcuc_state_store(struct xwsys_object * xwobj,
                               struct xwsys_attribute * soattr,
                               const char * buf,
                               size_t count)
{
        xwer_t rc;
        ssize_t ret;

        rc = -ENOSYS;
        if (0 == strncmp(buf, "start", sizeof("start") - 1)) {
                rc = mcuc_start(buf);
        } else if (0 == strcmp(buf, "stop")) {
                rc = mcuc_stop();
        } else {
        }
        if (rc < 0) {
                ret = rc;
        } else {
                ret = count;
        }
        return ret;
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********     init & exit   ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t mcuc_init(void)
{
        struct xwsys_object * xwsysobj;
        xwer_t rc;

        xwsysobj = xwsys_register("mcuc", NULL, NULL);
        if (is_err_or_null(xwsysobj)) {
                rc = ptr_err(xwsysobj);
                goto err_xwsys_reg;
        }
        xwsys_mcuc = xwsysobj;

        rc = xwsys_create_file(xwsys_mcuc, &xwsys_attr_file_mcuc_state);
        if (__xwcc_unlikely(rc < 0)) {
                mcuclogf(ERR,
                         "Create /sys/xwos/mcuc/state ... [FAILED], rc:%d\n",
                         rc);
                goto err_xwsys_create_file_state;
        }
        return XWOK;

err_xwsys_create_file_state:
        xwsys_unregister(xwsys_mcuc);
        xwsys_mcuc = NULL;
err_xwsys_reg:
        return rc;
}

void mcuc_exit(void)
{
        xwsys_remove_file(xwsys_mcuc, &xwsys_attr_file_mcuc_state);
        xwsys_unregister(xwsys_mcuc);
        xwsys_mcuc = NULL;
}
