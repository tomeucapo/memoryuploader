/*
 * memoryuploader.c
 *
 * Created: 8/20/2017 12:49:48 PM
 * Author : tomeu
 */ 

#include "globals.h"
#include "uart.h"
#include "xmem.h"
#include "command.h"

#include <avr/interrupt.h>
#include <util/delay.h>

#define CMD_EMPTY	0x00
#define CMD_CLEAR	0x01
#define CMD_WRITE   0x02
#define CMD_TEST	0xEE
#define CMD_UNKNW	0xFF

ISR(TIMER1_OVF_vect)
{
	PORTB ^= (1 << PB0);
	XMEMClearBusy();
}

void TimersInit()
{
	 TCCR1B |= (1 << CS11);
	 TCNT1 = 0;
	 TIMSK |= (1 << TOIE1);
}

void setup()
{
	  DDRB = (1<<DDB0);	//PORTB as OUTPUT
	  PORTB = 0x00;		//All pins of PORTB LOW	

	  PORTD = 0x00;
	  DDRD = 0x00;

	  DDRC = 0x00;
	  PORTC = 0x00;

	  XMEMInit();
	  TimersInit();
	  UARTInit();
	  InitCommand();

	  sei();
}

uint8_t ProcessCommand(unsigned char *commandIn)
{
	if (commandIn[0] == '\0')
	   return CMD_EMPTY;

	if (commandIn[0] == 'T') 
	   return CMD_TEST;
	
	if (commandIn[0] == 'C')
	   return CMD_CLEAR;

	if (commandIn[0] == 'W')
		return CMD_WRITE;

	return CMD_UNKNW;

}

unsigned char commandIn[5];

void ClearCommand(unsigned char *command)
{
	for(register unsigned char i=0;i<5;i++) { command[i]=0; };
}

int main(void)
{
    setup();

	FILE uartOutput = FDEV_SETUP_STREAM(UARTPutchar, NULL, _FDEV_SETUP_WRITE);
	stdout = &uartOutput;
	
	printf("\a\n*** Console v0.1\n>");
	while(1)
	{
		ClearCommand(commandIn);
		unsigned char ok = GetCommand(commandIn);
		if( ok != 0 )
		{
			switch (ProcessCommand(commandIn))
			{
				case CMD_TEST:
							printf("Write a test");
							XMEMTest();
							break;
				case CMD_CLEAR:
							printf("Clear memory");
							XMEMClear();
							break;
				case CMD_WRITE:
							
							break;
				case CMD_EMPTY:
							break;

				default:	printf("UNK\n");
							break;
			}
			printf("\n>");
		}
	}
}

