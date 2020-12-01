/* main.c
 * (c) Tom Trebisky  9-24-2016
 * (c) Tom Trebisky  11-26-2020
 *
 * Demo to interact with the user button.
 *
 * This began as the F411 blink demo and was modifed
 */

#include "f411.h"

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
	    delay ( 1000 );
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
	    delay ( 200 );
	    x = gpio_read ( GPIOA, 0 );
	    // printf ( "Button: %X\n", x );
	    if ( x == 0 )
		printf ( "Button !!\n" );
	}
}

/* Button test 1 - all handled at interrupt level */

static void
button_fn1 ( void )
{
	printf ( "Button event (1)!\n" );
}

void
button_int_test1 ( void )
{
	exti_setup ( GPIOA, 0, button_fn1 );
}

/* Button test 2 - use block/unblock to synchronize
 * with non-interrupt code.
 */

static void
button_fn2 ( void )
{
	unblock ();
}


void
button_int_test2 ( void )
{
	exti_setup ( GPIOA, 0, button_fn2 );

	for ( ;; ) {
	    block ();
	    printf ( "Button event!\n" );
	}
}

void
delay_test ( void )
{
	printf ( "Start delay test\n" );

	for ( ;; ) {
	    toggle_led ();
	    delay ( 500 );
	}
}

static int rep_id;

static void
event_fn1 ( void )
{
    repeat_cancel ( rep_id );
}

static void
event_fn2 ( void )
{
    (void) repeat ( 125, toggle_led);
}

extern void show_events ( void );

/* Works fine 12-1-2020 */
void
repeat_test ( void )
{
	rep_id = repeat ( 400, toggle_led );
	event ( 3000, event_fn1 );
	event ( 6000, event_fn2 );
	event ( 8000, show_events );
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

	ram_init ();

	// tested OK
	// delay_test ();

	// printf ( "Waiting for button push\n" );
	// button_scan ();
	// button_int_test1 ();
	// button_int_test2 ();

	printf ( "Set up repeat\n" );
	repeat_test ();

	printf ( "Enter idle loop\n" );
	idle ();
}

/* THE END */
