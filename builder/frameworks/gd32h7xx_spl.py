# Copyright (c) 2026-present Project AeroDENS Contributors
# SPDX-License-Identifier: Apache-2.0

"""
GigaDevice GD32H7xx Standard Peripheral Library (SPL V1.4.0) Framework Builder.
Includes:
  - ARM CMSIS-Core Cortex-M7 + GD32H7xx Device Peripheral Access Layer
  - GD32H7xx Standard Peripheral Library (all 49 peripheral drivers)
  - Optional GD32H7xx USBHS Device/Host Library (enabled via -DPIO_GD32H7_USBHS_DEVICE or -DPIO_GD32H7_USBHS_HOST)
  - Built-in GD32H759I-EVAL Board Support Package libraries (auto-discovered via LDF)
"""

import os
from os.path import isdir, isfile, join
from SCons.Script import DefaultEnvironment, SConscript

env = DefaultEnvironment()
platform = env.PioPlatform()
board = env.BoardConfig()

# First load CMSIS base (core_cm7, system_gd32h7xx, GCC startup & linker script)
SConscript("cmsis.py", exports="env")

sdk_dir = join(platform.get_dir(), "sdk")
spl_dir = join(sdk_dir, "GD32H7xx_standard_peripheral")
spl_inc = join(spl_dir, "Include")
spl_src = join(spl_dir, "Source")

assert isdir(spl_inc), f"Missing GD32H7xx SPL Include directory: {spl_inc}"
assert isdir(spl_src), f"Missing GD32H7xx SPL Source directory: {spl_src}"

env.Append(
    CPPPATH=[
        spl_inc,
    ],
    CPPDEFINES=[
        "USE_STDPERIPH_DRIVER",
    ],
)

# Build GD32H7xx Standard Peripheral Library sources
# Unused peripheral functions are automatically stripped by -ffunction-sections and -Wl,--gc-sections
env.BuildSources(
    join("$BUILD_DIR", "FrameworkSPL"),
    spl_src,
    src_filter=["+<*.c>"],
)

# Optional USBHS Library integration via build_flags
cppdefines_flat = [
    d[0] if isinstance(d, (tuple, list)) else str(d)
    for d in env.get("CPPDEFINES", [])
]

usbhs_dir = join(sdk_dir, "GD32H7xx_usbhs_library")
if isdir(usbhs_dir) and any(
    flag in cppdefines_flat
    for flag in ("PIO_GD32H7_USBHS", "PIO_GD32H7_USBHS_DEVICE", "PIO_GD32H7_USBHS_HOST")
):
    env.Append(
        CPPPATH=[
            join(usbhs_dir, "driver", "Include"),
            join(usbhs_dir, "ustd", "common"),
        ]
    )
    usb_src_filter = [
        "+<driver/Source/drv_usb_core.c>",
    ]
    if "PIO_GD32H7_USBHS_DEVICE" in cppdefines_flat or "PIO_GD32H7_USBHS" in cppdefines_flat:
        env.Append(
            CPPPATH=[
                join(usbhs_dir, "device", "core", "Include"),
                join(usbhs_dir, "device", "class", "cdc", "Include"),
                join(usbhs_dir, "device", "class", "hid", "Include"),
                join(usbhs_dir, "device", "class", "msc", "Include"),
                join(usbhs_dir, "device", "class", "audio", "Include"),
                join(usbhs_dir, "device", "class", "dfu", "Include"),
            ]
        )
        usb_src_filter.extend(
            [
                "+<driver/Source/drv_usb_dev.c>",
                "+<driver/Source/drv_usbd_int.c>",
                "+<device/core/Source/*.c>",
            ]
        )
    if "PIO_GD32H7_USBHS_HOST" in cppdefines_flat:
        env.Append(
            CPPPATH=[
                join(usbhs_dir, "host", "core", "Include"),
                join(usbhs_dir, "host", "class", "cdc", "Include"),
                join(usbhs_dir, "host", "class", "hid", "Include"),
                join(usbhs_dir, "host", "class", "msc", "Include"),
            ]
        )
        usb_src_filter.extend(
            [
                "+<driver/Source/drv_usb_host.c>",
                "+<driver/Source/drv_usbh_int.c>",
                "+<host/core/Source/*.c>",
            ]
        )
    env.BuildSources(
        join("$BUILD_DIR", "FrameworkUSBHS"),
        usbhs_dir,
        src_filter=usb_src_filter,
    )

