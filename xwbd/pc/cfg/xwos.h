/**
 * @file
 * @brief 内核配置
 */

#ifndef __cfg_xwos_h__
#define __cfg_xwos_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********        general Configuration        ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWKNCFG_CHECK_PARAMETERS                        1
#define XWKNCFG_BUG                                     1
#define XWKNCFG_SYSHWT_PERIOD                           (XWTM_S / HZ)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         xwlib Configuration         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWLIBCFG_XWLOG_LEVEL                            (0)

#define XWLIBCFG_XWBOP_FFS8                             1
#define XWLIBCFG_XWBOP_FLS8                             1
#define XWLIBCFG_XWBOP_RBIT8                            1
#define XWLIBCFG_XWBOP_FFS16                            1
#define XWLIBCFG_XWBOP_FLS16                            1
#define XWLIBCFG_XWBOP_RBIT16                           1
#define XWLIBCFG_XWBOP_RE16                             1
#define XWLIBCFG_XWBOP_RE16S32                          1
#define XWLIBCFG_XWBOP_FFS32                            1
#define XWLIBCFG_XWBOP_FLS32                            1
#define XWLIBCFG_XWBOP_RBIT32                           1
#define XWLIBCFG_XWBOP_RE32                             1
#define XWLIBCFG_XWBOP_RE32S64                          1
#define XWLIBCFG_XWBOP_FFS64                            1
#define XWLIBCFG_XWBOP_FLS64                            1
#define XWLIBCFG_XWBOP_RBIT64                           1
#define XWLIBCFG_XWBOP_RE64                             1

#define XWLIBCFG_XWAOP8                                 1
#define XWLIBCFG_XWAOP16                                1
#define XWLIBCFG_XWAOP32                                1
#define XWLIBCFG_XWAOP64                                1
#define XWLIBCFG_XWBMPAOP                               1

#define XWLIBCFG_CRC32                                  1
#define XWLIBCFG_CRC32_0X04C11DB7                       1
#define XWLIBCFG_CRC32_0XEDB88320                       1

/******** ******** ******** ******** ******** ******** ******** ********
 ********        general memory management Configuration        ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** common ********/
#define XWMMCFG_ALIGNMENT                               (8U)

/******** ******** default kernel memory allocater ******** ********/
#define XWMMCFG_DKMA_SIZE                               (4096U)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********        OS Core Configuration        ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWKNCFG_SYNC_EVT_MAXNUM                         32

#endif /* cfg/xwos.h */
