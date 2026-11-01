/*
 * GCC Linker Script for GigaDevice GD32H759IMK6 / GD32H759I-EVAL
 *
 * Memory Map:
 *   FLASH : 0x08000000 (2048K default safe boundary; up to 3840K physical on GD32H759IMK6)
 *   RAM   : 0x24000000 (512K AXI SRAM; total on-chip SRAM is 1024K with configurable TCM/AXI)
 *   SDRAM : 0xC0000000 (32MB External SDRAM on EXMC Device 0)
 */

ENTRY(Reset_Handler)

_estack = ORIGIN(RAM) + LENGTH(RAM);

_Min_Heap_Size = 0x2000;   /* 8 KB minimum heap check */
_Min_Stack_Size = 0x4000;  /* 16 KB minimum stack check */

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 2048K
    RAM   (xrw) : ORIGIN = 0x24000000, LENGTH = 512K
    SDRAM (xrw) : ORIGIN = 0xC0000000, LENGTH = 32768K
}

SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.glue_7)
        *(.glue_7t)
        *(.eh_frame)

        KEEP(*(.init))
        KEEP(*(.fini))

        . = ALIGN(4);
        _etext = .;
    } > FLASH

    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } > FLASH

    .ARM.extab :
    {
        . = ALIGN(4);
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        . = ALIGN(4);
    } > FLASH

    .ARM :
    {
        . = ALIGN(4);
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
        . = ALIGN(4);
    } > FLASH

    .preinit_array :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP(*(.preinit_array*))
        PROVIDE_HIDDEN(__preinit_array_end = .);
        . = ALIGN(4);
    } > FLASH

    .init_array :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array*))
        PROVIDE_HIDDEN(__init_array_end = .);
        . = ALIGN(4);
    } > FLASH

    .fini_array :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array*))
        PROVIDE_HIDDEN(__fini_array_end = .);
        . = ALIGN(4);
    } > FLASH

    _sidata = LOADADDR(.data);

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        *(.RamFunc)
        *(.RamFunc*)
        . = ALIGN(4);
        _edata = .;
    } > RAM AT> FLASH

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = _ebss;
    } > RAM

    /* Dedicated 64-byte cacheline-aligned AXI SRAM sections for TLI LCD & Media buffers */
    .display_fb (NOLOAD) :
    {
        . = ALIGN(64);
        *(.display_fb)
        *(.display_fb*)
        . = ALIGN(64);
    } > RAM

    .video_decode_buf (NOLOAD) :
    {
        . = ALIGN(64);
        *(.video_decode_buf)
        *(.video_decode_buf*)
        . = ALIGN(64);
    } > RAM

    . = ALIGN(8);
    _end = .;
    PROVIDE(end = .);
    PROVIDE(_end = .);

    /* External 32MB SDRAM section (NOLOAD) */
    .sdram (NOLOAD) :
    {
        . = ALIGN(64);
        _ssdram = .;
        *(.sdram)
        *(.sdram*)
        . = ALIGN(64);
        _esdram = .;
    } > SDRAM

    /DISCARD/ :
    {
        *(.note.GNU-stack)
        *(.comment)
    }
}

