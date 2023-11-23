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

# The previous default crashes the current firmware
function patch_smaug_nvram() {
  echo -n "Patching nvram default ccodes...";

  sed -i 's/ccode=0/ccode=XY/'  ${LINEAGE_ROOT}/vendor/google/sphynx/opm8/bcm_firmware/bcm4354/bcmdhd.cal

  echo "";
}

patch_smaug_nvram;
