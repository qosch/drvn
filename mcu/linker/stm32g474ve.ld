/*
 * Linker script for STM32G474VE (512 KB flash, 128 KB SRAM)
 *
 * Memory layout: DS12288 Rev 6, section 3.5 "Embedded SRAM"
 *                RM0440, section 3.3.1 "Flash memory organization"
 *
 * Deliberately minimal: no heap, no code in CCM SRAM, no parameter area,
 * no .noinit. Add these when the features that need them are implemented.
 *
 * Interface for the startup code:
 *   .isr_vector                          section holding the vector table
 *   Reset_Handler                        entry point
 *   __stack_top                          initial stack pointer (vector table entry 0)
 *   __data_load                          load address of .data in flash
 *   __data_start, __data_end             copy .data from __data_load to here
 *   __bss_start, __bss_end               fill with zeros
 *   __init_array_start, __init_array_end call every function pointer in between
 */

ENTRY(Reset_Handler)

/* Main stack size. Override from the build with -Wl,--defsym=__stack_size=8K */
PROVIDE(__stack_size = 4K);

MEMORY
{
  /* Both banks. In dual-bank mode, bank 2 starts at 0x08040000. */
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 512K

  /* SRAM1 (80 KB at 0x20000000) and SRAM2 (16 KB at 0x20014000) are contiguous.
   * CCM SRAM (32 KB) is aliased right behind them at 0x20018000, but is left out
   * on purpose: it is reserved for time-critical code later. */
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 96K
}

/* Full-descending stack starting at the top of RAM */
__stack_top = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
  /* The core loads SP and the reset vector from the start of flash.
   * READONLY (binutils >= 2.38) keeps flash in a read-only segment even if
   * an input section is flagged writable. */
  .isr_vector (READONLY) :
  {
    KEEP(*(.isr_vector))
  } > FLASH

  .text :
  {
    . = ALIGN(4);
    *(.text .text.*)
    *(.rodata .rodata.*)
    . = ALIGN(4);
  } > FLASH

  /* Unwind tables. Own code is built with -fno-exceptions, but prebuilt
   * library objects (libgcc) may still contain entries. */
  .ARM.extab :
  {
    *(.ARM.extab .ARM.extab.*)
  } > FLASH

  .ARM.exidx :
  {
    *(.ARM.exidx .ARM.exidx.*)
  } > FLASH

  /* Constructors of static objects, called by the startup code before main().
   * .preinit_array first, then .init_array sorted by priority.
   * The compiler flags these sections writable; READONLY avoids the linker
   * warning about a LOAD segment with RWX permissions. */
  .init_array (READONLY) :
  {
    . = ALIGN(4);
    __init_array_start = .;
    KEEP(*(.preinit_array))
    KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)))
    KEEP(*(.init_array))
    __init_array_end = .;
  } > FLASH

  /* Initialized data: stored in flash, copied to RAM by the startup code */
  .data : ALIGN(4)
  {
    __data_start = .;
    *(.data .data.*)
    . = ALIGN(4);
    __data_end = .;
  } > RAM AT > FLASH

  __data_load = LOADADDR(.data);

  /* Zero-initialized data: cleared by the startup code */
  .bss (NOLOAD) : ALIGN(4)
  {
    __bss_start = .;
    *(.bss .bss.*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end = .;
  } > RAM

  /* The stack grows down from __stack_top. Only check that it fits. */
  ASSERT(__stack_top - __bss_end >= __stack_size,
         "Not enough RAM left for the main stack")
}
