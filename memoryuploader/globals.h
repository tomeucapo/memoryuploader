/*
 * globals.h
 *
 * Created: 8/22/2017 1:07:19 AM
 *  Author: tomeu
 */ 


#ifndef GLOBALS_H_
#define GLOBALS_H_

#define VERSION		"0.1"
#define F_CPU		8000000UL
#define BAUD		9600L

#include <util/setbaud.h>
#include <avr/io.h>

#define sbi(x,y) x |= _BV(y)
#define cbi(x,y) x &= ~(_BV(y))


#endif /* GLOBALS_H_ */