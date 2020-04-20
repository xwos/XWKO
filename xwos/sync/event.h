/**
 * @file
 * @brief XuanWuOS的同步机制：事件
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

#ifndef __xwos_sync_event_h__
#define __xwos_sync_event_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lock/spinlock.h>
#include <xwos/sync/object.h>
#include <xwos/sync/condition.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWSYNC_EVT_MAXNUM (XWKNCFG_SYNC_EVT_MAXNUM) /**< 事件最大数量 */
#define XWSYNC_EVT_DECLARE_BITMAP(name) xwbmp_t name[BITS_TO_BMPS(XWSYNC_EVT_MAXNUM)]

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 事件属性枚举
 */
enum xwsync_evt_attr_em {
        XWSYNC_EVT_TYPE_FLAG = 0, /**< 事件信号旗 */
        XWSYNC_EVT_TYPE_SELECTOR, /**< 选择器 */
        XWSYNC_EVT_TYPE_BARRIER, /**< 线程同步栅栏 */
        XWSYNC_EVT_TYPE_MAX,
        XWSYNC_EVT_TYPE_MASK = 0xFF,
};

/**
 * @brief 事件触发条件枚举
 */
enum xwsync_evt_trigger_em {
        XWSYNC_EVT_TRIGGER_SET_ALL = 0, /**< 掩码中所有位被置1 */
        XWSYNC_EVT_TRIGGER_SET_ANY, /**< 掩码中任何位被置1 */
        XWSYNC_EVT_TRIGGER_CLR_ALL, /**< 掩码中所有位被清0 */
        XWSYNC_EVT_TRIGGER_CLR_ANY, /**< 掩码中任何位被清0 */
        XWSYNC_EVT_TRIGGER_TGL_ALL, /**< 掩码中任何位翻转 */
        XWSYNC_EVT_TRIGGER_TGL_ANY, /**< 掩码中任何位翻转 */
        XWSYNC_EVT_TRIGGER_NUM,
};

/**
 * @brief 事件触发后的动作枚举
 */
enum xwsync_evt_action_em {
        XWSYNC_EVT_ACTION_NONE = 0, /**< 无动作 */
        XWSYNC_EVT_ACTION_CONSUMPTION, /**< 消费事件 */
        XWSYNC_EVT_ACTION_NUM,
};

/**
 * @brief 事件对象
 */
struct xwsync_evt {
        struct xwsync_cdt cdt; /**< C语言面向对象：继承struct xwsync_cdt */
        xwsq_t attr; /**< 属性 */
        DECLARE_BITMAP(bmp, XWSYNC_EVT_MAXNUM); /**< 事件位图 */
        DECLARE_BITMAP(msk, XWSYNC_EVT_MAXNUM); /**< 掩码位图 */
        struct xwlk_splk lock; /**< 保护位图的锁 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ********       internal inline function implementations        ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 增加对象的引用计数
 * @param evt: (I) 事件对象指针
 * @return 错误码
 */
static __xw_inline
xwer_t xwsync_evt_grab(struct xwsync_evt * evt)
{
        return xwsync_cdt_grab(&evt->cdt);
}

/**
 * @brief 减少对象的引用计数
 * @param evt: (I) 事件对象指针
 * @return 错误码
 */
static __xw_inline
xwer_t xwsync_evt_put(struct xwsync_evt * evt)
{
        return xwsync_cdt_put(&evt->cdt);
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     internal function prototypes    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_code
xwer_t xwsync_cdt_cache_create(void);

__xwos_code
void xwsync_cdt_cache_destroy(void);

__xwos_code
xwer_t xwsync_cdt_xwfs_init(void);

__xwos_code
void xwsync_cdt_xwfs_exit(void);

__xwos_code
xwer_t xwsync_evt_obj_bind(struct xwsync_evt * evt,
                           struct xwsync_object * obj,
                           xwsq_t pos,
                           bool exclusive);

__xwos_code
xwer_t xwsync_evt_obj_unbind(struct xwsync_evt * evt,
                             struct xwsync_object * obj,
                             bool exclusive);

__xwos_code
xwer_t xwsync_evt_obj_s1i(struct xwsync_evt * evt, struct xwsync_object * obj);

__xwos_code
xwer_t xwsync_evt_obj_c0i(struct xwsync_evt * evt, struct xwsync_object * obj);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_api
xwer_t xwsync_evt_init(struct xwsync_evt * evt, xwbmp_t initval[], xwsq_t attr);

__xwos_api
xwer_t xwsync_evt_destroy(struct xwsync_evt * evt);

__xwos_api
xwer_t xwsync_evt_create(struct xwsync_evt ** ptrbuf, xwbmp_t initval[], xwsq_t attr);

__xwos_api
xwer_t xwsync_evt_delete(struct xwsync_evt * evt);

__xwos_api
xwer_t xwsync_evt_bind(struct xwsync_evt * evt, struct xwsync_evt * slt, xwsq_t pos);

__xwos_api
xwer_t xwsync_evt_unbind(struct xwsync_evt * evt, struct xwsync_evt * slt);

__xwos_api
xwer_t xwsync_evt_intr_all(struct xwsync_evt * evt);

__xwos_api
xwer_t xwsync_evt_s1m(struct xwsync_evt * evt, xwbmp_t msk[]);

__xwos_api
xwer_t xwsync_evt_s1i(struct xwsync_evt * evt, xwsq_t pos);

__xwos_api
xwer_t xwsync_evt_c0m(struct xwsync_evt * evt, xwbmp_t msk[]);

__xwos_api
xwer_t xwsync_evt_c0i(struct xwsync_evt * evt, xwsq_t pos);

__xwos_api
xwer_t xwsync_evt_x1m(struct xwsync_evt * evt, xwbmp_t msk[]);

__xwos_api
xwer_t xwsync_evt_x1i(struct xwsync_evt * evt, xwsq_t pos);

__xwos_api
xwer_t xwsync_evt_read(struct xwsync_evt * evt, xwbmp_t out[]);

__xwos_api
xwer_t xwsync_evt_trywait(struct xwsync_evt * evt,
                          xwsq_t trigger, xwsq_t action,
                          xwbmp_t origin[], xwbmp_t msk[]);

__xwos_api
xwer_t xwsync_evt_timedwait(struct xwsync_evt * evt,
                            xwsq_t trigger, xwsq_t action,
                            xwbmp_t origin[], xwbmp_t msk[],
                            xwtm_t * xwtm);

__xwos_api
xwer_t xwsync_evt_tryselect(struct xwsync_evt * evt, xwbmp_t msk[], xwbmp_t trg[]);

__xwos_api
xwer_t xwsync_evt_timedselect(struct xwsync_evt * evt, xwbmp_t msk[], xwbmp_t trg[],
                              xwtm_t * xwtm);

__xwos_api
xwer_t xwsync_evt_timedsync(struct xwsync_evt * evt, xwsq_t pos, xwbmp_t sync[],
                            xwtm_t * xwtm);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      inline API implementations     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwos_inline_api
xwer_t xwsync_evt_wait(struct xwsync_evt * evt,
                       xwsq_t trigger, xwsq_t action,
                       xwbmp_t origin[], xwbmp_t msk[])
{
        xwtm_t expected = XWTM_MAX;
        return xwsync_evt_timedwait(evt, trigger, action, origin, msk, &expected);
}

static __xwos_inline_api
xwer_t xwsync_evt_select(struct xwsync_evt * evt, xwbmp_t msk[], xwbmp_t trg[])
{
        xwtm_t expected = XWTM_MAX;
        return xwsync_evt_timedselect(evt, msk, trg, &expected);
}

static __xwos_inline_api
xwer_t xwsync_evt_sync(struct xwsync_evt * evt, xwsq_t pos, xwbmp_t sync[])
{
        xwtm_t expected = XWTM_MAX;
        return xwsync_evt_timedsync(evt, pos, sync, &expected);
}

#endif /* xwos/sync/event.h */
