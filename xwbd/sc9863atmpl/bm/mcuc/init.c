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

#include <xwos/standard.h>
#include <linux/version.h>
#include <linux/kconfig.h>
#include <linux/parser.h>
#include <xwos/lib/xwlog.h>
#include <xwos/lib/xwaop.h>
#include <xwmd/sysfs/core.h>
#include <xwmd/xwfs/fs.h>
#include <bdl/board.h>
#include <bm/mcuc/msgnode.h>
#include <bm/mcuc/session.h>
#include <bm/mcuc/wkup.h>
#include <bm/mcuc/init.h>

struct xwfs_dir * xwfs_dir_mcuc = NULL;
xwsq_a mcuc_state = MCUC_STATE_STOP;

xwer_t mcuc_start(void)
{
        xwer_t rc;

        rc = xwaop_teq_then_add(xwsq, &mcuc_state, MCUC_STATE_STOP, 1, NULL, NULL);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_perm;
        }

        rc = xwfs_holdon();
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EOWNERDEAD;
                goto err_xwfs_not_ready;
        }

        rc = xwfs_mkdir("mcuc", NULL, &xwfs_dir_mcuc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mkdir_mcuc;
        }

        rc = mcuc_msgnode_init();
        if (__xwcc_unlikely(rc < 0)) {
                goto err_msgnode_init;
        }

        rc = mcuc_session_init();
        if (__xwcc_unlikely(rc < 0)) {
                goto err_session_init;
        }

        rc = mcuc_wkup_init();
        if (__xwcc_unlikely(rc < 0)) {
                goto err_wkup_init;
        }
        return XWOK;

err_wkup_init:
        mcuc_session_exit();
err_session_init:
        mcuc_msgnode_exit();
err_msgnode_init:
        xwfs_rmdir(xwfs_dir_mcuc);
        xwfs_dir_mcuc = NULL;
err_mkdir_mcuc:
        xwfs_giveup();
err_xwfs_not_ready:
        xwaop_teq_then_sub(xwsq, &mcuc_state, MCUC_STATE_START, 1, NULL, NULL);
err_perm:
        return rc;
}

xwer_t mcuc_stop(void)
{
        xwer_t rc;

        rc = xwaop_teq_then_sub(xwsq, &mcuc_state, MCUC_STATE_START, 1, NULL, NULL);
        if (rc < 0) {
                rc = -EPERM;
                goto err_state;
        }

        mcuc_wkup_exit();
        mcuc_session_exit();
        mcuc_msgnode_exit();

        xwfs_rmdir(xwfs_dir_mcuc);
        xwfs_dir_mcuc = NULL;

        xwfs_giveup();

        return XWOK;

err_state:
        return rc;
}

xwer_t mcuc_grab(void)
{
        xwer_t rc;

        rc = xwaop_tge_then_add(xwsq, &mcuc_state, MCUC_STATE_START, 1,
                                NULL, NULL);
        return rc;
}

xwer_t mcuc_put(void)
{
        xwer_t rc;

        rc = xwaop_tge_then_sub(xwsq, &mcuc_state, MCUC_STATE_START, 1,
                                NULL, NULL);
        return rc;
}

xwsq_t mcuc_get_state(void)
{
        xwsq_t st;

        xwaop_read(xwsq, &mcuc_state, &st);
        return st;
}

struct xwsys_object * mcuc_xwsys_obj;

/******** /sys/xwos/mcuc/cmd ********/
static
ssize_t mcuc_xwsys_cmd_show(struct xwsys_object * xwobj,
                            struct xwsys_attribute * xwattr,
                            char * buf);

static
ssize_t mcuc_xwsys_cmd_store(struct xwsys_object * xwobj,
                             struct xwsys_attribute * xwattr,
                             const char * buf,
                             size_t cnt);

XWSYS_ATTR(file_mcuc_cmd, cmd, 0644,
           mcuc_xwsys_cmd_show,
           mcuc_xwsys_cmd_store);

enum xwfs_xwsys_cmd_em {
        MCUC_XWSYS_CMD_STOP = 0,
        MCUC_XWSYS_CMD_START,
        MCUC_XWSYS_CMD_NUM,
};

static const match_table_t mcuc_cmd_tokens = {
        {MCUC_XWSYS_CMD_STOP, "stop"},
        {MCUC_XWSYS_CMD_START, "start"},
        {MCUC_XWSYS_CMD_NUM, NULL},
};

static
ssize_t mcuc_xwsys_cmd_show(struct xwsys_object * xwobj,
                            struct xwsys_attribute * xwattr,
                            char * buf)
{
        return -ENOSYS;
}

static
xwer_t mcuc_xwsys_cmd_parse(const char * cmdstring, size_t cnt)
{
        int token;
        substring_t tmp[MAX_OPT_ARGS];
        char * p, * pos;
        xwer_t rc = -ENOSYS;
        char cmd[cnt + 1];

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

        mcuclogf(INFO, "cmd:\"%s\"\n", cmd);
        pos = (char *)cmd;
        while ((p = strsep(&pos, ";")) != NULL) {
                if (!*p) {
                        continue;
                }
                token = match_token(p, mcuc_cmd_tokens, tmp);
                switch (token) {
                case MCUC_XWSYS_CMD_STOP:
                        rc = mcuc_stop();
                        break;
                case MCUC_XWSYS_CMD_START:
                        rc = mcuc_start();
                        break;
                }
        }
        return rc;
}

static
ssize_t mcuc_xwsys_cmd_store(struct xwsys_object * xwobj,
                             struct xwsys_attribute * xwattr,
                             const char * buf,
                             size_t cnt)
{
        xwer_t rc;

        rc = mcuc_xwsys_cmd_parse(buf, cnt);
        if (__xwcc_unlikely(rc < 0)) {
                cnt = rc;
        }
        return cnt;
}

/******** /sys/xwos/mcuc/state ********/
static
ssize_t mcuc_xwsys_state_show(struct xwsys_object * xwobj,
                              struct xwsys_attribute * xwattr,
                              char * buf);

static
ssize_t mcuc_xwsys_state_store(struct xwsys_object * xwobj,
                               struct xwsys_attribute * xwattr,
                               const char * buf,
                               size_t cnt);

XWSYS_ATTR(file_mcuc_state, state, 0644,
           mcuc_xwsys_state_show,
           mcuc_xwsys_state_store);

static
ssize_t mcuc_xwsys_state_show(struct xwsys_object * xwobj,
                              struct xwsys_attribute * xwattr,
                              char * buf)
{
        ssize_t showcnt;

        showcnt = 0;
        showcnt += sprintf(&buf[showcnt], "[MCUC]\n");
        if (MCUC_STATE_START == mcuc_get_state()) {
                showcnt += sprintf(&buf[showcnt], "State: ON\n");
        } else {
                showcnt += sprintf(&buf[showcnt], "State: OFF\n");
        }
        buf[showcnt] = '\0';
        return showcnt;
}

static
ssize_t mcuc_xwsys_state_store(struct xwsys_object * xwobj,
                               struct xwsys_attribute * xwattr,
                               const char * buf,
                               size_t cnt)
{
        return -ENOSYS;
}

xwer_t mcuc_init(void)
{
        struct xwsys_object * xwsysobj;
        xwer_t rc;

        xwsysobj = xwsys_register("mcuc", NULL, NULL);
        if (is_err_or_null(xwsysobj)) {
                rc = ptr_err(xwsysobj);
                goto err_xwsys_reg;
        }
        mcuc_xwsys_obj = xwsysobj;

        rc = xwsys_create_file(mcuc_xwsys_obj, &xwsys_attr_file_mcuc_cmd);
        if (__xwcc_unlikely(rc < 0)) {
                mcuclogf(ERR,
                         "Create /sys/xwos/mcuc/cmd ... [FAILED], rc:%d\n",
                         rc);
                goto err_xwsys_create_file_cmd;
        }

        rc = xwsys_create_file(mcuc_xwsys_obj, &xwsys_attr_file_mcuc_state);
        if (__xwcc_unlikely(rc < 0)) {
                mcuclogf(ERR,
                         "Create /sys/xwos/mcuc/state ... [FAILED], rc:%d\n",
                         rc);
                goto err_xwsys_create_file_state;
        }
        return XWOK;

err_xwsys_create_file_state:
        xwsys_remove_file(mcuc_xwsys_obj, &xwsys_attr_file_mcuc_cmd);
err_xwsys_create_file_cmd:
        xwsys_unregister(mcuc_xwsys_obj);
        mcuc_xwsys_obj = NULL;
err_xwsys_reg:
        return rc;
}

void mcuc_exit(void)
{
        xwsys_remove_file(mcuc_xwsys_obj, &xwsys_attr_file_mcuc_state);
        xwsys_remove_file(mcuc_xwsys_obj, &xwsys_attr_file_mcuc_cmd);
        xwsys_unregister(mcuc_xwsys_obj);
        mcuc_xwsys_obj = NULL;
}
