/* Tom Trebisky (c) 11-22-2020
 *
 * f411.h
 */

/* names to call functions with */
#define UART1	0
#define UART2	1
// #define UART3	2

/* names to index the bases array */
#define GPIOA	0
#define GPIOB	1
#define GPIOC	2

/* pointer to void function */
typedef void (*vfptr) ( void );
typedef void (*ifptr) ( int );

/* Handy macros */

/* These can be used as locks around critical sections */
// #define enable_irq	asm volatile( "cpsie i" ::: "memory" )
// #define disable_irq	asm volatile( "cpsid i" ::: "memory" )

/* I like these better because they look like C functions,
 * and I keep putting () after the above.
 * Also the above were just not working right,
 * This seems * impossible, as simple as they are,
 *   but that is what I observed.
 *   (the disable seemed to be having no effect).
 *   Disassembled code looked fine, who knows.
 *
 * The following work just fine;
 * You need to tell the compiler to optimize for these
 *  to actually go inline.
 */
static inline void irq_enable( void )
{
  __asm__ __volatile__ ("cpsie i"); /* Clear PRIMASK */
}

static inline void irq_disable( void )
{
  __asm__ __volatile__ ("cpsid i"); /* Set PRIMASK */
}

/* This macro in particular, I am intending to discipline myself
 * to use more often.  It allows you to look at a datasheet and
 * just copy a bit number rather than working out a hex constant
 * as I have long done.  My traditional method is both slow and
 * error prone, and wastes additional time when you make a typo.
 * So, rather than 0x200 use BIT(9).
 */
#define BIT(nr)		(1<<(nr))

/* THE END */
