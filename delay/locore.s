/* locore.s
 * Assembler startup file for the STM32F411
 * Tom Trebisky  9-24-2016 11-21-2020
 */

@ The Cortex M3 and M4 are thumb only processors

.section .vectors
.cpu cortex-m4
.thumb

@ First the "standard" 16 entries for a cortex-m4
.word   0x20020000  	/* stack top address */
.word   _reset      	/* 1 Reset */
.word   fault        	/* 2 NMI */
.word   fault        	/* 3 Hard Fault */
.word   fault        	/* 4 MM Fault */
.word   fault        	/* 5 Bus Fault */
.word   fault        	/* 6 Usage Fault */
.word   fault        	/* 7 RESERVED */
.word   fault        	/* 8 RESERVED */
.word   fault        	/* 9 RESERVED*/
.word   fault        	/* 10 RESERVED */
.word   fault        	/* 11 SV call */
.word   fault        	/* 12 Debug reserved */
.word   fault        	/* 13 RESERVED */
.word   fault        	/* 14 PendSV */
.word   systick_handler /* 15 SysTick */

@ and now 68 IRQ vectors
.word   bogus           /* IRQ  0 */
.word   bogus           /* IRQ  1 */
.word   bogus           /* IRQ  2 */
.word   bogus           /* IRQ  3 -- RTC */
.word   bogus           /* IRQ  4 */
.word   bogus           /* IRQ  5 */
.word   exti0_handler   /* IRQ  6 */
.word   bogus           /* IRQ  7 */
.word   bogus           /* IRQ  8 */
.word   bogus           /* IRQ  9 */
.word   bogus           /* IRQ 10 */
.word   bogus           /* IRQ 11 */
.word   bogus           /* IRQ 12 */
.word   bogus           /* IRQ 13 */
.word   bogus           /* IRQ 14 */
.word   bogus           /* IRQ 15 */
.word   bogus           /* IRQ 16 */
.word   bogus           /* IRQ 17 */
.word   bogus           /* IRQ 18 */
.word   bogus           /* IRQ 19 */
.word   bogus           /* IRQ 20 */
.word	bogus		/* IRQ 21 */
.word	bogus		/* IRQ 22 */
.word	bogus		/* IRQ 23 */
.word	bogus		/* IRQ 24 -- Timer 1 break */
.word	bogus		/* IRQ 25 -- Timer 1 update */
.word	bogus		/* IRQ 26 -- Timer 1 trig */
.word	bogus		/* IRQ 27 -- Timer 1 cc */

.word	bogus		/* IRQ 28 -- Timer 2 */
.word	bogus		/* IRQ 29 -- Timer 3 */
.word	bogus		/* IRQ 30 -- Timer 4 */

.word	bogus		/* IRQ 31 */
.word	bogus		/* IRQ 32 */
.word	bogus		/* IRQ 33 */
.word	bogus		/* IRQ 34 */
.word	bogus		/* IRQ 35 */
.word	bogus		/* IRQ 36 */
.word	uart1_handler	/* IRQ 37 -- UART 1 */
.word	uart2_handler	/* IRQ 38 -- UART 2 */
.word	bogus		/* IRQ 39 -- UART 3 */
.word	bogus		/* IRQ 40 */
.word	bogus		/* IRQ 41 */
.word	bogus		/* IRQ 42 */
.word	bogus		/* IRQ 43 */
.word	bogus		/* IRQ 44 */
.word	bogus		/* IRQ 45 */
.word	bogus		/* IRQ 46 */
.word	bogus		/* IRQ 47 */
.word	bogus		/* IRQ 48 */
.word	bogus		/* IRQ 49 */
.word	bogus		/* IRQ 50 */
.word	bogus		/* IRQ 51 */
.word	bogus		/* IRQ 52 */
.word	bogus		/* IRQ 53 */
.word	bogus		/* IRQ 54 */
.word	bogus		/* IRQ 55 */
.word	bogus		/* IRQ 56 */
.word	bogus		/* IRQ 57 */
.word	bogus		/* IRQ 58 */
.word	bogus		/* IRQ 59 */
.word	bogus		/* IRQ 60 */
.word	bogus		/* IRQ 61 */
.word	bogus		/* IRQ 62 */
.word	bogus		/* IRQ 63 */
.word	bogus		/* IRQ 64 */
.word	bogus		/* IRQ 65 */
.word	bogus		/* IRQ 66 */
.word	bogus		/* IRQ 67 */


.section .text

.thumb_func
_reset:
    bl stm_init
    b .

        .globl get_sp
get_sp:
        add     r0, sp, #0
        bx      lr

        .globl get_pri
get_pri:
        movw	r0, #999
        mrs	r0, primask
        bx      lr

        .globl get_con
get_con:
        movw	r0,#999
        movs	r0,#99
        bx      lr

/* THE END */
