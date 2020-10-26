#! /bin/make -f
# @file
# @brief 玄武OS内核的的编译规则
# @author
# + 隐星魂 (Roy.Sun) <https://xwos.tech>
# @copyright
# + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
# > This Source Code Form is subject to the terms of the Mozilla Public
# > License, v. 2.0 (the "MPL"). If a copy of the MPL was not distributed
# > with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
# >
# > Alternatively, the contents of this file may be used under the
# > terms of the GNU General Public License version 2 (the "GPL"), in
# > which case the provisions of the GPL are applicable instead of the
# > above. If you wish to allow the use of your version of this file
# > only under the terms of the GPL and not to allow others to use your
# > version of this file under the MPL, indicate your decision by
# > deleting the provisions above and replace them with the notice and
# > other provisions required by the GPL. If you do not delete the
# > provisions above, a recipient may use your version of this file
# > under either the MPL or the GPL.
#

XWOS_EOBJS :=
XWOS_CSRCS :=
XWOS_ASRCS :=

######## ######## ######## ########   libs   ######## ######## ######## ########
XWOS_CSRCS += lib/object.c
XWOS_CSRCS += lib/xwbop.c
XWOS_CSRCS += lib/xwlog.c

ifeq ($(XWLIBCFG_XWAOP8),y)
    ifneq ($(ARCHCFG_LIB_XWAOP8),y)
        XWOS_CSRCS += lib/xwaopu8.c
        XWOS_CSRCS += lib/xwaops8.c
    endif
endif

ifeq ($(XWLIBCFG_XWAOP16),y)
    ifneq ($(ARCHCFG_LIB_XWAOP16),y)
        XWOS_CSRCS += lib/xwaopu16.c
        XWOS_CSRCS += lib/xwaops16.c
    endif
endif

ifeq ($(XWLIBCFG_XWAOP32),y)
    ifneq ($(ARCHCFG_LIB_XWAOP32),y)
        XWOS_CSRCS += lib/xwaopu32.c
        XWOS_CSRCS += lib/xwaops32.c
    endif
endif

ifeq ($(XWLIBCFG_XWAOP64),y)
    ifneq ($(ARCHCFG_LIB_XWAOP64),y)
        XWOS_CSRCS += lib/xwaopu64.c
        XWOS_CSRCS += lib/xwaops64.c
    endif
endif

ifeq ($(XWLIBCFG_XWBMPAOP),y)
    ifneq ($(ARCHCFG_LIB_XWBMPAOP),y)
        XWOS_CSRCS += lib/xwbmpaop.c
    endif
endif

ifeq ($(XWLIBCFG_CRC32),y)
    XWOS_CSRCS += lib/crc32.c
endif

######## ######## ######## ######## init ######## ######## ######## ########
XWOS_CSRCS += init.c

######## ######## ######## ######## Mp ######## ######## ######## ########
XWOS_CSRCS += mp/ksym.c
XWOS_CSRCS += mp/skd.c
XWOS_CSRCS += mp/thd.c
XWOS_CSRCS += mp/swt.c
XWOS_CSRCS += mp/pm.c

######## lock ########
XWOS_CSRCS += mp/lock/mtx.c

######## sync ########
XWOS_CSRCS += mp/sync/sem.c
XWOS_CSRCS += mp/sync/cond.c
XWOS_CSRCS += mp/sync/evt.c

######## ######## ######## ######## mm ######## ######## ######## ########
XWOS_CSRCS += mm/sma.c
XWOS_CSRCS += mm/kma.c
XWOS_CSRCS += mm/bma.c
XWOS_CSRCS += mm/memslice.c
