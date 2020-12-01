/* event.c
 * Tom Trebisky 12-1-2020
 *
 * Code to handle "after" and "repeat"
 *
 * Taken from Kyu.
 */

#include "f411.h"

struct event {
	struct event *next;
	int delay;
	int rep_count;
	int rep_reload;
	int id;
	vfptr func;
};

/* ========================================================= */

/* This really belongs elsewhere.
 * This is the simplest possible memory allocator,
 * with no facility to free memory already allocated.
 * We maintain 4 byte alignment.
 */

extern char __end;

static unsigned int ram_end;

void *
ram_alloc ( int size )
{
	ram_end -= size;
	ram_end &= 0xfffffffc;
	return (void *) ram_end;
}

void
ram_init ( void )
{
	struct event *ep;
	/* I see:
	    END: 20000060
	    SP: 2001FFD8
	*/
	printf ( "END: %X\n", &__end );
	printf ( "SP: %X\n", get_sp() );

	ram_end = (unsigned int) &__end;
	ram_end &= 0xfffffffc;
	printf ( "ram end: %X\n", ram_end );
	printf ( "alloc: %d bytes\n", sizeof(struct event) );
	ep = (struct event *) ram_alloc ( sizeof(struct event) );
	printf ( "ep: %X\n", ep );
	printf ( "ram end: %X\n", ram_end );
}


/* ========================================================= */

static volatile int delay_counts = 0;

/* Holds lists of waits and repeats waiting
 * on timer events.
 */
static struct event *event_freelist = 0;

static struct event *wait_head = 0;
static struct event *repeat_head = 0;
static unsigned int event_id = 1;

static struct event *
event_alloc ( void )
{
        struct event *ep;

	if ( event_freelist ) {
	    ep = event_freelist;
	    event_freelist = ep->next;
	} else {
	    ep = (struct event *) ram_alloc ( sizeof(struct event) );
	}
	ep->id = event_id++;
}

static void
event_free ( struct event *ep )
{
    ep->next = event_freelist;
    event_freelist = ep;
}

/* Expects to be called with
 * interrupts locked.
 * (or by interrupt code).
 */
static void
remove_wait ( struct event *ep )
{
        struct event *lp;

        if ( wait_head == ep ) {
            wait_head = ep->next;
            if ( wait_head )
                wait_head->delay += ep->delay;
        } else {
            for ( lp = wait_head; lp; lp = lp->next )
                if ( lp == ep ) {
                    lp = ep->next;
                    if ( lp )
                        lp->delay += ep->delay;
                }
        }

	event_free ( ep );
}

/* Called once for every timer interrupt.
 * (i.e. from systick at 1000 Hz)
 * Runs at interrupt level.
 * Handles delays and repeats.
 */
void
event_tick ( void )
{
        struct event *ep;

	/* Handle "delay" */
	if ( delay_counts )
	    --delay_counts;

        /* Process events */
        if ( wait_head ) {
            --wait_head->delay;
            while ( wait_head && wait_head->delay == 0 ) {
                ep = wait_head;
                wait_head = wait_head->next;
		(*ep->func) ();
		remove_wait ( ep );
            }
        }

        /* Process repeats.
         *  repeats are not like waits where we only have to fool
         *  with the head of the list, we loop through all of them
         *  and keep them in no particular order.
         */
        for ( ep=repeat_head; ep; ep = ep->next ) {
            --ep->rep_count;
            if ( ep->rep_count < 1 ) {
                ep->rep_count = ep->rep_reload;
		(*ep->func) ();
            }
        }
}

/* maintain a linked list of folks waiting on
 * timer delay activations.
 * In time-honored fashion, the list is kept in
 * sorted order, with the soon to be scheduled
 * entries at the front.  Each tick then just
 * needs to decrement the leading entry, and
 * when it becomes zero, one or more entries
 * get launched.
 *
 * Only called (now) from timer_add_wait()
 */
static void
setup_after ( struct event *ep, int delay )
{
        struct event *p, *lp;

        p = wait_head;

        while ( p && p->delay <= delay ) {
            delay -= p->delay;
            lp = p;
            p = p->next;
        }

        if ( p )
            p->delay -= delay;

        ep->delay = delay;
        ep->next = p;

        if ( p == wait_head )
            wait_head = ep;
        else
            lp->next = ep;
}

/* Public */
void
after ( int delay, vfptr fn )
{
	struct event *ep;

	ep = event_alloc ();
	ep->func = fn;

	disable_irq;
	setup_after ( ep, delay );
	enable_irq;
}

/* Public */
void
repeat ( int delay, vfptr fn )
{
	struct event *ep;

	ep = event_alloc ();
	ep->func = fn;

        ep->rep_reload = delay;
        ep->rep_count = delay;

        /* add to front of list */
	disable_irq;
        ep->next = repeat_head;
        repeat_head = ep;
	enable_irq;
}

/* Rarely called, if ever.
 * There is less of an issue with a race since
 * interrupt code never removes or cancels the repeat.
 */
void
repeat_cancel ( int id )
{
        struct event *ep;
        struct event *cp;
        struct event *pp;

        for ( ep=repeat_head; ep; ep = ep->next ) {
	    if ( ep->id == id )
		break;
	}

	if ( ! ep )
	    return;

	disable_irq;

        if ( repeat_head == ep ) {
            repeat_head = ep->next;
        } else {
	    pp = repeat_head;
	    cp = pp->next;
	    while ( cp ) {
		if ( cp == ep )
		    pp->next = cp->next;
		else
		    pp = cp;
		cp = pp->next;
	    }
	}

	enable_irq;

	event_free ( ep );
}

/* This gets called from thr_unblock() when we
 * unblock a thread with a running delay timer.
 * could be called from interrupt code.
 * We lock interrupts to avoid a race with the
 * interrupt code, which could also decide to
 * remove the wait.
 */
void
after_cancel ( int id )
{
        struct event *ep;

	disable_irq;

        for ( ep=repeat_head; ep; ep = ep->next ) {
	    if ( ep->id == id )
		break;
	}

	if ( ep )
	    remove_wait ( ep );

	enable_irq;
}

/* ======================================================================= */

/* These are sort of related to the above in that they
 * involve synchronization between interrupt code and
 * non-interrupt code.
 */

/* The idea here is that this is an "idle loop", i.e. a place
 * for the processor to sit and wait for interrupts.
 * This was originally just a hard spin loop.
 * Now I use "wfe" - there is also "wfi" and people talk
 * about masking interrupts before launching one of these,
 * which sounds like exactly the wrong thing to do,
 * but if you let the processor come out of wfe and then
 * immediately unmask interrupts, maybe that is the thing to
 * do and will let the processor enter a deeper sleep mode.
 * It certainly does work.
 */
void
idle ( void )
{
	for ( ;; ) {
	    disable_irq;
	    asm volatile( "wfe" );
	    enable_irq;
	}
}

/* This is the guts of the above that you can wrap in your
 * own loop.  Note that it will wake up on EVERY interrupt,
 * and given that systick is running at 1000 Hz, you will get
 * at least that.
 */
void
sleep ( void )
{
	disable_irq;
	asm volatile( "wfe" );
	enable_irq;
}

/* User code can call this to sleep until a certain number
 * of systicks elapse, i.e a delay for so many milliseconds.
 */
void
delay ( int counts )
{
	disable_irq;
	delay_counts = counts;
	enable_irq;

	for ( ;; ) {
	    sleep ();
	    if ( ! delay_counts )
		break;
	}
}

static volatile char block_flag = 0;

/* These really have nothing to do with systick, but are here
 * (temporarily) because they are in essence a generalization
 * of the delay scheme above.  We have user code and a variety
 * of interrupt callback code.  This is a facility for user code
 * to wait until signaled by interrupt code.
 */
void
block ( void )
{
	disable_irq;
	block_flag = 1;
	enable_irq;

	for ( ;; ) {
	    sleep ();
	    if ( ! block_flag )
		break;
	}
}

void
unblock ( void )
{
	block_flag = 0;
}


/* THE END */
