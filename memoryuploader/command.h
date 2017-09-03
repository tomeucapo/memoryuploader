/*
 * command.h
 *
 * Created: 8/28/2017 2:18:25 AM
 *  Author: tomeu
 */ 


#ifndef COMMAND_H_
#define COMMAND_H_

#define MAX_COMMAND_SIZE	10

#define CMD_EMPTY	0x00
#define CMD_CLEAR	0x01
#define CMD_WRITE   0x02
#define CMD_READ	0x03
#define CMD_VERSION	0xE0
#define CMD_TEST	0xEE
#define CMD_ERR		0xFE
#define CMD_UNKNW	0xFF

struct Cmd
{
	uint16_t currentAddr;
	uint8_t currentData;
	uint8_t commandId;
};

typedef struct Cmd CmdType;

void InitCommand();
unsigned char GetCommand(unsigned char *commandIn);
CmdType ProcessCommand(unsigned char *commandIn);

#endif /* COMMAND_H_ */