#! /bin/make -f
# @file
# @brief ARCH描述层的编译规则
# @author
# + 隐星魂 (Roy.Sun) <www.starsoul.tech>
# @copyright
# + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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

ARCH_INCDIRS :=
ARCH_CFLAGS :=
ARCH_AFLAGS :=
ARCH_LDFLAGS :=
ARCH_EOBJS :=

ARCH_CSRCS :=
ARCH_CSRCS += asmlib/xwbop.c

ifeq ($(ARCHCFG_LIB_XWAOP8),y)
    ARCH_CSRCS += asmlib/xwaop/xws8_t.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t.c
endif
ifeq ($(ARCHCFG_LIB_XWAOP16),y)
    ARCH_CSRCS += asmlib/xwaop/xws16_t.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t.c
endif

ifeq ($(ARCHCFG_LIB_XWAOP32),y)
    ARCH_CSRCS += asmlib/xwaop/xws32_t.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t.c
endif

ifeq ($(ARCHCFG_LIB_XWAOP64),y)
    ARCH_CSRCS += asmlib/xwaop/xws64_t.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t.c
endif

ifeq ($(ARCHCFG_LIB_XWBMPAOP),y)
    ARCH_CSRCS += asmlib/xwbmpaop.c
endif

ARCH_CSRCS += asmlib/lfq.c
