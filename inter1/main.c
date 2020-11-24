/* main.c
 * (c) Tom Trebisky  9-24-2016
 * (c) Tom Trebisky  11-20-2020
 *
 * Basic serial IO demo.
 *
 * This began as the F411 blink demo and was modifed
 */

#include "f411.h"

/* Someday we will write a proper delay routine and dump this */

#define FAST	200
#define FASTER	50
#define SLOWER	800
#define EVEN_SLOWER	1600

void
blink_delay ( int rate )
{
	volatile unsigned int count = 10000 * rate;

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
	    putc ( '8' );
	}
}

static void
test3 ( void )
{
	for ( ;; ) {
	    blink_delay ( EVEN_SLOWER );
	    puts ( "Keep a stiff upper lip\n" );
	}
}

static void
test4 ( void )
{
	int xx;

	for ( xx = 1; ; xx++ ) {
	    blink_delay ( EVEN_SLOWER );
	    printf ( "Testing %d\n", xx );
	}
}

static void
echo_test ( void )
{
	int c;

	for ( ;; ) {
	    c = getc ();
	    printf ( "Got: %d 0x%x ", c, c );
	    if ( c >= ' ' )
		printf ( "%c", c );
	    printf ( "\n" );
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
	int fd;

	rcc_init ();
	systick_init ();

	fd = serial_begin ( UART1, 115200 );
	// fd = serial_begin ( UART2, 115200 );

	set_std_serial ( fd );

	printf ( "CPU running at %d Hz\n", get_cpu_hz() );
	printf ( "Console on UART%d\n", fd+1 );

	led_init ();
	led_off ();

	// test_blink ();
	// test2 ();
	// test3 ();
	// test4 ();

	puts ( "\n" );
	puts ( "Up and running\n" );

	// test_blink ();
	// test4 ();
	echo_test ();

	led_on ();
	for ( ;; ) ;
}

/* THE END */
