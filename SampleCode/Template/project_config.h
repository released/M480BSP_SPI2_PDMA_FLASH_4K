/*************************************************************************//**

*****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"


#define _debug_log_UART_					(1)

//#define LED_R							(PH0)
//#define LED_Y							(PH1)
//#define LED_G							(PH2)

#define UART_PORT  						(UART5)
#define UART_TX_DMA_CH 				(11)
#define UART_TX_PDMA_OPENED_CH   		((1 << UART_TX_DMA_CH))

#define SPI_FLASH_CS_LOW				(PF6 = 0)
#define SPI_FLASH_CS_HIGH				(PF6 = 1)


#define TEST_NUMBER 					(16)   /* page numbers */
//#define TEST_SPI_PAGE
#define TEST_SPI_SECTOR

#define SPI_FLASH_CLK_FREQ  				(200000)
#define SPI_FLASH_PORT  					(SPI2)
#define SPI_FLASH_TX_DMA_CH 			(14)
#define SPI_FLASH_RX_DMA_CH 			(15)
#define SPI_FLASH_PDMA_OPENED_CH   	((1 << SPI_FLASH_TX_DMA_CH) | (1 << SPI_FLASH_RX_DMA_CH))
#define SPI_FLASH_PAGE_BYTE 				(0x100)
#define SPI_FLASH_SECTOR_SIZE 			(0x1000)


//uint8_t TxBuffer[SPI_FLASH_PAGE_BYTE] = {0};
//uint8_t RxBuffer[SPI_FLASH_PAGE_BYTE] = {0};
extern uint8_t TxBuffer[SPI_FLASH_PAGE_BYTE];
extern uint8_t RxBuffer[SPI_FLASH_PAGE_BYTE];
extern uint8_t Tx4KBuffer[SPI_FLASH_SECTOR_SIZE];
extern uint8_t Rx4KBuffer[SPI_FLASH_SECTOR_SIZE];

//uint8_t SPI_FLASH_page_counter = 0;
extern uint8_t SPI_FLASH_page_counter;


/*
	Device ID (command: AB hex) : 15
	Device ID (command: 90 hex) : C2 15
	RDID (command: 9F hex) : C2 20 16	
*/
#define CUSTOM_SPI_FLASH_ID				(0xC216)	//0xC215 : MX25L6433F
#define EVM_SPI_FALSH_ID					(0xEF15)		//W25Q32JV


typedef enum{

	flag_uart_rx = 0 ,
	flag_compare_error ,			
	flag_WDT ,	
	flag_UART_PDMA ,

	flag_SPI_LED_TX ,
	
	flag_DEFAULT	
}Flag_Index;

//volatile uint32_t BitFlag = 0;
extern volatile uint32_t BitFlag;
#define BitFlag_ON(flag)							(BitFlag|=flag)
#define BitFlag_OFF(flag)							(BitFlag&=~flag)
#define BitFlag_READ(flag)							((BitFlag&flag)?1:0)
#define ReadBit(bit)								(uint32_t)(1<<bit)

#define is_flag_set(idx)							(BitFlag_READ(ReadBit(idx)))
#define set_flag(idx,en)							( (en == 1) ? (BitFlag_ON(ReadBit(idx))) : (BitFlag_OFF(ReadBit(idx))))




void dump_buffer(uint8_t *pucBuff, int nBytes);

void  dump_buffer_hex(uint8_t *pucBuff, int nBytes);





/*** (C) COPYRIGHT 2016 Nuvoton Technology Corp. ***/
