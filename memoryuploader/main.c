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
#include "buffer.h"

#include <avr/interrupt.h>
#include <util/delay.h>

// Simple timer for LED blink (status core led)
ISR(TIMER1_OVF_vect)
{
	PORTB ^= (1 << PB1);
	
	switch(CheckBusyStatus())
	{
		case MEM_BUSY: sbi(PORTB, PB0);
					   break;
		case MEM_NO_BUSY: cbi(PORTB, PB0);
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
	  DDRB = 0b00000111; 
	  PORTB = 0x00;		

	  PORTD = 0x00;
	  DDRD = 0x00;

	  DDRC = 0xFF;
	  PORTC = 0x00;
	   
	  // Enable trigger INT0 on any logical level
	  GICR |= 1<<INT0;
	  MCUCR |= 1<<ISC11;

	  XMEMInit();
	  TimersInit();
	  UARTInit(51);
	  InitCommand();
	  
	  sei();
}

unsigned char commandIn[MAX_COMMAND_SIZE+1];

uint16_t initialAddr = 0, lastAddr = 0;

inline void ClearCommandBuffer(unsigned char *command)
{
	 for(register unsigned char i=0;i<MAX_COMMAND_SIZE+1;i++) { command[i]=0; };
}

unsigned char CommandProcessor(unsigned char currentState, CmdType cmd)
{
		unsigned char nextState = currentState;
		
		switch (cmd.commandId)
		{
			case CMD_TEST:
						printf("\nTEST");
						XMEMTest();
						break;
			case CMD_CLEAR:
						printf("\nCLEAR");
						XMEMClear();
						break;
			case CMD_READ:
						nextState = STATE_RD_MEM;
						break;
			case CMD_WRITE:
						nextState = STATE_BEGIN_WR_MEM;
						break;

			case CMD_LAST_ADDR:
						printf("%04X", lastAddr);
						break;

			case CMD_EMPTY:
						if (currentState == STATE_WR_MEM)
							nextState = STATE_END_WR_MEM;
						break;

			case CMD_ERR: 
						printf("\nERR");
						break;

			case CMD_VERSION:
						printf("\nVersion %s",VERSION);
						break;

			default:	printf("\nUNK");
						break;
		}

		return nextState;
}

int main(void)
{
    setup();

	FILE uartOutput = FDEV_SETUP_STREAM(UARTPutchar, NULL, _FDEV_SETUP_WRITE);
	stdout = &uartOutput;
	
	unsigned char currentState = STATE_CMD, nextState = STATE_CMD;
	
	printf("\a\n*** Console Memory v%s\n", VERSION);
	printf("*** TCC 2019 (C)\n");
    printf("    Memory size = %d bytes\n\n>", XMEMSize());
	UARTFlush();

	volatile CmdType cmd;

	while(1)
	{
	    switch (currentState)
		{
			case STATE_CMD:
				ClearCommandBuffer(commandIn);
				unsigned char ok = ReadLine(commandIn);
				if( !ok )
					continue;
				cmd = ProcessCommand(commandIn);
				nextState = CommandProcessor(currentState, cmd);
				break;

			case STATE_BEGIN_WR_MEM:
				PORTB |= (1 << PB2);
				nextState = STATE_WR_MEM;
				initialAddr = cmd.currentAddr;
				
				printf("\n%04X: WAITING_DATA\n", cmd.currentAddr);
				ClearBuffer();
				break;
			
			case STATE_WR_MEM: 
			{
				unsigned char bufferIn[MAX_COMMAND_SIZE-1];
				volatile unsigned char end = 0;
				volatile uint16_t addr = initialAddr;
				while( !end )
				{
					ClearCommandBuffer(bufferIn);
					if ( !ReadLine(bufferIn) )
						continue;

					if (bufferIn[0]=='.') {
						end = 1;
					} else {				
						volatile uint8_t dataWrite = (uint8_t) a_to_uint16(bufferIn);
						printf("%04X: %02X\n", addr++, dataWrite);
						if (!AddByteToBuffer( dataWrite ))
						{
							lastAddr = WriteBuffer(initialAddr);
							printf("ST %04X:%04X\n", initialAddr, lastAddr);
							initialAddr += lastAddr;
							ClearBuffer();
						}
					}
				}
				nextState = STATE_END_WR_MEM;
			}
			break;

			case STATE_END_WR_MEM:
				if (!EmptyBuffer())
				{
					lastAddr = WriteBuffer(initialAddr);
					printf("ST %04X:%04X\n", initialAddr, lastAddr);
					ClearBuffer();
					initialAddr = 0;
				}
				PORTB &= ~(1 << PB2);

				nextState = STATE_CMD;
				break;
		
			case STATE_RD_MEM:
			{
				if (cmd.haveAddress)
					lastAddr = cmd.currentAddr;

				printf("\nR %04X:%04X", lastAddr, lastAddr+BLOCK_SIZE);
				unsigned char readBuff[BLOCK_SIZE];
				XMEMReadBuff( lastAddr, readBuff, BLOCK_SIZE );
				for(register unsigned int i = 0;i<BLOCK_SIZE;i++)
				{
					if (lastAddr%8==0)
						printf("\n%04X: ", lastAddr);
					printf("%02X ", (unsigned char)readBuff[i]);
					lastAddr++;
				}
			}
			nextState = STATE_CMD;
			break;
		} 

		if (currentState != STATE_WR_MEM && currentState != STATE_BEGIN_WR_MEM)
			printf("\n>");

		currentState = nextState;
	}
}

