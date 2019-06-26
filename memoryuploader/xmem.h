/*
 * xmem.h
 *
 * Created: 8/26/2017 2:15:52 AM
 *  Author: tomeu
 */ 
	

#ifndef XMEM_H_
#define XMEM_H_

//0x8000
#define OFFSET			0x8000
#define MAX_SIZE		2048
#define MAX_TRY_BUSY	1000
#define	MEM_NO_BUSY		0
#define MEM_BUSY		1
#define MEM_TIMEOUT		2	

void XMEMInit(void);
void XMEMWrite(uint16_t pos, unsigned char value);
void XMEMReadBuff(uint16_t pos, unsigned char *buff, uint8_t len);
int XMEMWriteBuff(uint16_t pos, unsigned char *buff, uint8_t len);
void XMEMClear();
void XMEMTest();

uint8_t CheckBusyStatus();
uint8_t IsBusy();
uint16_t XMEMSize();


#endif /* XMEM_H_ */