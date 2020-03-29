/**
 * @file
 * @brief XuanWuOS内核：调度器
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
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
  #include <uapi/linux/sched/types.h>
#endif
#include <xwmd/xwfs/fs.h>
#include <xwmd/xwfs/bs.h>
#include <xwos/core/thread.h>
#include <xwos/core/scheduler.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********   XWOS scheduler  ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t xwos_scheduler_start_lc(void)
{
        return OK;
}
EXPORT_SYMBOL(xwos_scheduler_start_lc);

xwtm_t xwos_scheduler_get_timetick_lc(void)
{
        return (xwtm_t)jiffies_to_nsecs(jiffies);
}
EXPORT_SYMBOL(xwos_scheduler_get_timetick_lc);

xwu64_t xwos_scheduler_get_tickcount_lc(void)
{
        return (xwu64_t)jiffies;
}
EXPORT_SYMBOL(xwos_scheduler_get_tickcount_lc);

xwtm_t xwos_scheduler_get_timestamp_lc(void)
{
        struct hrtimer hrt;
        ktime_t now;

        hrtimer_init_on_stack(&hrt, HRTIMER_BASE_MONOTONIC, HRTIMER_MODE_ABS);
        now = hrt.base->get_time();
        /* destroy_hrtimer_on_stack(&hrt); */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,10,0)
        return (xwtm_t)now;
#else
        return (xwtm_t)now.tv64;
#endif
}
EXPORT_SYMBOL(xwos_scheduler_get_timestamp_lc);

void xwos_scheduler_dspmpt_lc(void)
{
        preempt_disable();
}
EXPORT_SYMBOL(xwos_scheduler_dspmpt_lc);

void xwos_scheduler_enpmpt_lc(void)
{
        preempt_enable();
}
EXPORT_SYMBOL(xwos_scheduler_enpmpt_lc);

bool xwos_scheduler_prio_tst_valid(xwpr_t prio)
{
        return !!((prio > 0) && (prio < MAX_PRIO));
}
EXPORT_SYMBOL(xwos_scheduler_prio_tst_valid);

xwer_t xwos_scheduler_init(void)
{
        xwer_t rc;

        rc = linux_thrd_ksym_load();
        if (__unlikely(rc < 0)) {
                xwlogf(ERR,
                       "linux_thrd_ksym_load(): faild! <rc:%d>\n",
                       rc);
                goto err_ksymload;
        }
        return OK;

err_ksymload:
        return rc;
}

void xwos_scheduler_exit(void)
{
        linux_thrd_ksym_unload();
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       XWFS        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** static function prototypes ******** ********/
static
ssize_t xwos_scheduler_xwfsnode_priority_write(struct xwfs_node * xwfsnode,
                                               struct file * file,
                                               const char __user * usdata,
                                               size_t count,
                                               loff_t * pos);

/******** ******** .data ******** ********/
struct xwfs_node * xwos_scheduler_xwfsnode_priority = NULL;
struct xwfs_dir * xwos_scheduler_xwfsdir = NULL;
const struct xwfs_operations xwos_scheduler_xwfsnode_priority_ops = {
        .write = xwos_scheduler_xwfsnode_priority_write,
};

/******** ******** function implementations ******** ********/
static
ssize_t xwos_scheduler_xwfsnode_priority_write(struct xwfs_node * xwfsnode,
                                               struct file * file,
                                               const char __user * usdata,
                                               size_t count,
                                               loff_t * pos)
{
        char argstring[count];
        char *tidstr, *prstr;
        int tid, pr;
        struct xwos_tcb *tcb;
        struct sched_param schparam;
        xwer_t rc;
        ssize_t ret;

        rc = copy_from_user(argstring, usdata, count);
        prstr = (char *)argstring;
        tidstr = strsep(&prstr, ":");
        if (!((NULL != prstr) && (NULL != tidstr) && ('\0' != (*prstr)))) {
                ret = (ssize_t)-EINVAL;
                goto err_null;
        }

        rc = (xwer_t)kstrtoint(tidstr, 0, &tid);
        if (__unlikely(rc < 0)) {
                ret = (ssize_t)rc;
                goto err_invalcmd;
        }

        rc = (xwer_t)kstrtoint(prstr, 0, &pr);
        if (__unlikely(rc < 0)) {
                ret = (ssize_t)rc;
                goto err_invalcmd;
        }
        rc = xwos_thrd_get_tcb_by_tid((xwid_t)tid, &tcb);
        if (__unlikely(rc < 0)) {
                ret = (ssize_t)rc;
                goto err_nosuchtid;
        }
        schparam.sched_priority = pr;
        rc = sched_setscheduler(tcb, SCHED_FIFO, &schparam);
        xwos_thrd_put(tcb);
        if (rc < 0) {
                ret = count;
        } else {
                ret = count;
        }
err_nosuchtid:
err_invalcmd:
err_null:
        return ret;
}

xwer_t xwos_scheduler_xwfs_init(void)
{
        struct xwfs_dir *dir;
        struct xwfs_node *node;
        xwer_t rc;

        rc = xwfs_mkdir("scheduler", dir_core, &dir);
        xwos_scheduler_xwfsdir = dir;
        if (__unlikely(rc < 0)) {
                goto err_mknod_scheduler;
        }

        rc = xwfs_mknod("pr", 0666, &xwos_scheduler_xwfsnode_priority_ops, NULL,
                        xwos_scheduler_xwfsdir, &node);
        if (__unlikely(rc < 0)) {
                goto err_mknod_priority;
        }
        xwos_scheduler_xwfsnode_priority = node;
        return OK;

err_mknod_priority:
        xwfs_rmdir(xwos_scheduler_xwfsdir);
        xwos_scheduler_xwfsdir = NULL;
err_mknod_scheduler:
        return rc;
}

void xwos_scheduler_xwfs_exit(void)
{
        xwfs_rmdir(xwos_scheduler_xwfsdir);
        xwos_scheduler_xwfsdir = NULL;
        xwfs_rmnod(xwos_scheduler_xwfsnode_priority);
        xwos_scheduler_xwfsnode_priority = NULL;
}
