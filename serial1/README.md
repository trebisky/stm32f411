This is my serial demo for the STM32F411

I discovered that the UART register layout and bits
are essentially the same as the STM32F103, so I just
copied my driver from my STM32F103 project and fiddled
with it.  The base addresses for the registers changed.

There will be other details. The bus clocks are likely
(surely) different, so the baud rate code will need to
change accordingly.  Also the gpio pin setup to work
with the serial mode will have to be done as per
the F411.

I began splitting things into separate files for each
bit of hardware, which makes it clearer for me anyway
to be dealing with just one aspect of things in each
source file.
