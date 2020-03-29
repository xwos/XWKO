/**
 * @file
 * @brief MCU programmer server接口
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

#ifndef __xwmd_xwmcupgm_usi_server_h__
#define __xwmd_xwmcupgm_usi_server_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwmd/xwmcupgm/usi/server.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct uapi_xwmcupgmsrv_ioc_data {
        xwu8_t __user * frm;
        xwsz_t size;
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define USI_XWMCUPGMSRV_STATE_STOP      0
#define USI_XWMCUPGMSRV_STATE_START     1

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern struct xwmcupgmsrv usi_xwmcupgmsrv;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern
xwer_t usi_xwmcupgmsrv_start(void);

extern
xwer_t usi_xwmcupgmsrv_stop(void);

extern
xwsq_t usi_xwmcupgmsrv_get_state(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********  inline functions ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwmd/xwmcupgm/usi/server.h */
