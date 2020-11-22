This is my blink1 demo for the STM32F103 ported to the
STM32F411.  The idea is to discover what I need to tweak
to get the code to run on the F411.

It turns out quite a lot is different.
The base addresses for the RCC and GPIO are different.
The RCC register layout is quite different.
The GPIO registers are entirely different.

I don't need to set up bus clocks or any of that.
