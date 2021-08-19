/**
 * @file
 * @brief 基本配置
 */

#ifndef __cfg_XuanWuOS_h__
#define __cfg_XuanWuOS_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********  ARCH & compiler  ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_ARCH                               arm
#define XuanWuOS_CFG_SUBARCH                            v8a
#define XuanWuOS_CFG_COMPILER                           gcc
#include <cfg/arch.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********        CPU        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_CPU                                cpu
#include <cfg/cpu.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********        SOC        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_SOC                                sc9863a
#include <cfg/soc.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       board       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_BOARD                              sc9863atmpl
#include <cfg/board.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********    XWOS kernel    ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_CORE                               mp
#include <cfg/xwos.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********   chip & device   ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_XWCD                               1
#include <cfg/xwcd.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********  XWOS middleware  ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_XWMD                               1
#include <cfg/xwmd.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********  external module  ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_XWEM                               1
#include <cfg/xwem.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********    APP module     ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XuanWuOS_CFG_XWAM                               1
#include <cfg/xwam.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********        auto-generated header        ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <autogen.h>

#endif /* cfg/XuanWuOS.h */