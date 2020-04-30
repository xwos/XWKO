/**
 * @file
 * @brief XWOS在/sys中的接口
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
#include <linux/slab.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/parser.h>
#include <xwmd/isc/xwpcp/usi.h>
#include <xwmd/isc/xwscp/usi.h>
#include <xwmd/xwmcupgm/usi/server.h>
#include <xwmd/xwmcupgm/usi/client.h>
#include <xwmd/xwfs/fs.h>
#include <xwmd/sysfs/core.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWSYS_ARGBUFSIZE        32

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
enum xwsys_state_cmd_em {
        XWSYS_STATE_CMD_START = 0,
        XWSYS_STATE_CMD_STOP,
        XWSYS_STATE_CMD_NUM,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** xwsys entry ******** ********/
static
int xwsys_uevent_filter(struct kset *kset, struct kobject *kobj);
static
ssize_t xwsys_attr_show(struct kobject *kobj,
                        struct attribute *attr,
                        char *buf);
static
ssize_t xwsys_attr_store(struct kobject *kobj,
                         struct attribute *attr,
                         const char *buf,
                         size_t count);
static
void xwsys_release(struct kobject *kobj);

static __xw_inline
struct xwsys_object *xwsys_get(struct xwsys_object *xwobj);

static __xw_inline
void xwsys_put(struct xwsys_object *xwobj);

/******** ******** /sys/xwos/state ******** ********/
static
ssize_t xwsys_attr_state_show(struct kobject *kobj,
                              struct kobj_attribute *attr,
                              char *buf);

static
xwer_t xwsys_attr_state_parse_cmd(const char *cmdstring);

static
ssize_t xwsys_attr_state_store(struct kobject *kobj,
                               struct kobj_attribute *attr,
                               const char *buf,
                               size_t count);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********   .sdata & .data  ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
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
 * @brief /sys/xwos entry
 */
static struct kset *xwsys_kset;

/******** ******** /sys/xwos/state entry ******** ********/
/**
 * @brief /sys/xwos/state entry
 */
KOBJ_ATTR(state, xwsys_attr_state_show, xwsys_attr_state_store);

static const match_table_t xwsys_state_cmd_tokens = {
	{XWSYS_STATE_CMD_START, "start=%s"},
	{XWSYS_STATE_CMD_STOP, "stop=%s"},
	{XWSYS_STATE_CMD_NUM, NULL},
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief sysfs bindings for xwos: read syscall
 * @param kobj: (I) kobject
 * @param attr: (I) sysfs attribute file
 * @param buf: (O) buffer related to user buffer of syscall `read'
 * @retval >= 0: actual size of `read'
 * @retval errno: error code
 */
static
ssize_t xwsys_attr_show(struct kobject *kobj,
                        struct attribute *attr,
                        char *buf)
{
        struct xwsys_attribute *soattr = TO_SOATTR(attr);
        struct xwsys_object *xwobj = TO_SOOBJ(kobj);
        ssize_t rc = 0;

        if (soattr->show) {
                rc = soattr->show(xwobj, soattr, buf);
        }
        return rc;
}

/**
 * @brief sysfs bindings for xwos: write syscall
 * @param kobj: (I) kobject
 * @param attr: (I) sysfs attribute file
 * @param buf: (I) buffer related to user buffer of syscall `write'
 * @param count: (I) count of `write'
 * @retval actual size of `write'
 * @retval errno: error code
 */
static
ssize_t xwsys_attr_store(struct kobject *kobj,
                         struct attribute *attr,
                         const char *buf,
                         size_t count)
{
        struct xwsys_attribute *soattr = TO_SOATTR(attr);
        struct xwsys_object *xwobj = TO_SOOBJ(kobj);
        ssize_t rc = 0;

        if (soattr->store) {
                rc = soattr->store(xwobj, soattr, buf, count);
        }
        return rc;
}

static
void xwsys_release(struct kobject *kobj)
{
        struct xwsys_object *xwobj = container_of(kobj, typeof(*xwobj), kset.kobj);
        kfree(xwobj);
}

static __xw_inline
struct xwsys_object *xwsys_get(struct xwsys_object *xwobj)
{
        if (xwobj) {
                kset_get(&xwobj->kset);
        } else {
        }
        return xwobj;
}

static __xw_inline
void xwsys_put(struct xwsys_object *xwobj)
{
        if (xwobj) {
                kset_put(&xwobj->kset);
        }
}

/**
 * @brief Create file in /sys/xwos
 * @param xwobj: (I) xwos sysfs object
 * @param attr: (I) attribute file to create
 * @retval 0: OK
 * @retval errno: error code
 */
xwer_t xwsys_create_file(struct xwsys_object *xwobj,
                         struct xwsys_attribute *soattr)
{
        xwer_t rc;
        if (xwsys_get(xwobj)) {
                rc = sysfs_create_file(&xwobj->kset.kobj, &soattr->attr);
                xwsys_put(xwobj);
        } else {
                rc = -EINVAL;
        }
        return rc;
}

/**
 * @brief Remove file in /sys/xwos
 * @param xwobj: (I) xwos sysfs object
 * @param attr: (I) attribute file to remove
 */
void xwsys_remove_file(struct xwsys_object *xwobj,
                       struct xwsys_attribute *soattr)
{
        if (xwsys_get(xwobj)) {
                sysfs_remove_file(&xwobj->kset.kobj, &soattr->attr);
                xwsys_put(xwobj);
        }
}

/**
 * @brief Filter the uevent
 * @param kset: (I) kset of kobj
 * @param kobj: (I) kobject to filter
 * @retval 1: Don't filter it. generate uevent
 * @retval 0: Filter it. Don't generate uevent
 * @node
 * * Helper to decide whether to generate uevent.
 */
static
int xwsys_uevent_filter(struct kset *kset, struct kobject *kobj)
{
        struct kobj_type *ktype = get_ktype(kobj);

        if (ktype == &xwsys_ktype) {
                return 1;
        } else {
                return 0;
        }
}

/**
 * @brief Register a kset in sysfs (root: /sys/xwos)
 * @param name: (I) xwos sysfs object to register
 * @param parent: (I) parent directory
 * @param uevent_ops: (I) uevent operations
 * @retval 0: OK
 * @retval errno: error code
 * @node
 * Register the magic filesystem with the kobject infrastructure.
 */
struct xwsys_object *xwsys_register(const char *name,
                                    struct xwsys_object *parent,
                                    const struct kset_uevent_ops *uevent_ops)
{
        xwer_t rc;
        struct xwsys_object *xwobj;

        xwobj = kzalloc(sizeof(struct xwsys_object), GFP_KERNEL);
        if (is_err_or_null(xwobj)) {
                xwlogf(ERR, "Failed to alloc a xwos sysfs object!\n");
                rc = -ENOMEM;
                goto err_out;
        }

        rc = kobject_set_name(&xwobj->kset.kobj, "%s", name);
        if (rc < 0) {
                xwlogf(ERR, "Failed to set kobject name! (errno:%d)\n", rc);
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
                xwlogf(ERR, "Failed to kset_register()! (errno:%d)\n", rc);
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

void xwsys_unregister(struct xwsys_object *xwobj)
{
        if (!is_err_or_null(xwobj)) {
                kset_unregister(&xwobj->kset);
        }
}

/******** ******** /sys/xwos/state ******** ********/
static
ssize_t xwsys_attr_state_show(struct kobject *kobj,
                              struct kobj_attribute *attr,
                              char *buf)
{
        ssize_t count;
        count = sprintf(buf,
                        "xwos state:\n"
                        "XWFS: %s\n"
#if defined(XWMDCFG_isc_xwpcp) && (1 == XWMDCFG_isc_xwpcp)
                        "XWPCP: %s\n"
#endif
#if defined(XWMDCFG_isc_xwscp) && (1 == XWMDCFG_isc_xwscp)
                        "XWSCP: %s\n"
#endif
                        ,
                        xwfs_is_started() ? "ON" : "OFF"
#if defined(XWMDCFG_isc_xwpcp) && (1 == XWMDCFG_isc_xwpcp)
                        ,(USI_XWPCP_STATE_START == usi_xwpcp_get_state())? "ON" : "OFF"
#endif
#if defined(XWMDCFG_isc_xwscp) && (1 == XWMDCFG_isc_xwscp)
                        ,(USI_XWSCP_STATE_START == usi_xwscp_get_state())? "ON" : "OFF"
#endif
                       );
        return count;
}

static
xwer_t xwsys_attr_state_parse_cmd(const char *cmdstring)
{
        int token;
        substring_t tmp[MAX_OPT_ARGS];
        char *p;
        char arg[XWSYS_ARGBUFSIZE];
        size_t argsize;
        xwer_t rc = -ENOSYS;

        xwlogf(INFO, "cmd=\"%s\"\n", cmdstring);
        p = (char *)cmdstring;
        if (*p) {
                token = match_token(p, xwsys_state_cmd_tokens, tmp);
                xwlogf(INFO, "cmd=\"%s\"\n", cmdstring);
                switch (token) {
                case XWSYS_STATE_CMD_START:
                        argsize = match_strlcpy(arg, &tmp[0], XWSYS_ARGBUFSIZE);
                        xwlogf(INFO, "Get start cmd, arg:%s\n", arg);
                        if (0 == strcmp(arg, "xwfs")) {
                                rc = xwfs_start();
#if defined(XWMDCFG_isc_xwpcp) && (1 == XWMDCFG_isc_xwpcp)
                        } else if (0 == strncmp(arg, "xwpcp", sizeof("xwpcp") - 1)) {
                                rc = usi_xwpcp_start(arg);
#endif
#if defined(XWMDCFG_isc_xwscp) && (1 == XWMDCFG_isc_xwscp)
                        } else if (0 == strncmp(arg, "xwscp", sizeof("xwscp") - 1)) {
                                rc = usi_xwscp_start();
#endif
#if defined(XWMDCFG_xwmcupgm) && (1 == XWMDCFG_xwmcupgm)
                        } else if (0 == strcmp(arg, "xwmcupgmsrv")) {
                                rc = usi_xwmcupgmsrv_start();
                        } else if (0 == strcmp(arg, "xwmcupgmc")) {
                                rc = usi_xwmcupgmc_start();
#endif
                        } else {
                                xwlogf(ERR, "un-support cmd, arg:%s\n", arg);
                        }
                        break;
                case XWSYS_STATE_CMD_STOP:
                        argsize = match_strlcpy(arg, &tmp[0], XWSYS_ARGBUFSIZE);
                        xwlogf(INFO, "Get stop cmd, arg:%s\n", arg);
                        if (0 == strcmp(arg, "xwfs")) {
                                rc = xwfs_stop();
#if defined(XWMDCFG_isc_xwpcp) && (1 == XWMDCFG_isc_xwpcp)
                        } else if (0 == strcmp(arg, "xwpcp")) {
                                rc = usi_xwpcp_stop();
#endif
#if defined(XWMDCFG_isc_xwscp) && (1 == XWMDCFG_isc_xwscp)
                        } else if (0 == strcmp(arg, "xwscp")) {
                                rc = usi_xwscp_stop();
#endif
#if defined(XWMDCFG_xwmcupgm) && (1 == XWMDCFG_xwmcupgm)
                        } else if (0 == strcmp(arg, "xwmcupgmsrv")) {
                                rc = usi_xwmcupgmsrv_stop();
                        } else if (0 == strcmp(arg, "xwmcupgmc")) {
                                rc = usi_xwmcupgmc_stop();
#endif
                        } else {
                                xwlogf(ERR, "un-support cmd, arg:%s\n", arg);
                        }
                        break;
                }
        }
        return rc;
}

static
ssize_t xwsys_attr_state_store(struct kobject *kobj,
                               struct kobj_attribute *attr,
                               const char *buf,
                               size_t count)
{
        xwer_t rc;
        char cmdstring[count + 1];
        char *lf;

        memcpy(cmdstring, buf, count);
        cmdstring[count] = '\0';
        lf = cmdstring;
        strsep(&lf, "\n");
        rc = xwsys_attr_state_parse_cmd(cmdstring);
        if (__unlikely(rc < 0)) {
                count = rc;
        } /* else {} */

        return count;
}

/******** ******** init & exit******** ********/
xwer_t xwsys_init(void)
{
        xwer_t rc;

        xwsys_kset = kset_create_and_add("xwos", &xwsys_uevent_ops, NULL);
        if (unlikely(is_err_or_null(xwsys_kset))) {
                rc = -ENOMEM;
                xwlogf(ERR, "Can't create xwsys kset!\n");
                goto err_out;
        }

        rc = sysfs_create_file(&xwsys_kset->kobj, &kobj_attr_state.attr);
        if (__unlikely(rc < 0)) {
                xwlogf(ERR, "Can't create attr state!\n");
                goto err_create_attr_state;
        }

        return OK;

err_create_attr_state:
        kset_unregister(xwsys_kset);
err_out:
        xwsys_kset = NULL;
        return rc;
}

void xwsys_exit(void)
{
        sysfs_remove_file(&xwsys_kset->kobj, &kobj_attr_state.attr);
        kset_unregister(xwsys_kset);
        xwsys_kset = NULL;
}
