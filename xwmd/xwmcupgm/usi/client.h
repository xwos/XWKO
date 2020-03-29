/**
 * @file
 * @brief MCU programmer client接口层
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

#ifndef __xwmd_xwmcupgm_usi_client_h__
#define __xwmd_xwmcupgm_usi_client_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwmd/xwmcupgm/usi/client.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define USI_XWMCUPGMC_STATE_STOP        0
#define USI_XWMCUPGMC_STATE_START       1

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern
xwer_t usi_xwmcupgmc_start(void);

extern
xwer_t usi_xwmcupgmc_stop(void);

extern
xwsq_t usi_xwmcupgmc_get_state(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********  inline functions ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwmd/xwmcupgm/usi/client.h */