#
# Copyright (C) 2022-2023 The LineageOS Project
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

# Tegra pcie host
BOARD_VENDOR_KERNEL_MODULES_LOAD := \
    pci-tegra

# Nvhost podgov
BOARD_VENDOR_KERNEL_MODULES_LOAD += \
    governor_pod_scaling

# Proprietary gpu driver
BOARD_VENDOR_KERNEL_MODULES_LOAD += \
    nvgpu

# Realtek 8168
BOARD_VENDOR_KERNEL_MODULES_LOAD += \
    r8168

# Bluedroid power management
BOARD_VENDOR_KERNEL_MODULES_LOAD += \
    bluedroid_pm

# Sphynx specific audio drivers
BOARD_VENDOR_KERNEL_MODULES_LOAD += \
    snd_soc_max98357a \
    snd_soc_rt5677 \
    snd_soc_nau8825

# Tegra hdmi audio
BOARD_VENDOR_KERNEL_MODULES_LOAD += \
    snd-hda-tegra

# Tegra audio processing engine
BOARD_VENDOR_KERNEL_MODULES_LOAD += \
<<<<<<< HEAD   (c4ef6b modules: Fix up names to match filenames)
    snd-soc-tegra210-alt-xbar \
    snd-soc-tegra210-alt-admaif \
    snd-soc-tegra210-alt-sfc \
    snd-soc-tegra210-alt-i2s \
    snd-soc-tegra210-alt-mixer \
    snd-soc-tegra210-alt-afc \
    snd-soc-tegra210-alt-adx \
    snd-soc-tegra210-alt-amx \
    snd-soc-tegra210-alt-dmic \
    snd-soc-tegra210-alt-mvc \
    snd-soc-tegra210-alt-ope \
    snd-soc-tegra-machine-driver
=======
    snd_soc_tegra210_alt_xbar \
    snd_soc_tegra210_alt_admaif \
    snd_soc_tegra210_alt_sfc \
    snd_soc_tegra210_alt_i2s \
    snd_soc_tegra210_alt_mixer \
    snd_soc_tegra210_alt_afc \
    snd_soc_tegra210_alt_adx \
    snd_soc_tegra210_alt_amx \
    snd_soc_tegra210_alt_dmic \
    snd_soc_tegra210_alt_mvc \
    snd_soc_tegra210_alt_ope \
    snd_soc_tegra_machine_driver_rt5677_mobile
>>>>>>> CHANGE (641501 [DNM] [TMP] sphynx: Build device-specific audio machine driv)

# Userspace aes crypto access
BOARD_VENDOR_KERNEL_MODULES_LOAD += \
    tegra-cryptodev

# Input cpufreq boost
BOARD_VENDOR_KERNEL_MODULES_LOAD += \
    input-cfboost

# Fan
BOARD_VENDOR_KERNEL_MODULES_LOAD += \
    pwm_fan \
    therm_fan_est

# Power Monitor
BOARD_VENDOR_KERNEL_MODULES_LOAD += \
    ina3221

# USB Storage
BOARD_VENDOR_KERNEL_MODULES_LOAD += \
    usb-storage

# USB Modem
BOARD_VENDOR_KERNEL_MODULES_LOAD += \
    cdc-acm

# Misc Controllers
BOARD_VENDOR_KERNEL_MODULES_LOAD += \
    hid-xinmo \
    hid-betopff

# Copy to recovery
BOARD_RECOVERY_RAMDISK_KERNEL_MODULES_LOAD := \
    hid-nvidia-blake \
    hid-jarvis-remote \
    usb-storage

RECOVERY_KERNEL_MODULES := $(addsuffix .ko,$(BOARD_RECOVERY_RAMDISK_KERNEL_MODULES_LOAD))
