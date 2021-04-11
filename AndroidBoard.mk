# Copyright (C) 2020 The LineageOS Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

ifeq ($(TARGET_PREBUILT_KERNEL),)
INSTALLED_KERNEL_TARGET := $(PRODUCT_OUT)/kernel
INSTALLED_RECOVERYIMAGE_TARGET := $(PRODUCT_OUT)/recovery.img

DTB_TARGETS := tegra210-darcy-p2894-0000-a00-00.dtb \
               tegra210-darcy-p2894-0050-a04-00.dtb \
               tegra210-darcy-p2894-0050-a08-00.dtb \
               tegra210-darcy-p2894-0052-a08-00.dtb \
               tegra210-foster-e-hdd-p2530-0932-e00-00.dtb \
               tegra210-foster-e-hdd-p2530-0932-e01-00.dtb \
               tegra210-foster-e-hdd-p2530-0932-e02-00.dtb \
               tegra210-foster-e-p2530-0930-e00-00.dtb \
               tegra210-foster-e-p2530-0930-e01-00.dtb \
               tegra210-foster-e-p2530-0930-e02-00.dtb \
               tegra210-jetson-tx1-p2597-2180-a01-android-devkit.dtb \
               tegra210-loki-e-p2530-0030-e00-00.dtb \
               tegra210-loki-e-p2530-0030-e01-00.dtb \
               tegra210-loki-e-p2530-0030-e02-00.dtb \
               tegra210-loki-e-p2530-0030-e03-00.dtb \
               tegra210-loki-e-p2530-0031-e00-00.dtb \
               tegra210-loki-e-p2530-0031-e01-00.dtb \
               tegra210-loki-e-p2530-0031-e02-00.dtb \
               tegra210-loki-e-p2530-0031-e03-00.dtb
INSTALLED_DTB_TARGETS := $(DTB_TARGETS:%=$(PRODUCT_OUT)/install/%)
$(INSTALLED_DTB_TARGETS): $(INSTALLED_KERNEL_TARGET) | $(ACP)
	echo -e ${CL_GRN}"Copying individual DTBs"${CL_RST}
	@mkdir -p $(PRODUCT_OUT)/install
	cp $(@F:%=$(KERNEL_OUT)/arch/arm64/boot/dts/%) $(PRODUCT_OUT)/install/

DTBO_TARGETS := tegra210-foster-e-p2530-audio-overlay.dtbo \
                tegra210b01-darcy-p2894-audio-overlay.dtbo
INSTALLED_DTBO_TARGETS := $(DTBO_TARGETS:%=$(TARGET_OUT_VENDOR)/firmware/dtb/%)
$(INSTALLED_DTBO_TARGETS): $(INSTALLED_KERNEL_TARGET) | $(ACP)
	echo -e ${CL_GRN}"Copying individual DTBOs"${CL_RST}
	@mkdir -p $(TARGET_OUT_VENDOR)/firmware/dtb
	cp $(@F:%=$(KERNEL_OUT)/arch/arm64/boot/dts/%) $(TARGET_OUT_VENDOR)/firmware/dtb/

ALL_DEFAULT_INSTALLED_MODULES += $(INSTALLED_DTB_TARGETS) $(INSTALLED_DTBO_TARGETS)

RECOVERY_KMOD_TARGETS := \
    hid-jarvis-remote.ko \
    hid-nvidia-blake.ko
INSTALLED_RECOVERY_KMOD_TARGETS := $(RECOVERY_KMOD_TARGETS:%=$(TARGET_RECOVERY_ROOT_OUT)/lib/modules/%)
$(INSTALLED_RECOVERY_KMOD_TARGETS): $(INSTALLED_KERNEL_TARGET)
	echo -e ${CL_GRN}"Copying kernel modules to recovery"${CL_RST}
	@mkdir -p $(dir $@)
	cp $(@F:%=$(TARGET_OUT_VENDOR)/lib/modules/%) $(TARGET_RECOVERY_ROOT_OUT)/lib/modules/

$(INSTALLED_RECOVERYIMAGE_TARGET): $(INSTALLED_RECOVERY_KMOD_TARGETS)
endif

EKS_DAT_SYMLINK := $(TARGET_OUT_VENDOR)/app/eks2/eks2.dat
$(EKS_DAT_SYMLINK): $(LOCAL_INSTALLED_MODULE)
	$(hide) ln -sf /data/vendor/eks2/ek2.dat $@

ALL_DEFAULT_INSTALLED_MODULES += $(EKS_DAT_SYMLINK)
