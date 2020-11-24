11-24-2020

This builds on my serial2 demo.

The game here is to get some things working
 with interrupts, in particular:

Systick running at 1000 Hz
Serial read interrupts.

My idea is for both of the above to allow "hooks",
which would lead to an event driven "callback" way
of programming.
