/* blink.lds
 * linker script for blink demo.
 *
 * Memory layout on my STM32F411 is pretty much like this:
 *
 * 0x00000000 - 0x07ffffff - aliased to flash or sys memory depending on BOOT jumpers
 * 0x08000000 - 0x0807ffff - Flash (512K)
 * 0x1fff0000 - 0x1fff07ff - Boot firmware in system memory
 * 0x20000000 - 0x2001ffff - SRAM (128k)
 * 0x40000000 - 0x40023400 - peripherals
 */
MEMORY
{
   flash(RX)  : ORIGIN = 0x08000000, LENGTH = 512K
   sram(WAIL) : ORIGIN = 0x20000000, LENGTH = 128K
}

SECTIONS
{
   .text : {
       *(.vectors*)
       *(.text*)
   } > flash
   .bss  : { *(.bss*) } > sram
}
