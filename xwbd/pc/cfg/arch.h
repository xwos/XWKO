/**
 * @file
 * @brief 架构描述层(ADL)配置
 */

#ifndef __cfg_arch_h__
#define __cfg_arch_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      architecture specification     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define ARCHCFG_LITTLE_ENDIAN                   1
#define ARCHCFG_BIG_ENDIAN                      0
#define ARCHCFG_16BIT                           0
#define ARCHCFG_32BIT                           0
#define ARCHCFG_64BIT                           1

/******** ******** architecture libs ******** ********/
/******** clib ********/
#define ARCHCFG_COMPILER_CLIB                   1

/******** xwbop ********/
#define ARCHCFG_LIB_XWBOP_SBIT8                 0
#define ARCHCFG_LIB_XWBOP_CBIT8                 0
#define ARCHCFG_LIB_XWBOP_IBIT8                 0
#define ARCHCFG_LIB_XWBOP_FFS8                  0
#define ARCHCFG_LIB_XWBOP_FLS8                  0
#define ARCHCFG_LIB_XWBOP_RBIT8                 0

#define ARCHCFG_LIB_XWBOP_SBIT16                0
#define ARCHCFG_LIB_XWBOP_CBIT16                0
#define ARCHCFG_LIB_XWBOP_IBIT16                0
#define ARCHCFG_LIB_XWBOP_FFS16                 0
#define ARCHCFG_LIB_XWBOP_FLS16                 0
#define ARCHCFG_LIB_XWBOP_RBIT16                0
#define ARCHCFG_LIB_XWBOP_RE16                  0
#define ARCHCFG_LIB_XWBOP_RE16S32               0

#define ARCHCFG_LIB_XWBOP_SBIT32                0
#define ARCHCFG_LIB_XWBOP_CBIT32                0
#define ARCHCFG_LIB_XWBOP_IBIT32                0
#define ARCHCFG_LIB_XWBOP_FFS32                 0
#define ARCHCFG_LIB_XWBOP_FLS32                 0
#define ARCHCFG_LIB_XWBOP_RBIT32                0
#define ARCHCFG_LIB_XWBOP_RE32                  0
#define ARCHCFG_LIB_XWBOP_RE32S64               0

#define ARCHCFG_LIB_XWBOP_SBIT64                0
#define ARCHCFG_LIB_XWBOP_CBIT64                0
#define ARCHCFG_LIB_XWBOP_IBIT64                0
#define ARCHCFG_LIB_XWBOP_FFS64                 0
#define ARCHCFG_LIB_XWBOP_FLS64                 0
#define ARCHCFG_LIB_XWBOP_RBIT64                0
#define ARCHCFG_LIB_XWBOP_RE64                  0

#define ARCHCFG_LIB_XWBMPOP_FFS                 0
#define ARCHCFG_LIB_XWBMPOP_FFZ                 0
#define ARCHCFG_LIB_XWBMPOP_FLS                 0
#define ARCHCFG_LIB_XWBMPOP_FLZ                 0
#define ARCHCFG_LIB_XWBMPOP_SBIT                0
#define ARCHCFG_LIB_XWBMPOP_CBIT                0
#define ARCHCFG_LIB_XWBMPOP_IBIT                0
#define ARCHCFG_LIB_XWBMPOP_TBIT                0

/******** atomic operations ********/
#define ARCHCFG_LIB_XWAOP8                      0
#define ARCHCFG_LIB_XWAOP16                     0
#define ARCHCFG_LIB_XWAOP32                     0
#define ARCHCFG_LIB_XWAOP64                     0
#define ARCHCFG_LIB_XWBMPAOP                    0

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         default configures          ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if (((!defined(ARCHCFG_BIG_ENDIAN)) || (1 != ARCHCFG_BIG_ENDIAN)) && \
     ((!defined(ARCHCFG_LITTLE_ENDIAN)) || (1 != ARCHCFG_LITTLE_ENDIAN)))
  #error "Bit-endian is not defined!"
#endif

#if ((defined(ARCHCFG_64BIT) && (1 != ARCHCFG_64BIT)) && \
     (defined(ARCHCFG_32BIT) && (1 != ARCHCFG_32BIT)) && \
     (defined(ARCHCFG_16BIT) && (1 != ARCHCFG_16BIT)))
  #error "ARCH-bits is not defined!"
#endif

#if ((defined(ARCHCFG_64BIT) && (1 == ARCHCFG_64BIT)) && \
     (defined(ARCHCFG_32BIT) && (1 == ARCHCFG_32BIT)) && \
     (defined(ARCHCFG_16BIT) && (1 == ARCHCFG_16BIT)))
  #error "ARCH-bits is multi-defined!"
#endif

#if ((defined(ARCHCFG_32BIT) && (1 == ARCHCFG_32BIT)) && \
     (defined(ARCHCFG_16BIT) && (1 == ARCHCFG_16BIT)))
  #error "ARCH-bits is multi-defined!"
#endif

#if ((defined(ARCHCFG_64BIT) && (1 == ARCHCFG_64BIT)) && \
     (defined(ARCHCFG_16BIT) && (1 == ARCHCFG_16BIT)))
  #error "ARCH-bits is multi-defined!"
#endif

#if ((defined(ARCHCFG_64BIT) && (1 == ARCHCFG_64BIT)) && \
     (defined(ARCHCFG_32BIT) && (1 == ARCHCFG_32BIT)))
  #error "ARCH-bits is multi-defined!"
#endif

#endif /* cfg/arch.h */
