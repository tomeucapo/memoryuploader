/*
* xmem.c
*
* Created: 8/26/2017 2:11:46 AM
*  Author: tomeu
*/
#include "globals.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#define OFFSET 0x8001
#define MAX_SIZE 0x00FF

volatile uint8_t busy;

ISR(INT0_vect)
{
     busy = 1;
}

void XMEMClearBusy()
{
  	 busy = 0;
}

void XMEMInit(void)
{
	// External memory interface enable
	MCUCR |= (1<<SRW10); // | (1<<SRW11);
	SFIOR = (1<<XMM2) | (1<<XMM0);
	
	// Enable trigger INT0 on any logical level
	GICR |= 1<<INT0;
	MCUCR |= ( 1 << ISC11 ) | ( 1 <<ISC10 );

	busy = 0;
}

uint8_t WaitMemory()
{
	uint8_t busy_state = 0;
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
			busy_state = busy;
	}
	return busy_state;
}

void XMEMWrite(unsigned char pos, unsigned char value)
{
	unsigned char *p = (unsigned char *) (OFFSET+pos);
	*p = value;
}

void XMEMClear()
{
	while(WaitMemory()) {}

	unsigned char *p = (unsigned char *) (OFFSET);

	sbi(MCUCR, SRE);
	for(unsigned int i=0;i<MAX_SIZE;i++) 
	{	
		*p++ = 0;
	}
	cbi(MCUCR, SRE);
}

/************************************************************************/
/* Write some first characters of ASCII Table into memory               */
/************************************************************************/

void XMEMTest()
{
	while(WaitMemory()) {}

	sbi(MCUCR, SRE);
	for(register unsigned char i=0;i<100;i++)
	{
		XMEMWrite(i, i+48);
	}
	cbi(MCUCR, SRE);
}