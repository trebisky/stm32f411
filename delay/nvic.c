/* Nvic.c
 * (c) Tom Trebisky  7-7-2017
 *
 * Driver for the STM32F103 NVIC interrupt controller
 *
 * The registers for this are part of the ARM Cortex M3
 *  "System Control Space" which contains:
 *
 *  0xE000E000 - interrupt type register
 *  0xE000E010 - system timer (SysTick)
 *  0xE000E100 - NVIC (nested vectored interrupt controller)
 *  0xE000ED00 - system control block (includes CPU ID)
 *  0xE000EF00 - software trigger exception registers
 *  0xE000EFD0 - ID space
 */

#include "f411.h"

struct nvic {
	volatile unsigned int iser[8];	/* 00 set enable */
	int __pad1[24];
	volatile unsigned int icer[8];	/* 80 clear enable */
	int __pad2[24];
	volatile unsigned int ispr[8];	/* 100 set pending */
	int __pad3[24];
	volatile unsigned int icpr[8];	/* 180 clear pending */
	int __pad4[24];
	volatile unsigned int iabr[8];	/* 200 active bit */
	int __pad5[56];
	volatile unsigned char ip[240];	/* 300 priority */
	int __pad6[644];
	volatile unsigned int stir;	/* EF00 - software trigger */
};

#define NVIC_BASE	((struct nvic *) 0xe000e100)

#define NUM_IRQ	68

/* I don't do anything to initialize the NVIC and this works just fine
 *  for what I do.  Some people might want to fiddle with priorities.
 */
void
nvic_init ( void )
{
	struct nvic *np = NVIC_BASE;

	// Be sure the serial IO system is initialized
	//  before printing from here.
	// show_reg ( "nvic stir", &np->stir );
}

void
nvic_enable ( int irq )
{
	struct nvic *np = NVIC_BASE;

	if ( irq >= NUM_IRQ )
	    return;

	np->iser[irq/32] = 1 << (irq%32);
}

/* =========================================================== */

/* Here is the EXTI controller stuff for the F411
 * it isn't part of the NVIC, but this file seems
 * like an OK place for it at least for now.
 */

#define IRQ_EXTI0	6
#define IRQ_EXTI1	7
#define IRQ_EXTI2	8
#define IRQ_EXTI3	9
#define IRQ_EXTI4	10
/* XXX there are a bunch more, see the RM chapter 10 */

/* Each EXTI "line" is driven by the corresponding pin of the GPIO.
 * In other words PA0, PB0, PC0, ... drive EXTI0
 * with 16 pins per GPIO, this means we need EXTI0 .. EXTI15
 *
 * Also 16, 17, 18, 21, 22 have special uses.
 */

struct exti {
	volatile unsigned int imask;	/* 0x00 */
	volatile unsigned int mask;	/* 0x04 */
	volatile unsigned int rtrigger;	/* 0x08 */
	volatile unsigned int ftrigger;	/* 0x0c */
	volatile unsigned int soft;	/* 0x10 - software trigger*/
	volatile unsigned int pending;	/* 0x14 */
};

#define EXTI_BASE	(struct exti *) 0x40013c00

/* The RM also mentions configuring the SYSCFG_EXTICR1 register.
 * for events from GPIO A0 and B0,
 * and corresponding SYSCFG registers for other pins.
 * This is in section 7 of the RM.
 */

struct syscfg {
	volatile unsigned int memremap;	/* 0x00 */
	volatile unsigned int pmc;	/* 0x04 */
	volatile unsigned int exti_conf[4];	/* 0x08 .. 0x14*/
	    int __pad[2];
	volatile unsigned int cmpcr;	/* 0x20 */
};

#define SYSCFG_BASE	(struct syscfg *) 0x40013800

/* The conf registers each have 4 fields of 4 bits.
 * conf[0] handles pins 0-3 and routes
 * them to exti lines 0-3
 */
static void
exti_sysconfig ( int gpio, int pin, int line )
{
	struct syscfg *sp = SYSCFG_BASE;
	int l_index;
	int l_shift;
	int p_index;
	int p_val;

	l_index = line/4;
	l_shift = (line%4) * 4;

	p_index = pin/4;
	p_val = pin & 3;

	if ( p_index != l_index )
	    return;

	sp->exti_conf[l_index] &= ~(0xf<<l_shift);
	sp->exti_conf[l_index] |= ~(p_val<<l_shift);
}

static vfptr exti_hook;
static int exti_mask;

/* This is referenced from the table in locore.s
 */
void
exti0_handler ( void )
{
	struct exti *ep = EXTI_BASE;

	ep->pending |= exti_mask;

	// printf ( "EXTI !\n" );
        if ( exti_hook )
            (*exti_hook) ();
}

#define EXTI0	0
#define EXTI1	1
#define EXTI2	2
#define EXTI3	3

/* Hackish first cut, suitable for A0.
 * will take a little work to generalize,
 *  but we wanted to be sure we were thinking
 *  straight first.
 * We should pick the "line" based on gpio and pin,
 *  and perhaps it isn't possible to accomodate all
 *  possible gpio inputs?  So if we get saturated we
 *  may need to reject requests.
 */
/* Nothing special needs to be done for the GPIO
 * other than setting it up as an input.
 * Next set the sysconfig muxes to route the
 *  gpio signal to one of 4 exti lines that
 *  are available to it.
 * Last, set up the exti business for that line.
 */
void
exti_setup ( int gpio, int pin, vfptr fn )
{
	struct exti *ep = EXTI_BASE;
	int line = EXTI0;

	gpio_input_init ( gpio, pin );

	exti_sysconfig ( gpio, pin, line );

	ep->ftrigger = 1<<line;
	ep->rtrigger = ~(1<<line);
	ep->mask = 1<<line;
	ep->imask = 1<<line;
	exti_mask = 1<<line;

	exti_hook = fn;
	nvic_enable ( IRQ_EXTI0 );

}

/* =========================================================== */

/* These are default entrys in the interrupt vector
 * If we don't spin, we end up in a furious interrupt loop
 * since we don't cancel the interrupt
 * No telling if we will get the messages or not,
 *  but we usually do.
 */
void
bogus ( void )
{
	printf ( "Unexpected interrupt!\n" );
	printf ( "Spinning\n" );
	for ( ;; ) ;
}

void
fault ( void )
{
	printf ( "Unexpected fault!\n" );
	printf ( "Spinning\n" );
	for ( ;; ) ;
}
/* THE END */
