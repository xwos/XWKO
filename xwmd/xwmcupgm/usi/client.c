/**
 * @file
 * @brief MCU programmer client接口层
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
#include <xwos/lib/crc32.h>
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
#include <xwmd/xwmcupgm/client.h>
#include <xwmd/xwmcupgm/usi/client.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         /sys/xwos/xwmcupgmc         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** .data ******** ********/
struct xwsys_object * usi_xwmcupgmc_sysfs_kobj;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      /sys/xwos/xwmcupgmc/state      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
ssize_t usi_xwmcupgmc_sysfs_state_show(struct xwsys_object * xwobj,
                                       struct xwsys_attribute * soattr,
                                       char * buf);

static
ssize_t usi_xwmcupgmc_sysfs_state_store(struct xwsys_object * xwobj,
                                     struct xwsys_attribute * soattr,
                                     const char * buf,
                                     size_t count);

/******** ******** .data ******** ********/
/**
 * @brief /sys/xwosal/xwmcupgmc/state entry
 */
static XWSYS_ATTR(file_xwmcupgmc_state, state, 0644,
                  usi_xwmcupgmc_sysfs_state_show,
                  usi_xwmcupgmc_sysfs_state_store);

/******** ******** function implementations ******** ********/

static
ssize_t usi_xwmcupgmc_sysfs_state_show(struct xwsys_object * xwobj,
                                    struct xwsys_attribute * soattr,
                                    char * buf)
{
        return 0;
}

static
ssize_t usi_xwmcupgmc_sysfs_state_store(struct xwsys_object * xwobj,
                                     struct xwsys_attribute * soattr,
                                     const char * buf,
                                     size_t count)
{
        return count;
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********     operations    ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** macros ******** ********/
#define USI_XWMCUPGMC_THREAD_PRIORITY    20

/******** ******** static function prototypes ******** ********/
static
xwer_t usi_xwmcupgmc_ops_init(struct xwmcupgmc * pgmc, char * seedbuf);

static
xwer_t usi_xwmcupgmc_ops_start(struct xwmcupgmc * pgmc);

static
xwer_t usi_xwmcupgmc_ops_tx(struct xwmcupgmc * pgmc, const void * data, xwsz_t size);

static
xwer_t usi_xwmcupgmc_ops_rx(struct xwmcupgmc * pgmc, union xwmcupgm_frame * buf);

static
xwer_t usi_xwmcupgmc_ops_program(struct xwmcupgmc * pgmc,
                                 const union xwmcupgm_frame * frm);

static
xwer_t usi_xwmcupgmc_ops_finish(struct xwmcupgmc * pgmc, xwu32_t crc32);

/******** ******** .data ******** ********/
struct xwmcupgmc usi_xwmcupgmc;

__xwmd_rodata const struct xwmcupgmc_operations usi_xwmcupgmc_ops = {
        .init = usi_xwmcupgmc_ops_init,
        .start = usi_xwmcupgmc_ops_start,
        .tx = usi_xwmcupgmc_ops_tx,
        .rx = usi_xwmcupgmc_ops_rx,
        .pgm = usi_xwmcupgmc_ops_program,
        .finish = usi_xwmcupgmc_ops_finish,
};

xwid_t usi_xwmcupgmc_tid = 0;

xwu8_t *usi_xwmcupgmc_imgbuf;

/******** ******** function implementations ******** ********/
static
xwer_t usi_xwmcupgmc_ops_init(struct xwmcupgmc * pgmc, char * seedbuf)
{
        XWOS_UNUSED(pgmc);
        XWOS_UNUSED(seedbuf); /* use the random value in SRAM as seed. */
        memset(seedbuf, 0, XWMCUPGM_SEED_SIZE); /* use for debug */
        return OK;
}

static
xwer_t usi_xwmcupgmc_ops_start(struct xwmcupgmc * pgmc)
{
        XWOS_UNUSED(pgmc);
        usi_xwmcupgmc_imgbuf = kmalloc(pgmc->size, GFP_KERNEL);
        if (!is_err_or_null(usi_xwmcupgmc_imgbuf)) {
                xwmcupgmlogf(INFO,
                             "start, alloc firmware buffer<%p, %d>\n",
                             usi_xwmcupgmc_imgbuf, pgmc->size);
        }
        return OK;
}

static
xwer_t usi_xwmcupgmc_ops_tx(struct xwmcupgmc * pgmc,
                            const void * data,
                            xwsz_t datasize)
{
        xwtm_t xwtm;

        xwtm = XWTM_MAX;
        return xwscp_tx(pgmc->ifcb, data, &datasize, &xwtm);
}

static
xwer_t usi_xwmcupgmc_ops_rx(struct xwmcupgmc * pgmc,
                            union xwmcupgm_frame * buf)
{
        xwtm_t xwtm;
        xwsz_t size;

        xwtm = XWTM_MAX;
        size = sizeof(union xwmcupgm_frame);
        return xwscp_rx(pgmc->ifcb, (void *)buf, &size, &xwtm);
}

static
xwer_t usi_xwmcupgmc_ops_program(struct xwmcupgmc * pgmc,
                                 const union xwmcupgm_frame * frm)
{
        xwu64_t data;

        data = ((xwu64_t)frm->dfrm.text[0] << 56ULL);
        data |= ((xwu64_t)frm->dfrm.text[1] << 48ULL);
        data |= ((xwu64_t)frm->dfrm.text[2] << 40ULL);
        data |= ((xwu64_t)frm->dfrm.text[3] << 32ULL);
        data |= ((xwu64_t)frm->dfrm.text[4] << 24ULL);
        data |= ((xwu64_t)frm->dfrm.text[5] << 16ULL);
        data |= ((xwu64_t)frm->dfrm.text[6] << 8ULL);
        data |= ((xwu64_t)frm->dfrm.text[7] << 0ULL);
        usi_xwmcupgmc_imgbuf[pgmc->pos + 0] = frm->dfrm.text[0];
        usi_xwmcupgmc_imgbuf[pgmc->pos + 1] = frm->dfrm.text[1];
        usi_xwmcupgmc_imgbuf[pgmc->pos + 2] = frm->dfrm.text[2];
        usi_xwmcupgmc_imgbuf[pgmc->pos + 3] = frm->dfrm.text[3];
        usi_xwmcupgmc_imgbuf[pgmc->pos + 4] = frm->dfrm.text[4];
        usi_xwmcupgmc_imgbuf[pgmc->pos + 5] = frm->dfrm.text[5];
        usi_xwmcupgmc_imgbuf[pgmc->pos + 6] = frm->dfrm.text[6];
        usi_xwmcupgmc_imgbuf[pgmc->pos + 7] = frm->dfrm.text[7];
        xwmcupgmlogf(INFO,
                     "program(size<%d>, data<0x%llX>)\n",
                     data, frm->dfrm.size);
        return OK;
}

static
xwer_t usi_xwmcupgmc_ops_finish(struct xwmcupgmc * pgmc,
                                xwu32_t crc32)
{
        xwu32_t  val;
        xwer_t rc;

        val = xwlib_crc32_calms((xwu8_t *)usi_xwmcupgmc_imgbuf, pgmc->size);
        if (val == crc32) {
                rc = OK;
                xwmcupgmlogf(INFO,
                             "Finish ... [OK], CRC32:0x%X\n",
                             crc32);
        } else {
                rc = -EFAULT;
                xwmcupgmlogf(INFO,
                             "Finish ... [%d], CRC32:<(R)0x%X:(L)0x%X>\n",
                             rc, crc32, val);
        }
        return rc;
}

static
xwer_t usi_xwmcupgmc_thread(void * arg)
{
        xwer_t rc;

        rc = OK;
        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                rc = xwmcupgmc_fsm(&usi_xwmcupgmc);
                if (__unlikely(rc < 0)) {
                        xwmcupgmlogf(INFO,
                                     "xwmcupgmc_fsm() return %d.\n", rc);
                }
                if (XWMCUPGMC_STATE_FINISHED == usi_xwmcupgmc.state) {
                        break;
                }
        }
        if (usi_xwmcupgmc_imgbuf) {
                kfree(usi_xwmcupgmc_imgbuf);
                usi_xwmcupgmc_imgbuf = NULL;
        }
        xwosal_cthrd_wait_exit();
        return rc;
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********    start & stop   ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** macros ******** ********/

/******** ******** .data ******** ********/
xwsq_t usi_xwmcupgmc_state = USI_XWMCUPGMC_STATE_STOP;

xwer_t usi_xwmcupgmc_start(void)
{
        xwid_t tcbd;
        xwer_t rc;

        tcbd = 0;
        if (USI_XWMCUPGMC_STATE_START == usi_xwmcupgmc_state) {
                rc = -EALREADY;
                goto err_already;
        }

        xwmcupgmc_init(&usi_xwmcupgmc, &usi_xwmcupgmc_ops, &usi_xwscp);

        usi_xwmcupgmc_sysfs_kobj = xwsys_register("xwmcupgmc", NULL, NULL);
        if (__unlikely(is_err_or_null(usi_xwmcupgmc_sysfs_kobj))) {
                rc = PTR_ERR(usi_xwmcupgmc_sysfs_kobj);
                xwmcupgmlogf(ERR,
                             "Create \"/sys/xwosal/xwmcupgmc\" ... [Failed], "
                             "errno: %d\n",
                             rc);
                goto err_xwsys_reg;
        }
        xwmcupgmlogf(INFO, "Create \"/sys/xwosal/xwmcupgmc\" ... [OK]\n");

        rc = xwsys_create_file(usi_xwmcupgmc_sysfs_kobj,
                               &xwsys_attr_file_xwmcupgmc_state);
        if (__unlikely(rc < 0)) {
                xwmcupgmlogf(ERR,
                             "Create \"/sys/xwosal/xwmcupgmc/state\" ... [Failed], "
                             " errno: %d",
                             rc);
                goto err_xwsys_create;
        }
        xwmcupgmlogf(INFO, "Create \"/sys/xwosal/xwmcupgmc/state\" ... [OK]\n");

        rc = xwosal_thrd_create(&tcbd, "usi_xwmcupgmc",
                                (xwosal_thrd_f)usi_xwmcupgmc_thread,
                                NULL, XWOS_UNUSED_ARGUMENT,
                                USI_XWMCUPGMC_THREAD_PRIORITY,
                                XWOS_UNUSED_ARGUMENT);
        if (__unlikely(rc < 0)) {
                xwmcupgmlogf(ERR,
                             "Create xwmcupgmc thread ... [Failed], errno %d\n",
                             rc);
                goto err_xwmcupgmc_thread_create;
        }
        usi_xwmcupgmc_tid = tcbd;
        xwmcupgmlogf(INFO, "Create xwscp thread ... [OK]\n");

        usi_xwmcupgmc_state = USI_XWMCUPGMC_STATE_START;
        return OK;

err_xwmcupgmc_thread_create:
        xwsys_remove_file(usi_xwmcupgmc_sysfs_kobj,
                          &xwsys_attr_file_xwmcupgmc_state);
err_xwsys_create:
        xwsys_unregister(usi_xwmcupgmc_sysfs_kobj);
        usi_xwmcupgmc_sysfs_kobj = NULL;
err_xwsys_reg:
err_already:
        return rc;
}

xwer_t usi_xwmcupgmc_stop(void)
{
        xwer_t rc;

        if (USI_XWMCUPGMC_STATE_START != usi_xwmcupgmc_state) {
                rc = -EPERM;
                goto err_notstart;
        }

        rc = xwosal_thrd_terminate(usi_xwmcupgmc_tid, &rc);
        if (OK == rc) {
                rc = xwosal_thrd_delete(usi_xwmcupgmc_tid);
                if (OK == rc) {
                        usi_xwmcupgmc_tid = 0;
                        xwmcupgmlogf(INFO, "Terminate xwmcupgmc thread... [OK]\n");
                }
        }

        xwsys_remove_file(usi_xwmcupgmc_sysfs_kobj,
                          &xwsys_attr_file_xwmcupgmc_state);
        xwsys_unregister(usi_xwmcupgmc_sysfs_kobj);
        usi_xwmcupgmc_sysfs_kobj = NULL;
        usi_xwmcupgmc_state = USI_XWMCUPGMC_STATE_STOP;

        return OK;

err_notstart:
        return rc;
}

xwsq_t usi_xwmcupgmc_get_state(void)
{
        return usi_xwmcupgmc_state;
}
