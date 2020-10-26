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

#include <linux/slab.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/parser.h>
#include <xwos/standard.h>
#include <xwos/lib/xwlog.h>
#include <xwmd/sysfs/core.h>

#define XWSYS_ARGBUFSIZE        32

enum xwsys_state_cmd_em {
        XWSYS_STATE_CMD_START = 0,
        XWSYS_STATE_CMD_STOP,
        XWSYS_STATE_CMD_NUM,
};

/******** ******** xwsys entry ******** ********/
static
int xwsys_uevent_filter(struct kset * kset, struct kobject * kobj);
static
ssize_t xwsys_attr_show(struct kobject * kobj,
                        struct attribute * attr,
                        char * buf);
static
ssize_t xwsys_attr_store(struct kobject * kobj,
                         struct attribute * attr,
                         const char * buf,
                         size_t count);
static
void xwsys_release(struct kobject * kobj);

static __xwcc_inline
struct xwsys_object * xwsys_get(struct xwsys_object * xwobj);

static __xwcc_inline
void xwsys_put(struct xwsys_object * xwobj);

/******** ******** /sys/xwos entry ******** ********/
static const struct kset_uevent_ops xwsys_uevent_ops = {
        .filter = xwsys_uevent_filter,
};

static const struct sysfs_ops xwsys_sysfs_ops = {
        .show = xwsys_attr_show,
        .store = xwsys_attr_store,
};

static struct kobj_type xwsys_ktype = {
        .sysfs_ops = &xwsys_sysfs_ops,
        .release = xwsys_release,
};

/**
 * @brief directory entry: /sys/xwos
 */
static struct kset * xwsys_kset;

/**
 * @brief sysfs bindings for xwos: read syscall
 * @param kobj: (I) kobject
 * @param attr: (I) sysfs attribute file
 * @param buf: (O) buffer related to user buffer of syscall `read'
 * @return error number
 * @retval >= 0: actual size of `read'
 * @retval <0: error code
 */
static
ssize_t xwsys_attr_show(struct kobject * kobj,
                        struct attribute * attr,
                        char * buf)
{
        struct xwsys_attribute * xwattr = TO_XWATTR(attr);
        struct xwsys_object * xwobj = TO_XWOBJ(kobj);
        ssize_t rc = 0;

        if (xwattr->show) {
                rc = xwattr->show(xwobj, xwattr, buf);
        }
        return rc;
}

/**
 * @brief sysfs bindings for xwos: write syscall
 * @param kobj: (I) kobject
 * @param attr: (I) sysfs attribute file
 * @param buf: (I) buffer related to user buffer of syscall `write'
 * @param count: (I) count of `write'
 * @return error number
 * @retval >= 0: actual size of `write'
 * @retval < 0: error code
 */
static
ssize_t xwsys_attr_store(struct kobject * kobj,
                         struct attribute * attr,
                         const char * buf,
                         size_t count)
{
        struct xwsys_attribute * xwattr = TO_XWATTR(attr);
        struct xwsys_object * xwobj = TO_XWOBJ(kobj);
        ssize_t rc = 0;

        if (xwattr->store) {
                rc = xwattr->store(xwobj, xwattr, buf, count);
        }
        return rc;
}

static
void xwsys_release(struct kobject * kobj)
{
        struct xwsys_object * xwobj = xwcc_baseof(kobj, typeof(*xwobj), kset.kobj);
        kfree(xwobj);
}

static __xwcc_inline
struct xwsys_object * xwsys_get(struct xwsys_object *xwobj)
{
        if (xwobj) {
                kset_get(&xwobj->kset);
        } else {
        }
        return xwobj;
}

static __xwcc_inline
void xwsys_put(struct xwsys_object * xwobj)
{
        if (xwobj) {
                kset_put(&xwobj->kset);
        }
}

/**
 * @brief Create a file in /sys/xwos/
 * @param xwobj: (I) xwos sysfs object
 * @param attr: (I) sysfs attribute file to be created
 * @return error number
 * @retval 0: OK
 * @retval < 0: error code
 */
xwer_t xwsys_create_file(struct xwsys_object * xwobj,
                         struct xwsys_attribute * xwattr)
{
        xwer_t rc;
        if (xwsys_get(xwobj)) {
                rc = sysfs_create_file(&xwobj->kset.kobj, &xwattr->attr);
                xwsys_put(xwobj);
        } else {
                rc = -EINVAL;
        }
        return rc;
}

/**
 * @brief Remove file in /sys/xwos
 * @param xwobj: (I) xwos sysfs object
 * @param attr: (I) sysfs attribute file to be removed
 */
void xwsys_remove_file(struct xwsys_object * xwobj,
                       struct xwsys_attribute * xwattr)
{
        if (xwsys_get(xwobj)) {
                sysfs_remove_file(&xwobj->kset.kobj, &xwattr->attr);
                xwsys_put(xwobj);
        }
}

/**
 * @brief Filter the uevent
 * @param kset: (I) kset of kobj
 * @param kobj: (I) kobject to filter
 * @return The result to tell the caller whether to generate an uevent.
 * @retval 1: Don't filter it. Generate an uevent.
 * @retval 0: Filter it. Don't generate an uevent.
 * @node
 * * Helper to decide whether to generate uevent.
 */
static
int xwsys_uevent_filter(struct kset * kset, struct kobject * kobj)
{
        struct kobj_type * ktype = get_ktype(kobj);

        if (ktype == &xwsys_ktype) {
                return 1;
        } else {
                return 0;
        }
}

/**
 * @brief Register a xwobj in sysfs (root: /sys/xwos/)
 * @param name: (I) xwos sysfs object to register
 * @param parent: (I) parent directory
 * @param uevent_ops: (I) uevent operations
 * @return A pointer related to the xwobj
 * @retval 0: OK
 * @retval < 0: error number
 */
struct xwsys_object * xwsys_register(const char * name,
                                     struct xwsys_object * parent,
                                     const struct kset_uevent_ops * uevent_ops)
{
        xwer_t rc;
        struct xwsys_object * xwobj;

        xwobj = kzalloc(sizeof(struct xwsys_object), GFP_KERNEL);
        if (is_err_or_null(xwobj)) {
                xwsyslogf(ERR, "Failed to alloc a xwos sysfs object!\n");
                rc = -ENOMEM;
                goto err_out;
        }

        rc = kobject_set_name(&xwobj->kset.kobj, "%s", name);
        if (rc < 0) {
                xwsyslogf(ERR, "Failed to set kobject name! (errno:%d)\n", rc);
                goto err_free_xwobj;
        }
        if (parent) {
                xwobj->kset.kobj.kset = &parent->kset;
        } else {
                xwobj->kset.kobj.kset = xwsys_kset;
        }
        xwobj->kset.kobj.ktype = &xwsys_ktype;
        if (uevent_ops) {
                xwobj->kset.uevent_ops = uevent_ops;
        } else {
                xwobj->kset.uevent_ops = &xwsys_uevent_ops;
        }

        rc = kset_register(&xwobj->kset);
        if (rc) {
                xwsyslogf(ERR, "Failed to kset_register()! (errno:%d)\n", rc);
                kfree(xwobj->kset.kobj.name);
                xwobj->kset.kobj.name = NULL;
                goto err_free_xwobj;
        }
        return xwobj;

err_free_xwobj:
        kfree(xwobj);
err_out:
        return err_ptr(rc);
}

/**
 * @brief Unregister xwobj in sysfs
 * @param xwobj: (I) The pointer related to the xwobj
 */
void xwsys_unregister(struct xwsys_object * xwobj)
{
        if (!is_err_or_null(xwobj)) {
                kset_unregister(&xwobj->kset);
        }
}

/******** ******** init & exit******** ********/
xwer_t xwsys_init(void)
{
        xwer_t rc;

        xwsys_kset = kset_create_and_add("xwos", &xwsys_uevent_ops, NULL);
        if (is_err_or_null(xwsys_kset)) {
                rc = -ENOMEM;
                xwsyslogf(ERR, "Can't create xwsys kset!\n");
                goto err_out;
        }
        return XWOK;

err_out:
        xwsys_kset = NULL;
        return rc;
}

void xwsys_exit(void)
{
        kset_unregister(xwsys_kset);
        xwsys_kset = NULL;
}
