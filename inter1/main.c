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

/* ================================================= */
/* Working 11-24-2020 */

/* Called at interrupt level */
void
serial_fn ( int c )
{
	printf ( "Rcv: 0x%x ", c );
	if ( c >= ' ' )
	    printf ( "%c", c );
	printf ( "\n" );
}

/* This could be improved with some ARM trick */
static void
idle_loop ( void )
{
	/*
	for ( ;; )
	    ;
	*/
	for ( ;; ) {
	    blink_delay ( EVEN_SLOWER );
	    // serial_debug ( UART1 );
	}
}

static void
inter_test ( int fd )
{
	serial_read_hookup ( fd, serial_fn );
	idle_loop ();
}

/* ================================================= */
/* Working 11-24-2020 */

static int ss;

static void
toggle_led ( void )
{
	if ( ss ) {
	    led_on ();
	    ss = 0;
	} else {
	    led_off ();
	    ss = 1;
	}
}

/* This gets called at 1000 Hz at
 * interrupt level.
 */
static int nvic_count;

static void
systick_fn ( void )
{
	nvic_count++;
	if ( nvic_count >= 1000 ) {
	    toggle_led ();
	    nvic_count = 0;
	}
}

static void
systick_test ( void )
{
	unsigned int count;

	nvic_count = 0;
	systick_hookup ( systick_fn );
	led_on ();

	for ( ;; ) {
	    blink_delay ( EVEN_SLOWER );
	    count = get_systick_count ();
	    printf ( "Systick count: %d\n", count );
	}
}

/* to disable then enable interrupts ..
asm volatile( "cpsid i" ::: "memory" );
  -- critical section --
asm volatile( "cpsie i" ::: "memory" );
 */

void
startup ( void )
{
	int fd;

	rcc_init ();
	systick_init ();

	fd = serial_begin ( UART1, 115200 );
	// fd = serial_begin ( UART2, 115200 );

	set_std_serial ( fd );

	nvic_init ();

	led_init ();
	led_off ();

	puts ( "\n" );
	puts ( "Up and running\n" );
	// printf ( "CPU running at %d Hz\n", get_cpu_hz() );
	printf ( "Console on UART%d\n", fd+1 );

	// test_blink ();
	// test2 ();
	// test3 ();
	// test4 ();

	// The chip seems to come up with interrupts enabled,
	// so this is not necessary.
	// asm volatile( "cpsie i" ::: "memory" );

	/* And executing this indeed blocks interrupts,
	 *  and Systick as well !!
	 */
	// asm volatile( "cpsid i" ::: "memory" );

	// test_blink ();
	// test4 ();
	// echo_test ();
	// systick_test ();
	inter_test ( fd );

	led_on ();
	for ( ;; ) ;
}

/* THE END */
