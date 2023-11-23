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

function handle_extract() {
    local sname="${1}";
    local type="${2}";
    if [ -z "${sname}" -o -z "${type}" ]; then return -1; fi;

    local ESPATH="${TMPDIR}/extract/${sname}";
    local devname="${sname#*-}";

    if [ "${type}" == "google-recovery" ]; then
        mkdir ${ESPATH}/system;
        mkdir ${ESPATH}/vendor;

        unzip -d ${ESPATH} ${TMPDIR}/downloads/${sname}.zip 1>/dev/null 2>&1;
        unzip -d ${ESPATH} ${ESPATH}/${devname}-*/image-${devname}-*.zip 1>/dev/null system.img vendor.img 2>&1;

        ${HOST_BINS}/simg2img ${ESPATH}/system.img ${ESPATH}/system.raw.img;
        7z x -o${ESPATH}/system ${ESPATH}/system.raw.img 1>/dev/null 2>&1;

        ${HOST_BINS}/simg2img ${ESPATH}/vendor.img ${ESPATH}/vendor.raw.img;
        7z x -o${ESPATH}/vendor ${ESPATH}/vendor.raw.img 1>/dev/null 2>&1;

        rm -rf \
          ${ESPATH}/${devname}-* \
          ${ESPATH}/system*.img \
          ${ESPATH}/vendor*.img;

	return 0;
    fi;

    return -1;
}
