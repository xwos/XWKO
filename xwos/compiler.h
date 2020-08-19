/**
 * @file
 * @brief 玄武OS的编译器宏定义
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
 * @note
 * + 段说明
 *   - __xwos_init_*
 *     - code: XWOS初始化阶段的代码
 *     - rodata: XWOS初始化阶段的const数据
 *     - （初始化阶段可能数据还未重定向至内存，因此不存在__xwos_init_data）
 *   - __xwos_exit_*
 *     - code: XWOS退出阶段的代码
 *     - rodata: XWOS退出阶段的const数据
 *   - __xwos_*
 *     - code: XWOS内核的代码
 *     - vctbl: 中断向量表
 *     - isr: 中断函数
 *     - bh: 底半部函数
 *     - api: XWOS的内核API函数
 *     - rodata: XWOS内核的const数据
 *     - data: XWOS内核的数据
 *   - __xwlib_*
 *     - code: XWOS的C库函数
 *     - rodata: XWOS的C库const数据
 *     - data: XWOS的C库数据
 *   - __xwbsp_*
 *     - init_code: BSP的初始化阶段的代码
 *     - init_rodata: BSP的初始化阶段的const数据
 *     - exit_code: BSP的退出阶段的代码
 *     - exit_rodata: BSP的退出阶段的const数据
 *     - code: BSP中的代码
 *     - api: BSP中的API函数
 *     - isr: BSP中的中断函数
 *     - rodata: BSP中的const数据
 *     - data: BSP中的数据
 *   - __xwmd_*
 *     - code: XWOS中间件的代码
 *     - isr: XWOS中间件中断函数
 *     - api: XWOS中间件的API函数
 *     - rodata: XWOS中间件的const数据
 *     - data: XWOS中间件的数据
 */

#ifndef __xwos_compiler_h__
#define __xwos_compiler_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <cfg/XuanWuOS.h>
#include <linux/version.h>
#include <linux/compiler.h>
#include <asm/barrier.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      section      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#ifndef __xwos_init_code
  #define __xwos_init_code
#endif

#ifndef __xwos_init_rodata
  #define __xwos_init_rodata
#endif

#ifndef __xwos_exit_code
  #define __xwos_exit_code
#endif

#ifndef __xwos_exit_rodata
  #define __xwos_exit_rodata
#endif

#ifndef __xwos_vctbl
  #define __xwos_vctbl
#endif

#ifndef __xwos_isr
  #define __xwos_isr
#endif

#ifndef __xwos_bh
  #define __xwos_bh             __xwos_isr
#endif

#ifndef __xwos_code
  #define __xwos_code
#endif

#ifndef __xwos_api
  #define __xwos_api            __xwos_code
#endif

#ifndef __xwos_inline_api
  #define __xwos_inline_api     __xwcc_inline
#endif

#ifndef __xwos_rodata
  #define __xwos_rodata
#endif

#ifndef __xwos_data
  #define __xwos_data
#endif

#ifndef __xwlib_code
  #define __xwlib_code          __xwos_code
#endif

#ifndef __xwlib_inline
  #define __xwlib_inline        __xwcc_inline
#endif

#ifndef __xwlib_data
  #define __xwlib_data          __xwos_data
#endif

#ifndef __xwlib_rodata
  #define __xwlib_rodata        __xwos_rodata
#endif

#ifndef __xwbsp_init_code
  #define __xwbsp_init_code     __xwos_init_code
#endif

#ifndef __xwbsp_init_rodata
  #define __xwbsp_init_rodata   __xwos_init_rodata
#endif

#ifndef __xwbsp_exit_code
  #define __xwbsp_exit_code     __xwos_exit_code
#endif

#ifndef __xwbsp_exit_rodata
  #define __xwbsp_exit_rodata   __xwos_exit_rodata
#endif

#ifndef __xwbsp_code
  #define __xwbsp_code          __xwos_code
#endif

#ifndef __xwbsp_api
  #define __xwbsp_api           __xwos_api
#endif

#ifndef __xwbsp_isr
  #define __xwbsp_isr           __xwos_isr
#endif

#ifndef __xwbsp_inline
  #define __xwbsp_inline        __xwcc_inline
#endif

#ifndef __xwbsp_data
  #define __xwbsp_data          __xwos_data
#endif

#ifndef __xwbsp_rodata
  #define __xwbsp_rodata        __xwos_rodata
#endif

#ifndef __xwmd_code
  #define __xwmd_code
#endif

#ifndef __xwmd_api
  #define __xwmd_api            __xwmd_code
#endif

#ifndef __xwmd_inline
  #define __xwmd_inline         __xwcc_inline
#endif

#ifndef __xwmd_inline_api
  #define __xwmd_inline_api     __xwos_inline_api
#endif

#ifndef __xwmd_isr
  #define __xwmd_isr
#endif

#ifndef __xwmd_bh
  #define __xwmd_bh             __xwmd_isr
#endif

#ifndef __xwmd_rodata
  #define __xwmd_rodata
#endif

#ifndef __xwmd_data
  #define __xwmd_data
#endif

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#ifndef __xwcc_section
  #define __xwcc_section(s)     __attribute__((__section__(#s)))
#endif

#ifndef __xwcc_aligned
  #define __xwcc_aligned(x)     __attribute__((aligned(x)))
#endif

#ifndef __xwcc_weak
  #define __xwcc_weak           __attribute__((weak))
#endif

#ifndef __xwcc_alias
  #define __xwcc_alias(s)       __attribute__((alias(#s)))
#endif

#ifndef __xwcc_inline
  #define __xwcc_inline         inline __attribute__((always_inline))
#endif

#ifndef __xwcc_pure
  #define __xwcc_pure           __attribute__((pure))
#endif

#ifndef __xwcc_packed
  #define __xwcc_packed         __attribute__((packed))
#endif

#ifndef __xwcc_must_check
  #define __xwcc_must_check     __attribute__((warn_unused_result))
#endif

#ifndef __xwcc_naked
  #define __xwcc_naked          __attribute__((naked))
#endif

#ifndef __xwcc_unused
  #define __xwcc_unused         __attribute__((unused))
#endif

#ifndef __xwcc_used
  #define __xwcc_used           __attribute__((used))
#endif

#ifndef __xwcc_noreturn
  #define __xwcc_noreturn       __attribute__((noreturn))
#endif

#ifndef __xwcc_hot
  #define __xwcc_hot            __attribute__((hot))
#endif

#ifndef __xwcc_atomic
  #define __xwcc_atomic         volatile
#endif

#ifndef __xwcc_likely
  #define __xwcc_likely(x)      __builtin_expect(!!(x), 1)
#endif

#ifndef __xwcc_unlikely
  #define __xwcc_unlikely(x)    __builtin_expect(!!(x), 0)
#endif

#ifndef __xwcc_aligned_l1cacheline
  #define __xwcc_aligned_l1cacheline \
          __attribute__((aligned(CPU_CONFIG_L1_CACHELINE_SIZE)))
#endif

#ifndef xwcc_offsetof
/**
 * @brief 计算某个成员在结构体中偏移量
 * @param type: (I) 结构体类型
 * @param member: (I) 成员在结构体中符号名
 */
  #define xwcc_offsetof(type, member) __builtin_offsetof(type, member)
#endif

#ifndef xwcc_baseof
/**
 * @brief 从结构体的某个成员的地址计算出结构体的首地址
 * @param ptr: (I) 结构体某个成员的地址
 * @param type: (I) 结构体类型
 * @param member: (I) 成员在结构体中符号名
 */
  #define xwcc_baseof(ptr, type, member) \
          ((type *)(((xwptr_t)(ptr)) - xwcc_offsetof(type, member)))
#endif

#define xwmb_smp_mb()           smp_mb()
#define xwmb_smp_ddb()          smp_read_barrier_depends()
#define xwmb_smp_rmb()          smp_rmb()
#define xwmb_smp_wmb()          smp_wmb()
#define xwccmb()                barrier()
#define xwmb_access(x)          ACCESS_ONCE(x)
#define xwmb_fcrd(x)            READ_ONCE(x)
#define xwmb_fcwr(x)            WRITE_ONCE(x)

/******** ******** SMP primitives ******** ********/
#ifndef xwmb_smp_load_acquire
/**
 * @brief: macro to load a pointer safely for later dereference
 * @param v: (I) lvalue
 * @param p: (I) the address to load
 */
  #define xwmb_smp_load_acquire(v, p)  do {v = smp_load_acquire(p);} while(0)
#endif

#ifndef xwmb_smp_store_release
/**
 * @brief Store a value to a pointer and prevents the compiler/HW from
 *        reordering the code that initializes the structure after the pointer
 *        assignment.
 * @param p: (O) pointer to the address that is assigned to
 * @param v: (I) value to assign (publish)
 */
  #define xwmb_smp_store_release(p, v)  smp_store_release(p, v)
#endif

#ifndef xwmb_smp_store_mb
/**
 * @brief Store a value to a pointer and prevents the compiler/HW from
 *        reordering the code that initializes the structure before the pointer
 *        assignment.
 * @param p: (O) pointer to the address that is assigned to
 * @param v: (I) value to assign
 */
  #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 2, 0)
    #define xwmb_smp_store_mb(p, v)  smp_store_mb((*(p)), v)
  #else
    #define xwmb_smp_store_mb(p, v)  \
            do {WRITE_ONCE((*(p)), v); smp_mb();} while (0)
  #endif
#endif

#ifndef stringify
/**
 * @brief 字符串化
 * @param x: (I) 将x变为字符串
 * @note
 * C语言宏展开的规则：遇到#和##就停止展开宏。
 * 因此如果直接定义stringify(m)为#m，
 * 遇到下面的代码将不能正常工作：
 * ```C
 * #define stringify(m)       #m
 * #define NAME               Roy
 * const char my_name[] = stringify(NAME);
 * // 结果是 my_name[] = "NAME";
 * ```
 * 正确的定义方法应该是：
 * ```C
 * __stringify(m)             #m
 * #define stringify(m)       __stringify(m)
 * #define NAME               Roy
 * const char my_name[] = stringify(NAME);
 * // 结果是 my_name[] = "Roy";
 * ```
 * 因为NAME在stringify(m)中展开，此时没有遇到#或##。
 */
  #define stringify(m)                    __stringify(m)
#endif /* stringify */

#endif /* xwos/compiler.h */
