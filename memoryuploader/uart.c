#include "globals.h"
#include "uart.h"
#include <util/setbaud.h>

void UARTInit(void) 
{
   UBRRH = UBRRH_VALUE;
   UBRRL = UBRRL_VALUE;
   #if USE_2X
   UCSRA |= (1 << U2X);
   #else
   UCSRA &= ~(1 << U2X);
   #endif

   UCSRC = (1<<UCSZ1) | (1<<UCSZ0);			      /* 8-bit data */
   UCSRB = (1<<RXEN) | (1<<TXEN) | (1<<RXCIE);   /* Enable RX and TX */
}

int UARTPutchar(char c, FILE *stream)
{
	if (c == '\n') {
		UARTPutchar('\r', stream);
	}
	loop_until_bit_is_set(UCSRA, UDRE);
	UDR = c;
	return 0;
}

char UARTGetchar(FILE *stream) {
	loop_until_bit_is_set(UCSRA, RXC);
	return UDR;
}