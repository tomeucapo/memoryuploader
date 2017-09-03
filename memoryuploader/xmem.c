/*
* xmem.c
*
* Created: 8/26/2017 2:11:46 AM
*  Author: tomeu
*/

#include "globals.h"
#include "xmem.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>


volatile uint8_t busy;
volatile uint8_t count_busy = 0;

ISR(INT0_vect)
{
     busy = 1;
}

uint8_t CheckBusyStatus()
{
	if (!IsBusy())
		return 0;

	count_busy++;
	if (count_busy>MAX_TRY_BUSY)
	{
		busy = 0;
		return 2;
	}
	return 1;
}

void XMEMInit(void)
{
	// External memory interface enable
	MCUCR |= (1<<SRW10); // | (1<<SRW11);
	SFIOR = (1<<XMM2) | (1<<XMM0);
	
	// Enable trigger INT0 on any logical level
	GICR |= 1<<INT0;
	MCUCR |= 1<<ISC11;

	busy = 0;
}

uint8_t IsBusy()
{
	uint8_t busy_state = 0;
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
			busy_state = busy;
	}
	return busy_state;
}

void XMEMWrite(uint16_t pos, unsigned char value)
{
	unsigned char *p = (unsigned char *) (OFFSET+pos);
	*p = value;
}

void XMEMReadBuff(uint16_t pos, unsigned char *buff, uint8_t len )
{
	unsigned char *p = (unsigned char *) (OFFSET+pos);

	sbi(MCUCR, SRE);
	for(unsigned int i=0;i<len;i++)
	{
		*buff++ = *p++;
	}
	cbi(MCUCR, SRE);
}

void XMEMWriteBuff(uint16_t pos, unsigned char *buff, uint8_t len )
{
	while(IsBusy()) {}

	unsigned char *p = (unsigned char *) (OFFSET+pos);

	sbi(MCUCR, SRE);
	for(unsigned int i=0;i<len;i++)
	{
		*p++ = *buff++;
	}
	cbi(MCUCR, SRE);
}

void XMEMClear()
{
	while(IsBusy()) {}

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
	while(IsBusy()) {}

	sbi(MCUCR, SRE);
	for(register uint16_t i=0;i<100;i++)
	{
		XMEMWrite(i, i+48);
	}
	cbi(MCUCR, SRE);
}