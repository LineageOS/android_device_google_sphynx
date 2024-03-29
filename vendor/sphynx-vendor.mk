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
include device/google/sphynx/vendor/rel-shield-r/bcm_firmware/bcm.mk
include device/google/sphynx/vendor/sphynx-recovery.mk

PRODUCT_PACKAGES += public.libraries

# Smaug bpmp
PRODUCT_PACKAGES += bpmp-lite
