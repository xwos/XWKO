/**
 * @file
 * @brief 玄武OS内核：内核符号
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
#include <linux/kprobes.h>
#include <xwos/lib/xwlog.h>
#include <xwos/mp/ksym.h>

#define LOGTAG "ksym"


#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
typeof(kallsyms_lookup_name) * xwmp_allsyms_lookup_name = NULL;

int xwmp_ksym_kp_pre_handler(struct kprobe * p, struct pt_regs * regs)
{
	return 0;
}

static struct kprobe xwmp_ksym_kp = {
        .symbol_name = "kallsyms_lookup_name",
};
#else
typeof(kallsyms_lookup_name) * xwmp_allsyms_lookup_name = kallsyms_lookup_name;

#endif

xwer_t xwmp_ksym_init(void)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
        xwer_t rc = -1;

        xwmp_ksym_kp.pre_handler = xwmp_ksym_kp_pre_handler;
        rc = register_kprobe(&xwmp_ksym_kp);
        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "register_kprobe() return %d!\n", rc);
        } else {
                xwlogf(INFO, LOGTAG, "find kallsyms_lookup_name() addr %p!\n",
                       xwmp_ksym_kp.addr);
                xwmp_allsyms_lookup_name = (void *)xwmp_ksym_kp.addr;
                unregister_kprobe(&xwmp_ksym_kp);
        }
        return rc;
#else
        return XWOK;
#endif
}
