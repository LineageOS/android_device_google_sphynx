# Copyright (C) 2021 The LineageOS Project
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

# Needed by the Pixel C
function fetch_smaug_nvram() {
  echo -n "Fetching Smaug nvram from Android Dumps...";

  mkdir -p ${LINEAGE_ROOT}/${OUTDIR}/sphynx/bcm_firmware/bcm4354
  wget -q 'https://dumps.tadiphone.dev/dumps/google/dragon/-/raw/ryu-user-8.1.0-OPM8.190605.005-5749003-release-keys/system/etc/wifi/bcmdhd.cal' -O ${LINEAGE_ROOT}/${OUTDIR}/sphynx/bcm_firmware/bcm4354/bcmdhd.cal

  echo "";
}

# The previous default crashes the current firmware
function patch_nvrams() {
  echo -n "Patching nvram default ccodes...";

  sed -i 's/ccode=0/ccode=XY/'  ${LINEAGE_ROOT}/${OUTDIR}/sphynx/bcm_firmware/bcm4354/bcmdhd.cal
}

fetch_smaug_nvram;
patch_nvrams;
