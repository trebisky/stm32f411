/* gpio.c
 * (c) Tom Trebisky  9-24-2016
 * (c) Tom Trebisky  11-20-2020
 *
 * Basic GPIO driver for the F411
 *
 * Also includes LED routines
 */

/* names to index the bases array */
#define GPIOA	0
#define GPIOB	1
#define GPIOC	2

/* Where is the LED ?? */
#define LED_PIN		13	/* PC13 */
#define LED_GPIO	GPIOC	/* PC13 */

/* Here is the F411 gpio structure.
 *  very different from the F103.
 */
struct gpio {
	volatile unsigned int mode;	/* 0x00 */
	volatile unsigned int otype;	/* 0x04 */
	volatile unsigned int ospeed;	/* 0x08 */
	volatile unsigned int pupd;	/* 0x0c */
	volatile unsigned int idata;	/* 0x10 */
	volatile unsigned int odata;	/* 0x14 */
	volatile unsigned int bsrr;	/* 0x18 */
	volatile unsigned int lock;	/* 0x1c */
	volatile unsigned int afl;	/* 0x20 */
	volatile unsigned int afh;	/* 0x24 */
};

#define GPIOA_BASE	(struct gpio *) 0x40020000
#define GPIOB_BASE	(struct gpio *) 0x40020400
#define GPIOC_BASE	(struct gpio *) 0x40020800

static struct gpio *gpio_bases[] = {
    GPIOA_BASE, GPIOB_BASE, GPIOC_BASE
};

/* Change alternate function setting for a pin
 * These are 4 bit fields. All initially 0.
 */
void
gpio_af ( int gpio, int pin, int val )
{
	struct gpio *gp;
	int shift;

	gp = gpio_bases[gpio];

	if ( pin < 8 ) {
	    shift = pin * 4;
	    gp->afl &= ~(0xf<<shift);
	    gp->afl |= val<<shift;
	} else {
	    shift = (pin-8) * 4;
	    gp->afh &= ~(0xf<<shift);
	    gp->afh |= val<<shift;
	}
}

/* Note that UART1 can be moved around a lot.
 * I make a choice here.
 * I suppose a general interface would allow this to
 *  be selected via a call argument.
 */
void
gpio_uart_init ( int uart )
{
	if ( uart == 1 ) {
	    gpio_af ( GPIOA, 9, 7 );	/* Tx */
	    gpio_af ( GPIOA, 10, 7 );	/* Rx */
	    // gpio_af ( GPIOA, 15, 7 ); /* Tx */
	    // gpio_af ( GPIOB, 3, 7 );	/* Rx */
	    // gpio_af ( GPIOB, 6, 7 )	/* Tx */
	    // gpio_af ( GPIOB, 7, 7 );	/* Rx */
	} else if ( uart == 2 ) {
	    gpio_af ( GPIOA, 2, 7 );	/* Tx */
	    gpio_af ( GPIOA, 3, 7 );	/* Rx */
	} else { /* uart 3 */
	    gpio_af ( GPIOC, 6, 7 );	/* Tx */
	    gpio_af ( GPIOC, 7, 7 );	/* Rx */
	}
}

/* ========================================================== */

static struct gpio *led_gp;
static unsigned long on_mask;
static unsigned long off_mask;

void
led_init ( void )
{
	int conf;
	int shift;
	int pin = LED_PIN;

	// led_gp = GPIOC_BASE;
	led_gp = gpio_bases[LED_GPIO];

	shift = pin * 2;
	led_gp->mode &= ~(3<<shift);
	led_gp->mode |= (1<<shift);
	led_gp->otype &= ~(1<<pin);

	on_mask = 1 << pin;
	off_mask = 1 << (pin+16);
}

void
led_on ( void )
{
	led_gp->bsrr = on_mask;
}

void
led_off ( void )
{
	led_gp->bsrr = off_mask;
}

/* ========================================================== */
/* Does this belong here?  I don't think so. */
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
	// volatile int count = 1000 * FAST;
	volatile unsigned int count = 1000 * EVEN_SLOWER;

	while ( count-- )
	    ;
}

/* THE END */
