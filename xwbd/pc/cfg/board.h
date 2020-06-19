/**
 * @file
 * @brief 板级描述层(BDL)配置
 */

#ifndef __cfg_board_h__
#define __cfg_board_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       board description layer       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ISC ******** ********/
#define BRDCFG_XWPCPIF_UART_BAUDRATE    (B115200)
#define BRDCFG_XWSCPIF_UART_BAUDRATE    (B115200)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********   board modules   ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define BMCFG_mcuc                      1
#define BMCFG_demo_hixwos               1

#endif /* cfg/board.h */
