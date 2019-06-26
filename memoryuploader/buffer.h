/*
 * buffer.h
 *
 * Created: 26/06/2019 2:21:34
 *  Author: tomeu
 */ 


#ifndef BUFFER_H_
#define BUFFER_H_

#define		BUFFER_LEN		8

void ClearBuffer();
unsigned char EmptyBuffer();
unsigned char AddByteToBuffer(unsigned char);
unsigned int WriteBuffer(uint16_t);
unsigned int ChecksumBuffer();

#endif /* BUFFER_H_ */