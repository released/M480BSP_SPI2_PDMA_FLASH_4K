
#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

/*_____ I N C L U D E S ____________________________________________________*/
#include "NuMicro.h"

/*_____ D E F I N I T I O N S ______________________________________________*/

/*_____ M A C R O S ________________________________________________________*/

/*_____ D E C L A R A T I O N S ____________________________________________*/
	
/*_____ F U N C T I O N S ______------______________________________________*/


void SpiFlash_RX_PDMA(uint8_t* Rx , uint16_t len);

void SpiFlash_TX_PDMA(uint8_t* Tx , uint16_t len);

void SpiFlash_WriteEnable(void);

uint8_t SpiFlash_SendByte(uint8_t byte);

uint16_t SpiFlash_ReadMidDid(void);

void SpiFlash_ChipErase(void);

uint8_t SpiFlash_ReadStatusReg(void);

void SpiFlash_WriteStatusReg(uint8_t u8Value);

void SpiFlash_WaitReady(void);

void SpiFlash_NormalPageProgram(uint32_t StartAddress, uint8_t *u8DataBuffer , uint16_t NumByteToWrite , uint8_t EnablePDMA);

void SpiFlash_PageWrite(uint32_t page_no, uint8_t *u8DataBuffer , uint16_t NumByteToWrite, uint8_t EnablePDMA);

void SpiFlash_SectorWrite(uint32_t sector_no, uint8_t *u8DataBuffer , uint16_t NumByteToWrite, uint8_t EnablePDMA);

void SpiFlash_NormalRead(uint32_t StartAddress, uint8_t *u8DataBuffer , uint16_t NumByteToRead, uint8_t EnablePDMA);

void SpiFlash_PageRead(uint32_t page_no, uint8_t *u8DataBuffer , uint16_t NumByteToRead, uint8_t EnablePDMA);

void SpiFlash_SectorRead(uint32_t sector_no, uint8_t *u8DataBuffer , uint16_t NumByteToRead, uint8_t EnablePDMA);

void SpiFlash_Init(void);


#endif	/*__SPI_FLASH_H*/
