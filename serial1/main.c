/* main.c
 * (c) Tom Trebisky  9-24-2016
 * (c) Tom Trebisky  11-20-2020
 *
 * Basic serial IO demo.
 *
 * This began as the F411 blink demo and was modifed
 */

/* Someday we will write a proper delay routine and dump this */

/* On the STM32F103, this gives a blink rate of about 2.7 Hz */
/* i.e. the delay time is about 0.2 seconds (200 ms) */
#define FAST	200

#define FASTER	50
#define SLOWER	800
#define EVEN_SLOWER	1600

void
blink_delay ( void )
{
	volatile int count = 1000 * FAST;
	// volatile unsigned int count = 1000 * EVEN_SLOWER;

	while ( count-- )
	    ;
}

static void
test1 ( void )
{
	for ( ;; ) {
	    led_on ();
	    console_putc ( 'X' );
	    blink_delay ();
	    led_off ();
	    console_putc ( '-' );
	    blink_delay ();
	}
}

static void
test2 ( void )
{
	for ( ;; ) {
	    console_putc ( 'X' );
	}
}

void
startup ( void )
{
	rcc_init ();
	serial_init ();

	led_init ();

	test1 ();
	// test2 ();
}

/* THE END */
