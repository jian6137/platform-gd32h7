# Copyright (c) 2026-present Project AeroDENS Contributors
# SPDX-License-Identifier: Apache-2.0

from os.path import isfile, join
from SCons.Script import DefaultEnvironment

env = DefaultEnvironment()
platform = env.PioPlatform()
board = env.BoardConfig()

ldscript = board.get("build.ldscript", "gd32h759i_flash.ld")
if not isfile(ldscript):
    ldscript = join(platform.get_dir(), "ldscripts", ldscript)
env.Replace(LDSCRIPT_PATH=ldscript)

