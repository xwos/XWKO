/**
 * @file
 * @brief MCU通讯模块：用户态文件节点
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > Licensed under the MIT License (the "License");
 * > You may obtain a copy of the License at
 * >
 * >         https://mit-license.org
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

#ifndef __bm_mcuc_msgnode_h__
#define __bm_mcuc_msgnode_h__

#include <xwos/standard.h>

enum mcuc_msgnode_em {
        MCUC_MSGNODE_SYS,
        MCUC_MSGNODE_PWR,
        MCUC_MSGNODE_DAT,
        MCUC_MSGNODE_LOG,
        MCUC_MSGNODE_MFG,
        MCUC_MSGNODE_NUM,
};

struct mcuc_msgnode_info {
        struct {
                uint8_t port; /**< XWPCP port */
                uint8_t prio; /**< port default priority */
        } attr;
        struct xwfs_node * xwfsnode; /**< xwfs node who own this data */
};

extern struct mcuc_msgnode_info mcuc_msgnode[MCUC_MSGNODE_NUM];

xwer_t mcuc_msgnode_init(void);
xwer_t mcuc_msgnode_exit(void);

#endif /* bm/mcuc/msgnode.h */
