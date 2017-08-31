/*
 * xmem.h
 *
 * Created: 8/26/2017 2:15:52 AM
 *  Author: tomeu
 */ 


#ifndef XMEM_H_
#define XMEM_H_

#define OFFSET 0x8000

void XMEMInit(void);
void XMEMWrite(unsigned char pos, unsigned char value);
void XMEMClear();


#endif /* XMEM_H_ */