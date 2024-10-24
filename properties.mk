# AV
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.media.avsync=true \
    ro.hardware.audio.primary=sphynx

# Bluetooth
PRODUCT_PROPERTY_OVERRIDES += \
    bluetooth.device.default_name=Pixel C

# Bpf
PRODUCT_PROPERTY_OVERRIDES += \
    ro.kernel.ebpf.supported=1

# Charger
PRODUCT_SYSTEM_PROPERTY_OVERRIDES += \
    persist.sys.NV_ECO.IF.CHARGING=false

# USB configfs
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.lineage.tegra.model=Pixel C \
    ro.vendor.nv.usb.pid.adb=4EE7 \
    ro.vendor.nv.usb.pid.accessory.adb=2D01\
    ro.vendor.nv.usb.pid.audio_source.adb=2D03 \
    ro.vendor.nv.usb.pid.ecm=2D04 \
    ro.vendor.nv.usb.pid.ecm.adb=2D05 \
    ro.vendor.nv.usb.pid.midi=2D04 \
    ro.vendor.nv.usb.pid.midi.adb=2D05 \
    ro.vendor.nv.usb.pid.mtp=2D04 \
    ro.vendor.nv.usb.pid.mtp.adb=2D05 \
    ro.vendor.nv.usb.pid.ncm=2D04 \
    ro.vendor.nv.usb.pid.ncm.adb=2D05 \
    ro.vendor.nv.usb.pid.ptp=2D04 \
    ro.vendor.nv.usb.pid.ptp.adb=2D05 \
    ro.vendor.nv.usb.pid.rndis=2D04 \
    ro.vendor.nv.usb.pid.rndis.acm.adb=2D05 \
    ro.vendor.nv.usb.pid.rndis.adb=2D05 \
    ro.vendor.nv.usb.vid=18D1 \
    sys.usb.controller=700d0000.xudc \
    vendor.sys.usb.udc=700d0000.xudc
