/**
 * @file
 * @brief 玄武OS的基本类型定义
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

#ifndef __xwos_type_h__
#define __xwos_type_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <cfg/XuanWuOS.h>
#include <arch_type.h>
#include <stdarg.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#ifndef NULL
  #define NULL ((void *)0)
#endif
#define NIL  NULL

#if !defined(ARCH_HAVE_XWU8_T)
typedef arch_uint8_t xwu8_t;
#endif

#if !defined(ARCH_HAVE_XWS8_T)
typedef arch_int8_t xws8_t;
#endif

#if !defined(ARCH_HAVE_XWU16_T)
typedef arch_uint16_t xwu16_t;
#endif

#if !defined(ARCH_HAVE_XWS16_T)
typedef arch_int16_t xws16_t;
#endif

#if !defined(ARCH_HAVE_XWU32_T)
typedef arch_uint32_t xwu32_t;
#endif

#if !defined(ARCH_HAVE_XWS32_T)
typedef arch_int32_t xws32_t;
#endif

#if !defined(ARCH_HAVE_XWU64_T)
typedef arch_uint64_t xwu64_t;
#endif

#if !defined(ARCH_HAVE_XWS64_T)
typedef arch_int64_t xws64_t;
#endif

#if !defined(ARCH_HAVE_XWSZ_T)
typedef arch_size_t xwsz_t; /**< 大小值 (无符号) */
#endif

#if !defined(ARCH_HAVE_XWSSZ_T)
typedef arch_ssize_t xwssz_t; /**< 大小值 (有符号) */
#endif

#if !defined(ARCH_HAVE_XWSTK_T)
typedef unsigned long xwstk_t; /**< 栈槽 */
#endif

#if !defined(ARCH_HAVE_XWPTR_T)
typedef unsigned long xwptr_t; /**< 指针数值 */
#endif

#if !defined(ARCH_HAVE_XWREG_T)
typedef unsigned long xwreg_t; /**< 寄存器数值 (无符号) */
#endif

#if !defined(ARCH_HAVE_XWSREG_T)
typedef signed long xwsreg_t; /**< 寄存器数值 (有符号) */
#endif

#if !defined(ARCH_HAVE_XWER_T)
typedef signed int xwer_t; /**< 错误码 (signed) */
#endif

#if !defined(ARCH_HAVE_XWSQ_T)
typedef unsigned long xwsq_t; /**< 顺序值 (无符号) */
#endif

#if !defined(ARCH_HAVE_XWSSQ_T)
typedef signed long xwssq_t; /**< 顺序值 (有符号) */
#endif

#if !defined(ARCH_HAVE_XWID_T)
typedef unsigned long xwid_t; /**< ID (无符号) */
#endif

#if !defined(ARCH_HAVE_XWSID_T)
typedef signed long xwsid_t; /**< ID (有符号) */
#endif

#if !defined(ARCH_HAVE_XWISR_F)
typedef void (*xwisr_f)(void); /**< 中断向量 */
#endif

#if !defined(ARCH_HAVE_XWIRQ_T)
typedef signed int xwirq_t; /**< 中断号 (有符号) */
#endif

#if !defined(ARCH_HAVE_XWPR_T)
typedef signed int xwpr_t; /**< 优先级 (有符号) */
#endif

#if !defined(ARCH_HAVE_XWBMP_T)
typedef unsigned long xwbmp_t; /**< 位图 (无符号) */
#endif

#if !defined(ARCH_HAVE_XWTM_T)
typedef signed long long xwtm_t; /**< XWOS系统时间 (有符号) */
#endif

#if !defined(ARCH_HAVE_XWLFQ_T)
typedef xwptr_t xwlfq_t; /**< 无锁队列 */
#endif

typedef void (* ctor_f)(void * /*obj*/); /**< 构造函数 */
typedef void (* dtor_f)(void * /*obj*/); /**< 析构函数 */

typedef xws64_t (* xwsc_f)(void * /*arg*/, ...); /**< 系统调用 */

/**
 * @brief 原子变量测试函数指针类型
 */
typedef bool (* xwaop_tst_f)(const void * /* ov */ ,
                             void * /* args */);

/**
 * @brief 原子变量操作函数指针类型
 */
typedef void (* xwaop_op_f)(void * /* nv */,
                            const void * /* ov */,
                            void * /* args */);

/**
 * @brief 原子操作内存模型
 */
enum xwmb_memory_order_em {
        xwmb_modr_relaxed = __ATOMIC_RELAXED,
        xwmb_modr_consume = __ATOMIC_CONSUME,
        xwmb_modr_acquire = __ATOMIC_ACQUIRE,
        xwmb_modr_release = __ATOMIC_RELEASE,
        xwmb_modr_acq_rel = __ATOMIC_ACQ_REL,
        xwmb_modr_seq_cst = __ATOMIC_SEQ_CST,
};

/**
 * @breif XWOS的锁类型枚举
 */
enum xwos_lock_type_em {
        XWLK_TYPE_NONE, /**< 无 */
        XWLK_TYPE_MTX, /**< 互斥锁 */
        XWLK_TYPE_MTX_UNINTR, /**< 不可中断的互斥锁 */
        XWLK_TYPE_SPLK, /**< 自旋锁 */
        XWLK_TYPE_SQLK_WR, /**< 顺序写锁 */
        XWLK_TYPE_SQLK_RDEX, /**< 顺序读锁 */
        XWLK_TYPE_CALLBACK, /**< 抽象回调锁 */
        XWLK_TYPE_NUM, /**< 锁类型的数量 */
};

/**
 * @breif 类型为XWLK_TYPE_CALLBACK时的抽象锁函数
 */
struct xwlk_cblk {
        xwer_t (* lock)(void * /*arg*/); /**< 加锁函数 */
        xwer_t (* unlock)(void * /*arg*/); /**< 解锁函数*/
};

/**
 * @breif XWOS锁状态
 */
enum xwlk_state_em {
        XWLK_STATE_LOCKED = 0, /**< 锁定 */
        XWLK_STATE_UNLOCKED, /**< 未锁定 */
};

struct xwlk_mtx;
struct xwlk_splk;
struct xwlk_sqlk;
struct xwosal_mtx;
struct xwosal_splk;
struct xwosal_sqlk;

/**
 * @breif 所有锁的联合
 * @note
 * - 此联合只是一个指针数值，使用联合是为了避免强制类型转换。
 */
union xwlk_ulock {
        union {
                struct xwlk_mtx * mtx; /**< 互斥锁 */
                struct xwlk_splk * splk; /**< 自旋锁 */
                struct xwlk_sqlk * sqlk; /**< 顺序锁 */
        } xwos;
        union {
                struct xwosal_mtx * mtx; /**< 互斥锁 */
                struct xwosal_splk * splk; /**< 自旋锁 */
                struct xwosal_sqlk * sqlk; /**< 顺序锁 */
                xwid_t id; /**< 锁的ID */
        } osal;
        struct xwlk_cblk * cb; /**< 回调锁 */
        void * anon; /**< 匿名指针 */
        xwptr_t ptr; /**< 指针值 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#ifndef UCHAR_MAX
  #define UCHAR_MAX     ((unsigned char)(~((unsigned char)0)))
#endif
#ifndef CHAR_MAX
  #define CHAR_MAX      ((signed char)(UCHAR_MAX >> (unsigned char)1))
#endif
#ifndef CHAR_MIN
  #define CHAR_MIN      ((signed char)(-CHAR_MAX - (signed char)1))
#endif

#ifndef USHRT_MAX
  #define USHRT_MAX     ((unsigned short)(~((unsigned short)0)))
#endif
#ifndef SHRT_MAX
  #define SHRT_MAX      ((signed short)(USHRT_MAX >> (unsigned short)1))
#endif
#ifndef SHRT_MIN
  #define SHRT_MIN      ((signed short)(-SHRT_MAX - (signed short)1))
#endif

#ifndef UINT_MAX
  #define UINT_MAX      ((unsigned int)(~((unsigned int)0)))
#endif
#ifndef INT_MAX
  #define INT_MAX       ((signed int)(UINT_MAX >> (unsigned int)1))
#endif
#ifndef INT_MIN
  #define INT_MIN       ((signed int)(-INT_MAX - (signed int)1))
#endif

#ifndef ULONG_MAX
  #define ULONG_MAX     ((unsigned long)(~((unsigned long)0)))
#endif
#ifndef LONG_MAX
  #define LONG_MAX      ((signed long)(ULONG_MAX >> (unsigned long)1))
#endif
#ifndef LONG_MIN
  #define LONG_MIN      ((signed long)(-LONG_MAX - (signed long)1))
#endif

#ifndef ULLONG_MAX
  #define ULLONG_MAX    ((unsigned long long)(~((unsigned long long)0)))
#endif
#ifndef LLONG_MAX
  #define LLONG_MAX     ((signed long long)(ULLONG_MAX >> (unsigned long long)1))
#endif
#ifndef LLONG_MIN
  #define LLONG_MIN     ((signed long long)(-LLONG_MAX - (signed long long)1))
#endif

#ifndef XWU8_MAX
  #define XWU8_MAX      ((xwu8_t)(~((xwu8_t)0)))
#endif
#ifndef XWS8_MAX
  #define XWS8_MAX      ((xws8_t)(XWU8_MAX >> (xwu8_t)1))
#endif
#ifndef XWS8_MIN
  #define XWS8_MIN      ((xws8_t)(-XWS8_MAX - (xws8_t)1))
#endif

#ifndef XWU16_MAX
  #define XWU16_MAX     ((xwu16_t)(~((xwu16_t)0)))
#endif
#ifndef XWS16_MAX
  #define XWS16_MAX     ((xws16_t)(XWU16_MAX >> (xwu16_t)1))
#endif
#ifndef XWS16_MIN
  #define XWS16_MIN     ((xws16_t)(-XWS16_MAX - (xws16_t)1))
#endif

#ifndef XWU32_MAX
  #define XWU32_MAX     ((xwu32_t)(~((xwu32_t)0)))
#endif
#ifndef XWS32_MAX
  #define XWS32_MAX     ((xws32_t)(XWU32_MAX >> (xwu32_t)1))
#endif
#ifndef XWS32_MIN
  #define XWS32_MIN     ((xws32_t)(-XWS32_MAX - (xws32_t)1))
#endif

#ifndef XWU64_MAX
  #define XWU64_MAX     ((xwu64_t)(~((xwu64_t)0)))
#endif
#ifndef XWS64_MAX
  #define XWS64_MAX     ((xws64_t)(XWU64_MAX >> (xwu64_t)1))
#endif
#ifndef XWS64_MIN
  #define XWS64_MIN     ((xws64_t)(-XWS64_MAX - (xws64_t)1))
#endif

#ifndef XWSZ_MAX
  #define XWSZ_MAX      ((xwsz_t)(~((xwsz_t)0)))
#endif
#ifndef XWSSZ_MAX
  #define XWSSZ_MAX     ((xwssz_t)(XWSZ_MAX >> (xwsz_t)1))
#endif
#ifndef XWSSZ_MIN
  #define XWSSZ_MIN     ((xwssz_t)(-XWSSZ_MAX - (xwssz_t)1))
#endif

#ifndef XWPTR_MAX
  #define XWPTR_MAX     ((xwptr_t)(~(xwptr_t)0))
#endif

#ifndef XWREG_MAX
  #define XWREG_MAX     ((xwreg_t)(~((xwreg_t)0)))
#endif
#ifndef XWSREG_MAX
  #define XWSREG_MAX    ((xwsreg_t)(XWREG_MAX >> (xwreg_t)1))
#endif
#ifndef XWSREG_MIN
  #define XWSREG_MIN    ((xwsreg_t)(-XWSREG_MAX - (xwsreg_t)1))
#endif

#ifndef XWER_MAX
  #define XWER_MAX      ((xwer_t)OK)
#endif
#ifndef XWER_MIN
  #define XWER_MIN      ((xwer_t)(-MAX_ERRNO))
#endif

#ifndef XWSQ_MAX
  #define XWSQ_MAX      ((xwsq_t)(~((xwsq_t)0)))
#endif
#ifndef XWSSQ_MAX
  #define XWSSQ_MAX     ((xwssq_t)(XWSQ_MAX >> (xwsq_t)1))
#endif
#ifndef XWSSQ_MIN
  #define XWSSQ_MIN     ((xwssq_t)(-XWSSQ_MAX - (xwssq_t)1))
#endif

#ifndef XWID_MAX
  #define XWID_MAX      ((xwid_t)(~((xwid_t)0)))
#endif
#ifndef XWSID_MAX
  #define XWSID_MAX     ((xwsid_t)(XWID_MAX >> (xwid_t)1))
#endif
#ifndef XWSID_MIN
  #define XWSID_MIN     ((xwsid_t)(-XWSID_MAX - (xwsid_t)1))
#endif

#ifndef XWIRQ_MAX
  #define XWIRQ_MAX     ((xwirq_t)((~(xwu32_t)0) >> (xwu32_t)1))
#endif
#ifndef XWIRQ_MIN
  #define XWIRQ_MIN     ((xwirq_t)(-XWIRQ_MAX - (xwirq_t)1))
#endif

#ifndef XWPR_MAX
  #define XWPR_MAX      ((xwpr_t)((~(xwu32_t)0) >> (xwu32_t)1))
#endif
#ifndef XWPR_MIN
  #define XWPR_MIN      ((xwpr_t)(-XWPR_MAX - (xwpr_t)1))
#endif

#ifndef XWTM_MAX
  #define XWTM_MAX      ((xwtm_t)((~(xwu64_t)0) >> 1))
#endif
#ifndef XWTM_MIN
  #define XWTM_MIN      ((xwtm_t)(-XWTM_MAX - (xwtm_t)1))
#endif

#define XWTM_NS         (1LL)
#define XWTM_US         (1000LL)
#define XWTM_MS         (1000000LL)
#define XWTM_S          (1000000000LL)

#ifndef XWU8_T__SIZE
  #define BITS_PER_XWU8_T       8
  #define XWU8_T_SHIFT          3
#endif
#ifndef XWS8_T__SIZE
  #define BITS_PER_XWS8_T       8
  #define XWS8_T_SHIFT          3
#endif

#ifndef XWU16_T__SIZE
  #define BITS_PER_XWU16_T      16
  #define XWU16_T_SHIFT         4
#endif
#ifndef XWS16_T__SIZE
  #define BITS_PER_XWS16_T      16
  #define XWS16_T_SHIFT         4
#endif

#ifndef XWU32_T__SIZE
  #define BITS_PER_XWU32_T      32
  #define XWU32_T_SHIFT         5
#endif
#ifndef XWS32_T__SIZE
  #define BITS_PER_XWS32_T      32
  #define XWS32_T_SHIFT         5
#endif

#ifndef XWU64_T__SIZE
  #define BITS_PER_XWU64_T      64
  #define XWU64_T_SHIFT         6
#endif
#ifndef XWS64_T__SIZE
  #define BITS_PER_XWS64_T      64
  #define XWS64_T_SHIFT         6
#endif

#ifndef CHAR__SIZE
  #define BITS_PER_CHAR         8
  #define CHAR_SHIFT            3
#endif
#ifndef UCHAR__SIZE
  #define BITS_PER_UCHAR        8
  #define UCHAR_SHIFT           3
#endif

#ifndef SHORT__SIZE
  #define BITS_PER_SHORT        16
  #define SHORT_SHIFT           4
#endif
#ifndef USHORT__SIZE
  #define BITS_PER_USHORT       16
  #define USHORT_SHIFT          4
#endif

#ifndef INT__SIZE
  #define BITS_PER_INT          32
  #define INT_SHIFT             5
#endif
#ifndef UINT__SIZE
  #define BITS_PER_UINT         32
  #define UINT_SHIFT            5
#endif

#ifndef LONG__SIZE
  #define BITS_PER_LONG         64
  #define LONG_SHIFT            6
#endif
#ifndef ULONG__SIZE
  #define BITS_PER_ULONG        64
  #define LONG_SHIFT            6
#endif

#ifndef LONGLONG__SIZE
  #define BITS_PER_LONGLONG     64
  #define LONGLONG_SHIFT        6
#endif
#ifndef ULONGLONG__SIZE
  #define BITS_PER_ULONGLONG    64
  #define ULONGLONG_SHIFT       6
#endif

#ifndef FLOAT__SIZE
  #define BITS_PER_FLOAT        32
  #define FLOAT_SHIFT           5
#endif

#ifndef DOUBLE__SIZE
  #define BITS_PER_DOUBLE       64
  #define DOUBLE_SHIFT          6
#endif

#ifndef XWBMP_T__SIZE
  #define BITS_PER_XWBMP_T      BITS_PER_LONG
  #define XWBMP_T_SHIFT         LONG_SHIFT
#endif

#ifndef XWSZ_T__SIZE
  #define BITS_PER_XWSZ_T       BITS_PER_LONG
  #define XWSZ_T_SHIFT          LONG_SHIFT
#endif
#ifndef XWSSZ_T__SIZE
  #define BITS_PER_XWSSZ_T      BITS_PER_LONG
  #define XWSSZ_T_SHIFT         LONG_SHIFT
#endif

#ifndef XWPTR_T__SIZE
  #define BITS_PER_XWPTR_T      BITS_PER_LONG
  #define XWPTR_T_SHIFT         LONG_SHIFT
#endif

#ifndef XWER_T__SIZE
  #define BITS_PER_XWER_T       BITS_PER_INT
  #define XWER_T_SHIFT          INT_SHIFT
#endif

#ifndef XWSID_T__SIZE
  #define BITS_PER_XWSID_T      BITS_PER_LONG
  #define XWSID_T_SHIFT         LONG_SHIFT
#endif
#ifndef XWID_T__SIZE
  #define BITS_PER_XWID_T       BITS_PER_LONG
  #define XWID_T_SHIFT          LONG_SHIFT
#endif

#ifndef XWREG_T__SIZE
  #define BITS_PER_XWREG_T      BITS_PER_LONG
  #define XWREG_T_SHIFT         LONG_SHIFT
#endif
#ifndef XWSREG_T__SIZE
  #define BITS_PER_XWSREG_T     BITS_PER_LONG
  #define XWSREG_T_SHIFT        LONG_SHIFT
#endif

#ifndef XWSQ_T__SIZE
  #define BITS_PER_XWSQ_T       BITS_PER_LONG
  #define XWSQ_T_SHIFT          LONG_SHIFT
#endif
#ifndef XWSSQ_T__SIZE
  #define BITS_PER_XWSSQ_T      BITS_PER_LONG
  #define XWSSQ_T_SHIFT         LONG_SHIFT
#endif

#ifndef XWTM_T__SIZE
  #define BITS_PER_XWTM_T       BITS_PER_LONGLONG
  #define XWTM_T_SHIFT          LONGLONG_SHIFT
#endif

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      inline function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if !defined(ARCH_HAVE_XWTM_T)
/**
 * @brief 将两个系统时间相加，并检查溢出
 * @param a: (I) 加数a
 * @param b: (I) 加数b
 * @return 结果
 * @note
 * - 如果结果已经溢出，返回xwtm_t的最大值。
 */
static __xwcc_inline
xwtm_t xwtm_add_safely(const xwtm_t a, const xwtm_t b)
{
        xwtm_t res;

        res = a + b;
        if (__xwcc_unlikely(res < 0)) {
                res = XWTM_MAX;
        }/* else {} */
        return res;
}

/**
 * @brief 将两个系统时间相加
 * @param a: (I) 加数a
 * @param b: (I) 加数b
 * @retval 结果
 */
static __xwcc_inline
xwtm_t xwtm_add(const xwtm_t a, const xwtm_t b)
{
        return a + b;
}

/**
 * @brief 将两个系统时间相减
 * @param a: (I) 被减数
 * @param b: (I) 减数
 * @retval 结果
 */
static __xwcc_inline
xwtm_t xwtm_sub(const xwtm_t a, const xwtm_t b)
{
        return a - b;
}

/**
 * @brief 比较两个系统时间
 * @param a: (I) 系统时间a
 * @param b: (I) 系统时间b
 * @retval <0: a < b
 * @retval ==0: a == b
 * @retval >0: a > b
 */
static __xwcc_inline
xwer_t xwtm_cmp(const xwtm_t a, const xwtm_t b)
{
        xwer_t rc;

        if (a > b) {
                rc = 1;
        } else if (a < b) {
                rc = -1;
        } else {
                rc = 0;
        }
        return rc;
}
#endif /* #if !defined(ARCH_HAVE_XWTM_T) */

#endif /* xwos/type.h */
