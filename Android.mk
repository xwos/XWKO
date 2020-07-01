#! /bin/make -f
# @file
# @brief Android模块编译规则
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

LOCAL_PATH := $(call my-dir)
KDIR ?= kernel

######## xwos.ko in system image
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := xwos.ko
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_STRIP_MODULE := false
LOCAL_MODULE_RELATIVE_PATH := modules
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

ifeq ($(KERNEL_UBOOT_USE_ARCH_ARM64),true)
  EXT_FLAGS := ARCH=arm64 CROSS_COMPILE=aarch64-linux-android-
else
  EXT_FLAGS := ARCH=arm CROSS_COMPILE=arm-linux-androideabi-
endif
KDIR := $(ANDROID_PRODUCT_OUT)/obj/KERNEL_OBJ
BRD := msm89xxdkp

$(LOCAL_PATH)/$(LOCAL_MODULE): bootimage
	$(MAKE) -C $(shell dirname $@)/sobrd/$(BRD) $(EXT_FLAGS) KDIR=$(KDIR) clean
	$(MAKE) -C $(shell dirname $@)/sobrd/$(BRD) $(EXT_FLAGS) KDIR=$(KDIR)
	$(ANDROID_BUILD_TOP)/$(KDIR)/scripts/sign-file \
		sha512 \
		$(KDIR)/signing_key.priv \
		$(KDIR)/signing_key.x509 \
		$@

######## xwos.ko in recovery image

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := rcv.xwos.ko
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_STRIP_MODULE := false
LOCAL_MODULE_PATH := $(TARGET_RECOVERY_ROOT_OUT)
LOCAL_SRC_FILES := xwos.ko
include $(BUILD_PREBUILT)

ifeq ($(KERNEL_UBOOT_USE_ARCH_ARM64),true)
  EXT_FLAGS := ARCH=arm64 CROSS_COMPILE=aarch64-linux-android-
else
  EXT_FLAGS := ARCH=arm CROSS_COMPILE=arm-linux-androideabi-
endif
KDIR := $(ANDROID_PRODUCT_OUT)/obj/KERNEL_OBJ
BRD := msm89xxdkp

$(LOCAL_PATH)/$(LOCAL_SRC_FILES): bootimage
	$(MAKE) -C $(shell dirname $@)/sobrd/$(BRD) $(EXT_FLAGS) KDIR=$(KDIR) clean
	$(MAKE) -C $(shell dirname $@)/sobrd/$(BRD) $(EXT_FLAGS) KDIR=$(KDIR)
	$(ANDROID_BUILD_TOP)/$(KDIR)/scripts/sign-file \
		sha512 \
		$(KDIR)/signing_key.priv \
		$(KDIR)/signing_key.x509 \
		$@
