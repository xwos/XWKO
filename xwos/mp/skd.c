/**
 * @file
 * @brief 玄武OS内核：调度器
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
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
  #include <uapi/linux/sched/types.h>
#endif
#include <linux/smp.h>
#include <linux/preempt.h>
#include <xwos/mp/thd.h>
#include <xwos/mp/skd.h>
#include <xwmd/xwfs/fs.h>
#include <xwmd/xwfs/bs.h>

#define LOGTAG "skd"

xwer_t xwmp_skd_start_lc(void)
{
        return XWOK;
}
EXPORT_SYMBOL(xwmp_skd_start_lc);

xwid_t xwmp_skd_id_lc(void)
{
        return smp_processor_id();
}
EXPORT_SYMBOL(xwmp_skd_id_lc);

void xwmp_skd_get_context_lc(xwsq_t * ctxbuf, xwirq_t * irqnbuf)
{
        if (ctxbuf) {
                if (in_interrupt()) {
                        *ctxbuf = XWMP_SKD_CONTEXT_ISR;
                } else {
                        *ctxbuf = XWMP_SKD_CONTEXT_THD;
                }
        }
        if (irqnbuf) {
                *irqnbuf = -ENOSYS;
        }
}
EXPORT_SYMBOL(xwmp_skd_get_context_lc);

xwtm_t xwmp_skd_get_timetick_lc(void)
{
        return (xwtm_t)jiffies_to_nsecs(jiffies);
}
EXPORT_SYMBOL(xwmp_skd_get_timetick_lc);

xwu64_t xwmp_skd_get_tickcount_lc(void)
{
        return (xwu64_t)jiffies;
}
EXPORT_SYMBOL(xwmp_skd_get_tickcount_lc);

xwtm_t xwmp_skd_get_timestamp_lc(void)
{
        struct hrtimer hrt;
        ktime_t now;

        hrtimer_init_on_stack(&hrt, HRTIMER_BASE_MONOTONIC, HRTIMER_MODE_ABS);
        now = hrt.base->get_time();
        destroy_hrtimer_on_stack(&hrt);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,10,0)
        return (xwtm_t)now;
#else
        return (xwtm_t)now.tv64;
#endif
}
EXPORT_SYMBOL(xwmp_skd_get_timestamp_lc);

void xwmp_skd_dspmpt_lc(void)
{
        preempt_disable();
}
EXPORT_SYMBOL(xwmp_skd_dspmpt_lc);

void xwmp_skd_enpmpt_lc(void)
{
        preempt_enable();
}
EXPORT_SYMBOL(xwmp_skd_enpmpt_lc);

bool xwmp_skd_prio_tst_valid(xwpr_t prio)
{
        return !!((prio > 0) && (prio < MAX_PRIO));
}
EXPORT_SYMBOL(xwmp_skd_prio_tst_valid);

xwer_t xwmp_skd_init(void)
{
        xwer_t rc;

        rc = linux_thd_ksym_load();
        if (__xwcc_unlikely(rc < 0)) {
                xwlogf(ERR, LOGTAG,
                       "linux_thd_ksym_load(): error:%d\n",
                       rc);
                goto err_ksymload;
        }
        return XWOK;

err_ksymload:
        return rc;
}

void xwmp_skd_exit(void)
{
}

static
ssize_t xwmp_skd_xwfsnode_priority_write(struct xwfs_node * xwfsnode,
                                         struct file * file,
                                         const char __user * usdata,
                                         size_t count,
                                         loff_t * pos);

struct xwfs_node * xwmp_skd_xwfsnode_priority = NULL;
struct xwfs_dir * xwmp_skd_xwfsdir = NULL;
const struct xwfs_operations xwmp_skd_xwfsnode_priority_ops = {
        .write = xwmp_skd_xwfsnode_priority_write,
};

static
ssize_t xwmp_skd_xwfsnode_priority_write(struct xwfs_node * xwfsnode,
                                         struct file * file,
                                         const char __user * usdata,
                                         size_t count,
                                         loff_t * pos)
{
        char argstring[count];
        char * tidstr, * prstr;
        int tid, pr;
        struct xwmp_thd * thd;
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
        if (__xwcc_unlikely(rc < 0)) {
                ret = (ssize_t)rc;
                goto err_invalcmd;
        }

        rc = (xwer_t)kstrtoint(prstr, 0, &pr);
        if (__xwcc_unlikely(rc < 0)) {
                ret = (ssize_t)rc;
                goto err_invalcmd;
        }
        rc = xwmp_thd_tid2thd((xwsq_t)tid, &thd);
        if (__xwcc_unlikely(rc < 0)) {
                ret = (ssize_t)rc;
                goto err_nosuchtid;
        }
        schparam.sched_priority = pr;
        rc = sched_setscheduler(thd, SCHED_FIFO, &schparam);
        xwmp_thd_put(thd);
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

xwer_t xwmp_skd_xwfs_init(void)
{
        struct xwfs_dir * dir;
        struct xwfs_node * node;
        xwer_t rc;

        rc = xwfs_mkdir("skd", xwfs_dir_mp, &dir);
        xwmp_skd_xwfsdir = dir;
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mknod_skd;
        }

        rc = xwfs_mknod("pr", 0666, &xwmp_skd_xwfsnode_priority_ops, NULL,
                        xwmp_skd_xwfsdir, &node);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mknod_priority;
        }
        xwmp_skd_xwfsnode_priority = node;
        return XWOK;

err_mknod_priority:
        xwfs_rmdir(xwmp_skd_xwfsdir);
        xwmp_skd_xwfsdir = NULL;
err_mknod_skd:
        return rc;
}

void xwmp_skd_xwfs_exit(void)
{
        xwfs_rmdir(xwmp_skd_xwfsdir);
        xwmp_skd_xwfsdir = NULL;
        xwfs_rmnod(xwmp_skd_xwfsnode_priority);
        xwmp_skd_xwfsnode_priority = NULL;
}
