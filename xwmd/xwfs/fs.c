/**
 * @file
 * @brief XuanWu FileSystem
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
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
#include <linux/vfs.h>
#include <linux/fs.h>
#include <linux/dcache.h>
#include <linux/namei.h>
#include <linux/mount.h>
#include <linux/file.h>
#include <linux/fsnotify.h>
#include <linux/string.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/pagemap.h>
#include <linux/uaccess.h>
#include <xwos/lib/xwlog.h>
#include <xwos/lib/xwaop.h>
#include <xwos/mp/skd.h>
#include <xwmd/sysfs/core.h>
#include <xwmd/xwfs/fs.h>
#include <xwmd/xwfs/bs.h>

#define XWFS_DEFAULT_MODE       (S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
#define XWFS_MAGIC_NUM          0x6F736673  /* xwfs */

#define VFS_INODE_TO_XWFS_ENTRY(ind) xwcc_baseof(ind, union xwfs_entry, inode)

/**
 * @brief mount options
 */
enum xwfs_option {
        OPT_MODE, /**< option "mode=%o" */
        OPT_UID, /**< option "uid=%d" */
        OPT_GID, /**< option "gid=%d" */
        OPT_ERR, /**< error option */
};

/******** ******** mount operations ******** ********/
static
int xwfs_fill_superblock(struct super_block * sb, struct xwfs_mntopts * mntopts);

static
int xwfs_test_super(struct super_block * sb, void * data);

static
int xwfs_set_super(struct super_block * sb, void * data);

static
struct dentry * xwfs_mount(struct file_system_type * fst,
                           int flags, const char * dev_name,
                           void * optstring);

static
void xwfs_kill_sb(struct super_block * psb);

/******** ******** super_operations ******** ********/
static
void xwfs_entry_construct(void * addr);

static
struct inode * xwfs_sops_alloc_inode(struct super_block * sb);

static
struct inode * xwfs_new_entry(struct super_block * sb,
                              struct xwfs_dir * parent,
                              umode_t mode, dev_t dev);

static
void xwfs_sops_destroy_callback(struct rcu_head * head);

static
void xwfs_sops_destroy_inode(struct inode * inode);

static
int xwfs_sops_drop_inode(struct inode * inode);

static
void xwfs_sops_put_super(struct super_block * sb);

static
int xwfs_sops_statfs(struct dentry * dentry, struct kstatfs * buf);

static
int xwfs_sops_show_options(struct seq_file * seq, struct dentry * root);

/******** ******** dentry_operations ******** ********/
static
int xwfs_dops_delete_dentry(const struct dentry * dentry);

/******** ******** memory mapping ******** ********/
static
int xwfs_set_page_dirty_no_writeback(struct page * page);

/******** ******** directory inode operations ******** ********/
static
struct dentry * xwfs_dir_iops_lookup(struct inode * iparent,
                                     struct dentry * dentry,
                                     unsigned int flags);

/******** ******** directory file operations ******** ********/

/******** ******** node file operations ******** ********/
static
loff_t xwfs_node_fops_llseek(struct file * file, loff_t offset, int origin);

static
ssize_t xwfs_node_fops_read(struct file * file, char __user * buf,
                            size_t len, loff_t * pos);

static
ssize_t xwfs_node_fops_write(struct file *file, const char __user * buf,
                             size_t len, loff_t * pos);

static
int xwfs_node_fops_mmap(struct file * file, struct vm_area_struct * vma);

static
int xwfs_node_fops_open(struct inode * inode, struct file * file);

static
int xwfs_node_fops_release(struct inode * inode, struct file * file);

static
long xwfs_node_fops_unlocked_ioctl(struct file * file, unsigned int cmd,
                                   unsigned long args);

/******** ******** APIs internal functions ******** ********/
static
xwer_t xwfs_path_create(const char * name, struct xwfs_dir * parent,
                        struct dentry ** dptrbuf, struct path * pathbuf);

static
void xwfs_done_path_create(struct path * pathparent, struct dentry * dentry);

static
xwer_t xwfs_get_path_parent(struct xwfs_dir * parent, struct path * pathbuf);

static
void xwfs_put_path_parent(struct path * pathparent);

/******** ******** root path ******** ********/
struct path xwfs_rootpath = {
        .mnt = NULL,
        .dentry = NULL,
};

xwsq_a xwfs_mntcnt = 0;

static const match_table_t xwfs_tokens = {
        {OPT_MODE, "mode=%o"},
        {OPT_UID, "uid=%u"},
        {OPT_GID, "gid=%u"},
        {OPT_ERR, NULL},
};

static struct file_system_type xwfs_fstype = {
        .owner = THIS_MODULE,
        .name = "xwfs",
        .mount = xwfs_mount,
        .kill_sb = xwfs_kill_sb,
};

static struct kmem_cache * xwfs_entry_cache;
static const struct super_operations xwfs_sops = {
        .alloc_inode = xwfs_sops_alloc_inode,
        .destroy_inode = xwfs_sops_destroy_inode,
        .drop_inode = xwfs_sops_drop_inode,
        .put_super = xwfs_sops_put_super,
        .statfs = xwfs_sops_statfs,
        .show_options = xwfs_sops_show_options,
};

static const struct dentry_operations xwfs_dops = {
        .d_delete = xwfs_dops_delete_dentry,
};

static struct address_space_operations xwfs_aops = {
        .readpage = simple_readpage,
        .write_begin = simple_write_begin,
        .write_end = simple_write_end,
        .set_page_dirty = xwfs_set_page_dirty_no_writeback,
};

/**
 * @brief 目录的inode_operations
 */
static const struct inode_operations xwfs_dir_iops = {
        .lookup = xwfs_dir_iops_lookup,
};

/**
 * @brief 节点的inode_operations
 */
static const struct inode_operations xwfs_node_iops = {
        .setattr = simple_setattr,
        .getattr = simple_getattr,
};

/**
 * @brief 节点的file_operations
 */
static const struct file_operations xwfs_node_fops = {
        .llseek = xwfs_node_fops_llseek,
        .read = xwfs_node_fops_read,
        .write = xwfs_node_fops_write,
        .mmap = xwfs_node_fops_mmap,
        .open = xwfs_node_fops_open,
        .release = xwfs_node_fops_release,
        .unlocked_ioctl = xwfs_node_fops_unlocked_ioctl,
        .fsync = noop_fsync,
};

/**
 * @brief 填充文件系统超级块的结构体
 * @param sb: (I) 文件系统超级块的指针
 * @param mntopts: (I) 挂载选项
 * @return 错误码
 */
static
int xwfs_fill_superblock(struct super_block * sb, struct xwfs_mntopts * mntopts)
{
        struct xwfs_mnt_info *newmi;
        struct xwfs_dir *root;
        struct inode *rooti;
        struct dentry *rootd;
        int rc;

        /* Round up to L1_CACHE_BYTES to resist false sharing */
        newmi = kzalloc(max((int)sizeof(struct xwfs_mnt_info), L1_CACHE_BYTES),
                        GFP_KERNEL);
        if (__xwcc_unlikely(NULL == newmi)) {
                rc = -ENOMEM;
                xwfslogf(ERR,
                         "kmalloc() struct xwfs_mnt_info ... [FAILED], rc:%d\n",
                         rc);
                goto err_mi_alloc;
        }
        sb->s_fs_info = (void *)newmi;
        newmi->mntopts.mode = mntopts->mode;
        newmi->mntopts.kuid = mntopts->kuid;
        newmi->mntopts.kgid = mntopts->kgid;

        sb->s_maxbytes = MAX_LFS_FILESIZE;
        sb->s_blocksize = PAGE_SIZE;
        sb->s_blocksize_bits = PAGE_SHIFT;
        sb->s_magic = XWFS_MAGIC_NUM;
        sb->s_op = &xwfs_sops;
        sb->s_d_op = &xwfs_dops;
        sb->s_time_gran = 1;
        sb->s_root = NULL;

        newmi->sb = sb;

        rooti = xwfs_new_entry(sb, NULL, S_IFDIR | mntopts->mode, 0);
        if (rooti == NULL) {
                rc = -ENOMEM;
                goto err_new_rooti;
        }
        rooti->i_uid = mntopts->kuid;
        rooti->i_gid = mntopts->kgid;
        root = xwcc_baseof(rooti, struct xwfs_dir, inode);
        root->parent = NULL;

        rootd = d_make_root(rooti);
        if (NULL == rootd) {
                rc = -ENOMEM;
                goto err_make_rootd;
        }
        sb->s_root = rootd;
        return XWOK;

        /* xwfs_kill_sb will be called later, if failed. */
err_make_rootd:
        iput(rooti);
err_new_rooti:
        kfree(newmi);
        sb->s_fs_info = NULL;
err_mi_alloc:
        return rc;
}

/**
 * @brief 测试文件系统超级块是否存在
 * @param sb: (I) 文件系统超级块的指针
 * @param data: (I) 挂载选项，mount系统调用中的-o参数
 * @return 布尔值
 * @retval 1: 存在
 * @retval 0: 不存在
 * @note
 * - 当此函数返回1时表示文件系统超级块存在，VFS不会重新申请一个文件系统超级块
 *   只会把已经存在的文件系统超级块引用计数增加载；
 * - XWFS是单例文件系统，有且只有一个文件系统超级块，用户态的mount命令只会增加
 *   此文件系统超级块的引用计数。
 */
static
int xwfs_test_super(struct super_block * sb, void * data)
{
        return 1;
}

/**
 * @brief 初始化文件系统超级块
 * @param sb: (I) 文件系统超级块的指针
 * @param data: (I) 挂载选项，mount系统调用中的-o参数
 * @return 错误码
 */
static
int xwfs_set_super(struct super_block * sb, void * data)
{
        return set_anon_super(sb, NULL);
}

/**
 * @brief 挂载文件系统
 * @param fst: (I) 文件系统类型的指针
 * @param flags: (I) 挂载标志
 * @param dev_name: (I) mount系统调用中的'device'参数
 * @param data: (I) 挂载选项，mount系统调用中的-o参数
 * @return 文件系统根目录的dentry
 * @note
 * + Linux内核对于文件系统有两种挂载方式：
 *   - Kernel mount: 会设置标记MS_KERNMOUNT；
 *   - syscall mount: 通过命令mount挂载，命令的-o选项被由data传递到此函数。
 */
static
struct dentry * xwfs_mount(struct file_system_type * fst,
                           int flags, const char * dev_name,
                           void * data)
{
        struct super_block * sb;
        struct dentry * rootd;
        struct xwfs_mntopts mntopts = {
                .mode = XWFS_DEFAULT_MODE,
                .kuid = make_kuid(current_user_ns(), 0),
                .kgid = make_kgid(current_user_ns(), 0),
        };
        xwer_t rc;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0)
        if (SB_KERNMOUNT & flags) {
#else
        if (MS_KERNMOUNT & flags) {
#endif
                /* mounted by kernel */
                sb = sget(fst, NULL, xwfs_set_super, flags, data);
                if (__xwcc_unlikely(is_err(sb))) {
                        rootd = (void *)sb;
                        goto err_sget;
                }
                rc = xwfs_fill_superblock(sb, &mntopts);
                if (__xwcc_unlikely(rc < 0)) {
                        deactivate_locked_super(sb);
                        rootd = err_ptr(rc);
                        goto err_fill_sb;
                }
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0)
                sb->s_flags |= SB_ACTIVE;
#else
                sb->s_flags |= MS_ACTIVE;
#endif
        } else {
                /* mounted by syscall API: sys_mount() */
                if (xwfs_mntcnt < 1) {
                        rootd = err_ptr(-ENOENT);
                        goto err_notmnt;
                }
                sb = sget(fst, xwfs_test_super, xwfs_set_super, flags, data);
                if (__xwcc_unlikely(is_err(sb))) {
                        rootd = (void *)sb;
                        goto err_sget;
                }
        }
        rootd = dget(sb->s_root);

err_fill_sb:
err_sget:
err_notmnt:
        return rootd;
}

/**
 * @brief 删除文件系统超级块
 * @param sb: (I) 文件系统超级块的指针
 */
static
void xwfs_kill_sb(struct super_block * sb)
{
        kill_litter_super(sb);
}

/******** ******** super_operations ******** ********/
/**
 * @brief union xwfs_entry的slab缓存的构造函数
 * @param data: (I) 指向union xwfs_entry的指针
 */
static
void xwfs_entry_construct(void * addr)
{
        union xwfs_entry * oe = (union xwfs_entry *)addr;
        inode_init_once(&oe->inode);
}

/**
 * @brief XWFS的SOPS：新建一个inode
 * @param sb: (I) 文件系统超级块的指针
 * @return inode的指针
 */
static
struct inode * xwfs_sops_alloc_inode(struct super_block * sb)
{
        union xwfs_entry * oe;
        struct inode * ind;

        oe = kmem_cache_alloc(xwfs_entry_cache, GFP_KERNEL);
        if (__xwcc_likely(oe)) {
                ind = &oe->inode;
        } else {
                ind = NULL;
        }
        return ind;
}

static
struct inode * xwfs_new_entry(struct super_block * sb,
                              struct xwfs_dir * parent,
                              umode_t mode, dev_t dev)
{
        union xwfs_entry * oe;
        struct inode * iprnt;

        iprnt = parent ? &parent->inode : NULL;
        oe = VFS_INODE_TO_XWFS_ENTRY(new_inode(sb));
        if (oe) {
                oe->inode.i_ino = get_next_ino();
                inode_init_owner(&oe->inode, iprnt, mode);
                oe->inode.i_mapping->a_ops = &xwfs_aops;
                mapping_set_gfp_mask(oe->inode.i_mapping, GFP_HIGHUSER);
                mapping_set_unevictable(oe->inode.i_mapping);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
                oe->inode.i_atime =
                                oe->inode.i_mtime =
                                oe->inode.i_ctime = current_time(&oe->inode);
#else
                oe->inode.i_atime =
                                oe->inode.i_mtime =
                                oe->inode.i_ctime = CURRENT_TIME;
#endif
                switch (mode & S_IFMT) {
                case S_IFREG:
                        oe->inode.i_op = &xwfs_node_iops;
                        oe->inode.i_fop = &xwfs_node_fops;
                        break;
                case S_IFDIR:
                        oe->inode.i_op = &xwfs_dir_iops;
                        oe->inode.i_fop = &simple_dir_operations;
                        /* directory inodes start off with
                           i_nlink == 2 (for "." entry) */
                        inc_nlink(&oe->inode);
                        break;
                case S_IFLNK:
                        oe->inode.i_op = &page_symlink_inode_operations;
                        break;
                default:
                        init_special_inode(&oe->inode, mode, dev);
                        break;
                }
        }
        return &oe->inode;
}

/**
 * @brief XWFS的SOPS：释放inode时的rcu-callback
 * @param head: (I) rcu链表头
 * @note
 * - 当调用 @ref xwfs_sops_destroy_inode() 彻底销毁inode时，此函数会在RCU的
 *   宽限期(grace period)结束后调用。
 */
static
void xwfs_sops_destroy_callback(struct rcu_head * head)
{
        union xwfs_entry * oe;

        oe = xwcc_baseof(head, union xwfs_entry, inode.i_rcu);
        kmem_cache_free(xwfs_entry_cache, oe);
}

/**
 * @brief XWFS的SOPS：彻底销毁inode
 * @param inode: (I) 待删除的inode
 */
static
void xwfs_sops_destroy_inode(struct inode * inode)
{
        call_rcu(&inode->i_rcu, xwfs_sops_destroy_callback);
}

/**
 * @brief XWFS的SOPS：告诉调用者（VFS），是否需要删除inode
 * @param inode: (I) 待删除的inode
 * @return 布尔值
 * @retval 1: 删除它
 * @retval 0: 不能删除它
 * @note
 * - 此函数在```iput_final()```中调用，此时inode->i_lock已经被锁定；
 * - 如果此函数返回1，iput_final()会调用```xwfs_sops_destroy_inode()```释放掉inode；
 * - 如果此函数返回0，并且文件系统的超级块拥有标志```MS_ACTIVE```，VFS会将此inode加入
 *   到lru链表，以减少下次申请inode时的系统开销；
 * - 当挂载文件系统时，@ref xwfs_mount() 会设置```MS_ACTIVE```标志，xwfs_mount()
 *   函数由mount system call调用。
 */
static
int xwfs_sops_drop_inode(struct inode * inode)
{
        return 1;
}

/**
 * @brief XWFS的SOPS：释放文件系统的超级块
 * @param sb: (I) 文件系统的超级块指针
 */
static
void xwfs_sops_put_super(struct super_block * sb)
{
        if (sb->s_fs_info) {
                kfree(sb->s_fs_info);
                sb->s_fs_info = NULL;
        }
}

/**
 * @brief XWFS的SOPS：获取文件系统状态
 * @param sb: (I) 文件系统的超级块指针
 * @param buf: (O) 指向缓冲区的指针，通过此缓冲区返回文件系统状态
 */
static
int xwfs_sops_statfs(struct dentry * dentry, struct kstatfs * buf)
{
        return simple_statfs(dentry, buf);
}

/**
 * @brief XWFS的SOPS：显示挂载选项
 * @param seq: (I) 指向用于输出的seq_file的指针
 * @param rootd: (I) 指向文件系统根目录的dentry
 * @retval 0
 */
static
int xwfs_sops_show_options(struct seq_file * seq, struct dentry * root)
{
        struct xwfs_mnt_info * fi;

        fi = (struct xwfs_mnt_info *)(root->d_sb->s_fs_info);
        seq_printf(seq, ",mode=0%o", fi->mntopts.mode);
        seq_printf(seq, ",uid=%u",
                   from_kuid_munged(&init_user_ns, fi->mntopts.kuid));
        seq_printf(seq, ",gid=%u",
                   from_kgid_munged(&init_user_ns, fi->mntopts.kgid));
        return 0;
}

/******** ******** dentry_operations ******** ********/
static
int xwfs_dops_delete_dentry(const struct dentry * dentry)
{
        return 1;
}

/******** ******** memory mapping operations ******** ********/
static
int xwfs_set_page_dirty_no_writeback(struct page * page)
{
        /* PageDirty() is defined by a macro PAGEFLAG in
           file include/linux/page_flags.h */
        int ret;
        if (!PageDirty(page)) {
                ret = !TestSetPageDirty(page);
        } else {
                ret = 0;
        }
        return ret;
}

/******** ******** directory inode operations ******** ********/
/**
 * @brief XWFS的IOPS：查找dentry (linux >= 3.6.0)
 * @param iparent: (I) 指向父目录inode的指针
 * @param dentry: (I) 指向dentry的指针，此dentry由VFS动态分配
 * @param flags: (I) 查找标志
 * @return 查找到的dentry的指针
 * @retval NULL: 告诉VFS，传递进来的/dentry/有效
 * @retval another dentry: 告诉VFS，此函数会返回另一个dentry，
 *                         刚才传递进来的dentry不使用。
 * @retval errno: 错误值
 * @note
 * - 可以在```fs/namei.c```中找到```__lookup_hash()```函数，此函数为VFS具体查找过程
 * - 当VFS找到了已经存在的dentry，此函数不会被调用；
 * - 仅当VFS找不到对应路径的dentry，从内存管理系统中新建了一个（通过函数```d_alloc()```）
 *   才会调用此函数，并将新建的dentry作为参数；
 * - 如果此函数的返回值不为空，表示此函数有自己的查找机制，VFS新建dentry无效，
 *   之后VFS会将刚才新建的dentry释放掉，可在```lookup_real()```中找到具体代码；
 * - linux内核版本 >= 3.6.0。
 */
static
struct dentry * xwfs_dir_iops_lookup(struct inode * iparent,
                                     struct dentry * dentry,
                                     unsigned int flags)
{
        dont_mount(dentry);
        return simple_lookup(iparent, dentry, flags);
}

/******** ******** directory file operations ******** ********/

/******** ******** node inode operations ******** ********/

/******** ******** node file operations ******** ********/
static
loff_t xwfs_node_fops_llseek(struct file * file, loff_t offset, int origin)
{
        xwer_t rc;
        struct xwfs_node * node;
        const struct xwfs_operations * xwfsops;

        node = xwcc_baseof(file->f_inode, struct xwfs_node, inode);
        xwfsops = node->xwfsops;
        if (xwfsops && xwfsops->llseek) {
                rc = xwfsops->llseek(node, file, offset, origin);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

static
ssize_t xwfs_node_fops_read(struct file * file, char __user * buf,
                            size_t len, loff_t * pos)
{
        xwer_t rc;
        struct xwfs_node * node;
        const struct xwfs_operations * xwfsops;

        node = xwcc_baseof(file->f_inode, struct xwfs_node, inode);
        xwfsops = node->xwfsops;
        if (xwfsops && xwfsops->read) {
                rc = xwfsops->read(node, file, buf, len, pos);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

static
ssize_t xwfs_node_fops_write(struct file * file, const char __user * buf,
                             size_t len, loff_t * pos)
{
        xwer_t rc;
        struct xwfs_node * node;
        const struct xwfs_operations * xwfsops;

        node = xwcc_baseof(file->f_inode, struct xwfs_node, inode);
        xwfsops = node->xwfsops;
        if (xwfsops && xwfsops->write) {
                rc = xwfsops->write(node, file, buf, len, pos);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

static
int xwfs_node_fops_mmap(struct file * file, struct vm_area_struct * vma)
{
        xwer_t rc;
        struct xwfs_node * node;
        const struct xwfs_operations * xwfsops;

        node = xwcc_baseof(file->f_inode, struct xwfs_node, inode);
        xwfsops = node->xwfsops;
        if (xwfsops && xwfsops->mmap) {
                rc = xwfsops->mmap(node, file, vma);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

static
int xwfs_node_fops_open(struct inode * inode, struct file * file)
{
        xwer_t rc;
        struct xwfs_node * node;
        const struct xwfs_operations * xwfsops;

        node = xwcc_baseof(inode, struct xwfs_node, inode);
        xwfsops = node->xwfsops;
        if (xwfsops && xwfsops->open) {
                rc = xwfsops->open(node, file);
        } else {
                rc = XWOK;
        }
        return rc;
}

static
int xwfs_node_fops_release(struct inode * inode, struct file * file)
{
        xwer_t rc;
        struct xwfs_node * node;
        const struct xwfs_operations * xwfsops;

        node = xwcc_baseof(inode, struct xwfs_node, inode);
        xwfsops = node->xwfsops;
        if (xwfsops && xwfsops->release) {
                rc = xwfsops->release(node, file);
        } else {
                rc = XWOK;
        }
        return rc;
}

static
long xwfs_node_fops_unlocked_ioctl(struct file * file, unsigned int cmd,
                                   unsigned long args)
{
        xwer_t rc;
        struct xwfs_node * node;
        const struct xwfs_operations * xwfsops;

        node = xwcc_baseof(file->f_inode, struct xwfs_node, inode);
        xwfsops = node->xwfsops;
        if (xwfsops && xwfsops->unlocked_ioctl) {
                rc = xwfsops->unlocked_ioctl(node, file, cmd, args);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}


/******** ******** xwfs APIs ******** ********/
/**
 * @brief 建立内核路径
 * @param name: (I) 相对于父目录的路径名
 * @param parent: (I) 父目录的指针
 * @param dptrbuf: (O) 指向缓冲区的指针，通过此缓冲区返回dentry的指针
 * @param pathbuf (O) 指向缓冲区的指针，通过此缓冲区返回内核路径
 * @return 错误码
 */
static
xwer_t xwfs_path_create(const char * name, struct xwfs_dir * parent,
                        struct dentry ** dptrbuf, struct path * pathbuf)
{
        struct dentry * newd;
        size_t nlen;
        xwer_t rc;

        if (NULL == xwfs_rootpath.mnt) {
                rc = -EPERM;
                goto err_not_mnt;
        }
        pathbuf->mnt = xwfs_rootpath.mnt;
        if (NULL == parent) {
                pathbuf->dentry = xwfs_rootpath.dentry;
        } else {
                pathbuf->dentry = hlist_entry(parent->inode.i_dentry.first,
                                              struct dentry,
                                              d_u.d_alias);
        }
        nlen = strlen(name);
        path_get(pathbuf);
        rc = mnt_want_write(pathbuf->mnt);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mnt_want_write;
        }
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,7,0)
        inode_lock_nested(pathbuf->dentry->d_inode, I_MUTEX_PARENT);
#else
        mutex_lock_nested(&pathbuf->dentry->d_inode->i_mutex, I_MUTEX_PARENT);
#endif
        newd = lookup_one_len(name, pathbuf->dentry, nlen);
        if (__xwcc_unlikely(is_err(newd))) {
                goto err_lookup;
        }
        if (d_is_positive(newd)) {
                rc = -EEXIST;
                goto err_exist;
        }
        *dptrbuf = newd;
        return XWOK;

err_exist:
        dput(newd);
err_lookup:
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,7,0)
        inode_unlock(pathbuf->dentry->d_inode);
#else
        mutex_unlock(&pathbuf->dentry->d_inode->i_mutex);
#endif
err_mnt_want_write:
        path_put(pathbuf);
err_not_mnt:
        return rc;
}

/**
 * @brief 释放内核路径
 * @param pathparent: (I) 父目录的内核路径
 * @param dentry: (I) 路径最后一级的dentry
 * @return 错误码
 */
static
void xwfs_done_path_create(struct path * pathparent, struct dentry * dentry)
{
        done_path_create(pathparent, dentry);
}

static
xwer_t xwfs_mknod_internal(struct xwfs_dir * parent,
                           struct dentry * dentry,
                           umode_t mode, dev_t dev)
{
        struct inode * newi;
        struct inode * iprnt;
        int rc;

        rc = -ENOSPC;
        iprnt = &parent->inode;
        newi = xwfs_new_entry(iprnt->i_sb, parent, mode, dev);
        if (newi) {
                d_instantiate(dentry, newi);
                dget(dentry); /* Extra count - pin the dentry in core */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
                iprnt->i_ctime = current_time(iprnt);
#else
                iprnt->i_ctime = CURRENT_TIME;
#endif
                iprnt->i_mtime = iprnt->i_ctime;
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief XWFS API：创建文件夹
 * @param name: (I) 文件夹名
 * @param parent: (I) 父目录
 * @param newdir: (O) 指向缓冲区的指针，通过此缓冲区返回新建的文件夹
 * @return 错误码
 */
xwer_t xwfs_mkdir(const char * name, struct xwfs_dir * parent,
                  struct xwfs_dir ** newdir)
{
        struct path pathparent;
        struct dentry * dnew;
        struct xwfs_dir * dir;
        unsigned int max_links;
        xwer_t rc;

        rc = xwfs_path_create(name, parent, &dnew, &pathparent);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xwfs_path_create;
        }
        parent = xwcc_baseof(pathparent.dentry->d_inode, struct xwfs_dir, inode);
        max_links = pathparent.mnt->mnt_sb->s_max_links;
        if (max_links && parent->inode.i_nlink >= max_links) {
                rc = -EMLINK;
                goto err_maxlink;
        }
        rc = xwfs_mknod_internal(parent, dnew,
                                 S_IFDIR | XWFS_DEFAULT_MODE,
                                 0);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mknod;
        }
        inc_nlink(&parent->inode);
        dir = xwcc_baseof(dnew->d_inode, struct xwfs_dir, inode);
        dir->parent = parent;
        fsnotify_mkdir(&parent->inode, dnew);
        xwfs_done_path_create(&pathparent, dnew);
        *newdir = dir;
        return XWOK;

err_mknod:
err_maxlink:
        xwfs_done_path_create(&pathparent, dnew);
err_xwfs_path_create:
        return rc;
}

/**
 * @brief XWFS API：创建文件节点
 * @param name: (I) 节点名
 * @param mode: (I) 文件的权限
 * @param xwfsops: (I) 节点的操作函数集合
 * @param data: (I) 用户数据指针
 * @param parent: (I) 父目录
 * @param newnode: (O) 指向缓冲区的指针，通过此缓冲区返回新建的节点
 * @return 错误码
 */
xwer_t xwfs_mknod(const char * name,
                  umode_t mode,
                  const struct xwfs_operations * xwfsops,
                  void * data,
                  struct xwfs_dir * parent,
                  struct xwfs_node ** newnode)
{
        struct path pathparent;
        struct dentry * dnew;
        struct xwfs_node * n;
        xwer_t rc;

        rc = xwfs_path_create(name, parent, &dnew, &pathparent);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xwfs_path_create;
        }
        parent = xwcc_baseof(pathparent.dentry->d_inode, struct xwfs_dir, inode);
        rc = xwfs_mknod_internal(parent, dnew,
                                 S_IFREG | (mode & 07777),
                                 0);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mknod;
        }
        n = xwcc_baseof(dnew->d_inode, struct xwfs_node, inode);
        n->xwfsops = xwfsops;
        n->inode.i_private = data;
        n->parent = parent;
        fsnotify_create(&parent->inode, dnew);
        xwfs_done_path_create(&pathparent, dnew);
        *newnode = n;
        return XWOK;

err_mknod:
        xwfs_done_path_create(&pathparent, dnew);
err_xwfs_path_create:
        return rc;
}

/**
 * @brief 获取父目录的内核路径
 */
static
xwer_t xwfs_get_path_parent(struct xwfs_dir * parent, struct path * pathbuf)
{
        xwer_t rc;

        if (NULL == xwfs_rootpath.mnt) {
                rc = -EPERM;
                goto err_not_mnt;
        }
        pathbuf->mnt = xwfs_rootpath.mnt;
        pathbuf->dentry = hlist_entry(parent->inode.i_dentry.first,
                                      struct dentry,
                                      d_u.d_alias);
        path_get(pathbuf);
        rc = mnt_want_write(pathbuf->mnt);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mnt_want_write;
        }
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,7,0)
        inode_lock_nested(&parent->inode, I_MUTEX_PARENT);
#else
        mutex_lock_nested(&parent->inode.i_mutex, I_MUTEX_PARENT);
#endif
        return XWOK;

err_mnt_want_write:
        path_put(pathbuf);
err_not_mnt:
        return rc;
}

/**
 * @brief 释放父目录的内核路径
 */
static
void xwfs_put_path_parent(struct path * pathparent)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,7,0)
        inode_unlock(pathparent->dentry->d_inode);
#else
        mutex_unlock(&pathparent->dentry->d_inode->i_mutex);
#endif
        mnt_drop_write(pathparent->mnt);
        path_put(pathparent);
}

/**
 * @brief XWFS API：删除目录
 * @param dir: (I) 目录的指针
 * @return 错误码
 */
xwer_t xwfs_rmdir(struct xwfs_dir * dir)
{
        struct xwfs_dir * parent;
        struct path pathparent;
        struct dentry * d;
        xwer_t rc;

        parent = dir->parent;
        if (__xwcc_unlikely(NULL == parent)) {
                BUG();
                rc = -EPERM;
                goto err_nullparent;
        }

        rc = xwfs_get_path_parent(parent, &pathparent);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_get_path_parent;
        }

        /* XWFS forbids hard link. So, there is only
           one dentry that instantiates the inode. */
        d = hlist_entry(dir->inode.i_dentry.first,
                        struct dentry,
                        d_u.d_alias);
        dget(d);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,7,0)
        inode_lock(&dir->inode);
#else
        mutex_lock(&dir->inode.i_mutex);
#endif
        shrink_dcache_parent(d);
        rc = simple_rmdir(&parent->inode, d);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_rmdir;
        }
        dir->inode.i_flags |= S_DEAD;
        /* FIXME: dont_mount(d); */
        /* FIXME: detach_mounts(d); */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,7,0)
        inode_unlock(&dir->inode);
#else
        mutex_unlock(&dir->inode.i_mutex);
#endif
        /* Call d_delete() when d->d_lockref.count is greater than 1.
           ref: do_rmdir() in fs/namei.c */
        d_delete(d);
        dput(d);
        xwfs_put_path_parent(&pathparent);
        return XWOK;

err_rmdir:
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,7,0)
        inode_unlock(&dir->inode);
#else
        mutex_unlock(&dir->inode.i_mutex);
#endif
        dput(d);
        xwfs_put_path_parent(&pathparent);
err_get_path_parent:
err_nullparent:
        return rc;
}

/**
 * @brief XWFS API：删除节点
 * @param node: (I) 节点的指针
 * @return 错误码
 */
xwer_t xwfs_rmnod(struct xwfs_node * node)
{
        struct xwfs_dir * parent;
        struct path pathparent;
        struct dentry * d;
        xwer_t rc;

        parent = node->parent;
        if (__xwcc_unlikely(NULL == parent)) {
                BUG();
                rc = -EPERM;
                goto err_nullparent;
        }

        rc = xwfs_get_path_parent(parent, &pathparent);
        if (__xwcc_unlikely(rc < 0))
                goto err_get_path_parent;

        /* XWFS不允许创建硬链接，inode只有一个dentry */
        d = hlist_entry(node->inode.i_dentry.first,
                        struct dentry,
                        d_u.d_alias);
        dget(d);
        ihold(&node->inode);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,7,0)
        inode_lock(&node->inode);
#else
        mutex_lock(&node->inode.i_mutex);
#endif
        rc = simple_unlink(&parent->inode, d);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_unlink;
        }
        /* FIXME: dont_mount(d); */
        /* FIXME: detach_mounts(d); */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,7,0)
        inode_unlock(&node->inode);
#else
        mutex_unlock(&node->inode.i_mutex);
#endif
        fsnotify_link_count(&node->inode);
        /* Call d_delete() when d->d_lockref.count is greater than 1.
           ref: do_unlinkat() in fs/namei.c */
        d_delete(d);
        iput(&node->inode);
        dput(d);
        xwfs_put_path_parent(&pathparent);
        return XWOK;

err_unlink:
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,7,0)
        inode_unlock(&node->inode);
#else
        mutex_unlock(&node->inode.i_mutex);
#endif
        iput(&node->inode);
        dput(d);
        xwfs_put_path_parent(&pathparent);
err_get_path_parent:
err_nullparent:
        return rc;
}

/**
 * @brief XWFS API：启动XWFS
 * @return 错误码
 */
xwer_t xwfs_start(void)
{
        struct vfsmount * mnt;
        xwer_t rc;

        if (NULL != xwfs_rootpath.mnt) {
                rc = -EALREADY;
                goto err_xwfs_alrdymnted;
        }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0)
        mnt = kern_mount(&xwfs_fstype);
#else
        mnt = kern_mount_data(&xwfs_fstype, NULL);
#endif
        if (__xwcc_unlikely(is_err(mnt))) {
                rc = ptr_err(mnt);
                xwfslogf(ERR,
                         "kernel mount xwfs ... [FAILED], rc:%d\n",
                         rc);
                goto err_kern_mnt;
        }

        xwfs_rootpath.mnt = mnt;
        xwfs_rootpath.dentry = mnt->mnt_root;
        xwaop_write(xwsq, &xwfs_mntcnt, 1, NULL);
        xwfslogf(INFO, "kernel mount xwfs ... [OK]\n");

        rc = xwfs_create_skeleton();
        if (__xwcc_unlikely(rc < 0)) {
                goto err_create_skeleton;
        }

        rc = xwmp_skd_xwfs_init();
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xwmp_skd_xwfs_init;
        }

        return XWOK;

err_xwmp_skd_xwfs_init:
        xwfs_delete_skeleton();
err_create_skeleton:
        kern_unmount(xwfs_rootpath.mnt);
        xwfs_rootpath.mnt = NULL;
        xwfs_rootpath.dentry = NULL;
err_kern_mnt:
err_xwfs_alrdymnted:
        return rc;
}

/**
 * @brief XWFS API：停止XWFS
 * @return 错误码
 */
xwer_t xwfs_stop(void)
{
        xwer_t rc;

        rc = xwaop_teq_then_sub(xwsq, &xwfs_mntcnt, 1, 1, NULL, NULL);
        if (XWOK == rc) {
                xwmp_skd_xwfs_exit();
                xwfs_delete_skeleton();
                kern_unmount(xwfs_rootpath.mnt);
                xwfs_rootpath.mnt = NULL;
                xwfs_rootpath.dentry = NULL;
                xwfslogf(INFO, "kernel umount xwfs ... [OK]\n");
        }
        return rc;
}

xwer_t xwfs_holdon(void)
{
        xwer_t rc;

        rc = xwaop_tge_then_add(xwsq, &xwfs_mntcnt, 1, 1, NULL, NULL);
        return rc;
}

xwer_t xwfs_giveup(void)
{
        xwer_t rc;

        rc = xwaop_tge_then_sub(xwsq, &xwfs_mntcnt, 1, 1, NULL, NULL);
        return rc;
}

struct xwsys_object * xwfs_xwsys_obj;

/******** /sys/xwos/xwfs/cmd ********/
#define XWFS_XWSYS_ARGBUFSIZE  32

enum xwfs_xwsys_cmd_em {
        XWFS_XWSYS_CMD_STOP = 0,
        XWFS_XWSYS_CMD_START,
        XWFS_XWSYS_CMD_NUM,
};

static
ssize_t xwfs_xwsys_cmd_show(struct xwsys_object * xwobj,
                            struct xwsys_attribute * xwattr,
                            char * buf);

static
ssize_t xwfs_xwsys_cmd_store(struct xwsys_object * xwobj,
                             struct xwsys_attribute * xwattr,
                             const char * buf,
                             size_t cnt);

static XWSYS_ATTR(file_xwfs_cmd, cmd, 0644,
                  xwfs_xwsys_cmd_show,
                  xwfs_xwsys_cmd_store);

static const match_table_t xwfs_cmd_tokens = {
        {XWFS_XWSYS_CMD_STOP, "stop"},
        {XWFS_XWSYS_CMD_START, "start"},
        {XWFS_XWSYS_CMD_NUM, NULL},
};

static
ssize_t xwfs_xwsys_cmd_show(struct xwsys_object * xwobj,
                            struct xwsys_attribute * xwattr,
                            char * buf)
{
        return -ENOSYS;
}

static
xwer_t xwfs_xwsys_cmd_parse(const char * cmdstring, size_t cnt)
{
        int token;
        substring_t tmp[MAX_OPT_ARGS];
        char cmd[cnt + 1];
        char * p, * pos;
        xwer_t rc = -ENOSYS;

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

        xwfslogf(INFO, "cmd:\"%s\"\n", cmd);
        pos = (char *)cmd;
        while ((p = strsep(&pos, ";")) != NULL) {
                if (!*p) {
                        continue;
                }
                token = match_token(p, xwfs_cmd_tokens, tmp);
                switch (token) {
                case XWFS_XWSYS_CMD_STOP:
                        rc = xwfs_stop();
                        break;
                case XWFS_XWSYS_CMD_START:
                        rc = xwfs_start();
                        break;
                }
        }

        return rc;
}

static
ssize_t xwfs_xwsys_cmd_store(struct xwsys_object * xwobj,
                             struct xwsys_attribute * xwattr,
                             const char * buf,
                             size_t cnt)
{
        xwer_t rc;

        rc = xwfs_xwsys_cmd_parse(buf, cnt);
        if (__xwcc_unlikely(rc < 0)) {
                cnt = rc;
        }
        return cnt;
}

/******** /sys/xwos/xwfs/state ********/
static
ssize_t xwfs_xwsys_state_show(struct xwsys_object * xwobj,
                              struct xwsys_attribute * xwattr,
                              char * buf);

static
ssize_t xwfs_xwsys_state_store(struct xwsys_object * xwobj,
                               struct xwsys_attribute * xwattr,
                               const char * buf,
                               size_t cnt);

static XWSYS_ATTR(file_xwfs_state, state, 0644,
                  xwfs_xwsys_state_show,
                  xwfs_xwsys_state_store);

static
ssize_t xwfs_xwsys_state_show(struct xwsys_object * xwobj,
                              struct xwsys_attribute * xwattr,
                              char * buf)
{
        ssize_t showcnt;

        showcnt = 0;
        showcnt += sprintf(&buf[showcnt], "[XWFS]\n");
        if (xwfs_mntcnt < 1) {
                showcnt += sprintf(&buf[showcnt], "State: OFF\n");
        } else {
                showcnt += sprintf(&buf[showcnt], "State: ON\n");
        }
        buf[showcnt] = '\0';
        return showcnt;
}

static
ssize_t xwfs_xwsys_state_store(struct xwsys_object * xwobj,
                               struct xwsys_attribute * xwattr,
                               const char * buf,
                               size_t cnt)
{
        return -ENOSYS;
}

xwer_t xwfs_init(void)
{
        xwer_t rc;

        xwfs_entry_cache = kmem_cache_create("xwfs_entry_cache",
                                             sizeof(union xwfs_entry),
                                             0,
                                             (SLAB_RECLAIM_ACCOUNT |
                                              SLAB_MEM_SPREAD |
                                              SLAB_PANIC),
                                             xwfs_entry_construct);
        if (__xwcc_unlikely(NULL == xwfs_entry_cache)) {
                rc = -ENOMEM;
                xwfslogf(ERR,
                         "Create slab xwfs_entry_cache ... [FAILED], rc:%d\n",
                         rc);
                goto err_xwfs_entry_cache;
        }
        rc = register_filesystem(&xwfs_fstype);
        if (__xwcc_unlikely(rc < 0)) {
                xwfslogf(ERR, "Register xwfs ... [FAILED], rc:%d\n", rc);
                goto err_reg_fs;
        }
        xwfslogf(INFO, "Register xwfs ... [OK]\n");

        xwfs_xwsys_obj = xwsys_register("xwfs", NULL, NULL);
        if (__xwcc_unlikely(is_err_or_null(xwfs_xwsys_obj))) {
                rc = PTR_ERR(xwfs_xwsys_obj);
                xwfslogf(ERR,
                         "Create \"/sys/xwos/xwfs\" ... [rc:%d]\n",
                         rc);
                goto err_xwfs_xwsys_obj_create;
        }
        xwfslogf(INFO, "Create \"/sys/xwos/xwfs\" ... [OK]\n");

        rc = xwsys_create_file(xwfs_xwsys_obj, &xwsys_attr_file_xwfs_cmd);
        if (__xwcc_unlikely(rc < 0)) {
                xwfslogf(ERR,
                         "Create \"/sys/xwos/xwfs/cmd\" ... [rc:%d]\n",
                         rc);
                goto err_xwfs_xwsys_attr_cmd_create;
        }
        xwfslogf(INFO, "Create \"/sys/xwos/xwfs/cmd\" ... [OK]\n");

        rc = xwsys_create_file(xwfs_xwsys_obj, &xwsys_attr_file_xwfs_state);
        if (__xwcc_unlikely(rc < 0)) {
                xwfslogf(ERR,
                         "Create \"/sys/xwos/xwfs/state\" ... [rc:%d]\n",
                         rc);
                goto err_xwfs_xwsys_attr_state_create;
        }
        xwfslogf(INFO, "Create \"/sys/xwos/xwfs/state\" ... [OK]\n");

        return XWOK;

err_xwfs_xwsys_attr_state_create:
        xwsys_remove_file(xwfs_xwsys_obj, &xwsys_attr_file_xwfs_cmd);
err_xwfs_xwsys_attr_cmd_create:
        xwsys_unregister(xwfs_xwsys_obj);
        xwfs_xwsys_obj = NULL;
err_xwfs_xwsys_obj_create:
        unregister_filesystem(&xwfs_fstype);
err_reg_fs:
        kmem_cache_destroy(xwfs_entry_cache);
err_xwfs_entry_cache:
        return rc;
}

void xwfs_exit(void)
{
        xwsys_remove_file(xwfs_xwsys_obj, &xwsys_attr_file_xwfs_state);
        xwfslogf(INFO, "Destory \"/sys/xwos/xwfs/state\" ... [OK]\n");
        xwsys_remove_file(xwfs_xwsys_obj, &xwsys_attr_file_xwfs_cmd);
        xwfslogf(INFO, "Destory \"/sys/xwos/xwfs/cmd\" ... [OK]\n");
        xwsys_unregister(xwfs_xwsys_obj);
        xwfs_xwsys_obj = NULL;
        xwfslogf(INFO, "Destory \"/sys/xwos/xwfs\" ... [OK]\n");

        unregister_filesystem(&xwfs_fstype);
        kmem_cache_destroy(xwfs_entry_cache);
        xwfslogf(INFO, "Unregister xwfs ... [OK]\n");
}
