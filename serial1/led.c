/* led.c
 * (c) Tom Trebisky  9-24-2016
 * (c) Tom Trebisky  11-20-2020
 *
 * Basic GPIO LED driver for the F411
 */

#define LED_PIN		13	/* PC13 */

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

/* ========================================================== */
/* On the STM32F103, this gives a blink rate of about 2.7 Hz */
/* i.e. the delay time is about 0.2 seconds (200 ms) */
#define FAST	200

#define FASTER	50
#define SLOWER	800

/* Does this belong here?  I don't think so. */
void
blink_delay ( void )
{
	// volatile int count = 1000 * FAST;
	volatile unsigned int count = 1000 * SLOWER;

	while ( count-- )
	    ;
}
/* ========================================================== */

struct gpio *gp;
unsigned long on_mask;
unsigned long off_mask;

void
led_init ( void )
{
	int conf;
	int shift;
	int pin = LED_PIN;

	gp = GPIOC_BASE;

	shift = pin * 2;
	gp->mode &= ~(3<<shift);
	gp->mode |= (1<<shift);
	gp->otype &= ~(1<<pin);

	on_mask = 1 << pin;
	off_mask = 1 << (pin+16);
}

void
led_on ( void )
{
	gp->bsrr = on_mask;
}

void
led_off ( void )
{
	gp->bsrr = off_mask;
}

/* THE END */
