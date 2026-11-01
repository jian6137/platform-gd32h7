# 兆易创新 GigaDevice GD32H7 PlatformIO 开发平台 (`platform-gd32h7`)

**Language Available / 可选语言：**
1. English - [Here](README.md)
2. Chinese (中文) - [Current](ZH_CN.md)

专为 **兆易创新（GigaDevice）GD32H7xx** 系列微控制器（**ARM Cortex-M7 @ 600 MHz**、双精度浮点单元 `fpv5-d16`、L1 指令/数据缓存、512 KB AXI SRAM、ITCM/DTCM）打造的开源、自包含 **PlatformIO 开发平台（Development Platform）**。本项目针对 **GD32H759I-EVAL** 官方全功能评估板（主控 `GD32H759IMK6`，BGA176 封装）提供了真正开箱即用的工具链配置、基于 OpenOCD 的片内 FMC Flash 烧录算法，以及 8 个模块化板级支持包（BSP）驱动库。

---

## 目录

1. [项目初衷（Why I Made It）](#项目初衷why-i-made-it)
2. [核心特性](#核心特性)
3. [安装与快速上手 (`platformio.ini`)](#安装与快速上手-platformioini)
4. [Linux 下板载 GD-Link V2 的 USB `udev` 权限配置](#linux-下板载-gd-link-v2-的-usb-udev-权限配置)
5. [GD32H759I-EVAL 硬件跳线帽（Jumper）配置速查表](#gd32h759i-eval-硬件跳线帽jumper配置速查表)
6. [内置 BSP 驱动库与代码示例](#内置-bsp-驱动库与代码示例)
   - [1. `GD32H759I_EVAL`（板载 LED、按键、USART0、SysTick、32 MB SDRAM）](#1-gd32h759i_eval板载-led按键usart0systick32-mb-sdram)
   - [2. `GD32H759I_LCD`（480x272 RGB565 TLI 液晶屏与屏幕终端）](#2-gd32h759i_lcd480x272-rgb565-tli-液晶屏与屏幕终端)
   - [3. `GD32H759I_Touch`（GT911 五点电容触摸驱动）](#3-gd32h759i_touchgt911-五点电容触摸驱动)
   - [4. `GD32H759I_SDIO_FatFs`（SDIO0 MicroSD 卡与 FatFs R0.15 文件系统）](#4-gd32h759i_sdio_fatfssdio0-microsd-卡与-fatfs-r015-文件系统)
   - [5. `GD32H759I_Audio`（SPI1/I2S1 DMA 双缓冲音频输出）](#5-gd32h759i_audiospi1i2s1-dma-双缓冲音频输出)
   - [6. `GD32H759I_OSPI_Flash`（GD25X512ME 64 MB 八线 SPI NOR Flash）](#6-gd32h759i_ospi_flashgd25x512me-64-mb-八线-spi-nor-flash)
   - [7. `GD32H759I_EEPROM`（AT24C02 I2C1 EEPROM）](#7-gd32h759i_eepromat24c02-i2c1-eeprom)
   - [8. `GD32H759I_Camera`（OV2640 DCI 数字摄像头）](#8-gd32h759i_cameraov2640-dci-数字摄像头)
7. [编译选项与宏配置说明](#编译选项与宏配置说明)
8. [内存映射与链接脚本（Linker Scripts）](#内存映射与链接脚本linker-scripts)
9. [内置示例工程](#内置示例工程)
10. [开源协议](#开源协议)

---

## 项目初衷（Why I Made It）

这个项目诞生于我参加 **RT-Thread 嵌入式软件大赛** 期间。当时在选型阶段，**GD32H759I-EVAL** 评估板精致大气的硬件设计与丰富的外设配置一下子吸引了我的目光。然而在实际上手开发后我发现，开源社区中针对这款评估板的现代化、开箱即用开发工具链支持非常匮乏，于是我决定自己动手构建一套完整支持，并将其开源回馈给社区。

恰逢同一时期，我们的 **BCA152** 课程在 ESP32 开发中引入了 **PlatformIO**。亲身体验到 PlatformIO 简洁、优雅且高效的工程管理体验后，我便萌生了将这套流畅的一键编译、一键烧录开发工作流完整移植到 **GD32H759I-EVAL** 开发板上的想法，让每一位开发者都能轻松上手这块性能强大的 Cortex-M7 评估板。

---

## 核心特性

- **零外部 SDK 依赖**：完整内置 **CMSIS Core**、**GD32H7xx 器件支持包**、**GD32H7xx 标准外设库（SPL V1.4.0）** 以及可选的 **GD32H7xx USBHS 主机/设备协议栈**，拉取即用。
- **内置 OpenOCD GD32H7xx FMC Flash 烧录算法 (`gd32h7_flash_bin`)**：
  - 直接兼容标准上游 OpenOCD（`v0.11.0+`）与板载 **GD-Link V2 (`0x28e9:0x0497`)** CMSIS-DAP 调试器，无需单独寻找兆易创新定制版 OpenOCD。
  - 自动完成 GD32H7xx 片内 `FMC`（`0x52002000`）解锁、4 KB 扇区按需擦除、在 AXI SRAM（`0x24000000`）中运行 24 字节 Cortex-M7 Thumb-2 高速 RAM 搬运算法将固件写入 `0x08000000`（带 `DSB` 内存屏障同步）、通过 `verify_image` 校验数据、重新上锁 `FMC` 并自动复位运行。
- **完善的 GCC 启动文件与链接脚本**：
  - `startup_gd32h7xx.c` 提供完整的 217 个外设中断向量表（`g_pfnVectors`）、弱符号（`weak`）默认中断服务函数别名以及 Newlib 系统调用桩函数（`_sbrk`、`_write` 等）。
  - 支持通过 `-DPIO_GD32H7_PRINTF_USART0` 宏一键将标准 C `printf` 重定向至板载 `USART0`（`PA9`/`PA10`）。
  - 提供 `gd32h759i_flash.ld` 与 `gd32h759i_flash_3840k.ld` 链接脚本，内置 64 字节缓存行对齐的 AXI SRAM 显存段（`.display_fb`）以及 32 MB 外部 SDRAM 段（`.sdram`，基地址 `0xC0000000`）。
- **智能工具链检测**：优先自动调用系统 `PATH` 中已安装的 `arm-none-eabi-gcc`，若未安装则自动回退使用 PlatformIO 托管的 `toolchain-gccarmnoneeabi`。

---

## 安装与快速上手 (`platformio.ini`)

### 方式一：通过 PlatformIO 官方仓库（PlatformIO Registry）安装
本平台已发布至 PlatformIO 官方注册中心：**[https://registry.platformio.org/platforms/jian6137/gd32h7](https://registry.platformio.org/platforms/jian6137/gd32h7)** *（注：截至 2026 年 11 月 1 日，该软件包正在等待 PlatformIO 官方审核；在审核通过前，可直接使用下方的“方式二”通过 GitHub 地址安装）*。

全局安装（安装后即可在 **PIO Home -> New Project** 新建工程向导的下拉列表中直接选择 **GigaDevice GD32H759I-EVAL** 开发板）：
```bash
pio pkg install --global --platform jian6137/gd32h7
```

或者直接在工程的 `platformio.ini` 中引用：
```ini
[env:gd32h759i_eval]
platform = jian6137/gd32h7
board = gd32h759i_eval
framework = gd32h7xx_spl
monitor_speed = 115200
build_flags =
    -DPIO_GD32H7_PRINTF_USART0
```

### 方式二：直接通过 GitHub 仓库地址安装
在你的 PlatformIO 工程 `platformio.ini` 中直接指定本仓库的 GitHub 地址，PlatformIO 会自动完成平台拉取与配置：

```ini
[env:gd32h759i_eval]
platform = https://github.com/jian6137/platform-gd32h7.git
board = gd32h759i_eval
framework = gd32h7xx_spl
monitor_speed = 115200
build_flags =
    -DPIO_GD32H7_PRINTF_USART0
```

若希望将其全局安装至 **PIO Home -> New Project** 下拉列表中：
```bash
pio pkg install --global --platform https://github.com/jian6137/platform-gd32h7.git
```

### 方式三：本地克隆（Clone）/ 软链接引用
将本仓库克隆到本地目录并通过 `symlink://` 引用：

```bash
git clone https://github.com/jian6137/platform-gd32h7.git
```

```ini
[env:gd32h759i_eval]
platform = symlink:///path/to/platform-gd32h7
board = gd32h759i_eval
framework = gd32h7xx_spl
monitor_speed = 115200
```

---

## Linux 下板载 GD-Link V2 的 USB `udev` 权限配置

**GD32H759I-EVAL** 板载了 **GD-Link V2** SWD 调试器（`USB VID:PID 28e9:0497`，识别名称为 `GDMicroelectronics GDLinker_V2`）。在 Linux 系统下，只需安装一次仓库附带的 `udev` 规则，即可免 `sudo` 直接进行一键烧录与在线调试：

```bash
sudo cp misc/udev/99-gdlink.rules /etc/udev/rules.d/99-gdlink.rules
sudo udevadm control --reload-rules && sudo udevadm trigger
```

---

## GD32H759I-EVAL 硬件跳线帽（Jumper）配置速查表

`GD32H759IMK6` 的部分引脚在评估板上通过物理跳线帽与多个板载外设复用。启用对应外设前请核对以下跳线位置：

| 板载外设 | MCU 引脚 | GD32H759I-EVAL 跳线帽位置要求 | 复用冲突与注意事项 |
| :--- | :--- | :--- | :--- |
| **`LED1` 指示灯** | `PF10` | `JP50` -> `LED` | **与液晶屏使能脚 `LCD_DE`（`JP50` -> `LCD`）及 OSPI（`JP50` -> `OSPI`）复用。** 使用 RGB 液晶屏时，请将 `JP50` 保持在 `LCD` 侧，并使用 `LED2` 作为状态灯。 |
| **`LED2` 指示灯** | `PA6` | `JP66` -> `LED` | 可与 RGB 液晶屏同时正常使用。 |
| **`USART0` (`EVAL_COM0`)** | `PA9` (TX), `PA10` (RX) | `JP2` -> `USART0` | 115200 8N1 串口终端（配合 `-DPIO_GD32H7_PRINTF_USART0`）。 |
| **TLI 480x272 RGB 液晶屏** | RGB565 + `PG13`（背光） | `JP41`–`JP59` -> `LCD` | `PG13` 控制屏幕背光开关（`gd32h759i_lcd_backlight_set(1/0)`）。 |
| **GT911 电容触摸屏** | `PH7` (SCL), `PF9` (SDA), `PF6` (RST), `PH6` (INT) | `JP13` -> `I2C` | 基于软件 I2C（地址 `0xBA`）的 5 点电容触摸控制。 |
| **SDIO0 MicroSD 卡槽** | `PD2` (CMD), `PC12` (CLK), `PB13` (`DAT0`) / `PC8..11` | `JP4` -> `SDCard`; `JP13`, `JP19`, `JP23` -> `SDIO` | 支持 1-bit / 4-bit SDIO 模式，无缝对接 FatFs R0.15。 |
| **SPI1/I2S1 音频 DAC** | `PA2`, `PB12`, `PD3`, `PC1` | `JP22`, `JP41`, `JP42`, `JP43` -> `I2S`; `P2`/`P3` -> `P2-2`/`P3-3` | 由小数分频 `PLL1` 音频时钟 + DMA0 通道 0 驱动。 |
| **OSPI Flash (`GD25X512ME`)** | `PF6..10`, `PG6`, `PB2` | `JP33`..`JP38`, `JP42`, `JP43`, `JP49`, `JP50` -> `OSPI` | 板载 64 MB 八线 SPI NOR Flash。 |

---

## 内置 BSP 驱动库与代码示例

全部 8 个模块化 BSP 库位于 [`libraries/`](libraries/) 目录下。只要在代码中 `#include` 对应头文件，PlatformIO 的库依赖查找器（LDF）就会**自动识别并参与编译链接**，无需手动配置 `lib_deps`。

| 驱动库名称 | 需包含的头文件 (`#include`) | 功能概述 |
| :--- | :--- | :--- |
| **`GD32H759I_EVAL`** | `gd32h759i_eval.h`, `systick.h`, `exmc_sdram.h` | 核心板级支持：`LED1`/`LED2`、Wakeup/Tamper/User 按键、`USART0` 串口、1 ms `SysTick` 延时（`delay_1ms()`）及 32 MB EXMC SDRAM 驱动。 |
| **`GD32H759I_LCD`** | `gd32h759i_lcd.h`, `lcd_console.h` | 480x272 RGB565 TLI 液晶屏驱动（内置 D-Cache 刷写同步 `gd32h759i_lcd_flush()`）及 8x8 点阵字库滚动终端（`lcd_console_printf()`）。 |
| **`GD32H759I_Touch`** | `bsp_ts_gt911.h`, `bsp_i2c_touch.h` | 汇顶 GT911 5 点电容触摸屏驱动（`gt911_init()`、`gt911_handler()`）。 |
| **`GD32H759I_SDIO_FatFs`** | `sdcard.h`, `ff.h`, `diskio.h` | SDIO0 MicroSD 卡底层驱动 + ChaN FatFs R0.15 文件系统（`f_mount`、`f_open`、`f_read`、`f_write`、`f_opendir`）。 |
| **`GD32H759I_Audio`** | `audio_player.h` | SPI1/I2S1 DMA 双缓冲音频播放驱动（`audio_player_init()`、`audio_player_start()`、`audio_player_get_BackBuffer()`）。 |
| **`GD32H759I_OSPI_Flash`** | `gd25x512me.h` | 板载 `GD25X512ME`（64 MB）八线 SPI（OSPI）NOR Flash 驱动。 |
| **`GD32H759I_EEPROM`** | `at24cxx.h`, `i2c.h` | 板载 `AT24C02` 硬件 `I2C1` EEPROM 读写驱动。 |
| **`GD32H759I_Camera`** | `dci_ov2640.h`, `sccb.h` | `OV2640` 摄像头模块 DCI 数字摄像头接口与 SCCB（`I2C1`）驱动。 |

### 1. `GD32H759I_EVAL`（板载 LED、按键、USART0、SysTick、32 MB SDRAM）

```c
#include "gd32h7xx.h"
#include "gd32h759i_eval.h"
#include "systick.h"
#include "exmc_sdram.h"
#include <stdio.h>

int main(void)
{
    SCB_EnableICache();
    SCB_EnableDCache();
    systick_config();

    /* 初始化板载 LED（LED1 需 JP50->LED，LED2 需 JP66->LED）与 USART0（115200 8N1） */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_com_init(EVAL_COM0);

    /* 可选：初始化映射在 0xC0000000 的 32 MB 外部 SDRAM */
    exmc_synchronous_dynamic_ram_init(EXMC_SDRAM_DEVICE0);

    printf("SystemCoreClock: %lu Hz\r\n", (unsigned long)SystemCoreClock);

    while (1) {
        gd_eval_led_toggle(LED1);
        gd_eval_led_toggle(LED2);
        delay_1ms(500);
    }
}
```

### 2. `GD32H759I_LCD`（480x272 RGB565 TLI 液晶屏与屏幕终端）

```c
#include "gd32h759i_lcd.h"
#include "lcd_console.h"

void lcd_demo(void)
{
    gd32h759i_lcd_init();      /* 配置 TLI 引脚、PLL2 像素时钟、LAYER0 图层并开启 PG13 背光 */
    lcd_console_init();        /* 清空 480x272 显存并初始化 8x8 字符终端 */

    lcd_console_printf("GD32H759I-EVAL Display Ready (%ux%u)\n", LCD_WIDTH, LCD_HEIGHT);
    gd32h759i_lcd_fill_rect(400, 16, 48, 48, LCD_COLOR_GREEN);
}
```

### 3. `GD32H759I_Touch`（GT911 五点电容触摸驱动）

```c
#include "bsp_ts_gt911.h"

void touch_poll(void)
{
    if (gt911_handler() == SUCCESS && (gt911.pressed_info & 0x0F)) {
        uint16_t x = gt911.x[0];
        uint16_t y = gt911.y[0];
        /* 处理触控坐标 (x, y) */
    }
}
```

### 4. `GD32H759I_SDIO_FatFs`（SDIO0 MicroSD 卡与 FatFs R0.15 文件系统）

```c
#include "ff.h"

static FATFS g_fs;

void sd_mount_demo(void)
{
    if (f_mount(&g_fs, "0:", 1) == FR_OK) {
        FIL file;
        if (f_open(&file, "0:/test.txt", FA_READ) == FR_OK) {
            /* 使用 f_read(&file, ...) 读取数据 */
            f_close(&file);
        }
    }
}
```

### 5. `GD32H759I_Audio`（SPI1/I2S1 DMA 双缓冲音频输出）

```c
#include "audio_player.h"

void audio_demo(void)
{
    audio_player_init(44100U); /* 配置 PLL1 音频时钟 + SPI1/I2S1 + DMA0 双缓冲 */
    audio_player_start();

    while (1) {
        int16_t *pcm_buf = audio_player_get_BackBuffer();
        if (pcm_buf != NULL) {
            /* 向 pcm_buf 填充 AUDIO_BUFFER_SAMPLES 个 16-bit 立体声采样点 */
            audio_player_submit_BackBuffer();
        }
    }
}
```

---

## 编译选项与宏配置说明

| `build_flags` 宏 / `platformio.ini` 配置项 | 默认状态 | 说明 |
| :--- | :--- | :--- |
| `-D__SYSTEM_CLOCK_600M_PLL0_HXTAL=600000000` | **默认启用** | 基于板载 25 MHz 外部晶振（`HXTAL`）通过 `PLL0` 倍频至 **600 MHz** 主频。 |
| `-D__SYSTEM_CLOCK_600M_PLL0_IRC64M=600000000` | 未启用 | 基于片内 64 MHz RC 振荡器（`IRC64M`）倍频至 **600 MHz** 主频。 |
| `-DPIO_GD32H7_PRINTF_USART0` | 可选 | 将标准 C 库 `printf()` / `_write()` 输出重定向至 `USART0`（`EVAL_COM0`，`PA9`/`PA10`）。 |
| `-DPIO_GD32H7_USBHS_DEVICE` | 可选 | 编译内置的 GD32H7xx USBHS 设备（Device）协议栈（支持 CDC、HID、MSC、IAP、Printer、Audio）。 |
| `-DPIO_GD32H7_USBHS_HOST` | 可选 | 编译内置的 GD32H7xx USBHS 主机（Host）协议栈（支持 CDC、HID、MSC）。 |
| `board_build.ldscript = gd32h759i_flash_3840k.ld` | `gd32h759i_flash.ld` | 将链接脚本从默认的 1024 KB Flash 布局切换至完整的 3840 KB 片内 Flash 布局。 |
| `upload_protocol = cmsis-dap` | **`cmsis-dap`** | 同时支持 `gd-link`（`cmsis-dap` 别名）、`jlink` 以及 `dfu`。 |

---

## 内存映射与链接脚本（Linker Scripts）

| 存储区域 | 起始地址 | 容量大小 | 链接脚本段名 | 使用说明 |
| :--- | :--- | :--- | :--- | :--- |
| **片内 Flash** | `0x08000000` | 1024 KB / 3840 KB | `.isr_vector`, `.text`, `.rodata` | 由 OpenOCD `gd32h7_flash_bin` 算法直接烧录 |
| **片内 AXI SRAM** | `0x24000000` | 512 KB | `.display_fb`, `.video_decode_buf`, `.data`, `.bss` | `.display_fb`（255 KB）按 64 字节对齐，确保 TLI 与 D-Cache 刷写安全 |
| **板载外部 SDRAM** | `0xC0000000` | 32 MB | `.sdram` (`NOLOAD`) | 使用前调用 `exmc_synchronous_dynamic_ram_init(EXMC_SDRAM_DEVICE0)` 初始化 |

将大容量缓冲区放置在 32 MB 外部 SDRAM 中的方法：
```c
__attribute__((section(".sdram"), aligned(32)))
uint8_t g_sdram_buffer[4 * 1024 * 1024];
```

---

## 内置示例工程

[`examples/`](examples/) 目录下提供了 3 个开箱即用的完整 PlatformIO 示例工程：

1. **[`examples/01-blinky-eval`](examples/01-blinky-eval)** — `LED1`/`LED2` 交替闪烁、Wakeup/Tamper 按键轮询、`USART0` 串口 `printf`、Cortex-M7 MPU 配置及 32 MB EXMC SDRAM 读写校验。
2. **[`examples/02-tli-lcd-touch`](examples/02-tli-lcd-touch)** — 480x272 RGB565 TLI 液晶屏初始化、屏幕滚动日志终端以及 GT911 五点电容触摸画板演示。
3. **[`examples/03-sdio-fatfs-audio`](examples/03-sdio-fatfs-audio)** — SDIO0 MicroSD 卡挂载与 FatFs 目录遍历、I2S1 DMA 音频输出初始化及 LCD 终端状态显示。

---

## 开源协议

本项目基于 [Apache-2.0 协议](LICENSE) 开源发布。内置的原厂 SDK 头文件与外设驱动版权分别归属于 GigaDevice Semiconductor Inc.（BSD-3-Clause）、ARM Limited（Apache-2.0）以及 ChaN（FatFs BSD 风格许可协议）。

