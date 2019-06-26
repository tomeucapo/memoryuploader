/*
 * command.c
 *
 * Created: 8/28/2017 2:16:56 AM
 *  Author: Tomeu Capó
 */ 

#include "globals.h"
#include "command.h"
#include <util/atomic.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

unsigned char data_in[MAX_COMMAND_SIZE];
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
		 if (data_count<MAX_COMMAND_SIZE)
			data_count++;
		 else data_count=0;
	 }
}

void InitCommand()
{
   data_count = 0;
   command_ready = 0;
}

unsigned char ReadLine(unsigned char *line)
{
	 register unsigned char command_ready_cpy;

     ATOMIC_BLOCK(ATOMIC_FORCEON)
     {
		command_ready_cpy = command_ready;
	 }

	 if (command_ready_cpy != 0)
	 {
		 for(register unsigned char i=0;data_in[i]!=0;i++)
		 {
			 line[i] = data_in[i];
		 }
		
		 ATOMIC_BLOCK(ATOMIC_FORCEON)
		 {
			  command_ready = 0;
		 }

		 return 1;
	 }

	 return 0;
}

uint16_t a_to_uint16(unsigned char * ptr)
{
	uint16_t val = 0;
	char c;
	
	while ((c = *ptr++))
	{
		if (isdigit(c))        
		{
			val *= 10UL;
			val += (uint16_t)(c - '0');
		}
	}
	return val;
}

CmdType ProcessCommand(unsigned char *commandIn)
{
	CmdType cmdRet;
	
	if (commandIn[0] == '\0')
	{
		cmdRet.commandId = CMD_EMPTY;
		return cmdRet;
	}
	
	if (commandIn[0] == 'T' || commandIn[0] == 't')
	{
		cmdRet.commandId = CMD_TEST;
		return cmdRet;
	}

	if (commandIn[0] == 'L' || commandIn[0] == 'l')
	{
		cmdRet.commandId = CMD_LAST_ADDR;
		return cmdRet;
	}

	if (commandIn[0] == 'V' || commandIn[0] == 'v')
	{
		cmdRet.commandId = CMD_VERSION;
		return cmdRet;
	}

	if (commandIn[0] == 'C' || commandIn[0] == 'c')
	{
		cmdRet.commandId = CMD_CLEAR;
		return cmdRet;
	}

	if (commandIn[0] == 'R' || commandIn[0] == 'r')
	{
		register uint8_t i = 1;
		unsigned char addr[5] = {0,0,0,0,0};
		
		while(commandIn[i]!='\0')
		{
			addr[i-1] = commandIn[i];
			i++;
		}
		
		cmdRet.commandId = CMD_READ;
		if (i>1) {
			cmdRet.currentAddr = a_to_uint16(addr); //(uint16_t) strtol(addr, NULL, 16);
			cmdRet.haveAddress = 1;
		} else {
		    cmdRet.currentAddr = 0;
			cmdRet.haveAddress = 0;
		}
		return cmdRet;
	}

	if (commandIn[0] == 'W' || commandIn[0] == 'w')
	{
		register uint8_t i = 1, j = 0;
		unsigned char addr[5] = {0,0,0,0,0};
		while(commandIn[i]!='\0')
		{
			addr[j] = commandIn[i];
			j++;i++;
		}

		addr[j] = 0;
		
		cmdRet.commandId = CMD_WRITE;
		cmdRet.currentAddr = a_to_uint16(addr); 
		return cmdRet;
	}
	
	cmdRet.commandId = CMD_UNKNW;
	return cmdRet;

}