# Copyright (c) 2026-present Project AeroDENS Contributors
# SPDX-License-Identifier: Apache-2.0

import copy
import os
import shutil

from platformio.platform.base import PlatformBase


class Gd32h7Platform(PlatformBase):
    """
    PlatformIO Development Platform for GigaDevice GD32H7xx series MCUs
    and the GD32H759I-EVAL development board.
    """

    def __init__(self, manifest_path):
        super().__init__(os.path.abspath(manifest_path))

    def is_embedded(self):
        return True

    def configure_default_packages(self, options, targets):
        # Require PlatformIO's toolchain-gccarmnoneeabi if arm-none-eabi-gcc is not
        # already available on the host PATH (or if already installed in PIO packages).
        pio_toolchain = self.get_package("toolchain-gccarmnoneeabi")
        host_gcc = shutil.which("arm-none-eabi-gcc")
        if pio_toolchain or not host_gcc:
            if "toolchain-gccarmnoneeabi" in self.packages:
                self.packages["toolchain-gccarmnoneeabi"]["optional"] = False

        # Enable appropriate upload/debug tool when uploading
        upload_protocol = options.get("upload_protocol")
        board_id = options.get("board")
        if board_id and not upload_protocol:
            board_cfg = self.board_config(board_id)
            upload_protocol = board_cfg.get("upload.protocol", "cmsis-dap")

        if any("upload" in t for t in targets) or "program" in targets:
            if upload_protocol in ("cmsis-dap", "gd-link", "stlink", "openocd"):
                if not shutil.which("openocd") and "tool-openocd" in self.packages:
                    self.packages["tool-openocd"]["optional"] = False
            elif upload_protocol == "jlink":
                if not shutil.which("JLinkExe") and "tool-jlink" in self.packages:
                    self.packages["tool-jlink"]["optional"] = False
            elif upload_protocol == "dfu":
                if not shutil.which("dfu-util") and "tool-dfuutil" in self.packages:
                    self.packages["tool-dfuutil"]["optional"] = False

        return super().configure_default_packages(options, targets)

    def get_lib_storages(self):
        storages = super().get_lib_storages()
        platform_libs = os.path.join(self.get_dir(), "libraries")
        if os.path.isdir(platform_libs):
            storages.append({"name": "platform-gd32h7-bsp", "path": platform_libs})
        return storages

    def get_boards(self, id_=None):
        result = super().get_boards(id_)
        if not result:
            return result
        if id_:
            return self._add_default_debug_tools(result)
        for key, value in result.items():
            result[key] = self._add_default_debug_tools(value)
        return result

    def _add_default_debug_tools(self, board):
        debug = board.manifest.get("debug", {})
        upload_protocols = board.manifest.get("upload", {}).get("protocols", [])
        if "tools" not in debug:
            debug["tools"] = {}

        openocd_target = debug.get("openocd_target", "gd32h7x.cfg")
        platform_openocd_dir = os.path.join(self.get_dir(), "misc", "openocd")

        for link in ("cmsis-dap", "gd-link", "stlink", "jlink"):
            if link not in upload_protocols or link in debug["tools"]:
                continue

            if link == "jlink":
                assert debug.get("jlink_device"), (
                    f"Missed J-Link Device ID for {board.id}"
                )
                debug["tools"][link] = {
                    "server": {
                        "package": "tool-jlink",
                        "arguments": [
                            "-singlerun",
                            "-if",
                            "SWD",
                            "-select",
                            "USB",
                            "-port",
                            "2331",
                            "-device",
                            debug.get("jlink_device"),
                        ],
                        "executable": "JLinkGDBServerCL.exe"
                        if os.name == "nt"
                        else "JLinkGDBServer",
                    },
                    "onboard": link in debug.get("onboard_tools", []),
                }
            else:
                interface_cfg = "cmsis-dap" if link in ("cmsis-dap", "gd-link") else link
                server_args = [
                    "-s",
                    platform_openocd_dir,
                    "-s",
                    "$PACKAGE_DIR/openocd/scripts",
                    "-f",
                    f"interface/{interface_cfg}.cfg",
                ]
                if interface_cfg == "cmsis-dap":
                    server_args.extend(
                        [
                            "-c",
                            "cmsis_dap_vid_pid 0x28e9 0x0497 0x28e9 0x058f 0x28e9 0x018a",
                        ]
                    )
                server_args.extend(
                    [
                        "-c",
                        "transport select swd",
                        "-f",
                        openocd_target,
                    ]
                )
                debug["tools"][link] = {
                    "server": {
                        "package": "tool-openocd",
                        "executable": "bin/openocd",
                        "arguments": server_args,
                    },
                    "onboard": link in debug.get("onboard_tools", []),
                }

        board.manifest["debug"] = copy.deepcopy(debug)
        return board
