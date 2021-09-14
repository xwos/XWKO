/**
 * @file
 * @brief Hi, XuanWuOS模块
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
#include <xwos/lib/xwlog.h>
#include <xwos/osal/skd.h>
#include <xwos/osal/sync/sem.h>

#define HIXWOS_PRODUCER_PRIORITY                                        \
        XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 20) /* 生产者线程优先级*/
#define HIXWOS_CONSUMER_PRIORITY                                        \
        XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 20) /* 消费者线程优先级*/

struct xwos_sem hixwos_sem; /**< 信号量 */
struct xwos_thd * hixwos_producer_thd; /**< 生产者线程id */
struct xwos_thd * hixwos_consumer_thd; /**< 消费者线程id */

/**
 * @brief 日志输出函数
 */
#define hixwoslogf(lv, fmt, ...)  xwlogf(lv, "hixwos", fmt, ##__VA_ARGS__)

/**
 * @brief 生产者线程
 */
xwer_t hixwos_producer_thd_main(void * arg)
{
        struct xwos_sem * sem = arg;
        xwtm_t time;
        xwsq_t i;

        hixwoslogf(INFO, "Hi, XuanWuOS! I'am producer, tik:0x%X.\n",
                   xwos_thd_gettik(xwos_cthd_self()));
        for (i = 0; i < 3; i++) { /* 循环3次 */
                time = 1 * XWTM_S;
                xwos_cthd_sleep(&time); /* 睡眠1s钟 */
                hixwoslogf(INFO, "Producer: Post once, ID:%d\n", i);
                xwos_sem_post(sem); /* 发送信号量 */
        }
        hixwoslogf(INFO, "Producer exit.\n");
        return XWOK;
}

/**
 * @brief 消费者线程
 */
xwer_t hixwos_consumer_thd_main(void * arg)
{
        struct xwos_sem * sem = arg;
        xwer_t rc;
        xwsq_t i;

        hixwoslogf(INFO, "Hi, XuanWuOS! I'am consumer, tik:0x%X.\n",
                   xwos_thd_gettik(xwos_cthd_self()));
        for (i = 0; i < 3; i++) { /* 循环3次 */
                rc = xwos_sem_wait(sem); /* 等待信号量 */
                if (XWOK == rc) {
                        hixwoslogf(INFO, "Consumer: Got! ID:%d\n", i);
                }
        }
        hixwoslogf(INFO, "Consumer exit.\n");
        return XWOK;
}

xwer_t hixwos_start(void)
{
        xwer_t rc;
        xwer_t trc;
        struct xwos_sem * sem;

        /* 初始化信号量 */
        rc = xwos_sem_init(&hixwos_sem, 0, XWSSQ_MAX);
        if (rc < 0) {
                hixwoslogf(ERR,
                           "Failed to hixwos_sem ... [rc:%d]\n",
                           rc);
                goto err_init_hixwos_sem;
        }
        sem = &hixwos_sem;

        /* 建立生产者线程 */
        rc = xwos_thd_create(&hixwos_producer_thd,
                              "hixwos_producer_thd",
                              hixwos_producer_thd_main,
                              (void *)sem, XWOS_UNUSED_ARGUMENT,
                              HIXWOS_PRODUCER_PRIORITY,
                              XWOS_UNUSED_ARGUMENT);
        if (rc < 0) {
                hixwoslogf(ERR,
                           "Create hixwos_producer_thd ... [FAILED]! rc:%d\n",
                           rc);
                goto err_create_hixwos_producer_thd;
        }

        /* 建立消费者线程 */
        rc = xwos_thd_create(&hixwos_consumer_thd,
                              "hixwos_consumer_thd",
                              hixwos_consumer_thd_main,
                              (void *)sem, XWOS_UNUSED_ARGUMENT,
                              HIXWOS_CONSUMER_PRIORITY,
                              XWOS_UNUSED_ARGUMENT);
        if (rc < 0) {
                hixwoslogf(ERR,
                           "Create hixwos_consumer_thd ... [FAILED]! rc:%d\n",
                           rc);
                goto err_create_hixwos_consumer_thd;
        }

        return XWOK;

err_create_hixwos_consumer_thd:
        xwos_thd_stop(hixwos_producer_thd, &trc);
err_create_hixwos_producer_thd:
        xwos_sem_fini(&hixwos_sem);
err_init_hixwos_sem:
        return rc;
}

void hixwos_stop(void)
{
        xwer_t trc;

        /* 销毁消费者线程 */
        xwos_thd_stop(hixwos_consumer_thd, &trc);

        /* 销毁生产者线程 */
        xwos_thd_stop(hixwos_producer_thd, &trc);

        /* 销毁信号量 */
        xwos_sem_fini(&hixwos_sem);
}

xwer_t hixwos_init(void)
{
        return hixwos_start();
}

void hixwos_exit(void)
{
        hixwos_stop();
}
