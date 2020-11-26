/* main.c
 * (c) Tom Trebisky  9-24-2016
 * (c) Tom Trebisky  11-26-2020
 *
 * Demo to interact with the user button.
 *
 * This began as the F411 blink demo and was modifed
 */

#include "f411.h"

/* Someday we will write a proper delay routine and dump this */

#define FAST	250
#define FASTER	50
#define SLOW	500
#define SLOWER	800
#define EVEN_SLOWER	1600
#define REALLY_SLOW	2000

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

void blinker ( void ) { test_blink(); }

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

/* ================================================= */
/* Working 11-24-2020 */

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

/* ================================================= */

/* Working 11-26-2020, very easy, it just involved adding
 * two simple routines to gpio.c
 * The button is indeed on A0 on my board and is a switch
 * to ground.  So it reads 0x01 when not pushed and 0x00
 * when pushed.
 */
void
button_scan ( void )
{
	int x;

	gpio_input_init ( GPIOA, 0 );

	for ( ;; ) {
	    blink_delay ( FAST );
	    x = gpio_read ( GPIOA, 0 );
	    // printf ( "Button: %X\n", x );
	    if ( x == 0 )
		printf ( "Button !!\n" );
	}
}

static void
button_fn ( void )
{
	printf ( "Button event !\n" );
}

void
button_int_test ( void )
{
	exti_setup ( GPIOA, 0, button_fn );

	idle ();
}

/* ================================================= */

void
startup ( void )
{
	int fd;

	fd = serial_begin ( UART1, 115200 );
	// fd = serial_begin ( UART2, 115200 );
	set_std_serial ( fd );

	puts ( "\n" );
	puts ( "Up and running\n" );

	// printf ( "CPU running at %d Hz\n", get_cpu_hz() );
	// printf ( "Console on UART%d\n", fd+1 );

	// systick_test ();

	printf ( "Waiting for button push\n" );
	// button_scan ();
	button_int_test ();

	printf ( "Done, spinning\n" );
	for ( ;; ) ;
}

/* THE END */
