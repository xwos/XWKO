/**
 * @file
 * @brief 精简的点对点通讯协议：应用接口层
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

#ifndef __xwmd_isc_xwscp_api_h__
#define __xwmd_isc_xwscp_api_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwscp;
struct xwscp_hwifal_operations;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwmd_api
void xwscp_init(struct xwscp * xwscp);

__xwmd_api
xwer_t xwscp_start(struct xwscp * xwscp, const char * name,
                   const struct xwscp_hwifal_operations * hwifops);

__xwmd_api
xwer_t xwscp_stop(struct xwscp * xwscp);

__xwmd_api
xwer_t xwscp_connect(struct xwscp * xwscp, xwtm_t * xwtm);

__xwmd_api
xwer_t xwscp_tx(struct xwscp * xwscp, const xwu8_t msg[], xwsz_t * size, xwtm_t * xwtm);

__xwmd_api
xwer_t xwscp_rx(struct xwscp * xwscp, xwu8_t buf[], xwsz_t * size, xwtm_t * xwtm);

#endif /* xwmd/isc/xwscp/api.h */
