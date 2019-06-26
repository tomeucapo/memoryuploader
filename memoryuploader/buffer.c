/*
 * buffer.c
 *
 * Created: 26/06/2019 2:21:04
 *  Author: tomeu
 */ 

 #include "globals.h"
 #include "buffer.h"
 #include "xmem.h"

 unsigned char buffer[BUFFER_LEN];
 uint8_t posBuff = 0;

 inline void ClearBuffer()
 {
	 for(register unsigned char i=0;i<BUFFER_LEN;i++) { buffer[i]=0; };
	 posBuff = 0;
 }

 inline unsigned char EmptyBuffer()
 {
	return (posBuff == 0);
 }

 inline unsigned char AddByteToBuffer(unsigned char data)
 {
	 if (posBuff<BUFFER_LEN)
	 {
		 buffer[posBuff++] = data;
		 return 1;
	 }

	 return 0;
 }

 /************************************************************************/
 /*  Fletcher 16 bit checksum calculation                                */
 /************************************************************************/

 inline unsigned int ChecksumBuffer()
 {
	uint16_t sum1 = 0, sum2 = 0;

	for(register unsigned char i=0;i<BUFFER_LEN;i++) 
	{ 
		sum1 = (sum1 + buffer[i]) % 255; 
		sum2 = (sum2 + sum1) % 255;
	}
	return (sum2 << 8) | sum1;
 }

 inline unsigned int WriteBuffer(uint16_t addr)
 {
	 unsigned int writePos = XMEMWriteBuff(addr, buffer, posBuff);
	 return addr+writePos;
 }