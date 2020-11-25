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

struct nvic {
	volatile unsigned long iser[3];	/* 00 */
	volatile unsigned long icer[3];	/* 0c */
	/* ... */
};

#define NVIC_BASE	((struct nvic *) 0xe000e100)

#define NUM_IRQ	68

void
nvic_enable ( int irq )
{
	struct nvic *np = NVIC_BASE;

	if ( irq >= NUM_IRQ )
	    return;

	np->iser[irq/32] = 1 << (irq%32);
}

/* THE END */
