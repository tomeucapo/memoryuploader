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

unsigned char GetCommand(unsigned char *commandIn)
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
			 commandIn[i] = data_in[i];
		 }
		
		 ATOMIC_BLOCK(ATOMIC_FORCEON)
		 {
			  command_ready = 0;
		 }

		 return 1;
	 }

	 return 0;
}

CmdType ProcessCommand(unsigned char *commandIn)
{
	CmdType cmdRet;
	
	if (commandIn[0] == '\0')
	{
		cmdRet.commandId = CMD_EMPTY;
		return cmdRet;
	}
	
	if (commandIn[0] == 'T')
	{
		cmdRet.commandId = CMD_TEST;
		return cmdRet;
	}

	if (commandIn[0] == 'V')
	{
		cmdRet.commandId = CMD_VERSION;
		return cmdRet;
	}

	if (commandIn[0] == 'C')
	{
		cmdRet.commandId = CMD_CLEAR;
		return cmdRet;
	}

	if (commandIn[0] == 'R')
	{
		register uint8_t i = 1, j = 0;
		volatile unsigned char addr[5], data[3];
		while(commandIn[i]!='\0')
		{
			addr[j] = commandIn[i];
			j++;i++;
		}

		cmdRet.commandId = CMD_READ;
		cmdRet.currentAddr = (uint16_t) strtol(addr, NULL, 16);
		return cmdRet;
	}

	if (commandIn[0] == 'W')
	{
		register uint8_t i = 1, j = 0;
		volatile unsigned char addr[5], data[3];
		while(commandIn[i]!=32 && commandIn[i]!='\0')
		{
			addr[j] = commandIn[i];
			j++;i++;
		}

		if (commandIn[i]=='\0')
		{
			cmdRet.commandId = CMD_ERR;
			return cmdRet;
		}

		addr[j] = 0;

		i++;j=0;
		while(commandIn[i]!=0 && j<2)
		{
			data[j] = commandIn[i];
			i++;j++;
		}
		data[j] = 0;
		
		cmdRet.commandId = CMD_WRITE;
		cmdRet.currentAddr = (uint16_t) strtol(addr, NULL, 16);
		cmdRet.currentData = (uint8_t) strtol(data, NULL, 16);
		return cmdRet;
	}
	
	cmdRet.commandId = CMD_UNKNW;
	return cmdRet;

}