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

void
nvic_init ( void )
{
	struct nvic *np = NVIC_BASE;

	show_reg ( "nvic stir", &np->stir );
}

void
nvic_enable ( int irq )
{
	struct nvic *np = NVIC_BASE;

	if ( irq >= NUM_IRQ )
	    return;

	np->iser[irq/32] = 1 << (irq%32);
}

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
