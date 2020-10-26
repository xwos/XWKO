/**
 * @file
 * @brief 玄武OS内核：内核符号
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

#ifndef __xwos_mp_ksym_h__
#define __xwos_mp_ksym_h__

#include <xwos/standard.h>
#include <linux/kallsyms.h>

extern typeof(kallsyms_lookup_name) * xwmp_allsyms_lookup_name;
#define KSYM_DCLR(sym)          typeof(sym) *__ksym__##sym
#define KSYM_CALL(sym, x...)    __ksym__##sym(x)
#define KSYM_LOAD(sym)							        \
	({                                                                      \
                xwer_t rc = XWOK;                                               \
		__ksym__##sym =	(typeof(sym) *)xwmp_allsyms_lookup_name(#sym);  \
		if (IS_ERR_OR_NULL(__ksym__##sym))                              \
			rc = PTR_ERR(__ksym__##sym);                            \
                rc;                                                             \
	})

xwer_t xwmp_ksym_init(void);

#endif /* xwos/mp/ksym.h */
