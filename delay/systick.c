/* systick.c
 * (c) Tom Trebisky  11-23-2020
 *
 * Basic Systick driver for the F411
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
 * AN179 is helpful
 *  "Cortex M3 Embedded Software Development"
 *
 * The systick timer counts down to zero and then reloads.
 * It sets the flag each time it reaches 0.
 * This is a 24 bit timer (max reload is 0x00FFFFFF).
 *
 * The registers for this are part of the ARM Cortex M3
 *  "System Control Space" which contains:
 *
 *  0xE000E000 - interrupt type register
 *  0xE000E010 - system timer (SysTick)
 *  0xE000E100 - NVIC (nested vectored interrupt controller)
 *  0xE000ED00 - system control block (includes CPU ID)
 *  0xE000EF00 - software trigger exception registers
 *   ... and so forth
 *
 * Systick can be driven either by the AHB clock / 8
 * or by the CPU clock. (set the SOURCE bit high to get the CPU)
 * I set the source bit so I can use the full speed clock.
 *
 * Systick is a 24 bit down counter.
 *
 * With a 96 Mhz clock, to get a 1 ms tick rate, we load 96000
 * which is 0x17700, which is OK in 24 bits.
 *
 * Note that we don't have to enable anything in the NVIC to get
 * this to generate interrupts.  This must operate entirely
 * outside of the NVIC, i.e. the NVIC only deals with IRQ.
 */

/* We would like the systick to tick at 1000 Hz (every ms)
 */
#define SYSTICK_RATE	1000

struct systick {
	volatile unsigned int csr;
	volatile unsigned int reload;
	volatile unsigned int value;
	volatile unsigned int cal;
};

#define	CSR_ENABLE	1		/* enable the counter */
#define	CSR_INTENA	2		/* enable interrupt */
#define	CSR_SYSCLK	4		/* 1 = core clock, 0 = AHB/8 */
#define	CSR_FLAG	0x00010000	/* 1 if downcount since last read */

/* This is really part of a big block of "system control"
 * stuff that is documented in the ARM Cortex-M4 manual.
 */
#define SYSTICK_BASE	(struct systick *) 0xE000E010

#define IRQ_SYSTICK	15

/* This really has nothing to do with systick.
 */
#define CPUID_BASE	(unsigned int *) 0xE000ED00

static unsigned int systick_count;

unsigned int
get_systick_count ( void )
{
	return systick_count;
}

static vfptr systick_hook;

/* This is referenced from the table in locore.s
 */
void
systick_handler ( void )
{
	systick_count++;

	event_tick ();

	// toggle_led ();
	if ( systick_hook )
	    (*systick_hook) ();
}

void
systick_hookup ( vfptr fn )
{
	systick_hook = fn;
	// show_reg ( "systick hookup", &systick_hook );
}

/* Systick is a 24 bit counter.
 * 96,000,000 = 0x5B8D800 (too big)
 *  9,600,000 = 0x927C00  (ok - 10 Hz)
 */ 
void
systick_init ( void )
{
	struct systick *sp = SYSTICK_BASE;
	unsigned int rate;
	
	systick_count = 0;
	systick_hook = (vfptr) 0;

	rate = get_cpu_hz () / SYSTICK_RATE;

	sp->csr = CSR_SYSCLK;	/* stop the timer */
	sp->reload = rate - 1;
	sp->value = 0;
	sp->csr = CSR_SYSCLK | CSR_INTENA | CSR_ENABLE;

	// show32 ( "Systick CSR: ", stp->csr );
	// show32 ( "Systick Cal: ", stp->cal );
}

#ifdef notdef
static void
show_cpuid ( void )
{
	/* returns: 0x410FC241 */
	// show32 ( "CPU id: ", *CPUID_BASE );
}
#endif

/* THE END */
