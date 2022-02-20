#
# Copyright (C) 2023 The LineageOS Project
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
#

BOARD_FLASH_BLOCK_SIZE             := 4096
BOARD_BOOTIMAGE_PARTITION_SIZE     := 33554432
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 26767360
BOARD_CACHEIMAGE_PARTITION_SIZE    := 419430400
BOARD_USERDATAIMAGE_PARTITION_SIZE := 10099646976
BOARD_SYSTEMIMAGE_PARTITION_SIZE   := 3758096384
BOARD_VENDORIMAGE_PARTITION_SIZE   := 318767104
TARGET_USERIMAGES_USE_EXT4         := true
BOARD_CACHEIMAGE_FILE_SYSTEM_TYPE  := ext4
BOARD_VENDORIMAGE_FILE_SYSTEM_TYPE := ext4
TARGET_COPY_OUT_VENDOR             := vendor
BOARD_BUILD_SYSTEM_ROOT_IMAGE      := true
BOARD_USES_FULL_RECOVERY_IMAGE     := true

# Assert
TARGET_OTA_ASSERT_DEVICE := dragon,sphynx

# Boot image
BOARD_BOOT_HEADER_VERSION := 1
BOARD_MKBOOTIMG_ARGS += --header_version $(BOARD_BOOT_HEADER_VERSION)

# Bootloader versions
TARGET_BOARD_INFO_FILE := device/google/sphynx/board-info.txt

# Manifest
DEVICE_MANIFEST_FILE := device/google/sphynx/manifest.xml

# Bluetooth
ifeq ($(TARGET_TEGRA_BT),bcm)
BOARD_CUSTOM_BT_CONFIG := device/google/sphynx/comms/vnd_sphynx.txt
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := device/google/sphynx/comms
endif

# Charger
WITH_LINEAGE_CHARGER := false

# Display
TARGET_SCREEN_DENSITY := 320

# Kernel
TARGET_KERNEL_CLANG_COMPILE    := false
KERNEL_TOOLCHAIN               := $(shell pwd)/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-gnu-9.3/bin
KERNEL_TOOLCHAIN_PREFIX        := aarch64-buildroot-linux-gnu-
TARGET_KERNEL_SOURCE           := kernel/nvidia/kernel-$(TARGET_TEGRA_KERNEL)
TARGET_KERNEL_CONFIG           := tegra_android_defconfig
TARGET_KERNEL_RECOVERY_CONFIG  := tegra_android_recovery_defconfig
BOARD_KERNEL_IMAGE_NAME        := Image.fit
TARGET_KERNEL_ADDITIONAL_FLAGS := "NV_BUILD_KERNEL_OPTIONS=$(TARGET_TEGRA_KERNEL)"
TARGET_KERNEL_VARIANT_CONFIG   := smaug_variant_defconfig
include device/google/sphynx/modules.mk

# Recovery
TARGET_RECOVERY_FSTAB        := device/google/sphynx/initfiles/fstab.dragon
TARGET_RECOVERY_UPDATER_LIBS := librecoveryupdater_tegra
TARGET_RECOVERY_PIXEL_FORMAT := BGRA_8888

# Security Patch Level
VENDOR_SECURITY_PATCH := 2022-04-05

# Treble
BOARD_PROPERTY_OVERRIDES_SPLIT_ENABLED := true
BOARD_VNDK_VERSION                     := current
PRODUCT_FULL_TREBLE_OVERRIDE           := true

include device/nvidia/t210-common/BoardConfigCommon.mk
