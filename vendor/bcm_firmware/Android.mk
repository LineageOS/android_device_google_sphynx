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

LOCAL_PATH := $(call my-dir)
SPHYNX_EXTERNAL_BCM_PATH := ../../../../../vendor/nvidia/foster/external/bcm_firmware
SPHYNX_BCM_PATH := ../../../../../vendor/nvidia/foster/rel-shield-r/bcm_firmware

include $(CLEAR_VARS)
LOCAL_MODULE               := nvram_smaug_4354
LOCAL_SRC_FILES            := $(SPHYNX_EXTERNAL_BCM_PATH)/bcm4354/bcmdhd.cal
LOCAL_MODULE_SUFFIX        := .txt
LOCAL_MODULE_CLASS         := ETC
LOCAL_MODULE_PATH          := $(TARGET_OUT_VENDOR)/firmware
LOCAL_MODULE_TAGS          := optional
LOCAL_MODULE_OWNER         := nvidia
include $(BUILD_NVIDIA_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE               := bcmdhd_clm_foster
LOCAL_SRC_FILES            := $(SPHYNX_BCM_PATH)/bcm4354/foster.clm_blob
LOCAL_MODULE_SUFFIX        := .blob
LOCAL_MODULE_CLASS         := ETC
LOCAL_MODULE_PATH          := $(TARGET_OUT_VENDOR)/firmware
LOCAL_MODULE_TAGS          := optional
LOCAL_MODULE_OWNER         := nvidia
include $(BUILD_NVIDIA_PREBUILT)