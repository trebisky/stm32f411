/* main.c
 * (c) Tom Trebisky  9-24-2016
 * (c) Tom Trebisky  11-20-2020
 *
 * Basic serial IO demo.
 *
 * This began as the F411 blink demo and was modifed
 */

#define STM32F4

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

#define RCC_BASE	(struct rcc *) 0x40023800

/* On AHB1 */
#define GPIOA_ENABLE	0x01
#define GPIOB_ENABLE	0x02
#define GPIOC_ENABLE	0x04
#define GPIOD_ENABLE	0x08
#define GPIOE_ENABLE	0x10
#define GPIOH_ENABLE	0x80

/* On APB1 */
#define UART2_ENABLE	0x20000

/* On APB2 */
#define UART1_ENABLE	0x10
#define UART3_ENABLE	0x20

/* Someday this RCC stuff will migrate to rcc.c or something of the sort.
 * Note that only GPIO A,B,C are wired to pins, so it is
 * pointless to power up D,E,H
 */
static void
rcc_init ( void )
{
	struct rcc *rp = RCC_BASE;

	/* The F411 chip powers up with resets not being asserted,
	 *  so we don't need to do anything with resets here.
	 * We do need to enable all the clocks though.
	 */
	rp->ahb1_e |= GPIOA_ENABLE;
	rp->ahb1_e |= GPIOB_ENABLE;
	rp->ahb1_e |= GPIOC_ENABLE;

	rp->apb1_e |= UART2_ENABLE;

	rp->apb2_e |= UART1_ENABLE;
	rp->apb2_e |= UART3_ENABLE;
}

void
startup ( void )
{
	rcc_init ();
	serial_init ();

	led_init ();

	for ( ;; ) {
	    led_on ();
	    blink_delay ();
	    led_off ();
	    blink_delay ();
	}
}

/* THE END */
