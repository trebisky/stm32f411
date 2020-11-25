/* serial.c
 * (c) Tom Trebisky  7-2-2017
 *
 * Serial (uart) driver for the F411
 * For the 411 this is section 19 of RM0383
 *
 * This began (2017) as a simple polled output driver for
 *  console messages on port 1
 * In 2020, I decided to extend it to listen to a GPS receiver
 *  on port 2.
 *
 * Notice that I number these 1,2,3.
 * However my "3" is what they call "6" in the manual.
 * 
 * On the F411, USART1 and USART6 are on the APB2 bus.
 * On the F411, USART2 is on the APB1 bus.
 *
 * On the F411, after reset, with no fiddling with RCC
 *  settings, both are running at 16 Mhz.
 *  Apparently on the F411 both APB1 and APB2
 *   always run at the same rate.
 *
 * NOTE: On my black pill boards, pins C6 and C7 are not available,
 *  Meaning that UART3 (aka UART6) is not available.
 *  The code is in here, but not of much use if you can't
 *  get to the pins!!
 */

#include <stdarg.h>
#include "f411.h"

/* This is the same register layout as the STM32F103,
 * which is handy.
 */
struct uart {
        volatile unsigned long status;  /* 00 */
        volatile unsigned long data;    /* 04 */
        volatile unsigned long baud;    /* 08 */
        volatile unsigned long cr1;     /* 0c */
        volatile unsigned long cr2;     /* 10 */
        volatile unsigned long cr3;     /* 14 */
        volatile unsigned long gtp;     /* 18 - guard time and prescaler */
};

/* These are different for the F411
 * Well, USART1 has the same base, USART2 is different
 * and then the RM doesn't give a USART3!
 * We are given USART6, which I call USART3.
 */
#define UART1_BASE      (struct uart *) 0x40011000
#define UART2_BASE      (struct uart *) 0x40004400
#define UART3_BASE      (struct uart *) 0x40011400

#define SERIAL1_IRQ	37
#define SERIAL2_IRQ	38
#define SERIAL3_IRQ	71

static struct uart *uart_bases[] = {
    UART1_BASE, UART2_BASE, UART3_BASE
};

#define NUM_UARTS 2

struct uart_stuff {
	vfptr uart_hook;
};

static struct uart_stuff uart_info[NUM_UARTS];

/* Bits in Cr1 */
#define	CR1_ENABLE	0x2000
#define	CR1_9BIT	0x1000
#define	CR1_WAKE	0x0800
#define	CR1_PARITY	0x0400
#define	CR1_ODD		0x0200
#define	CR1_PIE		0x0100
#define	CR1_TXEIE	0x0080
#define	CR1_TCIE	0x0040
#define	CR1_RXIE	0x0020
#define	CR1_IDLE_IE	0x0010
#define	CR1_TXE		0x0008
#define	CR1_RXE		0x0004
#define	CR1_RWU		0x0002
#define	CR1_BRK		0x0001

// I don't understand the 9 bit thing, but it is needed.
#define CR1_CONSOLE	0x340c

/* SAM_M8Q gps is 9600, no parity, one stop */
#define CR1_GPS		0x200c

/* bits in the status register */
#define	ST_PE		0x0001
#define	ST_FE		0x0002
#define	ST_NE		0x0004
#define	ST_OVER		0x0008
#define	ST_IDLE		0x0010
#define	ST_RXNE		0x0020		/* Receiver not empty */
#define	ST_TC		0x0040		/* Transmission complete */
#define	ST_TXE		0x0080		/* Transmitter empty */
#define	ST_BREAK	0x0100
#define	ST_CTS		0x0200

/* ========================================================================= */

void
uart1_handler ( void )
{
}

void
uart2_handler ( void )
{
}

/* The baud rate.  This is subdivided from the bus clock.
 * It is as simple as dividing the bus clock by the baud
 * rate.  We could worry about it not dividing evenly, but
 * what can we do if it does not?
 */
int
serial_begin ( int uart, int baud )
{
	struct uart *up;

	gpio_uart_init ( uart );

	up = uart_bases[uart];

	/* 1 start bit, even parity */
	up->cr1 = CR1_CONSOLE;
	// up->cr1 = CR1_GPS;

	up->cr2 = 0;
	up->cr3 = 0;
	up->gtp = 0;

	if ( uart == UART2 )
	    up->baud = get_pclk1() / baud;
	else
	    up->baud = get_pclk2() / baud;
	return uart;
}

int
serial_available ( int uart )
{
	struct uart *up = uart_bases[uart];

	return (up->status & ST_RXNE);
}

/* Polled read (blocks)
 * Not often called, we usually call
 * serial_getc (), but if a person really
 * did want to see returns, this would do it.
 */
int
serial_read ( int uart )
{
	struct uart *up = uart_bases[uart];

	while ( ! (up->status & ST_RXNE) )
	    ;
	return up->data & 0x7f;
}

int
serial_getc ( int uart )
{
	int c;

	c = serial_read ( uart );
	if ( c == '\r' )
	    c = '\n';
}

void
serial_read_hookup ( int uart, vfptr fn )
{
	uart_info[uart].uart_hook = fn;
}

void
serial_putc ( int uart, int c )
{
	struct uart *up = uart_bases[uart];

	if ( c == '\n' )
	    serial_putc ( uart, '\r' );

	while ( ! (up->status & ST_TXE) )
	    ;
	up->data = c;
}

/* rarely used, like putc, but treats newlines
 * verbatim.
 */
void
serial_write ( int uart, int c )
{
	struct uart *up = uart_bases[uart];

	while ( ! (up->status & ST_TXE) )
	    ;
	up->data = c;
}

void
serial_flush ( int uart )
{
}

void
serial_puts ( int uart, char *str )
{
    while (*str)
        serial_putc ( uart, *str++ );
}

/* ========================================================================= */

/* The idea here is to be able to call puts and printf
 * from anywhere without passing fd all over the world.
 */

static int std_serial = UART1;

void
set_std_serial ( int arg )
{
	std_serial = arg;
}

/* Common shortcut */
void
console_init ( void )
{
	int console;

	console = serial_begin ( UART1, 115200 );
	set_std_serial ( console );
}

int
getc ( void )
{
	return serial_getc ( std_serial );
}

void
putc ( int ch )
{
	serial_putc ( std_serial, ch );
}

void
puts ( char *msg )
{
	serial_puts ( std_serial, msg );
}

#define PRINTF_BUF_SIZE 128
static void asnprintf (char *abuf, unsigned int size, const char *fmt, va_list args);

void
printf ( char *fmt, ... )
{
	char buf[PRINTF_BUF_SIZE];
        va_list args;

        va_start ( args, fmt );
        asnprintf ( buf, PRINTF_BUF_SIZE, fmt, args );
        va_end ( args );

        serial_puts ( std_serial, buf );
}

/* The limit is absurd, so take care */
void
sprintf ( char *buf, char *fmt, ... )
{
        va_list args;

        va_start ( args, fmt );
        asnprintf ( buf, 256, fmt, args );
        va_end ( args );
}

/* ========================================================================= */

/* Here I develop a simple printf.
 * It only has 3 triggers:
 *  %s to inject a string
 *  %d to inject a decimal number
 *  %h to inject a 32 bit hex value as xxxxyyyy
 */

#define PUTCHAR(x)      if ( buf <= end ) *buf++ = (x)

static const char hex_table[] = "0123456789ABCDEF";

// #define HEX(x)  ((x)<10 ? '0'+(x) : 'A'+(x)-10)
#define HEX(x)  hex_table[(x)]

#ifdef notdef
static char *
sprintnb ( char *buf, char *end, int n, int b)
{
        char prbuf[16];
        register char *cp;

        if (b == 10 && n < 0) {
            PUTCHAR('-');
            n = -n;
        }
        cp = prbuf;

        do {
            // *cp++ = "0123456789ABCDEF"[n%b];
            *cp++ = hex_table[n%b];
            n /= b;
        } while (n);

        do {
            PUTCHAR(*--cp);
        } while (cp > prbuf);

        return buf;
}
#endif

static char *
sprintn ( char *buf, char *end, int n )
{
        char prbuf[16];
        char *cp;

        if ( n < 0 ) {
            PUTCHAR('-');
            n = -n;
        }
        cp = prbuf;

        do {
            // *cp++ = "0123456789"[n%10];
            *cp++ = hex_table[n%10];
            n /= 10;
        } while (n);

        do {
            PUTCHAR(*--cp);
        } while (cp > prbuf);

        return buf;
}

static char *
shex2( char *buf, char *end, int val )
{
        PUTCHAR( HEX((val>>4)&0xf) );
        PUTCHAR( HEX(val&0xf) );
        return buf;
}

#ifdef notdef
static char *
shex3( char *buf, char *end, int val )
{
        PUTCHAR( HEX((val>>8)&0xf) );
        return shex2(buf,end,val);
}

static char *
shex4( char *buf, char *end, int val )
{
        buf = shex2(buf,end,val>>8);
        return shex2(buf,end,val);
}
#endif

static char *
shex8( char *buf, char *end, int val )
{
        buf = shex2(buf,end,val>>24);
        buf = shex2(buf,end,val>>16);
        buf = shex2(buf,end,val>>8);
        return shex2(buf,end,val);
}

static void
asnprintf (char *abuf, unsigned int size, const char *fmt, va_list args)
{
    char *buf, *end;
    int c;
    char *p;

    buf = abuf;
    end = buf + size - 1;
    if (end < buf - 1) {
        end = ((void *) -1);
        size = end - buf + 1;
    }

    while ( c = *fmt++ ) {
	if ( c != '%' ) {
            PUTCHAR(c);
            continue;
        }
	c = *fmt++;
	if ( c == 'd' ) {
	    buf = sprintn ( buf, end, va_arg(args,int) );
	    continue;
	}
	if ( c == 'x' ) {
	    buf = shex2 ( buf, end, va_arg(args,int) & 0xff );
	    continue;
	}
	if ( c == 'h' ) {
	    buf = shex8 ( buf, end, va_arg(args,int) );
	    continue;
	}
	if ( c == 'c' ) {
            PUTCHAR( va_arg(args,int) );
	    continue;
	}
	if ( c == 's' ) {
	    p = va_arg(args,char *);
	    // printf ( "Got: %s\n", p );
	    while ( c = *p++ )
		PUTCHAR(c);
	    continue;
	}
    }
    if ( buf > end )
	buf = end;
    PUTCHAR('\0');
}


void
serial_printf ( int fd, char *fmt, ... )
{
	char buf[PRINTF_BUF_SIZE];
        va_list args;

        va_start ( args, fmt );
        asnprintf ( buf, PRINTF_BUF_SIZE, fmt, args );
        va_end ( args );

        serial_puts ( fd, buf );
}

/* ========================================================================= */
/* ========================================================================= */
/* ========================================================================= */
/* ========================================================================= */
/* XXX STOP XXX */
/* ========================================================================= */
/* ========================================================================= */
/* ========================================================================= */
/* ========================================================================= */

#ifdef no_no_no
void
console_puts ( char *s )
{
	while ( *s )
	    console_putc ( *s++ );
}

/* Just for fun, recursive base 10 print
 */
void
printn ( int x )
{
	int d;

	if ( x == 0 )
	    return;
	if ( x < 0 ) {
	    console_putc ( '-' );
	    printn ( -x );
	    return;
	}
	d = x % 10;
	printn ( x / 10 );
	console_putc ( '0' + d );
}

void
show_n ( char *s, int val )
{
	console_puts ( s );
	console_putc ( ' ' );
	printn ( val );
	console_putc ( '\n' );
}

/* Kyu IO utilities */
#define HEX(x)  ((x)<10 ? '0'+(x) : 'A'+(x)-10)

#define PUTCHAR(x)      *buf++ = (x)

/* single byte as xx */
static char *
shex2( char *buf, int val )
{
        PUTCHAR( HEX((val>>4)&0xf) );
        PUTCHAR( HEX(val&0xf) );
        return buf;
}

/* "short" as xxyy */
static char *
shex4( char *buf, int val )
{
        buf = shex2(buf,val>>8);
        return shex2(buf,val);
}

/* "long" as aabbxxyy */
static char *
shex8( char *buf, int val )
{
        buf = shex2(buf,val>>24);
        buf = shex2(buf,val>>16);
        buf = shex2(buf,val>>8);
        return shex2(buf,val);
}


static void
print32 ( int val )
{
	char buf[9];

	shex8 ( buf, val );
	buf[8] = '\0';
	console_puts ( buf );
}

void
show32 ( char *s, int val )
{
	console_puts ( s );
	print32 ( val );
	console_putc ( '\n' );
}

void
show_reg ( char *msg, int *addr )
{
	console_puts ( msg );
	console_putc ( ' ' );
	print32 ( (int) addr );
	console_putc ( ' ' );
	print32 ( *addr );
	console_putc ( '\n' );
}

#endif  /* no no no */

/* ------------------------------------------------------- */
/* ------------------------------------------------------- */

#ifdef F103

void
serial1_handler ( void )
{
}

void
serial2_handler ( void )
{
}

void
serial3_handler ( void )
{
	struct uart *up = UART3_BASE;
	int c;

	up->status = 0;

	c = up->data & 0x7f;
	serial_putc ( c );
}

#define SERIAL3_IRQ	39

/* ------------------------------------------------------- */

static void
uart_rx_enable ( struct uart *up )
{
	    up->cr1 |= CR1_RXIE;
}

void
serial_init ( void )
{
	gpio_uart1 ();
	// uart_init ( UART1_BASE, 9600 );
	// uart_init ( UART1_BASE, 38400 );
	// uart_init ( UART1_BASE, 57600 ); ??
	uart_init ( UART1_BASE, 115200 );

	/* SAM_M8Q gps is 9600, no parity, one stop */
	gpio_uart2 ();
	uart_init ( UART2_BASE, 9600 );

	gpio_uart3 ();
	uart_init ( UART3_BASE, 9600 );
}

void
serial_putc ( int c )
{
	struct uart *up = UART1_BASE;

	if ( c == '\n' )
	    serial_putc ( '\r' );

	while ( ! (up->status & ST_TXE) )
	    ;
	up->data = c;
}

/* Polled read (spins forever on console) */
int
serial_getc ( void )
{
	struct uart *up = UART1_BASE;

	while ( ! (up->status & ST_RXNE) )
	    ;
	return up->data & 0x7f;
}


void
serial_puts ( char *s )
{
	while ( *s )
	    serial_putc ( *s++ );
}

/* Quick and dirty */
int
serial2_getc ( void )
{
	struct uart *up = UART2_BASE;

	while ( ! (up->status & ST_RXNE) )
	    ;
	return up->data & 0x7f;
}

/* Quick and dirty */
int
serial3_getc ( void )
{
	struct uart *up = UART3_BASE;

	while ( ! (up->status & ST_RXNE) )
	    ;
	return up->data & 0x7f;
}

/* For this test, I have a Sparkfun ublox sAM-M8Q GPS unit
 * connected with the Tx on the GPS going to A3 on the STM32
 * (serial 2, Rx).
 * To test with serial 3, Rx -  I can use pin B11
 *
 *  9-4-2020 I tested with both Serial 2 and 3.
 */
void
serial_test ( void )
{
	int c;

	nvic_enable ( SERIAL3_IRQ );
	uart_rx_enable ( UART3_BASE );
	for ( ;; ) ;

	/* Original polling test */
	for ( ;; ) {
	    // c = serial2_getc ();
	    c = serial3_getc ();
	    serial_putc ( c );
	}
}

/* -------------------------------------------- */
/* Some utility IO stuff from Kyu follows */
/* -------------------------------------------- */

void
show16 ( char *s, int val )
{
	char buf[5];

	serial_puts ( s );
	shex4 ( buf, val );
	buf[4] = '\0';
	serial_puts ( buf );
	serial_putc ( '\n' );
}
#endif /* F103 */

/* THE END */
