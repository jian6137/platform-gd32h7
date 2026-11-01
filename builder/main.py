# Copyright (c) 2026-present Project AeroDENS Contributors
# SPDX-License-Identifier: Apache-2.0

import os
from os.path import isdir, isfile, join
from SCons.Script import (
    COMMAND_LINE_TARGETS,
    AlwaysBuild,
    Builder,
    Default,
    DefaultEnvironment,
)

env = DefaultEnvironment()
platform = env.PioPlatform()
board = env.BoardConfig()

env.Append(LIBSOURCE_DIRS=[join(platform.get_dir(), "libraries")])

env.Replace(
    AR="arm-none-eabi-gcc-ar",
    AS="arm-none-eabi-as",
    CC="arm-none-eabi-gcc",
    GDB="arm-none-eabi-gdb",
    CXX="arm-none-eabi-g++",
    OBJCOPY="arm-none-eabi-objcopy",
    RANLIB="arm-none-eabi-gcc-ranlib",
    SIZETOOL="arm-none-eabi-size",
    ARFLAGS=["rc"],
    SIZEPROGREGEXP=r"^(?:\.isr_vector|\.text|\.rodata|\.data|\.ARM\.exidx|\.init_array|\.fini_array)\s+(\d+).*",
    SIZEDATAREGEXP=r"^(?:\.data|\.bss|\.noinit|\.display_fb|\.video_decode_buf)\s+(\d+).*",
    SIZECHECKCMD="$SIZETOOL -A -d $SOURCES",
    SIZEPRINTCMD="$SIZETOOL -B -d $SOURCES",
    PROGSUFFIX=".elf",
)

# Allow user to override via pre:script
if env.get("PROGNAME", "program") == "program":
    env.Replace(PROGNAME="firmware")

cpu = board.get("build.cpu", "cortex-m7")
fpu = board.get("build.fpu", "fpv5-d16")
float_abi = board.get("build.float_abi", "hard")

machine_flags = [
    f"-mcpu={cpu}",
    "-mthumb",
    f"-mfpu={fpu}",
    f"-mfloat-abi={float_abi}",
]

env.Append(
    ASFLAGS=machine_flags,
    ASPPFLAGS=[
        "-x",
        "assembler-with-cpp",
    ],
    CCFLAGS=machine_flags
    + [
        "-Os" if "debug" not in env.GetBuildType() else "-Og",
        "-ffunction-sections",
        "-fdata-sections",
        "-Wall",
        "-Wextra",
        "-Wno-unused-parameter",
    ],
    CFLAGS=[
        "-std=gnu11",
    ],
    CXXFLAGS=[
        "-std=gnu++17",
        "-fno-rtti",
        "-fno-exceptions",
        "-fno-threadsafe-statics",
    ],
    CPPDEFINES=[
        ("F_CPU", "$BOARD_F_CPU"),
    ],
    LINKFLAGS=machine_flags
    + [
        "-Wl,--gc-sections",
        "-Wl,--check-sections",
        "-Wl,--unresolved-symbols=report-all",
        "-Wl,--warn-common",
        "-Wl,--no-warn-rwx-segments",
        "-Wl,-Map=$BUILD_DIR/${PROGNAME}.map",
        "--specs=nano.specs",
        "--specs=nosys.specs",
    ],
    LIBS=["c", "gcc", "m", "stdc++", "nosys"],
    BUILDERS=dict(
        ElfToBin=Builder(
            action=env.VerboseAction(
                " ".join(["$OBJCOPY", "-O", "binary", "$SOURCES", "$TARGET"]),
                "Building $TARGET",
            ),
            suffix=".bin",
        ),
        ElfToHex=Builder(
            action=env.VerboseAction(
                " ".join(["$OBJCOPY", "-O", "ihex", "-R", ".eeprom", "$SOURCES", "$TARGET"]),
                "Building $TARGET",
            ),
            suffix=".hex",
        ),
    ),
)

if not env.get("PIOFRAMEWORK"):
    env.SConscript("frameworks/_bare.py", exports="env")

#
# Target: Build executable and linkable firmware
#
target_elf = None
if "nobuild" in COMMAND_LINE_TARGETS:
    target_elf = join("$BUILD_DIR", "${PROGNAME}.elf")
    target_firm = join("$BUILD_DIR", "${PROGNAME}.bin")
    target_hex = join("$BUILD_DIR", "${PROGNAME}.hex")
else:
    target_elf = env.BuildProgram()
    target_firm = env.ElfToBin(join("$BUILD_DIR", "${PROGNAME}"), target_elf)
    target_hex = env.ElfToHex(join("$BUILD_DIR", "${PROGNAME}"), target_elf)
    env.Depends(target_firm, "checkprogsize")
    env.Depends(target_hex, "checkprogsize")

AlwaysBuild(env.Alias("nobuild", target_firm))
target_buildprog = env.Alias("buildprog", [target_firm, target_hex])

#
# Target: Print binary size
#
target_size = env.Alias(
    "size", target_elf, env.VerboseAction("$SIZEPRINTCMD", "Calculating size $SOURCE")
)
AlwaysBuild(target_size)

#
# Target: Upload by default .bin / .elf file
#
upload_protocol = env.subst("$UPLOAD_PROTOCOL")
upload_actions = []

if upload_protocol in ("cmsis-dap", "gd-link", "stlink", "openocd"):
    openocd_cfg = board.get("debug.openocd_target", "gd32h7x.cfg")
    interface_cfg = "cmsis-dap" if upload_protocol in ("cmsis-dap", "gd-link") else upload_protocol
    platform_openocd_dir = join(platform.get_dir(), "misc", "openocd")
    openocd_pkg_dir = platform.get_package_dir("tool-openocd") or ""

    openocd_args = [
        "-s",
        platform_openocd_dir,
    ]
    if openocd_pkg_dir and isdir(join(openocd_pkg_dir, "openocd", "scripts")):
        openocd_args.extend(["-s", join(openocd_pkg_dir, "openocd", "scripts")])

    openocd_args.extend(
        [
            "-f",
            f"interface/{interface_cfg}.cfg",
        ]
    )
    if interface_cfg == "cmsis-dap":
        openocd_args.extend(
            [
                "-c",
                "cmsis_dap_vid_pid 0x28e9 0x0497 0x28e9 0x058f 0x28e9 0x018a",
            ]
        )
    openocd_args.extend(
        [
            "-c",
            "transport select swd",
            "-f",
            openocd_cfg,
            "-c",
            "gd32h7_flash_bin {$SOURCE} 0x08000000",
        ]
    )

    env.Replace(
        UPLOADER="openocd",
        UPLOADERFLAGS=openocd_args,
        UPLOADCMD="$UPLOADER $UPLOADERFLAGS",
    )
    upload_actions = [env.VerboseAction("$UPLOADCMD", "Uploading $SOURCE")]

elif upload_protocol == "jlink":
    def _jlink_cmd_script(env, source):
        build_dir = env.subst("$BUILD_DIR")
        if not isdir(build_dir):
            os.makedirs(build_dir)
        script_path = join(build_dir, "upload.jlink")
        commands = [
            "h",
            f'loadbin "{source}", 0x08000000',
            "r",
            "q",
        ]
        with open(script_path, "w", encoding="utf-8") as fp:
            fp.write("\n".join(commands))
        return script_path

    env.Replace(
        __jlink_cmd_script=_jlink_cmd_script,
        UPLOADER="JLink.exe" if os.name == "nt" else "JLinkExe",
        UPLOADERFLAGS=[
            "-device",
            board.get("debug.jlink_device", "GD32H759IM"),
            "-speed",
            env.GetProjectOption("debug_speed", "4000"),
            "-if",
            "swd",
            "-autoconnect",
            "1",
            "-NoGui",
            "1",
        ],
        UPLOADCMD='$UPLOADER $UPLOADERFLAGS -CommanderScript "${__jlink_cmd_script(__env__, SOURCE)}"',
    )
    upload_actions = [env.VerboseAction("$UPLOADCMD", "Uploading $SOURCE")]

elif upload_protocol == "dfu":
    env.Replace(
        UPLOADER="dfu-util",
        UPLOADERFLAGS=[
            "-a",
            "0",
            "-s",
            "0x08000000:leave",
            "-D",
        ],
        UPLOADCMD='$UPLOADER $UPLOADERFLAGS "$SOURCE"',
    )
    upload_actions = [env.VerboseAction("$UPLOADCMD", "Uploading $SOURCE")]

elif upload_protocol == "custom":
    upload_actions = [env.VerboseAction("$UPLOADCMD", "Uploading $SOURCE")]

AlwaysBuild(
    env.Alias(
        "upload",
        target_firm,
        upload_actions,
    )
)

#
# Default targets
#
Default([target_buildprog, target_size])
