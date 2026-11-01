# Copyright (c) 2026-present Project AeroDENS Contributors
# SPDX-License-Identifier: Apache-2.0

"""
CMSIS Framework Builder for GigaDevice GD32H7xx series.
Compiles CMSIS-Core Cortex-M7 headers, system_gd32h7xx.c, and GCC startup vector table.
"""

import os
from os.path import isdir, isfile, join
from SCons.Script import DefaultEnvironment

env = DefaultEnvironment()
platform = env.PioPlatform()
board = env.BoardConfig()

sdk_dir = join(platform.get_dir(), "sdk")
cmsis_dir = join(sdk_dir, "CMSIS")
cmsis_core_inc = join(cmsis_dir, "Core", "Include")
cmsis_gd_dir = join(cmsis_dir, "GD", "GD32H7xx")
cmsis_gd_inc = join(cmsis_gd_dir, "Include")
cmsis_gd_src = join(cmsis_gd_dir, "Source")

assert isdir(cmsis_core_inc), f"Missing CMSIS Core Include directory: {cmsis_core_inc}"
assert isdir(cmsis_gd_inc), f"Missing GD32H7xx CMSIS Include directory: {cmsis_gd_inc}"

# Configure linker script
ldscript = board.get("build.ldscript", "gd32h759i_flash.ld")
if not isfile(ldscript):
    ldscript = join(platform.get_dir(), "ldscripts", ldscript)
env.Replace(LDSCRIPT_PATH=ldscript)

env.Append(
    CPPPATH=[
        cmsis_core_inc,
        cmsis_gd_inc,
    ],
    CPPDEFINES=[
        "GD32H7XX",
    ],
)

# Build CMSIS Device system_gd32h7xx.c and GCC startup_gd32h7xx.c
env.BuildSources(
    join("$BUILD_DIR", "FrameworkCMSIS"),
    cmsis_gd_src,
    src_filter=[
        "+<system_gd32h7xx.c>",
        "+<GCC/startup_gd32h7xx.c>",
    ],
)

