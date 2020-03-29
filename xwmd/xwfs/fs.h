/**
 * @file
 * @brief XuanWu FileSystem
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

#ifndef __xwmd_xwfs_fs_h__
#define __xwmd_xwfs_fs_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwlog.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief log function of OSAL FS
 */
#if (defined(XWMDCFG_XWFS_LOG) && (1 == XWMDCFG_XWFS_LOG))
#define xwfslogf(lv, fmt, ...)          xwlogf(lv, fmt, ##__VA_ARGS__)
#else
#define xwfslogf(lv, fmt, ...)
#endif
/* #if defined(XWMDCFG_XWFS_LOG) && (1 == XWMDCFG_XWFS_LOG) */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwfs_mntopts {
        umode_t mode;
        kuid_t kuid;
        kgid_t kgid;
};

struct xwfs_mnt_info {
        struct xwfs_mntopts mntopts; /**< mounting options */
        struct super_block *sb; /**< super block */
};

struct xwfs_dir {
        struct inode inode;
        struct xwfs_dir *parent;
};

struct xwfs_node;

struct xwfs_operations {
        loff_t (*llseek)(struct xwfs_node *, struct file *, loff_t, int);
        ssize_t (*read)(struct xwfs_node *, struct file *, char __user *,
                        size_t, loff_t *);
        ssize_t (*write)(struct xwfs_node *, struct file *, const char __user *,
                         size_t, loff_t *);
        int (*mmap)(struct xwfs_node *, struct file *, struct vm_area_struct *);
        int (*open)(struct xwfs_node *, struct file *);
        int (*release)(struct xwfs_node *, struct file *);
        long (*unlocked_ioctl)(struct xwfs_node *, struct file *,
                               unsigned int, unsigned long);
};

struct xwfs_node {
        struct inode inode;
        struct xwfs_dir *parent;
        const struct xwfs_operations *xwfsops;
};

union xwfs_entry {
        struct xwfs_dir dir;
        struct xwfs_node node;
        struct inode inode;
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern
xwer_t xwfs_start(void);

extern
xwer_t xwfs_stop(void);

extern
bool xwfs_is_started(void);

extern
xwer_t xwfs_holdon(void);

extern
xwer_t xwfs_giveup(void);

extern
xwer_t xwfs_mkdir(const char *name, struct xwfs_dir *parent,
                  struct xwfs_dir **newdir);

extern
xwer_t xwfs_mknod(const char *name,
                  umode_t mode,
                  const struct xwfs_operations *xwfsops,
                  void *data,
                  struct xwfs_dir *parent,
                  struct xwfs_node **newnode);

extern
xwer_t xwfs_rmdir(struct xwfs_dir *dir);

extern
xwer_t xwfs_rmnod(struct xwfs_node *node);

extern
xwer_t xwfs_init(void);

extern
void xwfs_exit(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __always_inline
void *xwfs_node_get_data(struct xwfs_node *node)
{
        return node->inode.i_private;
}

#endif /* xwmd/xwfs/fs.h */
