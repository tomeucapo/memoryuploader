/*
 * command.c
 *
 * Created: 8/28/2017 2:16:56 AM
 *  Author: tomeu
 */ 

#include "globals.h"
#include "command.h"
#include <util/atomic.h>

unsigned char data_in[4];
unsigned char data_count;
volatile unsigned char command_ready;

ISR(USART_RX_vect)
{
	 // Get data from the USART in register
	 data_in[data_count] = UDR;

	 if (data_in[data_count] == '\r')
	 {
		 data_in[data_count] = 0;
		 command_ready = 1;
		 data_count = 0;
	 }
	 else
	 {
		 if (data_count<4)
		 data_count++;
		 else data_count=0;
	 }
}

void InitCommand()
{
   data_count = 0;
   command_ready = 0;
}

unsigned char GetCommand(unsigned char *command_in)
{
	 unsigned char command_ready_cpy;
     ATOMIC_BLOCK(ATOMIC_FORCEON)
     {
		command_ready_cpy = command_ready;
	 }

	 if (command_ready_cpy != 0)
	 {
		 for(register unsigned char i=0;data_in[i]!=0;i++)
		 {
			 command_in[i] = data_in[i];
		 }
		
		 ATOMIC_BLOCK(ATOMIC_FORCEON)
		 {
			  command_ready = 0;
		 }

		 return 1;
	 }

	 return 0;
}
