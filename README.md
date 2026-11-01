# GigaDevice GD32H7 PlatformIO Development Platform (`platform-gd32h7`)

**Language Available:**
1. English - [Current](README.md)
2. Chinese (中文) - [Here](ZH_CN.md)

An open-source, self-contained **PlatformIO Development Platform** for the **GigaDevice GD32H7xx** series (**ARM Cortex-M7 @ 600 MHz**, double-precision FPU `fpv5-d16`, L1 I/D-Cache, 512 KB AXI SRAM, ITCM/DTCM), with complete out-of-the-box support, OpenOCD FMC flash programming, and 8 modular Board Support Package (BSP) libraries for the **GD32H759I-EVAL** evaluation board (`GD32H759IMK6`, BGA176).

---

## Table of Contents

1. [Why I Made It](#why-i-made-it)
2. [Key Features](#key-features)
3. [Installation & Quick Start (`platformio.ini`)](#installation--quick-start-platformioini)
4. [Linux USB / `udev` Setup for Onboard GD-Link V2](#linux-usb--udev-setup-for-onboard-gd-link-v2)
5. [GD32H759I-EVAL Hardware Jumper Reference](#gd32h759i-eval-hardware-jumper-reference)
6. [Built-In BSP Libraries & Code Snippets](#built-in-bsp-libraries--code-snippets)
   - [1. `GD32H759I_EVAL` (LEDs, Buttons, USART0, SysTick, 32 MB SDRAM)](#1-gd32h759i_eval-leds-buttons-usart0-systick-32-mb-sdram)
   - [2. `GD32H759I_LCD` (480x272 RGB565 TLI Display & Text Console)](#2-gd32h759i_lcd-480x272-rgb565-tli-display--text-console)
   - [3. `GD32H759I_Touch` (GT911 5-Point Capacitive Touch)](#3-gd32h759i_touch-gt911-5-point-capacitive-touch)
   - [4. `GD32H759I_SDIO_FatFs` (SDIO0 MicroSD + FatFs R0.15)](#4-gd32h759i_sdio_fatfs-sdio0-microsd--fatfs-r015)
   - [5. `GD32H759I_Audio` (SPI1/I2S1 DMA Double-Buffered Audio)](#5-gd32h759i_audio-spi1i2s1-dma-double-buffered-audio)
   - [6. `GD32H759I_OSPI_Flash` (GD25X512ME 64 MB Octal SPI NOR Flash)](#6-gd32h759i_ospi_flash-gd25x512me-64-mb-octal-spi-nor-flash)
   - [7. `GD32H759I_EEPROM` (AT24C02 I2C1 EEPROM)](#7-gd32h759i_eeprom-at24c02-i2c1-eeprom)
   - [8. `GD32H759I_Camera` (OV2640 DCI Camera)](#8-gd32h759i_camera-ov2640-dci-camera)
7. [Configuration Options & Build Flags](#configuration-options--build-flags)
8. [Memory Map & Linker Scripts](#memory-map--linker-scripts)
9. [Included Example Projects](#included-example-projects)
10. [License](#license)

---

## Why I Made It

I created `platform-gd32h7` during the **RT-Thread Embedded Software Competition**, where I chose the **GD32H759I-EVAL** board after its hardware design and appearance immediately caught my attention. While working with the board, I noticed that the open-source community had very little out of the box tooling support for this evaluation board, so I decided to build it and share it with the community.

Around the same time, our **BCA152** class introduced us to **PlatformIO** for ESP32 development. Seeing how clean, intuitive, and productive PlatformIO is inspired me to bring that same seamless PlatformIO workflow and one click build/upload integration to the **GD32H759I-EVAL** development board.

---

## Key Features

- **Zero External SDK Dependencies**: Bundles **CMSIS Core**, **GD32H7xx Device Support**, **GD32H7xx Standard Peripheral Library (SPL V1.4.0)**, and optional **GD32H7xx USBHS Device/Host Library**.
- **Built-In OpenOCD GD32H7xx FMC Flash Driver (`gd32h7_flash_bin`)**:
  - Works out of the box with standard upstream OpenOCD (`v0.11.0+`) and the onboard **GD-Link V2 (`0x28e9:0x0497`)** CMSIS-DAP probe, no proprietary GigaDevice OpenOCD fork required.
  - Unlocks the GD32H7xx `FMC` (`0x52002000`), erases 4 KB sectors, executes a 24-byte Cortex-M7 Thumb-2 RAM loader in AXI SRAM (`0x24000000`) to program flash at `0x08000000` with `DSB` synchronization, verifies the image via `verify_image`, locks `FMC`, and resets the target.
- **Complete GCC Startup & Linker Scripts**:
  - Full 217-IRQ vector table (`g_pfnVectors`) in `startup_gd32h7xx.c` with weak ISR aliases and Newlib syscall stubs (`_sbrk`, `_write`, etc.).
  - Optional `printf` retargeting to `USART0` (`PA9`/`PA10`) via `-DPIO_GD32H7_PRINTF_USART0`.
  - Linker scripts (`gd32h759i_flash.ld` and `gd32h759i_flash_3840k.ld`) with dedicated `NOLOAD` sections for 64-byte aligned D-Cache safe framebuffers (`.display_fb`) in AXI SRAM (`0x24000000`) and 32 MB external SDRAM (`.sdram`) at `0xC0000000`.
- **Automatic Host Toolchain Detection**: Automatically uses the system `arm-none-eabi-gcc` if installed on `PATH`, or falls back to PlatformIO's managed `toolchain-gccarmnoneeabi`.

---

## Installation & Quick Start (`platformio.ini`)

### Option 1: PlatformIO Registry
Published on the PlatformIO Registry at **[https://registry.platformio.org/platforms/jian6137/gd32h7](https://registry.platformio.org/platforms/jian6137/gd32h7)** *(Note: As of November 1, 2026, the package is pending review on the PlatformIO Registry; you can use Option 2 below while review is in progress).*

To install globally (so **GigaDevice GD32H759I-EVAL** is selectable in the **PIO Home -> New Project** dropdown):
```bash
pio pkg install --global --platform jian6137/gd32h7
```

Or reference it directly in your project's `platformio.ini`:
```ini
[env:gd32h759i_eval]
platform = jian6137/gd32h7
board = gd32h759i_eval
framework = gd32h7xx_spl
monitor_speed = 115200
build_flags =
    -DPIO_GD32H7_PRINTF_USART0
```

### Option 2: Direct GitHub Repository URL
Reference the GitHub repository URL directly in your project's `platformio.ini`—PlatformIO will download and configure the platform automatically:

```ini
[env:gd32h759i_eval]
platform = https://github.com/jian6137/platform-gd32h7.git
board = gd32h759i_eval
framework = gd32h7xx_spl
monitor_speed = 115200
build_flags =
    -DPIO_GD32H7_PRINTF_USART0
```

To install globally from GitHub for the **PIO Home -> New Project** wizard:
```bash
pio pkg install --global --platform https://github.com/jian6137/platform-gd32h7.git
```

### Option 3: Local Clone / Symlink
Clone the repository locally and link it via `symlink://`:

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

## Linux USB / `udev` Setup for Onboard GD-Link V2

The **GD32H759I-EVAL** includes an onboard **GD-Link V2** SWD debugger (`USB VID:PID 28e9:0497`, `GDMicroelectronics GDLinker_V2`). On Linux, install the included `udev` rule once so OpenOCD can access the probe without `sudo`:

```bash
sudo cp misc/udev/99-gdlink.rules /etc/udev/rules.d/99-gdlink.rules
sudo udevadm control --reload-rules && sudo udevadm trigger
```

---

## GD32H759I-EVAL Hardware Jumper Reference

Many pins on the `GD32H759IMK6` are multiplexed across onboard peripherals via physical jumpers. Check this table when enabling peripherals:

| Peripheral | MCU Pin(s) | Required Jumper Settings on GD32H759I-EVAL | Conflict / Notes |
| :--- | :--- | :--- | :--- |
| **`LED1`** | `PF10` | `JP50` -> `LED` | **Shared with `LCD_DE` (`JP50` -> `LCD`) and OSPI (`JP50` -> `OSPI`).** When using the RGB LCD, keep `JP50` on `LCD` and use `LED2` as your status LED. |
| **`LED2`** | `PA6` | `JP66` -> `LED` | Safe to use alongside the RGB LCD. |
| **`USART0` (`EVAL_COM0`)** | `PA9` (TX), `PA10` (RX) | `JP2` -> `USART0` | 115200 8N1 serial console (`-DPIO_GD32H7_PRINTF_USART0`). |
| **TLI 480x272 RGB LCD** | RGB565 + `PG13` (BL) | `JP41`–`JP59` -> `LCD` | `PG13` controls the LCD backlight (`gd32h759i_lcd_backlight_set(1/0)`). |
| **GT911 Capacitive Touch** | `PH7` (SCL), `PF9` (SDA), `PF6` (RST), `PH6` (INT) | `JP13` -> `I2C` | 5-point capacitive touch over software I2C (`0xBA`). |
| **SDIO0 MicroSD Card** | `PD2` (CMD), `PC12` (CLK), `PB13` (`DAT0`) / `PC8..11` | `JP4` -> `SDCard`; `JP13`, `JP19`, `JP23` -> `SDIO` | Supports 1-bit and 4-bit SDIO modes with FatFs R0.15. |
| **SPI1/I2S1 Audio DAC** | `PA2`, `PB12`, `PD3`, `PC1` | `JP22`, `JP41`, `JP42`, `JP43` -> `I2S`; `P2`/`P3` -> `P2-2`/`P3-3` | Driven by fractional `PLL1` audio clock + DMA0 Ch0. |
| **OSPI Flash (`GD25X512ME`)** | `PF6..10`, `PG6`, `PB2` | `JP33`..`JP38`, `JP42`, `JP43`, `JP49`, `JP50` -> `OSPI` | 64 MB Octal SPI NOR Flash. |

---

## Built-In BSP Libraries & Code Snippets

All 8 BSP libraries live in [`libraries/`](libraries/) and are **automatically discovered** by PlatformIO's Library Dependency Finder (LDF) as soon as you `#include` their header in your source files—no `lib_deps` configuration required.

| Library Name | Header(s) to `#include` | Summary |
| :--- | :--- | :--- |
| **`GD32H759I_EVAL`** | `gd32h759i_eval.h`, `systick.h`, `exmc_sdram.h` | Core board support: `LED1`/`LED2`, Wakeup/Tamper/User keys, `USART0`, 1 ms `SysTick` (`delay_1ms()`), and 32 MB EXMC SDRAM (`exmc_sdram_config()`). |
| **`GD32H759I_LCD`** | `gd32h759i_lcd.h`, `lcd_console.h` | 480x272 RGB565 TLI TFT LCD driver with D-Cache flush (`gd32h759i_lcd_flush()`) and 8x8 font scrolling console (`lcd_console_printf()`). |
| **`GD32H759I_Touch`** | `bsp_ts_gt911.h`, `bsp_i2c_touch.h` | Goodix GT911 5-point capacitive touch driver (`gt911_init()`, `gt911_handler()`). |
| **`GD32H759I_SDIO_FatFs`** | `sdcard.h`, `ff.h`, `diskio.h` | SDIO0 MicroSD driver + ChaN FatFs R0.15 (`f_mount`, `f_open`, `f_read`, `f_write`, `f_opendir`). |
| **`GD32H759I_Audio`** | `audio_player.h` | SPI1/I2S1 DMA double-buffered audio output driver (`audio_player_init()`, `audio_player_start()`, `audio_player_get_BackBuffer()`). |
| **`GD32H759I_OSPI_Flash`** | `gd25x512me.h` | Octal SPI (OSPI) NOR Flash driver for onboard `GD25X512ME` (64 MB). |
| **`GD32H759I_EEPROM`** | `at24cxx.h`, `i2c.h` | Hardware `I2C1` driver for onboard `AT24C02` EEPROM. |
| **`GD32H759I_Camera`** | `dci_ov2640.h`, `sccb.h` | Digital Camera Interface (DCI) + SCCB (`I2C1`) driver for `OV2640` camera module. |

### 1. `GD32H759I_EVAL` (LEDs, Buttons, USART0, SysTick, 32 MB SDRAM)

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

    /* LEDs (JP50->LED for LED1, JP66->LED for LED2) & USART0 (115200 8N1) */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_com_init(EVAL_COM0);

    /* Optional: Initialize 32 MB external SDRAM at 0xC0000000 */
    exmc_synchronous_dynamic_ram_init(EXMC_SDRAM_DEVICE0);

    printf("SystemCoreClock: %lu Hz\r\n", (unsigned long)SystemCoreClock);

    while (1) {
        gd_eval_led_toggle(LED1);
        gd_eval_led_toggle(LED2);
        delay_1ms(500);
    }
}
```

### 2. `GD32H759I_LCD` (480x272 RGB565 TLI Display & Text Console)

```c
#include "gd32h759i_lcd.h"
#include "lcd_console.h"

void lcd_demo(void)
{
    gd32h759i_lcd_init();      /* Configures TLI pins, PLL2 pixel clock, LAYER0, and PG13 backlight */
    lcd_console_init();        /* Clears 480x272 framebuffer and initializes 8x8 text console */

    lcd_console_printf("GD32H759I-EVAL Display Ready (%ux%u)\n", LCD_WIDTH, LCD_HEIGHT);
    gd32h759i_lcd_fill_rect(400, 16, 48, 48, LCD_COLOR_GREEN);
}
```

### 3. `GD32H759I_Touch` (GT911 5-Point Capacitive Touch)

```c
#include "bsp_ts_gt911.h"

void touch_poll(void)
{
    if (gt911_handler() == SUCCESS && (gt911.pressed_info & 0x0F)) {
        uint16_t x = gt911.x[0];
        uint16_t y = gt911.y[0];
        /* Handle touch at (x, y) */
    }
}
```

### 4. `GD32H759I_SDIO_FatFs` (SDIO0 MicroSD + FatFs R0.15)

```c
#include "ff.h"

static FATFS g_fs;

void sd_mount_demo(void)
{
    if (f_mount(&g_fs, "0:", 1) == FR_OK) {
        FIL file;
        if (f_open(&file, "0:/test.txt", FA_READ) == FR_OK) {
            /* Read data with f_read(&file, ...) */
            f_close(&file);
        }
    }
}
```

### 5. `GD32H759I_Audio` (SPI1/I2S1 DMA Double-Buffered Audio)

```c
#include "audio_player.h"

void audio_demo(void)
{
    audio_player_init(44100U); /* Configure PLL1 + SPI1/I2S1 + DMA0 double buffer */
    audio_player_start();

    while (1) {
        int16_t *pcm_buf = audio_player_get_BackBuffer();
        if (pcm_buf != NULL) {
            /* Fill AUDIO_BUFFER_SAMPLES 16-bit stereo samples into pcm_buf */
            audio_player_submit_BackBuffer();
        }
    }
}
```

---

## Configuration Options & Build Flags

| Build Flag / `platformio.ini` Setting | Default | Description |
| :--- | :--- | :--- |
| `-D__SYSTEM_CLOCK_600M_PLL0_HXTAL=600000000` | **Enabled** | Runs Cortex-M7 at **600 MHz** from onboard 25 MHz crystal (`HXTAL`). |
| `-D__SYSTEM_CLOCK_600M_PLL0_IRC64M=600000000` | Disabled | Runs Cortex-M7 at **600 MHz** from internal 64 MHz RC oscillator (`IRC64M`). |
| `-DPIO_GD32H7_PRINTF_USART0` | Optional | Routes standard C `printf()` / `_write()` to `USART0` (`EVAL_COM0`, `PA9`/`PA10`). |
| `-DPIO_GD32H7_USBHS_DEVICE` | Optional | Compiles the bundled GD32H7xx USBHS Device stack (CDC, HID, MSC, IAP, Printer, Audio). |
| `-DPIO_GD32H7_USBHS_HOST` | Optional | Compiles the bundled GD32H7xx USBHS Host stack (CDC, HID, MSC). |
| `board_build.ldscript = gd32h759i_flash_3840k.ld` | `gd32h759i_flash.ld` | Switches linker script from 1024 KB Flash layout to the full 3840 KB Flash layout. |
| `upload_protocol = cmsis-dap` | **`cmsis-dap`** | Also supports `gd-link` (alias for `cmsis-dap`), `jlink`, and `dfu`. |

---

## Memory Map & Linker Scripts

| Region | Start Address | Size | Linker Section | Usage Notes |
| :--- | :--- | :--- | :--- | :--- |
| **Internal Flash** | `0x08000000` | 1024 KB / 3840 KB | `.isr_vector`, `.text`, `.rodata` | Programmed via OpenOCD `gd32h7_flash_bin` |
| **AXI SRAM** | `0x24000000` | 512 KB | `.display_fb`, `.video_decode_buf`, `.data`, `.bss` | `.display_fb` (255 KB) is 64-byte aligned for TLI + D-Cache maintenance |
| **External SDRAM** | `0xC0000000` | 32 MB | `.sdram` (`NOLOAD`) | Initialize via `exmc_synchronous_dynamic_ram_init(EXMC_SDRAM_DEVICE0)` |

To place a large buffer in external 32 MB SDRAM:
```c
__attribute__((section(".sdram"), aligned(32)))
uint8_t g_sdram_buffer[4 * 1024 * 1024];
```

---

## Included Example Projects

All examples in [`examples/`](examples/) are standalone PlatformIO projects:

1. **[`examples/01-blinky-eval`](examples/01-blinky-eval)** — `LED1`/`LED2` blinky, Wakeup/Tamper button polling, `USART0` `printf`, Cortex-M7 MPU setup, and 32 MB EXMC SDRAM read/write verification.
2. **[`examples/02-tli-lcd-touch`](examples/02-tli-lcd-touch)** — 480x272 RGB565 TLI LCD initialization, on-screen scrolling text console, and GT911 5-point capacitive touch drawing.
3. **[`examples/03-sdio-fatfs-audio`](examples/03-sdio-fatfs-audio)** — SDIO0 MicroSD + FatFs directory listing, I2S1 DMA audio initialization, and LCD console status display.

---

## License

Released under the [Apache-2.0 License](LICENSE). Vendor SDK headers and peripheral drivers are copyright GigaDevice Semiconductor Inc. (BSD-3-Clause), ARM Limited (Apache-2.0), and ChaN (FatFs BSD-style license).
