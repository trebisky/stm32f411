/* gpio.c
 * (c) Tom Trebisky  9-24-2016
 * (c) Tom Trebisky  11-20-2020
 *
 * Basic GPIO driver for the F411
 *
 * Also includes LED routines
 */

#include "f411.h"

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

/* We have 3 gpio (more in the chip, but not routed on our 48 pin package).
 * We really only have A and B available.
 * PC 13 is the onboard LED
 * PC 14 and 15 are routed to the board edge, but are connected to
 * the 32 kHz crystal, so I can't see how that would be useful.
 * But PA and PB give us plenty of pins.
 */

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

#define MODE_INPUT	0
#define MODE_OUT	1
#define MODE_AF		2
#define MODE_ANALOG	3

/* This is a 2 bit field */
void
gpio_mode ( int gpio, int pin, int val )
{
	struct gpio *gp;
	int shift;

	gp = gpio_bases[gpio];
	shift = pin * 2;
	gp->mode &= ~(0x3<<shift);
	gp->mode |= val<<shift;
}

#define PUPD_NONE	0
#define PUPD_UP		1
#define PUPD_DOWN	2
#define PUPD_XXX	3

/* This is a 2 bit field */
void
gpio_pupd ( int gpio, int pin, int val )
{
	struct gpio *gp;
	int shift;

	gp = gpio_bases[gpio];
	shift = pin * 2;
	gp->pupd &= ~(0x3<<shift);
	gp->pupd |= val<<shift;
}

/* kludge for now */
void
gpio_uart ( int gpio, int pin )
{
	struct gpio *gp;
	int shift;

	gp = gpio_bases[gpio];

	gp->otype &= ~(1<<pin);

	shift = pin * 2;
	gp->ospeed |= (3<<shift);

	// gp->pupd &= ~(3<<shift);
	gpio_pupd ( gpio, pin, PUPD_NONE );
}

void
gpio_input_init ( int gpio, int pin )
{
	gpio_mode ( gpio, pin, MODE_INPUT );
	gpio_pupd ( gpio, pin, PUPD_UP );
}

int
gpio_read ( int gpio, int pin )
{
	struct gpio *gp = gpio_bases[gpio];

	return gp->idata & (1<<pin);
}

/* Note that UART1 can be moved around a lot.
 * I make a choice here.
 * I suppose a general interface would allow this to
 *  be selected via a call argument.
 */
void
gpio_uart_init ( int uart )
{
	if ( uart == UART1 ) {
	    gpio_af ( GPIOA, 9, 7 );	/* Tx */
	    gpio_mode ( GPIOA, 9, MODE_AF );
	    gpio_uart ( GPIOA, 9 );

	    gpio_af ( GPIOA, 10, 7 );	/* Rx */
	    gpio_mode ( GPIOA, 10, MODE_AF );
	    gpio_uart ( GPIOA, 10 );

	    /* UART1 could be on any of these pins
	     * if we wanted to move it there.
	     */
	    // gpio_af ( GPIOA, 15, 7 ); /* Tx */
	    // gpio_af ( GPIOB, 3, 7 );	/* Rx */
	    // gpio_af ( GPIOB, 6, 7 )	/* Tx */
	    // gpio_af ( GPIOB, 7, 7 );	/* Rx */
	} else if ( uart == UART2 ) {
	    gpio_af ( GPIOA, 2, 7 );	/* Tx */
	    gpio_mode ( GPIOA, 2, MODE_AF );
	    gpio_uart ( GPIOA, 2 );
	    gpio_af ( GPIOA, 3, 7 );	/* Rx */
	    gpio_mode ( GPIOA, 3, MODE_AF );
	    gpio_uart ( GPIOA, 3 );
	} else { /* UART3 */
	    gpio_af ( GPIOC, 6, 7 );	/* Tx */
	    gpio_mode ( GPIOC, 6, MODE_AF );
	    gpio_uart ( GPIOC, 6 );
	    gpio_af ( GPIOC, 7, 7 );	/* Rx */
	    gpio_mode ( GPIOC, 7, MODE_AF );
	    gpio_uart ( GPIOC, 7 );
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

	off_mask = 1 << pin;
	on_mask = 1 << (pin+16);
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

/* THE END */
