12-2-2020

This "project" is unusual because I really didn't add anything in the way
of hardware drivers.  I added some synchronization primitives, as follows;
all in event.c

First some basic concepts.  I am working towards and event driven, callback
oriented scheme.  A basic thought is that all spin loops are evil and must
be eradicated.  A second thought is that all event callbacks are done at
interrupt level.  This means they should be very short and simple.
The non-interrupt code is typically just a call to idle() where it sits
in an idle loop and callbacks do everything.

But if non-trivial code needs to run, it ought not to run in a callback
as part of an interrupt handler, so we need ways to synchronize between
interrupt events and "mainline code" (ARM documents would say code running
in "thread" rather than "handler" mode).  I will call mainline or "thread"
code "user" code, in spite of that being somewhat misleading.

idle/sleep - These are ways for user code to wait for interrupt events

block/unblock -- user code can call block, and then wait for handler code
to call unblock to set it running again.

delay - this allows user code to sleep for a certain number of milliseconds
without the usual spin counting rubbish.

event - this lets user code specify a function to be called (at interrupt level)
once at some number of milliseconds in the future.

repeat - this lets user code specify a function to be called (at interrupt level)
over and over at some interval in milliseconds.

Note that using repeat along with block/unblock allows things to be arranged
so that user code runs over and over at some interval.

Both repeats and events can be cancelled.
