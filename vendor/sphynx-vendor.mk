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

include device/nvidia/t210-common/vendor/t210-by-flags.mk
include device/nvidia/tegra-common/vendor/common-by-flags.mk

SPHYNX_BCM_PATH := vendor/google/sphynx/rel-shield-r/bcm
OPM8_BCM_PATH := vendor/google/sphynx/opm8/bcm
OPM8_BPMP_PATH := vendor/google/sphynx/opm8/firmware

include device/nvidia/tegra-common/vendor/rel-shield-r/bcm/bcm4354.mk

# Device specific bcm firmware
PRODUCT_COPY_FILES += \
    $(SPHYNX_BCM_PATH)/bcm4354/foster.clm_blob:$(TARGET_COPY_OUT_VENDOR)/firmware/bcmdhd_clm_foster.blob

# Device specific bcm nvram
PRODUCT_COPY_FILES += \
    $(OPM8_BCM_PATH)/bcm4354/bcmdhd.cal:$(TARGET_COPY_OUT_VENDOR)/firmware/nvram_smaug_4354.txt

# Device specific BPMP firmware
PRODUCT_COPY_FILES += \
    $(OPM8_BPMP_PATH)/bpmp.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/tegra21x/bpmp-lite.bin \
    $(OPM8_BPMP_PATH)/bpmp.bin:recovery/root/lib/firmware/bpmp-lite.bin
