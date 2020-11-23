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
blink_delay ( int rate )
{
	// volatile int count = 1000 * FAST;
	// volatile unsigned int count = 1000 * EVEN_SLOWER;
	volatile unsigned int count = 1000 * rate;

	while ( count-- )
	    ;
}

static void
test_blink ( void )
{
	for ( ;; ) {
	    led_on ();
	    blink_delay ( EVEN_SLOWER );
	    led_off ();
	    blink_delay ( EVEN_SLOWER );
	}
}

static void
test2 ( void )
{
	for ( ;; ) {
	    blink_delay ( FAST );
	    console_putc ( '8' );
	}
}

static void
test3 ( void )
{
	for ( ;; ) {
	    blink_delay ( EVEN_SLOWER );
	    blink_delay ( EVEN_SLOWER );
	    console_puts ( "Keep a stiff upper lip\n" );
	}
}

static void
test4 ( void )
{
	int xx;
	for ( xx = 1; ; xx++ ) {
	    blink_delay ( EVEN_SLOWER );
	    blink_delay ( EVEN_SLOWER );
	    show_n ( "Testing", xx );
	}
}

/* Can be called from within rcc_init */
void
blinker ( void )
{
	led_init ();
	test_blink ();
}

void
startup ( void )
{
	rcc_init ();
	serial_init ();

	led_init ();
	led_off ();

	// test_blink ();
	// test2 ();
	// test3 ();
	// test4 ();

	console_puts ( "\n" );
	console_puts ( "Up and running with default clock\n" );

	rcc_debug ();

	console_puts ( "Up and running with clock reconfigured\n" );
	console_puts ( "Hello World\n" );

	test_blink ();

	led_on ();
	for ( ;; ) ;
}

/* THE END */
