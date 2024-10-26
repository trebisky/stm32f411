STM32F411

This is a collection of things I am doing pertaining to the STM32F411
microcontroller that is found on so called "black pill" boards from
China.  I received my first of these on 11-20-2020 and away we go.

Compared to the F103 on the blue pill:

* 512K flash (instead of 128K or 64K)
* 128K ram (instead of 20K)
* They cost $4 (instead of $2)

For many things, the F103 is entirely adequate, but these give
a nice option when you need more.

Many of these examples are based on my STM32F103 projects on the
so called "blue pill" boards.  You can find these projects here:

https://github.com/trebisky/stm32f103

Also see notes on my web page here:

http://cholla.mmto.org/stm32all/black_pill

Also, David Welch has done a lot of work on these chips and boards
as well as other things in the STM32 world

https://github.com/dwelch67

The following is an index to what you find here.
I recommend taking things in this order if your aim is to learn
because each "example" builds on the one before it.

On the other hand, if you actually want to use this code for
something, the last in the list is the best developed.
I am slowly building to something, I'm not sure what.

By the time you get to "button" this has all evolved into
what is a good start on a useful "framework".

* ST-Link - initial experiments with openocd and my ST-Link adapter
* blink1 - the most basic LED blinking demo I can manage
* serial1 - basic serial IO (uart) without interrupts
* clocks - get the clock running at 25 or 96 Mhz, not 16
* serial2 - test all 3 uarts, add nice printf scheme
* inter1 - first interrupts (systick and serial read)
* bss - add code to clear bss and initialize variables
* button - get interrupts from the "user" button
* delay - delay via systick, also sleep, event, repeat, block
