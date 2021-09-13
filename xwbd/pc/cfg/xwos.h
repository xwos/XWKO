/**
 * @file
 * @brief 内核配置
 */

#ifndef __cfg_xwos_h__
#define __cfg_xwos_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********        Common Configurations        ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWKNCFG_CHECK_PARAMETERS                        1
#define XWKNCFG_BUG                                     1
#define XWKNCFG_SYSHWT_PERIOD                           (XWTM_S / HZ)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         xwlib Configuration         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWLIBCFG_XWLOG_LEVEL                            (0)

#define XWLIBCFG_XWAOP8                                 1
#define XWLIBCFG_XWAOP16                                0
#define XWLIBCFG_XWAOP32                                1
#define XWLIBCFG_XWAOP64                                1
#define XWLIBCFG_XWBMPAOP                               1

#define XWLIBCFG_CRC32                                  1
#define XWLIBCFG_CRC32_0X04C11DB7                       1
#define XWLIBCFG_CRC32_0XEDB88320                       1

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   Memory Management Configurations  ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** default kernel memory allocater ******** ********/
#define XWMMCFG_KMA_SIZE                                (4096U)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********        Kernel Configuration         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWKNCFG_SYNC_EVT_MAXNUM                         32

#endif /* cfg/xwos.h */
