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
#define enable_irq	asm volatile( "cpsie i" ::: "memory" )
#define disable_irq	asm volatile( "cpsie i" ::: "memory" )

/* This macro in particular, I am trying to discipline myself
 * to use more often.  It allows you to look at a datasheet and
 * just copy a bit number rather than working out a hex constant
 * as I have long done.  My traditional method is both slow and
 * error prone, and wastes additional time when you make a typo.
 * So, rather than 0x200 use BIT(9).
 */
#define BIT(nr)		(1<<(nr))

/* THE END */
