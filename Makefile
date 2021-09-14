#! /bin/make -f
# @file
# @brief Makefile script to build Linux kernel modules
# @author
# + 隐星魂 (Roy Sun) <xwos@xwos.tech>
# @copyright
# + Copyright © 2015 xwos.tech, All Rights Reserved.
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

M ?= .
ifeq ($(wildcard $(M)/XuanWuOS.cfg),)
  $(error "Can't find XuanWuOS.cfg. Build from the xwbd/<BOARD> folder to generate it!")
endif
-include $(M)/XuanWuOS.cfg
-include $(XuanWuOS_PATH)/$(XWBS_UTIL_MK_XWMO)

TARGET ?= xwos
KDIR ?= /lib/modules/`uname -r`/build
CROSS_COMPILE ?=
CC ?= $(CROSS_COMPILE)gcc
STRIP ?= $(CROSS_COMPILE)strip

INCDIRS :=
INCDIRS += $(addprefix -I,$(XuanWuOS_PATH)/$(XuanWuOS_ARCH_DIR))
INCDIRS += $(addprefix -I,$(XuanWuOS_PATH)/$(XuanWuOS_CPU_DIR))
INCDIRS += $(addprefix -I,$(XuanWuOS_PATH)/$(XuanWuOS_SOC_DIR))
INCDIRS += $(addprefix -I,$(XuanWuOS_PATH)/$(XuanWuOS_BRD_DIR))
INCDIRS += $(addprefix -I,$(XuanWuOS_WKSPC_DIR))

EOBJS =
obj-m = $(TARGET).o
######## ######## ######## ######## module ######## ######## ######## ########
$(TARGET)-y = init.o

######## ######## ######## ######## arch ######## ######## ######## ########
-include $(XuanWuOS_PATH)/$(XuanWuOS_ARCH_DIR)/arch.mk
ARCH_EOBJS := $(addprefix $(XuanWuOS_PATH)/$(XuanWuOS_ARCH_DIR)/, $(ARCH_EOBJS))
ARCH_CSRCS := $(addprefix $(XuanWuOS_ARCH_DIR)/, $(ARCH_CSRCS))
ARCH_COBJS := $(addsuffix .o,$(basename $(ARCH_CSRCS)))
$(TARGET)-y += $(ARCH_COBJS)
EOBJS += $(ARCH_EOBJS)

######## ######## ######## ######## cpu ######## ######## ######## ########
-include $(XuanWuOS_PATH)/$(XuanWuOS_CPU_DIR)/cpu.mk
CPU_EOBJS := $(addprefix $(XuanWuOS_PATH)/$(XuanWuOS_CPU_DIR)/, $(CPU_EOBJS))
CPU_CSRCS := $(addprefix $(XuanWuOS_CPU_DIR)/, $(CPU_CSRCS))
CPU_COBJS := $(addsuffix .o,$(basename $(CPU_CSRCS)))
$(TARGET)-y += $(CPU_COBJS)
EOBJS += $(CPU_EOBJS)

######## ######## ######## ######## soc ######## ######## ######## ########
-include $(XuanWuOS_PATH)/$(XuanWuOS_SOC_DIR)/soc.mk
SOC_EOBJS := $(addprefix $(XuanWuOS_PATH)/$(XuanWuOS_SOC_DIR)/, $(SOC_EOBJS))
SOC_CSRCS := $(addprefix $(XuanWuOS_SOC_DIR)/, $(SOC_CSRCS))
SOC_COBJS := $(addsuffix .o,$(basename $(SOC_CSRCS)))
$(TARGET)-y += $(SOC_COBJS)
EOBJS += $(SOC_EOBJS)

######## ######## ######## ######## board ######## ######## ######## ########
-include $(XuanWuOS_PATH)/$(XuanWuOS_BRD_DIR)/brd.mk
BRD_EOBJS := $(addprefix $(XuanWuOS_PATH)/$(XuanWuOS_BRD_DIR)/, $(BRD_EOBJS))
BRD_CSRCS := $(addprefix $(XuanWuOS_BRD_DIR)/, $(BRD_CSRCS))
BRD_COBJS := $(addsuffix .o,$(basename $(BRD_CSRCS)))
$(TARGET)-y += $(BRD_COBJS)
EOBJS += $(BRD_EOBJS)

######## ######## ######## ######## xwos ######## ######## ######## ########
-include $(XuanWuOS_PATH)/$(XuanWuOS_XWOS_DIR)/xwos.mk
XWOS_EOBJS := $(addprefix $(XuanWuOS_PATH)/$(XuanWuOS_XWOS_DIR)/, $(XWOS_EOBJS))
XWOS_CSRCS := $(addprefix $(XuanWuOS_XWOS_DIR)/, $(XWOS_CSRCS))
XWOS_COBJS := $(addsuffix .o,$(basename $(XWOS_CSRCS)))
$(TARGET)-y += $(XWOS_COBJS)
EOBJS += $(XWOS_EOBJS)

######## ######## ######## ######## xwmd ######## ######## ######## ########
XWMD_INCDIRS :=
XWMD_COBJS :=
XWMD_EOBJS :=
XWMD_LIST := $(shell test -d $(XuanWuOS_PATH)/$(XuanWuOS_XWMD_DIR) && \
                     find $(XuanWuOS_PATH)/$(XuanWuOS_XWMD_DIR) -type f -name "xwmo.mk" \
                     -exec dirname {} \;)

define findXwmdTarget
ifeq ($$(XWMDCFG$(call xwmoPathToName,$(1),$(XuanWuOS_PATH)/$(XuanWuOS_XWMD_DIR))),y)
    XWMO_INCDIRS :=
    XWMO_ESRCS :=
    XWMO_CSRCS :=
    include $(1)/xwmo.mk
    XWMD_NAME := $(call xwmoPathToName,$(1),$(XuanWuOS_PATH)/$(XuanWuOS_XWMD_DIR))
    XWMO_INCDIRS := $$(addprefix -I$(1)/,$(strip $$(XWMO_INCDIRS)))
    XWMD_INCDIRS += $$(XWMO_INCDIRS)
    XWMO_CSRC := $$(addprefix $(subst $(XuanWuOS_PATH)/,,$(1)/),$$(XWMO_CSRCS))
    XWMO_COBJS := $$(addsuffix .o,$$(basename $$(XWMO_CSRC)))
    XWMD_COBJS += $$(XWMO_COBJS)
    XWMO_EOBJS := $$(addprefix $(1)/,$$(XWMO_EOBJS))
    XWMD_EOBJS += $$(XWMO_EOBJS)
endif
# SHOULD add a blank line here

endef

$(eval $(foreach xwmo,$(XWMD_LIST),$(call findXwmdTarget,$(xwmo))))
$(TARGET)-y += $(XWMD_COBJS)
EOBJS += $(XWMD_EOBJS)
INCDIRS += $(XWMD_INCDIRS)

######## ######## ######## ######## bm ######## ######## ######## ########
XWBM_INCDIRS :=
XWBM_COBJS :=
XWBM_EOBJS :=
XWBM_LIST := $(shell test -d $(XuanWuOS_PATH)/$(XuanWuOS_BM_DIR) && \
                     find $(XuanWuOS_PATH)/$(XuanWuOS_BM_DIR) -type f -name "xwmo.mk" \
                     -exec dirname {} \;)

define findXwbmTarget
ifeq ($$(BMCFG$(call xwmoPathToName,$(1),$(XuanWuOS_PATH)/$(XuanWuOS_BM_DIR))),y)
    XWMO_INCDIRS :=
    XWMO_ESRCS :=
    XWMO_CSRCS :=
    include $(1)/xwmo.mk
    XWBM_NAME := $(call xwmoPathToName,$(1),$(XuanWuOS_PATH)/$(XuanWuOS_BM_DIR))
    XWMO_INCDIRS := $$(addprefix -I$(1)/,$(strip $$(XWMO_INCDIRS)))
    XWBM_INCDIRS += $$(XWMO_INCDIRS)
    XWMO_CSRC := $$(addprefix $(subst $(XuanWuOS_PATH)/,,$(1)/),$$(XWMO_CSRCS))
    XWMO_COBJS := $$(addsuffix .o,$$(basename $$(XWMO_CSRC)))
    XWBM_COBJS += $$(XWMO_COBJS)
    XWMO_EOBJS := $$(addprefix $(1)/,$$(XWMO_EOBJS))
    XWBM_EOBJS += $$(XWMO_EOBJS)
endif
# SHOULD add a blank line here

endef

$(eval $(foreach xwmo,$(XWBM_LIST),$(call findXwbmTarget,$(xwmo))))
$(TARGET)-y += $(XWBM_COBJS)
EOBJS += $(XWBM_EOBJS)
INCDIRS += $(XWBM_INCDIRS)

######## ######## ######## ######## am ######## ######## ######## ########
XWAM_INCDIRS :=
XWAM_COBJS :=
XWAM_EOBJS :=
XWAM_LIST := $(shell test -d $(XuanWuOS_PATH)/$(XuanWuOS_XWAM_DIR) && \
                     find $(XuanWuOS_PATH)/$(XuanWuOS_XWAM_DIR) -type f -name "xwmo.mk" \
                     -exec dirname {} \;)

define findXwamTarget
ifeq ($$(XWAMCFG$(call xwmoPathToName,$(1),$(XuanWuOS_PATH)/$(XuanWuOS_XWAM_DIR))),y)
    XWMO_INCDIRS :=
    XWMO_ESRCS :=
    XWMO_CSRCS :=
    include $(1)/xwmo.mk
    XWAM_NAME := $(call xwmoPathToName,$(1),$(XuanWuOS_PATH)/$(XuanWuOS_XWAM_DIR))
    XWMO_INCDIRS := $$(addprefix -I$(1)/,$(strip $$(XWMO_INCDIRS)))
    XWAM_INCDIRS += $$(XWMO_INCDIRS)
    XWMO_CSRC := $$(addprefix $(subst $(XuanWuOS_PATH)/,,$(1)/),$$(XWMO_CSRCS))
    XWMO_COBJS := $$(addsuffix .o,$$(basename $$(XWMO_CSRC)))
    XWAM_COBJS += $$(XWMO_COBJS)
    XWMO_EOBJS := $$(addprefix $(1)/,$$(XWMO_EOBJS))
    XWAM_EOBJS += $$(XWMO_EOBJS)
endif
# SHOULD add a blank line here

endef

$(eval $(foreach xwmo,$(XWAM_LIST),$(call findXwamTarget,$(xwmo))))
$(TARGET)-y += $(XWAM_COBJS)
EOBJS += $(XWAM_EOBJS)
INCDIRS += $(XWAM_INCDIRS)

######## ######## ######## ######## rule ######## ######## ######## ########
ccflags-y := -I$(XuanWuOS_PATH) $(INCDIRS) $(ARCH_CFLAGS) -fno-pic -Wno-vla
ldflags-y := $(ARCH_LDFLAGS) $(CPU_LDFLAGS) $(EOBJS)
ALLOBJS := $(addprefix $(M)/,$($(TARGET)-y))

modules: dir
	@$(MAKE) -C $(KDIR) M=$(M) src=$(XuanWuOS_PATH) $@
	$(STRIP) $(M)/$(TARGET).ko --strip-unneeded
	@cp $(M)/Module.symvers $(M)/$(TARGET).symvers

modules_install:
	@$(MAKE) -C $(KDIR) M=$(M) $@

dir:
	@for obj in $(ALLOBJS); do mkdir -p $$(dirname $${obj}); done

.DEFAULT:
	$(MAKE) -C $(KDIR) M=$(M) $@

clean:
	@rm -rf $(M)/.tmp_versions
	@rm -f $(M)/*.ko $(M)/*.order $(M)/*.symvers
	@rm -f $(M)/*.mod.c $(M)/*.mod $(M)/*.mod.o $(M)/$(TARGET).o
	@rm -f $(ALLOBJS)
	@rm -f $(ALLOBJS:.o=.o.ur-safe)
	@rm -f $(ALLOBJS:.o=.o.cmd)
	@rm -f $(ALLOBJS:.o=.o.d)
	@rm -f $(M)/autogen.h
	@rm -f $(M)/XuanWuOS.cfg

debug:
	@echo "XWMD_LIST:$(XWMD_LIST)"
	@echo "XWMD_INCDIRS:$(XWMD_INCDIRS)"
	@echo "XWMD_COBJS:$(XWMD_COBJS)"
	@echo "XWMD_EOBJS:$(XWMD_EOBJS)"
	@echo "XWBM_LIST:$(XWBM_LIST)"
	@echo "XWBM_INCDIRS:$(XWBM_INCDIRS)"
	@echo "XWBM_COBJS:$(XWBM_COBJS)"
	@echo "XWBM_EOBJS:$(XWBM_EOBJS)"
	@echo "XWAM_LIST:$(XWAM_LIST)"
	@echo "XWAM_INCDIRS:$(XWAM_INCDIRS)"
	@echo "XWAM_COBJS:$(XWAM_COBJS)"
	@echo "XWAM_EOBJS:$(XWAM_EOBJS)"

.PHONY: modules modules_install clean debug dir
