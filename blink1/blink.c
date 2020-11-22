/* blink.c
 * (c) Tom Trebisky  9-24-2016
 * (c) Tom Trebisky  11-20-2020
 */

#define STM32F4

#ifdef STM32F4
struct rcc {
	volatile unsigned int cr;	/* 0 - control reg */
	volatile unsigned int pll;	/* 4 - pll config */
	volatile unsigned int conf;	/* 8 - clock config */
	volatile unsigned int cir;	/* c - clock interrupt */
	volatile unsigned int ahb1_r;	/* 10 - AHB1 peripheral reset */
	volatile unsigned int ahb2_r;	/* 14 - AHB2 peripheral reset */
	int __pad1[2];
	volatile unsigned int apb1_r;	/* 20 - APB1 peripheral reset */
	volatile unsigned int apb2_r;	/* 24 - APB2 peripheral reset */
	int __pad2[2];
	volatile unsigned int ahb1_e;	/* 30 - AHB1 peripheral enable */
	volatile unsigned int ahb2_e;	/* 34 - AHB2 peripheral enable */
	int __pad3[2];
	volatile unsigned int apb1_e;	/* 40 - APB1 peripheral enable */
	volatile unsigned int apb2_e;	/* 44 - APB2 peripheral enable */
	int __pad4[2];
	volatile unsigned int ahb1_elp;	/* 50 - AHB1 peripheral enable in low power */
	volatile unsigned int ahb2_elp;	/* 54 - AHB2 peripheral enable in low power */
	int __pad5[2];
	volatile unsigned int apb1_elp;	/* 60 - APB1 peripheral enable in low power */
	volatile unsigned int apb2_elp;	/* 64 - APB2 peripheral enable in low power */
	int __pad6[2];
	volatile unsigned int bdcr;	/* 70 */
	volatile unsigned int csr;	/* 74 */
	int __pad7[2];
	volatile unsigned int sscgr;	/* 80 */
	volatile unsigned int plli2s;	/* 84 */
	int __pad8;
	volatile unsigned int dccr;	/* 8c */
};
#else
/* The reset and clock control module */
struct rcc {
	volatile unsigned long rc;	/* 0 - clock control */
	volatile unsigned long cfg;	/* 4 - clock config */
	volatile unsigned long cir;	/* 8 - clock interrupt */
	volatile unsigned long apb2;	/* c - peripheral reset */
	volatile unsigned long apb1;	/* 10 - peripheral reset */
	volatile unsigned long ape3;	/* 14 - peripheral enable */
	volatile unsigned long ape2;	/* 18 - peripheral enable */
	volatile unsigned long ape1;	/* 1c - peripheral enable */
	volatile unsigned long bdcr;	/* 20 - xx */
	volatile unsigned long csr;	/* 24 - xx */
};
#endif

#ifdef STM32F4
#define GPIOA_ENABLE	0x01
#define GPIOB_ENABLE	0x02
#define GPIOC_ENABLE	0x04
#define GPIOD_ENABLE	0x08
#define GPIOE_ENABLE	0x10
#define GPIOH_ENABLE	0x80
#else
#define GPIOA_ENABLE	0x04
#define GPIOB_ENABLE	0x08
#define GPIOC_ENABLE	0x10
#endif


#ifdef STM32F4
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
#else
/* One of the 3 gpios */
struct gpio {
	volatile unsigned long cr[2];
	volatile unsigned long idr;
	volatile unsigned long odr;
	volatile unsigned long bsrr;
	volatile unsigned long brr;
	volatile unsigned long lock;
};
#endif

#ifdef STM32F4
#define RCC_BASE	(struct rcc *) 0x40023800

#define GPIOA_BASE	(struct gpio *) 0x40020000
#define GPIOB_BASE	(struct gpio *) 0x40020400
#define GPIOC_BASE	(struct gpio *) 0x40020800
#else
#define RCC_BASE	(struct rcc *) 0x40021000

#define GPIOA_BASE	(struct gpio *) 0x40010800
#define GPIOB_BASE	(struct gpio *) 0x40010C00
#define GPIOC_BASE	(struct gpio *) 0x40011000
#endif

#define MODE_OUT_2	0x02	/* Output, 2 Mhz */

#define CONF_GP_UD	0x0	/* Pull up/down */
#define CONF_GP_OD	0x4	/* Open drain */

/* On the STM32F103, this gives a blink rate of about 2.7 Hz */
/* i.e. the delay time is about 0.2 seconds (200 ms) */
#define FAST	200

#define FASTER	50
#define SLOWER	800

void
delay ( void )
{
	// volatile int count = 1000 * FAST;
	volatile unsigned int count = 1000 * SLOWER;

	while ( count-- )
	    ;
}

struct gpio *gp;
unsigned long on_mask;
unsigned long off_mask;

void
led_init ( int bit )
{
	int conf;
	int shift;
	struct rcc *rp = RCC_BASE;

#ifdef STM32F4
	/* Chip powers up with resets not being asserted */
	rp->ahb1_e |= GPIOC_ENABLE;	/* enable the clock */
#else
	/* Turn on GPIO C */
	rp->ape2 |= GPIOC_ENABLE;
#endif

	gp = GPIOC_BASE;

#ifdef STM32F4
	shift = bit * 2;
	gp->mode &= ~(3<<shift);
	gp->mode |= (1<<shift);
	gp->otype &= ~(1<<bit);
#else
	shift = (bit - 8) * 4;
	conf = gp->cr[1] & ~(0xf<<shift);
	conf |= (MODE_OUT_2|CONF_GP_OD) << shift;
	gp->cr[1] = conf;
#endif

	on_mask = 1 << bit;
	off_mask = 1 << (bit+16);
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

#define PC13	13

void
startup ( void )
{
	led_init ( PC13 );

	for ( ;; ) {
	    led_on ();
	    delay ();
	    led_off ();
	    delay ();
	}
}

/* THE END */
