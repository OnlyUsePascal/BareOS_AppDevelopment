#include "../lib/uart0.h"

/**
 * Set baud rate and characteristics and map to GPIO
 */
void uart_init() {
	unsigned int r;

	/* Turn off UART0 */
	UART0_CR = 0x0;

	r = GPFSEL1;
	// enable GPIO 14, 15
	r &=  ~((7 << 12) | (7 << 15)); //clear bits 17-12 (FSEL15, FSEL14)
	r |= (0b100 << 12)|(0b100 << 15);   //Set value 0b100 (select ALT0: TXD0/RXD0)
	GPFSEL1 = r;
	
	/* enable GPIO 16, 17 */
	#ifdef RPI3 //RPI3
		GPPUD = 0;            //No pull up/down control
		r = 150; while(r--) { asm volatile("nop"); } //waiting 150 cycles
		GPPUDCLK0 = (1 << 14) | (1 << 15); //enable clock for GPIO 14,15,16,17
		r = 150; while(r--) { asm volatile("nop"); } //waiting 150 cycles
		GPPUDCLK0 = 0;        // flush GPIO setup
		
	#else //RPI4
		r = GPIO_PUP_PDN_CNTRL_REG0;
		r &= ~((3 << 28) | (3 << 30)); //No resistor is selected for GPIO 14, 15
		GPIO_PUP_PDN_CNTRL_REG0 = r;
	#endif

	/* Mask all interrupts. */
	UART0_IMSC = 0;
	/* Clear pending interrupts. */
	UART0_ICR = 0x7FF;

	/* Set integer & fractional part of Baud rate
	Divider = UART_CLOCK/(16 * Baud)            
	Default UART_CLOCK = 48MHz (old firmware it was 3MHz); 
	Integer part register UART0_IBRD  = integer part of Divider 
	Fraction part register UART0_FBRD = (Fractional part * 64) + 0.5 */
	//115200 baud
	UART0_IBRD = 26;       
	UART0_FBRD = 3;

	/* Set up the Line Control Register */
	/* Enable FIFO */
	/* Set length to 8 bit */
	/* Defaults for other bit are No parity, 1 stop bit */
	UART0_LCRH = UART0_LCRH_FEN | UART0_LCRH_WLEN_8BIT;

	// uart, rx, tx, rts, cts
	UART0_CR = UART0_CR_UARTEN | UART0_CR_TXE | UART0_CR_RXE;
	// UART0_CR = 0x301;     // enable Tx, Rx, FIFO
}



/**
 * Send a character
 */
void uart_sendc(char c) {

	/* Check Flags Register */
	/* And wait until transmitter is not full */
	do {
		asm volatile("nop");
	} while (UART0_FR & UART0_FR_TXFF & (UART0_FR & UART0_FR_CTS)); // also check CTS

	/* Write our data byte out to the data register */
	UART0_DR = c ;
}

/**
 * Receive a character
 */
char uart_getc() {
	UART0_CR |= (UART0_CR_RTS); //allow other to send
	
  char c = 0;
	/* Check Flags Register */
	/* Wait until Receiver is not empty
		* (at least one byte data in receive fifo)*/
	do {
		asm volatile("nop");
	} while ( UART0_FR & UART0_FR_RXFE );

	/* read it and return */
	c = (unsigned char) (UART0_DR);

	/* convert carriage return to newline */
	
	UART0_CR &= ~(UART0_CR_RTS);
	return (c == '\r' ? '\n' : c);
}


/**
 * Display a string
 */
void uart_puts(char *s) {
    while (*s) {
        /* convert newline to carriage return + newline */
        if (*s == '\n')
            uart_sendc('\r');
        uart_sendc(*s++);
    }
}
