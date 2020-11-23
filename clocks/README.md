This is my clocks demo for the STM32F411

It is really a copy of my serial1 demo,
but I add code to rcc.c to get the CPU
running at 100 Mhz.

This chip comes up using the HSI clock which
runs at 16 Mhz and uses an internal RC circuit.

The first step is to switch to using the HSE
clock, which uses an external 25 Mhz crystal
(on my board at least).

Then the game is to use the PLL to kick the
25 Mhz up to 100 Mhz to run the cpu.
Actually you must use 96 Mhz if you want a
proper 48 Mhz clock for USB.

I pulled my hair out for several hours.
When I switched to the PLL for 96 Mhz, the processor
would just hang.  Then I took a break and went for
a bike ride and got the idea.  Maybe 96 Mhz is too
fast for the flash memory without some configuring.
So I tried just using the PLL to generate 16 Mhz and
that worked fine, confirming my idea.
