/* rcc.c
 * (c) Tom Trebisky  11-20-2020
 *
 * Reset and Clock Control for the STM32F411
 */

/* ====================================================================== */

/* This doesn't really belong here, but here it is.
 */

struct flash {
	volatile unsigned int acr;
	volatile unsigned int keyr;
	volatile unsigned int optkeyr;
	volatile unsigned int sr;
	volatile unsigned int cr;
	volatile unsigned int optcr;
};

#define FLASH_BASE (struct flash *) 0x40023c00

/* In the ACR */
#define FL_PRFTEN	0x100
#define FL_ICEN		0x200
#define FL_DCEN		0x400
#define FL_ICRST	0x800
#define FL_DCRST	0x1000

void
flash_waits ( int nw )
{
	struct flash *fp = FLASH_BASE;
	unsigned int xyz;

	xyz = fp->acr;
	xyz &= ~0xf;
	xyz |= nw;
	fp->acr = xyz;
}

/* Enable data cache, instruction cache, and prefetch */
static void
flash_cache_enable ( void )
{
	struct flash *fp = FLASH_BASE;

	fp->acr |= FL_PRFTEN | FL_ICEN | FL_DCEN;
}

/* ====================================================================== */
/* ====================================================================== */

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

/* In the CR */
#define CR_HSEON	0x010000
#define CR_HSERDY	0x020000
#define CR_HSEBYP	0x040000

#define CR_PLLON	0x01000000
#define CR_PLLRDY	0x02000000

/* In the CONF */
#define CONF_CLOCK_BITS		0x3
#define CONF_HSI		0x0
#define CONF_HSE		0x1
#define CONF_PLL		0x2

#define APB1_SHIFT	10
#define APB2_SHIFT	13

#define APB_DIV1	0
#define APB_DIV2	4
#define APB_DIV4	5

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

/* In the PLL register */

#define PLL_RESERVED	0xF83C8000

#define PLL_SRC_HSE	0x400000
#define PLL_SRC_HSI	0x0

#define PLL_N_SHIFT	6
#define PLL_P_SHIFT	16
#define PLL_Q_SHIFT	24

/* Only 2 bits for this with special values
 * actually these are (P/2)-1
 */
#define PLL_P_2		0
#define PLL_P_4		1
#define PLL_P_6		2
#define PLL_P_8		3

#define PLL_M_VAL	25			/* 25/25 feeds 1 Mhz to PLL */
#define PLL_N_192	(192<<PLL_N_SHIFT)	/* PLL yiels 192 Mhz */
#define PLL_N_128	(128<<PLL_N_SHIFT)	/* PLL yiels 128 Mhz */
#define PLL_Q_VAL	(4<<PLL_Q_SHIFT)	/* 192/4 = 48 Mhz to USB */

#define PLL_P_DIV_2	(PLL_P_2<<PLL_P_SHIFT)
#define PLL_P_DIV_4	(PLL_P_4<<PLL_P_SHIFT)
#define PLL_P_DIV_6	(PLL_P_6<<PLL_P_SHIFT)
#define PLL_P_DIV_8	(PLL_P_8<<PLL_P_SHIFT)

#define PLL_P_VAL_96	(PLL_P_2<<PLL_P_SHIFT)	/* 192/2 = 96 Mhz to cpu */
#define PLL_P_VAL_48	(PLL_P_4<<PLL_P_SHIFT)	/* 192/4 = 48 Mhz to cpu */

// ===========================
// ===========================
// ===========================
// Pick one of the following

// #define CLOCK_16	// no init, run from HSI at default
// #define CLOCK_32	// hsi doubled by PLL
// #define CLOCK_25
// #define CLOCK_48
#define CLOCK_96
// #define CLOCK_HSI_96

#ifdef CLOCK_48
#define PLL_VAL ( PLL_M_VAL | PLL_N_192 | PLL_P_VAL_48 | PLL_Q_VAL )
#else	/* CLOCK_96 */
#define PLL_VAL ( PLL_M_VAL | PLL_N_192 | PLL_P_VAL_96 | PLL_Q_VAL )
#endif

/* For an experiment, see if we can run 16 in and out of the PLL */
#define PLL_16 ( 16 | PLL_N_128 | PLL_P_DIV_8 | PLL_Q_VAL )

#define PLL_32 ( 16 | PLL_N_128 | PLL_P_DIV_4 | PLL_Q_VAL )
#define PLL_96 ( 16 | PLL_N_192 | PLL_P_DIV_2 | PLL_Q_VAL )

/* The PLL is an alternate 3rd clock source.
 * We would like to run the CPU at 100 Mhz and
 * the USB clock at 48 Mhz, but this cannot be done.
 * But we can have the CPU at 96 Mhz and the proper
 * 48 Mhz USB, or we could run the CPU at 100 and
 * let the USB run at 50 (not good, but if you don't
 * intend to use the USB, why not.
 */
static void
cpu_clock_init_pll ( void )
{
	struct rcc *rp = RCC_BASE;
	unsigned int xyz;

	/* Turn on HSE oscillator */
	rp->cr |= CR_HSEON;
	while ( ! (rp->cr & CR_HSERDY) )
	    ;

	/* Configure PLL */
	xyz = rp->pll & PLL_RESERVED;
	xyz |= PLL_VAL;
	xyz |= PLL_SRC_HSE;
	rp->pll = xyz;

	/* Turn on PLL */
	rp->cr |= CR_PLLON;
	while ( ! (rp->cr & CR_PLLRDY) )
	    ;

	/* switch from HSI to PLL */
	xyz = rp->conf;
	xyz &= ~CONF_CLOCK_BITS;
	xyz |= CONF_PLL;
#ifdef CLOCK_96
	xyz |= (APB_DIV2<<APB1_SHIFT);
#endif
	rp->conf = xyz;
}

/* Here we just switch to the PLL using the
 * "on reset" values, which should give
 * 96 Mhz
 */
static void
cpu_clock_init_hsi ( void )
{
	struct rcc *rp = RCC_BASE;
	unsigned int xyz;

#ifdef ENSURE_HSI
	/* Configure PLL */
	xyz = rp->pll & PLL_RESERVED;
	// xyz |= PLL_16;
	// xyz |= PLL_32;
	xyz |= PLL_96;
	xyz |= PLL_SRC_HSI;
	rp->pll = xyz;
#endif

	/* Turn on PLL */
	rp->cr |= CR_PLLON;
	while ( ! (rp->cr & CR_PLLRDY) )
	    ;

	/* switch from HSI to PLL */
	/* The APB1 bus must run no faster than 50,
	 * The APB2 bus can run up to 100.
	 */
	xyz = rp->conf;
	xyz &= ~CONF_CLOCK_BITS;
	xyz |= CONF_PLL;
	xyz |= (APB_DIV2<<APB1_SHIFT);
	// show_reg ( "CONF is:", &rp->conf );
	// show32 ( "CONF will be:", xyz );
	rp->conf = xyz;
}

static void
cpu_clock_init_32 ( void )
{
	struct rcc *rp = RCC_BASE;
	unsigned int xyz;

	/* Configure PLL */
	xyz = rp->pll & PLL_RESERVED;
	// xyz |= PLL_16;
	xyz |= PLL_32;
	xyz |= PLL_SRC_HSI;
	rp->pll = xyz;

	/* Turn on PLL */
	rp->cr |= CR_PLLON;
	while ( ! (rp->cr & CR_PLLRDY) )
	    ;

	/* switch from HSI to PLL */
	/* The APB1 bus must run no faster than 50,
	 * The APB2 bus can run up to 100.
	 */
	xyz = rp->conf;
	xyz &= ~CONF_CLOCK_BITS;
	xyz |= CONF_PLL;
	// xyz |= (APB_DIV2<<APB1_SHIFT);
	rp->conf = xyz;
}

/* Some initial attempts locked up the chip.
 * Recovery required holding down BOOT0 and
 * doing reset, then SWD reflash would work.
 *
 * WeAct schematic shows a crystal on HSE,
 *  not a crystal oscillator.
 * This is apparently correct for my board.
 */
static void
cpu_clock_init_25 ( void )
{
	struct rcc *rp = RCC_BASE;
	unsigned int xyz;

	/* Turn on HSE oscillator */
	rp->cr |= CR_HSEON;
	while ( ! (rp->cr & CR_HSERDY) )
	    ;

	/* switch from HSI to HSE */
	xyz = rp->conf;
	xyz &= ~CONF_CLOCK_BITS;
	xyz |= CONF_HSE;
	rp->conf = xyz;
}

/* The RM has a table in section 3 for various voltages and
 * processor speeds that indicates how many flash wait states
 * are required.  I always run at 3.3 volts, so the rules
 * are as follows:
 *
 *  0-30 Mhz 0 waits
 * 30-64 Mhz 1 wait
 * 64-90 Mhz 2 waits
 * 90-100 Mhz 3 waits
 */
static void
cpu_clock_init ( void )
{
#ifdef CLOCK_25
	cpu_clock_init_25 ();
#endif
#if defined(CLOCK_96)
	flash_waits ( 3 );
	cpu_clock_init_pll ();
#endif
#if defined(CLOCK_48)
	flash_waits ( 1 );
	cpu_clock_init_pll ();
#endif
#ifdef CLOCK_HSI_96
	flash_waits ( 3 );
	cpu_clock_init_hsi ();
#endif
#ifdef CLOCK_32
	flash_waits ( 1 );
	cpu_clock_init_32 ();
#endif

	flash_cache_enable ();
}

/* Note that only GPIO A,B,C are wired to pins, so it is
 * pointless to power up D,E,H
 */

void
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

	cpu_clock_init ();
}

#ifdef notdef
/* Hook so we can use serial IO for debug
 * later after serial is initialized.
 */
void
rcc_debug ( void )
{
	// cpu_clock_init_xxx ();

}
#endif

/* On the Black Pill boards that I have, we have an external
 * 25 Mhz crystal.
 * We will multiply that by 4 someday, somehow to get 100 Mhz.
 * Actually we have to settle for 96 Mhz if we want
 * a proper USB clock.
 *
 * Like the F103, the APB1 domain cannot run full speed like
 *  the APB2 can.  On the F411, 50 Mhz is the limit.
 *
 * There is also a 16 Mhz RC internal oscillator (HSI)
 * The chip fires up using it.
 */

#ifdef CLOCK_16
#define PCLK1           16000000
#define PCLK2           16000000
#define CPU_HZ          16000000
#endif

#ifdef CLOCK_32
#define PCLK1           32000000
#define PCLK2           32000000
#define CPU_HZ          32000000
#endif

#ifdef CLOCK_HSI_96
#define PCLK1           48000000
#define PCLK2           96000000
#define CPU_HZ          96000000
#endif

#ifdef CLOCK_25
#define PCLK1           25000000
#define PCLK2           25000000
#define CPU_HZ          25000000
#endif

#ifdef CLOCK_48
#define PCLK1           48000000
#define PCLK2           48000000
#define CPU_HZ          48000000
#endif

#ifdef CLOCK_96
#define PCLK1           48000000
#define PCLK2           96000000
#define CPU_HZ          96000000
#endif

int
get_cpu_hz ( void )
{
	return CPU_HZ;
}

/* These will differ if we run with a clock over 50 Mhz, as
 * PCLK1 must not exceed 50.
 */
int
get_pclk1 ( void )
{
        return PCLK1;
}

int
get_pclk2 ( void )
{
        return PCLK2;
}

/* THE END */
