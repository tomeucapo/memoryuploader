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

// Simple timer for LED blink (status core led)
ISR(TIMER1_OVF_vect)
{
	PORTB ^= (1 << PB0);
	
	switch(CheckBusyStatus())
	{
		case MEM_BUSY: sbi(PORTB, PB1);
					   break;
		case MEM_NO_BUSY: cbi(PORTB, PB1);
					   break;
	}
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

unsigned char commandIn[MAX_COMMAND_SIZE+1];

inline void ClearCommand(unsigned char *command)
{
	for(register unsigned char i=0;i<MAX_COMMAND_SIZE+1;i++) { command[i]=0; };
}

#define		BUFFER_LEN		8

unsigned char buffer[BUFFER_LEN];
uint8_t posBuff = 0;

inline unsigned char AddByteToBuffer(uint8_t data)
{
	if (posBuff<BUFFER_LEN-1)
	{
		buffer[posBuff] = data;
		posBuff++;
		return 1;
	}

	buffer[posBuff] = 0;
	return 0;
}

inline void WriteBuffer(uint16_t addr)
{
	cli();
	printf("\nST %04X:%04X", addr, addr+(posBuff-1));
	XMEMWriteBuff(addr, buffer, posBuff);
	sei();
}

int main(void)
{
    setup();

	FILE uartOutput = FDEV_SETUP_STREAM(UARTPutchar, NULL, _FDEV_SETUP_WRITE);
	stdout = &uartOutput;
	
	unsigned char writeMode = 0;
	uint16_t initialAddr = 0;

	printf("\a\n*** Console v%s\n>", VERSION);
	while(1)
	{
		ClearCommand(commandIn);
		unsigned char ok = GetCommand(commandIn);
		if( !ok )
			continue;
	
		volatile CmdType cmd = ProcessCommand(commandIn);
		switch (cmd.commandId)
		{
			case CMD_TEST:
						printf("TEST");
						XMEMTest();
						break;
			case CMD_CLEAR:
						printf("CLEAR");
						XMEMClear();
						break;
			case CMD_READ:
						{
							unsigned char readBuff[100];
							XMEMReadBuff( cmd.currentAddr, readBuff, 100 );
							for(register unsigned int i = 0;i<100;i++)
							{
								if (i%8==0)
									printf("\n%04X: ", cmd.currentAddr+i);
								printf("%02X ", (int)readBuff[i]);
							}
						}
						break;
			case CMD_WRITE:
						if (!writeMode)
							initialAddr = cmd.currentAddr;
						
						writeMode = 1;
						printf("%04X: %02X", cmd.currentAddr, cmd.currentData);
						break;
			case CMD_EMPTY:
						writeMode = 0;
						break;
			case CMD_ERR: 
						printf("\nERR");
						break;

			case CMD_VERSION:
						printf("Version %s\n",VERSION);
						break;

			default:	printf("\nUNK");
						break;
		}
		
		if (writeMode)
		{
			if (!AddByteToBuffer(cmd.currentData))
			{
				WriteBuffer(initialAddr);
				initialAddr += posBuff;
				posBuff = 0;
			}			
		} else {
			if (posBuff>0)
			{
				WriteBuffer(initialAddr);
				posBuff = 0;
				initialAddr = 0;
			}
		}

		printf("\n%c", writeMode ?  '<' : '>');				
	}
}

