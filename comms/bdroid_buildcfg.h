/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _BDROID_BUILDCFG_H
#define _BDROID_BUILDCFG_H

// SERVICE_CLASS:0x5A (Bit17 -Networking,Bit19 - Capturing,Bit20 -Object Transfer,Bit22 -Telephony)
// MAJOR CLASS: COMPUTER
// MINOR CLASS: TABLET
#define BTA_DM_COD {0x5A, 0x01, 0x10}

#define BTIF_HF_WBS_PREFERRED TRUE
#define BTM_WBS_INCLUDED TRUE
#define BLE_VND_INCLUDED TRUE

#define BTSNOOP_MEM_BUFFER_SIZE (1024 * 1024)
#define DEBUG_FS_UART_PATH "/d/70006300.serial/tty_buffer_count"

// To sync with CONN TIMEOUT of pepper
#define BTM_BLE_CONN_TIMEOUT_DEF 1000

// Increase MAX GATT notification to 75
#define BTA_GATTC_NOTIF_REG_MAX 75

// Turn off BLE_PRIVACY_SPT. Remote reconnect fails often if this is enabled.
#define BLE_PRIVACY_SPT FALSE

#endif
