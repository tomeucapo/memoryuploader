#include "globals.h"
#include "uart.h"

void UARTInit() 
{
	UBRRH = (BAUDRATE>>8);                      // shift the register right by 8 bits
	UBRRL = BAUDRATE;                           // set baud rate
	UCSRB|= (1<<TXEN)|(1<<RXEN)| (1<<RXCIE);                // enable receiver and transmitter
	UCSRC|= (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);   // 8bit data format
}

int UARTPutchar(char c, FILE *stream)
{
	/*if (c == '\n') {
		UARTPutchar('\r', stream);
	}*/
	
	while (!(UCSRA & (1<<UDRE)))
	;

	UDR = c;
	return 0;
}

char UARTGetchar(FILE *stream) {
	while (!(UCSRA & (1<<RXC)))
	;
	return UDR;
}

void UARTFlush(void)
{
	unsigned char dummy;
	while(UCSRA & (1<<RXC)) dummy = UDR;
}