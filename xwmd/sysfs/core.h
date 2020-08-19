/**
 * @file
 * @brief XWOS在/sys中的接口
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

#ifndef __xwmd_sysfs_core_h__
#define __xwmd_sysfs_core_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define xwsyslogf(lv, fmt, ...)    xwlogf(lv, "xwsys", fmt, ##__VA_ARGS__)

#define KOBJ_ATTR(_sym, _name, _show, _store) \
        static struct kobj_attribute kobj_attr_##_sym = \
               __ATTR(_name, 0644, _show, _store)

#define XWSYS_ATTR(_sym, _name, _mode, _show, _store)    \
        struct xwsys_attribute xwsys_attr_##_sym = \
               __ATTR(_name, _mode, _show, _store)

#define XWSYS_ATTR_RW(_sym, _name) \
        struct xwsys_attribute xwsys_attr_##_sym = __ATTR_RW(_name)

#define XWSYS_ATTR_RO(_sym, _name) \
        struct xwsys_attribute xwsys_attr_##_sym = __ATTR_RO(_name)

#define TO_XWATTR(_attr) xwcc_baseof(_attr, struct xwsys_attribute, attr)

#define TO_XWOBJ(obj) xwcc_baseof(obj, struct xwsys_object, kset.kobj)
/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwsys_object {
        struct kset kset; /**< kset of object */
};

struct xwsys_attribute {
        struct attribute attr; /**< attribute file */
        xwptr_t private_data; /**< private data */
        ssize_t (*show)(struct xwsys_object *, struct xwsys_attribute *,
                        char *); /**< read */
        ssize_t (*store)(struct xwsys_object *, struct xwsys_attribute *,
                         const char *, size_t ); /**< write */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t xwsys_init(void);

void xwsys_exit(void);

struct xwsys_object * xwsys_register(const char * name, struct xwsys_object * parent,
                                     const struct kset_uevent_ops * uevent_ops);

void xwsys_unregister(struct xwsys_object * xwobj);

xwer_t xwsys_create_file(struct xwsys_object * xwobj, struct xwsys_attribute * xwattr);

void xwsys_remove_file(struct xwsys_object * xwobj, struct xwsys_attribute * xwattr);

#endif /* xwmd/sysfs/core.h */
