/* main.c
 * (c) Tom Trebisky  9-24-2016
 * (c) Tom Trebisky  11-26-2020
 *
 * Demo to interact with the user button.
 *
 * This began as the F411 blink demo and was modifed
 */

#include "f411.h"

extern void show_events ( void );
extern void led_off ( void );

/* ================ */

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
	    // printf ( "In FN, systick count: %d\n", get_systick_count() );
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

	// Yep, this disables them.
	// irq_disable ();
	// spin ();

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

static int rep_id1;
static int rep_id2;

static void
event_fn1 ( void )
{
    /* Doing this from interrupt level is questionable */
    repeat_cancel ( rep_id1 );
}

static void
event_fn2 ( void )
{
    rep_id2 = repeat ( 125, toggle_led);
}

static void
event_fn3 ( void )
{
    show_events ();
    unblock ();
}

/* Works fine 12-1-2020 */
void
repeat_test ( void )
{
	rep_id1 = repeat ( 400, toggle_led );
	event ( 3000, event_fn1 );
	event ( 6000, event_fn2 );
	// event ( 8000, show_events );
	event ( 8000, event_fn3 );
}

static void
button_fn3 ( void )
{
	unblock ();
}

/* As you can see, this test combines quite a few things.
 * The behavior that should be observed is that the LED
 * will begin flashing slowly.  It should flash 3 times
 * and then stop.  Then 3 seconds should go by with no
 * flashing.  Then it shouls start flashing rapidly.
 * After another 2 seconds, you should get a printed
 * message telling you to push the button to stop LED.
 * When you push the button, the LED should go out and
 * you should see the message "Enter idle loop" and
 * the test is finished.
 */

static void
repeat_test2 ( void )
{
	repeat_test ();

	block ();
	printf ( "Push button to stop LED\n" );
	exti_setup ( GPIOA, 0, button_fn3 );

	block ();
	repeat_cancel ( rep_id2 );

	/* No telling if the repeat stops with the
	 * LED on or off, this guarantees gets
	 * turned off.
	 */
	event ( 100, led_off );
}

static void
check_pri ( void )
{
	printf ( "Arm PRI in INT = %X\n", get_pri () );
}

/* It seems that just taking an interrupt
 * (going from thread to handler mode) does NOT
 * set the PRIMASK.  This is a surprise!
 * But cpsid does set it.
 * So we need to handle our own locking at interrupt level.
 */
static void
test_inter ( void )
{
	printf ( "Con: %d\n", get_con() );
	printf ( "Pri: %X\n", get_pri() );
	irq_disable ();
	/* Aha!! The primask gets set to 1 !! */
	printf ( "Pri: %X\n", get_pri() );
	irq_enable ();
	printf ( "Pri: %X\n", get_pri() );

	event ( 100, check_pri );
}

/* ================================================= */

#ifdef notdef
/* Just so I could examine the disassembly */
static void fish ( void ) {
	disable_irq;
	ss = 2;
	enable_irq;
}
#endif

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

	// tested OK
	// delay_test ();

	// printf ( "Waiting for button push\n" );
	// button_scan ();
	// button_int_test1 ();
	// button_int_test2 ();

	// printf ( "Set up repeat\n" );
	// repeat_test ();
	// test_inter ();

	repeat_test2 ();

	printf ( "Enter idle loop\n" );
	idle ();
}

/* THE END */
