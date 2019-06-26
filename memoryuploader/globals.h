/*
 * globals.h
 *
 * Created: 8/22/2017 1:07:19 AM
 *  Author: tomeu
 */ 


#ifndef GLOBALS_H_
#define GLOBALS_H_

#define VERSION		"1.0"
#define BLOCK_SIZE  104

#define F_CPU		8000000UL
#define BAUD		9600                        
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)   

#include <avr/io.h>

#define sbi(x,y) x |= _BV(y)
#define cbi(x,y) x &= ~(_BV(y))

#define STATE_CMD			0
#define STATE_BEGIN_WR_MEM	1
#define STATE_WR_MEM		2
#define STATE_END_WR_MEM	3
#define STATE_RD_MEM		4

#endif /* GLOBALS_H_ */