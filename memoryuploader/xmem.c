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
#include <util/delay.h>

volatile uint8_t busy;
volatile uint8_t count_busy = 0;


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
	MCUCR |= (1<<SRW10) | (1<<SRW11);
	DDRC = 0xFF;
	PORTC = 0x00;
	busy = 0;
}

uint8_t IsBusy()
{
	uint8_t busy_state = 0;
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
			busy_state = !((PIND & PIND3) || (PIND & PIND4));
	}
	return busy_state;
}


void XMEMWrite(uint16_t pos, unsigned char value)
{
	unsigned char *p = (unsigned char *) (OFFSET+pos);

	DDRC = 0xFF;
	PORTC = 0x00;

	SFIOR = (1<<XMM2) | (1<<XMM0);

	sbi(MCUCR, SRE);
	*p = value;
	cbi(MCUCR, SRE);
	
	SFIOR = 0x00;
}

unsigned char XMEMRead(uint16_t pos)
{
	unsigned char *p = (unsigned char *) (OFFSET+pos);

	DDRC = 0xFF;
	PORTC = 0x00;

	SFIOR = (1<<XMM2) | (1<<XMM0);

	sbi(MCUCR, SRE);
	unsigned char value = *p;
	cbi(MCUCR, SRE);
	
	SFIOR = 0x00;

	return value;
}

void XMEMReadBuff(uint16_t pos, unsigned char *buff, uint8_t len )
{
	unsigned char *p = (unsigned char *) (OFFSET+pos);
	
	DDRC = 0xFF;
	PORTC = 0x00;

	SFIOR = (1<<XMM2) | (1<<XMM0);

	sbi(MCUCR, SRE);
	for(register unsigned int i=0;i<len;i++)
	{
		*buff++ = *p++;
	}
	cbi(MCUCR, SRE);

	SFIOR = 0x00;
	
}

int XMEMWriteBuff(uint16_t pos, unsigned char *buff, uint8_t len )
{
	while(IsBusy()) {}

	unsigned char *p = (unsigned char *) (OFFSET+pos);
	unsigned int i=0;

	DDRC = 0xFF;
	PORTC = 0x00;

	SFIOR = (1<<XMM2) | (1<<XMM0);

	sbi(MCUCR, SRE);
	for(i=0;i<len;i++)
	{
		if (pos + i > MAX_SIZE)
			break;
		*p++ = buff[i];
	}
	cbi(MCUCR, SRE);
	
	SFIOR = 0x00;

	return i;
}

void XMEMClear()
{
	while(IsBusy()) {}

	unsigned char *p = (unsigned char *) (OFFSET);

	DDRC = 0xFF;
	PORTC = 0x00;

	SFIOR = (1<<XMM2) | (1<<XMM0);

	sbi(MCUCR, SRE);
	for(unsigned int i=0;i<MAX_SIZE;i++)
	{
		*p++ = 0;
	}
	cbi(MCUCR, SRE);

	SFIOR = 0x00;
}

/************************************************************************/
/* Write some first characters of ASCII Table into memory               */
/************************************************************************/

void XMEMTest()
{
	while(IsBusy()) {}

	unsigned char *p = (unsigned char *) (OFFSET);

	DDRC = 0xFF;
	PORTC = 0x00;

	SFIOR = (1<<XMM2) | (1<<XMM0);

	sbi(MCUCR, SRE);

	for(register int i=0;i<256;i++)
	{
		*p++ = i;
	}
	
	cbi(MCUCR, SRE);

	SFIOR = 0x00;
}

uint16_t XMEMSize()
{
    uint16_t size = 0;

	unsigned char *p = (unsigned char *) (OFFSET);

	DDRC = 0xFF;
	PORTC = 0x00;

	SFIOR = (1<<XMM2) | (1<<XMM0);

	sbi(MCUCR, SRE);

	for(register uint16_t i=0;i<MAX_SIZE;i++)
	{
		*p = 0xff;
		if (*p != 0xff) 
		   break;
		*p++;

		size = i;
	}

	cbi(MCUCR, SRE);

	SFIOR = 0x00;
	return size;
}