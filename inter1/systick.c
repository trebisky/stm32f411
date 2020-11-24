/* systick.c
 * (c) Tom Trebisky  11-23-2020
 *
 * Basic Systick driver for the F411
 *
 * Also includes LED routines
 */

#include "f411.h"

/* There is not much on systick in RM0383, this is basically
 * an ARM thing covered in the ARM manuals.
 *
 * The RM does mention that the systick calibration register
 * has a fixed value of 10500, expecting a 10.5 Mhz clock and
 * delivering 1 ms ticks given this setup.
 * Indeed the Cal register yields "Systick Cal: 40002904"
 * and 0x2904 = 10500, the high 4 is the skew bit.
 *
 * I intend to use the fast CPU clock and ignore this register.
 *
 * Systick can be driven either by the AHB clock / 8
 * or by the CPU clock. (set the SOURCE bit high to get the CPU)
 *
 * Systick is a 24 bit down counter.
 *
 * With a 96 Mhz clock, to get a 1 ms tick rate, we load 96000
 * which is 0x17700, which is OK in 24 bits.
 */

struct systick {
	volatile unsigned int csr;
	volatile unsigned int reload;
	volatile unsigned int value;
	volatile unsigned int cal;
};

#define	CSR_ENABLE	1		/* enable the counter */
#define	CSR_TICKINT	2		/* enable interrupt */
#define	CSR_SOURCE	4		/* 1 = core clock, 0 = AHB/8 */
#define	CSR_FLAG	0x00010000	/* 1 if downcount since last read */

/* This is really part of a big block of "system control"
 * stuff that is documented in the ARM Cortex-M4 manual.
 */
#define SYSTICK_BASE	(struct systick *) 0xE000E010

/* This really has nothing to do with systick.
 */
#define CPUID_BASE	(unsigned int *) 0xE000ED00

void
systick_init ( void )
{
	struct systick *stp = SYSTICK_BASE;

	/* returns: 0x410FC241 */
	// show32 ( "CPU id: ", *CPUID_BASE );

	// show32 ( "Systick CSR: ", stp->csr );
	// show32 ( "Systick Cal: ", stp->cal );
}

/* THE END */
