/*_____ I N C L U D E S ____________________________________________________*/

#include	"project_config.h"
#include	"SPI_Flash.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/
/*_____ D E F I N I T I O N S ______________________________________________*/
/*_____ M A C R O S ________________________________________________________*/
/*_____ F U N C T I O N S __________________________________________________*/


void SpiFlash_RX_PDMA(uint8_t* Rx , uint16_t len)
{
	uint32_t u32RegValue = 0;
	uint32_t u32Abort = 0;	
	
    PDMA_Open(PDMA, (1 << SPI_FLASH_RX_DMA_CH));

	//RX	
    PDMA_SetTransferCnt(PDMA,SPI_FLASH_RX_DMA_CH, PDMA_WIDTH_8, len);
    /* Set source/destination address and attributes */
    PDMA_SetTransferAddr(PDMA,SPI_FLASH_RX_DMA_CH, (uint32_t)&SPI_FLASH_PORT->RX, PDMA_SAR_FIX, (uint32_t)Rx, PDMA_DAR_INC);
    /* Set request source; set basic mode. */

    PDMA_SetTransferMode(PDMA,SPI_FLASH_RX_DMA_CH, PDMA_SPI2_RX, FALSE, 0);
	
    /* Single request type. SPI only support PDMA single request type. */
    PDMA_SetBurstType(PDMA,SPI_FLASH_RX_DMA_CH, PDMA_REQ_SINGLE, PDMA_BURST_128);
    /* Disable table interrupt */
    PDMA->DSCT[SPI_FLASH_RX_DMA_CH].CTL |= PDMA_DSCT_CTL_TBINTDIS_Msk;

    SPI_TRIGGER_RX_PDMA(SPI_FLASH_PORT);

    while(1)
    {
        /* Get interrupt status */
        u32RegValue = PDMA_GET_INT_STATUS(PDMA);
        /* Check the DMA transfer done interrupt flag */
        if(u32RegValue & PDMA_INTSTS_TDIF_Msk)
        {
            /* Check the PDMA transfer done interrupt flags */
            if((PDMA_GET_TD_STS(PDMA) & (1 << SPI_FLASH_RX_DMA_CH)) == (1 << SPI_FLASH_RX_DMA_CH))
            {
                /* Clear the DMA transfer done flags */
                PDMA_CLR_TD_FLAG(PDMA,1 << SPI_FLASH_RX_DMA_CH);
                /* Disable SPI PDMA RX function */
                SPI_DISABLE_RX_PDMA(SPI_FLASH_PORT);

//				dump_buffer_hex(Rx,len);
				
				TIMER_Delay(TIMER0,500);
    			SPI_FLASH_CS_HIGH;				
                break;
            }

            /* Check the DMA transfer abort interrupt flag */
            if(u32RegValue & PDMA_INTSTS_ABTIF_Msk)
            {
                /* Get the target abort flag */
                u32Abort = PDMA_GET_ABORT_STS(PDMA);
                /* Clear the target abort flag */
                PDMA_CLR_ABORT_FLAG(PDMA,u32Abort);
                break;
            }
        }
    }

}

void SpiFlash_TX_PDMA(uint8_t* Tx , uint16_t len)
{
	uint32_t u32RegValue = 0;
	uint32_t u32Abort = 0;	

	SPI_FLASH_CS_LOW;

    PDMA_Open(PDMA, (1 << SPI_FLASH_TX_DMA_CH));

	//TX
    PDMA_SetTransferCnt(PDMA,SPI_FLASH_TX_DMA_CH, PDMA_WIDTH_8, len);
    /* Set source/destination address and attributes */
    PDMA_SetTransferAddr(PDMA,SPI_FLASH_TX_DMA_CH, (uint32_t)Tx, PDMA_SAR_INC, (uint32_t)&SPI_FLASH_PORT->TX, PDMA_DAR_FIX);
    /* Set request source; set basic mode. */
	
    PDMA_SetTransferMode(PDMA,SPI_FLASH_TX_DMA_CH, PDMA_SPI2_TX, FALSE, 0);
	
    /* Single request type. SPI only support PDMA single request type. */
    PDMA_SetBurstType(PDMA,SPI_FLASH_TX_DMA_CH, PDMA_REQ_SINGLE, PDMA_BURST_128);
    /* Disable table interrupt */
    PDMA->DSCT[SPI_FLASH_TX_DMA_CH].CTL |= PDMA_DSCT_CTL_TBINTDIS_Msk;

    SPI_TRIGGER_TX_PDMA(SPI_FLASH_PORT);

    while(1)
    {
        /* Get interrupt status */
        u32RegValue = PDMA_GET_INT_STATUS(PDMA);
        /* Check the DMA transfer done interrupt flag */
        if(u32RegValue & PDMA_INTSTS_TDIF_Msk)
        {
            /* Check the PDMA transfer done interrupt flags */
            if((PDMA_GET_TD_STS(PDMA) & (1 << SPI_FLASH_TX_DMA_CH)) == (1 << SPI_FLASH_TX_DMA_CH))
            {
                /* Clear the DMA transfer done flags */
                PDMA_CLR_TD_FLAG(PDMA,1 << SPI_FLASH_TX_DMA_CH);
                /* Disable SPI PDMA TX function */
                SPI_DISABLE_TX_PDMA(SPI_FLASH_PORT);

				TIMER_Delay(TIMER0,500);				
				SPI_FLASH_CS_HIGH;
				
                break;
            }

            /* Check the DMA transfer abort interrupt flag */
            if(u32RegValue & PDMA_INTSTS_ABTIF_Msk)
            {
                /* Get the target abort flag */
                u32Abort = PDMA_GET_ABORT_STS(PDMA);
                /* Clear the target abort flag */
                PDMA_CLR_ABORT_FLAG(PDMA,u32Abort);
                break;
            }
        }
    }

}

void SpiFlash_WriteEnable(void)
{
    // /CS: active
    SPI_FLASH_CS_LOW;

    // send Command: 0x06, Write enable
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x06);

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // /CS: de-active
    SPI_FLASH_CS_HIGH;
}

uint8_t SpiFlash_SendByte(uint8_t byte)
{

	/*!< Loop while DR register in not emplty */
	while (SPI_GetStatus(SPI_FLASH_PORT, SPI_TX_EMPTY_MASK) == FALSE);
	/*!< Send byte through the SPI1 peripheral */
	SPI_WRITE_TX(SPI_FLASH_PORT, byte);

	/*!< Wait to receive a byte */
	while (SPI_GetStatus(SPI_FLASH_PORT, SPI_RX_EMPTY_MASK) != FALSE);
	/*!< Return the byte read from the SPI bus */
	return SPI_READ_RX(SPI_FLASH_PORT);
}

uint16_t SpiFlash_ReadMidDid(void)
{
    uint8_t u8RxData[6], u8IDCnt = 0;

    // /CS: active
    SPI_FLASH_CS_LOW;

    // send Command: 0x90, Read Manufacturer/Device ID
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x90);

    // send 24-bit '0', dummy
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);

    // receive 16-bit
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // /CS: de-active
    SPI_FLASH_CS_HIGH;

    while(!SPI_GET_RX_FIFO_EMPTY_FLAG(SPI_FLASH_PORT))
        u8RxData[u8IDCnt ++] = SPI_READ_RX(SPI_FLASH_PORT);


    return ( (u8RxData[4]<<8) | u8RxData[5] );
}

void SpiFlash_ChipErase(void)
{
	SpiFlash_WriteEnable();

    //////////////////////////////////////////

    // /CS: active
    SPI_FLASH_CS_LOW;

    // send Command: 0xC7, Chip Erase
    SPI_WRITE_TX(SPI_FLASH_PORT, 0xC7);

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // /CS: de-active
    SPI_FLASH_CS_HIGH;

    SPI_ClearRxFIFO(SPI_FLASH_PORT);

}

void SpiFlash_SectorErase(uint32_t SectorAddr)
{

	SpiFlash_WriteEnable();

    // /CS: active
    SPI_FLASH_CS_LOW;

    SpiFlash_SendByte(0x20);

    // send 24-bit start address
    SpiFlash_SendByte((SectorAddr & 0xFF0000) >> 16);
    SpiFlash_SendByte((SectorAddr & 0xFF00) >> 8);
    SpiFlash_SendByte(SectorAddr & 0xFF);

    // /CS: de-active
    SPI_FLASH_CS_HIGH;

	SpiFlash_WaitReady();
}

uint8_t SpiFlash_ReadStatusReg(void)
{
    // /CS: active
    SPI_FLASH_CS_LOW;

    // send Command: 0x05, Read status register
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x05);

    // read status
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // /CS: de-active
    SPI_FLASH_CS_HIGH;

    // skip first rx data
    SPI_READ_RX(SPI_FLASH_PORT);

    return (SPI_READ_RX(SPI_FLASH_PORT) & 0xff);
	
}

void SpiFlash_WriteStatusReg(uint8_t u8Value)
{
	SpiFlash_WriteEnable();

    ///////////////////////////////////////

    // /CS: active
    SPI_FLASH_CS_LOW;

    // send Command: 0x01, Write status register
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x01);

    // write status
    SPI_WRITE_TX(SPI_FLASH_PORT, u8Value);

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // /CS: de-active
    SPI_FLASH_CS_HIGH;

}

void SpiFlash_WaitReady(void)
{
    uint8_t ReturnValue = 0;
//    uint32_t cnt = 0;
	
    do
    {
        ReturnValue = SpiFlash_ReadStatusReg();
        ReturnValue = ReturnValue & 1;

		#if 0	//debug purpose
		printf("BUSY counter : %4d\r\n" , cnt++);
//		printf(".");
		
		#endif

    }
    while(ReturnValue!=0);   // check the BUSY bit

//	#if (_debug_log_UART_ == 1)	//debug
//	printf("\r\n");
//	#endif
}

void SpiFlash_NormalPageProgram(uint32_t StartAddress, uint8_t *u8DataBuffer, uint16_t NumByteToWrite , uint8_t EnablePDMA)
{
    uint32_t i = 0;
	
	SpiFlash_WriteEnable();

    // /CS: active
    SPI_FLASH_CS_LOW;

    // send Command: 0x02, Page program
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x02);

    // send 24-bit start address
    SPI_WRITE_TX(SPI_FLASH_PORT, (StartAddress>>16) & 0xFF);
    SPI_WRITE_TX(SPI_FLASH_PORT, (StartAddress>>8)  & 0xFF);
    SPI_WRITE_TX(SPI_FLASH_PORT, StartAddress       & 0xFF);


    // write data
	if (EnablePDMA)
	{
		SpiFlash_TX_PDMA(u8DataBuffer , NumByteToWrite);
//		TIMER_Delay(TIMER0,500);		
	}
	else
	{
//	    while(1)
//	    {
//	        if(!SPI_GET_TX_FIFO_FULL_FLAG(SPI_FLASH_PORT))
//	        {
//				printf("%3d\r\n" , i);		
//	            SPI_WRITE_TX(SPI_FLASH_PORT, u8DataBuffer[i++]);
//	            if(i >= 255) break;				
//	        }
//	    }

	    for(i = 0 ; i < NumByteToWrite ; i++)
		{
			SpiFlash_SendByte(u8DataBuffer[i]);
		}

	    // wait tx finish
	    while(SPI_IS_BUSY(SPI_FLASH_PORT));
		TIMER_Delay(TIMER0,500);
					
	    // /CS: de-active
	    SPI_FLASH_CS_HIGH;
		
	}
    SPI_ClearRxFIFO(SPI_FLASH_PORT);

}

void SpiFlash_PageWrite(uint32_t page_no, uint8_t *u8DataBuffer , uint16_t NumByteToWrite , uint8_t EnablePDMA)
{
	SpiFlash_NormalPageProgram(page_no*SPI_FLASH_PAGE_BYTE , u8DataBuffer ,NumByteToWrite, EnablePDMA);
	SpiFlash_WaitReady();
}

void SpiFlash_SectorWrite(uint32_t sector_no, uint8_t *u8DataBuffer, uint16_t NumByteToWrite , uint8_t EnablePDMA)
{
	uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

	uint32_t WriteAddr = sector_no*SPI_FLASH_SECTOR_SIZE ;

	Addr = WriteAddr % SPI_FLASH_PAGE_BYTE;
	count = SPI_FLASH_PAGE_BYTE - Addr;
	NumOfPage =  NumByteToWrite / SPI_FLASH_PAGE_BYTE;
	NumOfSingle = NumByteToWrite % SPI_FLASH_PAGE_BYTE;

//	printf("%s : Addr:0x%3X,count:0x%2X,NumOfPage:0x%2X,NumOfSingle:0x%2X,\r\n" ,__FUNCTION__ , Addr,count,NumOfPage,NumOfSingle);

	SpiFlash_SectorErase(sector_no);

	if (Addr == 0) /*!< WriteAddr is sFLASH_PAGESIZE aligned  */
	{
		if (NumOfPage == 0) /*!< SPI_FLASH_SECTOR_SIZE < sFLASH_PAGESIZE */
		{
			SpiFlash_NormalPageProgram(WriteAddr,u8DataBuffer,  NumByteToWrite, EnablePDMA);
		}
		else /*!< SPI_FLASH_SECTOR_SIZE > sFLASH_PAGESIZE */
		{
			while (NumOfPage--)
			{
//				printf("WriteAddr:0x%3X,NumOfPage:0x%2X,NumOfSingle:0x%2X,\r\n" , WriteAddr,NumOfPage,NumOfSingle);
				TIMER_Delay(TIMER0,500);

				SpiFlash_NormalPageProgram(WriteAddr,u8DataBuffer,  SPI_FLASH_PAGE_BYTE, EnablePDMA);
				WriteAddr +=  SPI_FLASH_PAGE_BYTE;
				u8DataBuffer += SPI_FLASH_PAGE_BYTE;
			}

			SpiFlash_NormalPageProgram(WriteAddr,u8DataBuffer,  NumOfSingle, EnablePDMA);
		}
	}
	else /*!< WriteAddr is not sFLASH_PAGESIZE aligned  */
	{
		if (NumOfPage == 0) /*!< SPI_FLASH_SECTOR_SIZE < sFLASH_PAGESIZE */
		{
			if (NumOfSingle > count) /*!< (SPI_FLASH_SECTOR_SIZE + WriteAddr) > sFLASH_PAGESIZE */
			{
				temp = NumOfSingle - count;

				SpiFlash_NormalPageProgram(WriteAddr,u8DataBuffer,  count, EnablePDMA);
				WriteAddr +=  count;
				u8DataBuffer += count;

				SpiFlash_NormalPageProgram(WriteAddr,u8DataBuffer,  temp, EnablePDMA);
			}
			else
			{
				SpiFlash_NormalPageProgram(WriteAddr,u8DataBuffer,  NumByteToWrite, EnablePDMA);
			}
		}
		else /*!< SPI_FLASH_SECTOR_SIZE > sFLASH_PAGESIZE */
		{
			NumByteToWrite -= count;
			NumOfPage =  NumByteToWrite / SPI_FLASH_PAGE_BYTE;
			NumOfSingle = NumByteToWrite % SPI_FLASH_PAGE_BYTE;

			SpiFlash_NormalPageProgram(WriteAddr,u8DataBuffer,  count, EnablePDMA);
			WriteAddr +=  count;
			u8DataBuffer += count;

			while (NumOfPage--)
			{
				SpiFlash_NormalPageProgram(WriteAddr,u8DataBuffer,  SPI_FLASH_PAGE_BYTE, EnablePDMA);
				WriteAddr +=  SPI_FLASH_PAGE_BYTE;
				u8DataBuffer += SPI_FLASH_PAGE_BYTE;
			}

			if (NumOfSingle != 0)
			{
				SpiFlash_NormalPageProgram(WriteAddr,u8DataBuffer,  NumOfSingle, EnablePDMA);
			}
		}
	}

	SpiFlash_WaitReady();
	
}

void SpiFlash_NormalRead(uint32_t StartAddress, uint8_t *u8DataBuffer , uint16_t NumByteToRead, uint8_t EnablePDMA)
{
    uint32_t i = 0;

   // /CS: active
    SPI_FLASH_CS_LOW;

    // send Command: 0x03, Read data
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x03);

    // send 24-bit start address
    SPI_WRITE_TX(SPI_FLASH_PORT, (StartAddress>>16) & 0xFF);
    SPI_WRITE_TX(SPI_FLASH_PORT, (StartAddress>>8)  & 0xFF);
    SPI_WRITE_TX(SPI_FLASH_PORT, StartAddress       & 0xFF);

    while(SPI_IS_BUSY(SPI_FLASH_PORT));
    // clear RX buffer
    SPI_ClearRxFIFO(SPI_FLASH_PORT);

    // read data
    if (EnablePDMA)
    {
		SpiFlash_RX_PDMA(u8DataBuffer , NumByteToRead);
		TIMER_Delay(TIMER0,500);
    }
	else
	{
	    for(i = 0 ; i < NumByteToRead ; i++)
	    {
	        SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);
	        while(SPI_IS_BUSY(SPI_FLASH_PORT));
	        u8DataBuffer[i] = SPI_READ_RX(SPI_FLASH_PORT);
	    }

	    // wait tx finish
	    while(SPI_IS_BUSY(SPI_FLASH_PORT));
		TIMER_Delay(TIMER0,500);
					
	    // /CS: de-active
	    SPI_FLASH_CS_HIGH;		
	}	
}

void SpiFlash_PageRead(uint32_t page_no, uint8_t *u8DataBuffer , uint16_t NumByteToRead, uint8_t EnablePDMA)
{
	SpiFlash_NormalRead(page_no*SPI_FLASH_PAGE_BYTE , u8DataBuffer ,NumByteToRead, EnablePDMA);
}

void SpiFlash_SectorRead(uint32_t sector_no, uint8_t *u8DataBuffer , uint16_t NumByteToRead, uint8_t EnablePDMA)
{

	uint32_t ReadAddr = sector_no*SPI_FLASH_SECTOR_SIZE;
	uint16_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

	Addr = ReadAddr % SPI_FLASH_PAGE_BYTE;
	count = SPI_FLASH_PAGE_BYTE - Addr;
	NumOfPage =  NumByteToRead / SPI_FLASH_PAGE_BYTE;
	NumOfSingle = NumByteToRead % SPI_FLASH_PAGE_BYTE;

	if (Addr == 0) /*!< WriteAddr is sFLASH_PAGESIZE aligned  */
	{
		if (NumOfPage == 0) /*!< SPI_FLASH_SECTOR_SIZE < sFLASH_PAGESIZE */
		{
			SpiFlash_NormalRead(ReadAddr,u8DataBuffer,  NumByteToRead, EnablePDMA);
		}
		else /*!< SPI_FLASH_SECTOR_SIZE > sFLASH_PAGESIZE */
		{
			while (NumOfPage--)
			{
//				printf("ReadAddr:0x%3X,NumOfPage:0x%2X,NumOfSingle:0x%2X,\r\n" , ReadAddr,NumOfPage,NumOfSingle);
				TIMER_Delay(TIMER0,500);
				
				SpiFlash_NormalRead(ReadAddr,u8DataBuffer,  SPI_FLASH_PAGE_BYTE, EnablePDMA);
				ReadAddr +=  SPI_FLASH_PAGE_BYTE;
				u8DataBuffer += SPI_FLASH_PAGE_BYTE;
			}
			
//			TIMER_Delay(TIMER0,500);
//			SpiFlash_NormalRead(ReadAddr,u8DataBuffer,  NumOfSingle, EnablePDMA);
		}
	}
	else /*!< WriteAddr is not sFLASH_PAGESIZE aligned  */
	{
		if (NumOfPage == 0) /*!< SPI_FLASH_SECTOR_SIZE < sFLASH_PAGESIZE */
		{
			if (NumOfSingle > count) /*!< (SPI_FLASH_SECTOR_SIZE + WriteAddr) > sFLASH_PAGESIZE */
			{
				temp = NumOfSingle - count;

				SpiFlash_NormalRead(ReadAddr,u8DataBuffer,  count, EnablePDMA);
				ReadAddr +=  count;
				u8DataBuffer += count;

				SpiFlash_NormalRead(ReadAddr,u8DataBuffer,  temp, EnablePDMA);
			}
			else
			{
				SpiFlash_NormalRead(ReadAddr,u8DataBuffer,  NumByteToRead, EnablePDMA);
			}
		}
		else /*!< SPI_FLASH_SECTOR_SIZE > sFLASH_PAGESIZE */
		{
			NumByteToRead -= count;
			NumOfPage =  NumByteToRead / SPI_FLASH_PAGE_BYTE;
			NumOfSingle = NumByteToRead % SPI_FLASH_PAGE_BYTE;

			SpiFlash_NormalRead(ReadAddr,u8DataBuffer,  count, EnablePDMA);
			ReadAddr +=  count;
			u8DataBuffer += count;

			while (NumOfPage--)
			{
				SpiFlash_NormalRead(ReadAddr,u8DataBuffer,  SPI_FLASH_PAGE_BYTE, EnablePDMA);
				ReadAddr +=  SPI_FLASH_PAGE_BYTE;
				u8DataBuffer += SPI_FLASH_PAGE_BYTE;
			}

			if (NumOfSingle != 0)
			{
				SpiFlash_NormalRead(ReadAddr,u8DataBuffer,  NumOfSingle, EnablePDMA);
			}
		}
	}
 
}

void SpiFlash_Init(void)
{
    uint16_t u16ID = 0;
    uint16_t i = 0;

    /* Configure SPI_FLASH_PORT as a master, MSB first, 8-bit transaction, SPI Mode-0 timing, clock is 20MHz */
    SPI_Open(SPI_FLASH_PORT, SPI_MASTER, SPI_MODE_0, 8, SPI_FLASH_CLK_FREQ);

   	SYS_UnlockReg();	
	
    SYS->GPF_MFPL &= ~( SYS_GPF_MFPL_PF6MFP_Msk);	
    SYS->GPF_MFPL |=  SYS_GPF_MFPL_PF6MFP_GPIO;
	GPIO_SetMode(PF,BIT6,GPIO_MODE_OUTPUT);
	
    SYS_LockReg();	

    /* Disable auto SS function, control SS signal manually. */
    SPI_DisableAutoSS(SPI_FLASH_PORT);
	

	u16ID = SpiFlash_ReadMidDid();
	#if (_debug_log_UART_ == 1)	//debug
	printf("ID : 0x%2X\r\n" , u16ID);
	#endif

	
	//initial TX , RX data
    for (i=0; i < SPI_FLASH_PAGE_BYTE; i++)
    {
        TxBuffer[i] = 0xFF;
        RxBuffer[i] = 0xFF;
    }

    for (i=0; i < SPI_FLASH_SECTOR_SIZE; i++)
    {
        Tx4KBuffer[i] = 0xFF;
        Rx4KBuffer[i] = 0xFF;
    }
	

}



