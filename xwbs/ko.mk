#! /bin/make -f
# @file
# @brief Makefile script to build Linux kernel modules
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

include $(XWOS_PATH)/$(XWOS_WKSPC_DIR)/XuanWuOS.cfg
include $(XWOS_PATH)/$(XWBS_UTIL_MK_XWMO)
KDIR ?= /lib/modules/`uname -r`/build
CROSS_COMPILE ?=
CC := $(CROSS_COMPILE)gcc
STRIP := $(CROSS_COMPILE)strip

INCDIRS :=
INCDIRS += $(addprefix -I,$(XWOS_PATH)/$(XWOS_ARCH_DIR))
INCDIRS += $(addprefix -I,$(XWOS_PATH)/$(XWOS_CPU_DIR))
INCDIRS += $(addprefix -I,$(XWOS_PATH)/$(XWOS_SOC_DIR))
INCDIRS += $(addprefix -I,$(XWOS_PATH)/$(XWOS_BRD_DIR))
INCDIRS += $(addprefix -I,$(XWOS_PATH)/$(XWOS_WKSPC_DIR))

EOBJS =
obj-m = $(TARGET).o
######## ######## ######## ######## module ######## ######## ######## ########
$(TARGET)-y = init.o

######## ######## ######## ######## arch ######## ######## ######## ########
-include $(XWOS_PATH)/$(XWOS_ARCH_DIR)/arch.mk
ARCH_EOBJS := $(addprefix $(XWOS_PATH)/$(XWOS_ARCH_DIR)/, $(ARCH_EOBJS))
ARCH_CSRCS := $(addprefix $(XWOS_ARCH_DIR)/, $(ARCH_CSRCS))
ARCH_COBJS := $(addsuffix .o,$(basename $(ARCH_CSRCS)))
$(TARGET)-y += $(ARCH_COBJS)
EOBJS += $(ARCH_EOBJS)

######## ######## ######## ######## cpu ######## ######## ######## ########
-include $(XWOS_PATH)/$(XWOS_CPU_DIR)/cpu.mk
CPU_EOBJS := $(addprefix $(XWOS_PATH)/$(XWOS_CPU_DIR)/, $(CPU_EOBJS))
CPU_CSRCS := $(addprefix $(XWOS_CPU_DIR)/, $(CPU_CSRCS))
CPU_COBJS := $(addsuffix .o,$(basename $(CPU_CSRCS)))
$(TARGET)-y += $(CPU_COBJS)
EOBJS += $(CPU_EOBJS)

######## ######## ######## ######## soc ######## ######## ######## ########
-include $(XWOS_PATH)/$(XWOS_SOC_DIR)/soc.mk
SOC_EOBJS := $(addprefix $(XWOS_PATH)/$(XWOS_SOC_DIR)/, $(SOC_EOBJS))
SOC_CSRCS := $(addprefix $(XWOS_SOC_DIR)/, $(SOC_CSRCS))
SOC_COBJS := $(addsuffix .o,$(basename $(SOC_CSRCS)))
$(TARGET)-y += $(SOC_COBJS)
EOBJS += $(SOC_EOBJS)

######## ######## ######## ######## board ######## ######## ######## ########
-include $(XWOS_PATH)/$(XWOS_BDL_DIR)/bdl.mk
BDL_EOBJS := $(addprefix $(XWOS_PATH)/$(XWOS_BDL_DIR)/, $(BDL_EOBJS))
BDL_CSRCS := $(addprefix $(XWOS_BDL_DIR)/, $(BDL_CSRCS))
BDL_COBJS := $(addsuffix .o,$(basename $(BDL_CSRCS)))
$(TARGET)-y += $(BDL_COBJS)
EOBJS += $(BDL_EOBJS)

######## ######## ######## ######## xwos ######## ######## ######## ########
-include $(XWOS_PATH)/$(XWOS_KN_DIR)/xwos.mk
XWOS_EOBJS := $(addprefix $(XWOS_PATH)/$(XWOS_KN_DIR)/, $(XWOS_EOBJS))
XWOS_CSRCS := $(addprefix $(XWOS_KN_DIR)/, $(XWOS_CSRCS))
XWOS_COBJS := $(addsuffix .o,$(basename $(XWOS_CSRCS)))
$(TARGET)-y += $(XWOS_COBJS)
EOBJS += $(XWOS_EOBJS)

######## ######## ######## ######## xwmd ######## ######## ######## ########
XWMD_INCDIRS :=
XWMD_COBJS :=
XWMD_EOBJS :=
XWMD_LIST := $(shell test -d $(XWOS_PATH)/$(XWOS_MD_DIR) && \
                     find $(XWOS_PATH)/$(XWOS_MD_DIR) -type f -name "xwmo.mk" \
                     -exec dirname {} \;)

define findXwmdTarget
ifeq ($$(XWMDCFG$(call xwmoPathToName,$(1),$(XWOS_PATH)/$(XWOS_MD_DIR))),y)
    XWMO_INCDIRS :=
    XWMO_ESRCS :=
    XWMO_CSRCS :=
    include $(1)/xwmo.mk
    XWMD_NAME := $(call xwmoPathToName,$(1),$(XWOS_PATH)/$(XWOS_MD_DIR))
    XWMO_INCDIRS := $$(addprefix -I$(1)/,$(strip $$(XWMO_INCDIRS)))
    XWMD_INCDIRS += $$(XWMO_INCDIRS)
    XWMO_CSRC := $$(addprefix $(subst $(XWOS_PATH)/,,$(1)/),$$(XWMO_CSRCS))
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
XWBM_LIST := $(shell test -d $(XWOS_PATH)/$(XWOS_BM_DIR) && \
                     find $(XWOS_PATH)/$(XWOS_BM_DIR) -type f -name "xwmo.mk" \
                     -exec dirname {} \;)

define findXwbmTarget
ifeq ($$(BMCFG$(call xwmoPathToName,$(1),$(XWOS_PATH)/$(XWOS_BM_DIR))),y)
    XWMO_INCDIRS :=
    XWMO_ESRCS :=
    XWMO_CSRCS :=
    include $(1)/xwmo.mk
    XWBM_NAME := $(call xwmoPathToName,$(1),$(XWOS_PATH)/$(XWOS_BM_DIR))
    XWMO_INCDIRS := $$(addprefix -I$(1)/,$(strip $$(XWMO_INCDIRS)))
    XWBM_INCDIRS += $$(XWMO_INCDIRS)
    XWMO_CSRC := $$(addprefix $(subst $(XWOS_PATH)/,,$(1)/),$$(XWMO_CSRCS))
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

######## ######## ######## ######## rule ######## ######## ######## ########
ccflags-y := -I$(XWOS_PATH) $(INCDIRS) $(ARCH_CFLAGS) -fno-pic
ldflags-y := $(ARCH_LDFLAGS) $(CPU_LDFLAGS) $(EOBJS)

all: modules strip

modules:
	@$(MAKE) -C $(KDIR) M=$(XWOS_PATH) modules
	@cp Module.symvers $(TARGET).symvers

.DEFAULT:
	$(MAKE) -C $(KDIR) M=$(XWOS_PATH) $@

strip: modules
	$(STRIP) $(TARGET).ko --strip-unneeded

clean:
	@rm -rf .tmp_versions
	@rm -f *~ *.o core .depend .*.cmd *.ko *.mod.c
	@rm -f *.markers *.symvers *.order *.mod.o .cache.mk
	@rm -f $(ARCH_COBJS)
	@rm -f $(ARCH_COBJS:.o=.o.ur-safe)
	@rm -f $(foreach O,$(ARCH_COBJS),$(addprefix $(dir $(O)),.$(notdir $(O)).cmd))
	@rm -f $(CPU_COBJS)
	@rm -f $(CPU_COBJS:.o=.o.ur-safe)
	@rm -f $(foreach O,$(CPU_COBJS),$(addprefix $(dir $(O)),.$(notdir $(O)).cmd))
	@rm -f $(SOC_COBJS)
	@rm -f $(SOC_COBJS:.o=.o.ur-safe)
	@rm -f $(foreach O,$(SOC_COBJS),$(addprefix $(dir $(O)),.$(notdir $(O)).cmd))
	@rm -f $(BDL_COBJS)
	@rm -f $(BDL_COBJS:.o=.o.ur-safe)
	@rm -f $(foreach O,$(BDL_COBJS),$(addprefix $(dir $(O)),.$(notdir $(O)).cmd))
	@rm -f $(XWOS_COBJS)
	@rm -f $(XWOS_COBJS:.o=.o.ur-safe)
	@rm -f $(foreach O,$(XWOS_COBJS),$(addprefix $(dir $(O)),.$(notdir $(O)).cmd))
	@rm -f $(XWMD_COBJS)
	@rm -f $(XWMD_COBJS:.o=.o.ur-safe)
	@rm -f $(foreach O,$(XWMD_COBJS),$(addprefix $(dir $(O)),.$(notdir $(O)).cmd))
	@rm -f $(XWBM_COBJS)
	@rm -f $(XWBM_COBJS:.o=.o.ur-safe)
	@rm -f $(foreach O,$(XWBM_COBJS),$(addprefix $(dir $(O)),.$(notdir $(O)).cmd))

define evalstr
$(strip $1)
endef

debug:
	@echo "XWMD_LIST:$(XWMD_LIST)"
	@echo "XWMD_INCDIRS:$(XWMD_INCDIRS)"
	@echo "XWMD_COBJS:$(XWMD_COBJS)"
	@echo "XWMD_EOBJS:$(XWMD_EOBJS)"
	@echo "XWBM_LIST:$(XWBM_LIST)"
	@echo "XWBM_INCDIRS:$(XWBM_INCDIRS)"
	@echo "XWBM_COBJS:$(XWBM_COBJS)"
	@echo "XWBM_EOBJS:$(XWBM_EOBJS)"

.PHONY: modules clean strip debug
