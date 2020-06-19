/**
 * @file
 * @brief Hi, XuanWuOS模块
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
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwos/osal/sync/semaphore.h>

#define HIXWOS_PRODUCER_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 20) /* 生产者线程优先级*/
#define HIXWOS_CONSUMER_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 20) /* 消费者线程优先级*/

struct xwosal_smr hixwos_smr; /**< 信号量 */
xwid_t hixwos_producer_thrd_tid; /**< 生产者线程id */
xwid_t hixwos_consumer_thrd_tid; /**< 消费者线程id */

/**
 * @brief 日志输出函数
 */
#define hixwoslogf(lv, fmt, ...)  xwlogf(lv, "hixwos", fmt, ##__VA_ARGS__)

/**
 * @brief 生产者线程
 */
xwer_t hixwos_producer_thrd(void * arg)
{
        xwid_t smrid = (xwid_t)arg;
        xwtm_t time;
        xwsq_t i;

        hixwoslogf(INFO, "Hi, XuanWuOS! I'am producer.\n");
        for (i = 0; i < 3; i++) { /* 循环3次 */
                time = 1 * XWTM_S;
                xwosal_cthrd_sleep(&time); /* 睡眠1s钟 */
                hixwoslogf(INFO, "Producer: Post once, ID:%d\n", i);
                xwosal_smr_post(smrid); /* 发送信号量 */
        }
        hixwoslogf(INFO, "Producer exit.\n");
        return OK;
}

/**
 * @brief 消费者线程
 */
xwer_t hixwos_consumer_thrd(void * arg)
{
        xwid_t smrid = (xwid_t)arg;
        xwer_t rc;
        xwsq_t i;

        hixwoslogf(INFO, "Hi, XuanWuOS! I'am consumer.\n");
        for (i = 0; i < 3; i++) { /* 循环3次 */
                rc = xwosal_smr_wait(smrid); /* 等待信号量 */
                if (OK == rc) {
                        hixwoslogf(INFO, "Consumer: Got! ID:%d\n", i);
                }
        }
        hixwoslogf(INFO, "Consumer exit.\n");
        return OK;
}

xwer_t hixwos_start(void)
{
        xwer_t rc;
        xwer_t trc;
        xwid_t smrid;

        /* 初始化信号量 */
        rc = xwosal_smr_init(&hixwos_smr, 0, XWSSQ_MAX);
        if (rc < 0) {
                hixwoslogf(ERR,
                           "Failed to hixwos_smr ... [rc:%d]\n",
                           rc);
                goto err_init_hixwos_smr;
        }
        smrid = xwosal_smr_get_id(&hixwos_smr);

        /* 建立生产者线程 */
        rc = xwosal_thrd_create(&hixwos_producer_thrd_tid,
                                "hixwos_producer_thrd",
                                hixwos_producer_thrd,
                                (void *)smrid, XWOS_UNUSED_ARGUMENT,
                                HIXWOS_PRODUCER_PRIORITY,
                                XWOS_UNUSED_ARGUMENT);
        if (rc < 0) {
                hixwoslogf(ERR,
                           "Create hixwos_producer_thrd ... [FAILED]! rc:%d\n",
                           rc);
                goto err_create_hixwos_producer_thrd;
        }

        /* 建立消费者线程 */
        rc = xwosal_thrd_create(&hixwos_consumer_thrd_tid,
                                "hixwos_consumer_thrd",
                                hixwos_consumer_thrd,
                                (void *)smrid, XWOS_UNUSED_ARGUMENT,
                                HIXWOS_CONSUMER_PRIORITY,
                                XWOS_UNUSED_ARGUMENT);
        if (rc < 0) {
                hixwoslogf(ERR,
                           "Create hixwos_consumer_thrd ... [FAILED]! rc:%d\n",
                           rc);
                goto err_create_hixwos_consumer_thrd;
        }

        return OK;

err_create_hixwos_consumer_thrd:
        xwosal_thrd_terminate(hixwos_producer_thrd_tid, &trc);
        xwosal_thrd_delete(hixwos_producer_thrd_tid);
err_create_hixwos_producer_thrd:
        xwosal_smr_destroy(&hixwos_smr);
err_init_hixwos_smr:
        return rc;
}

void hixwos_stop(void)
{
        xwer_t trc;

        /* 销毁消费者线程 */
        xwosal_thrd_terminate(hixwos_consumer_thrd_tid, &trc);
        xwosal_thrd_delete(hixwos_consumer_thrd_tid);

        /* 销毁生产者线程 */
        xwosal_thrd_terminate(hixwos_producer_thrd_tid, &trc);
        xwosal_thrd_delete(hixwos_producer_thrd_tid);

        /* 销毁信号量 */
        xwosal_smr_destroy(&hixwos_smr);
}

xwer_t hixwos_init(void)
{
        return hixwos_start();
}

void hixwos_exit(void)
{
        hixwos_stop();
}
