#include "spi1.h"

/**
  * @brief Read or write an byte from or to SPI bus.
  * @retval chByte: Receive an byte from SPI bus
  */
uint8_t spi_read_write_byte(uint8_t chByte)
{
    uint8_t chRetry=0, chTemp=0;
    uint32_t spixbase = 0x00;
    spixbase = (uint32_t)SPI1; 
    spixbase += 0x0C;	
    
    while((SPI1->SR & SPI_SR_TXE)==0)//wait transmit buffer empty
    {
        if(++chRetry>200)return 0;//timeout
    }
    *(__IO uint8_t *) spixbase = chByte;//SPI1->DR=spi_tValue;	
    
    chRetry=0;
    while((SPI1->SR  & SPI_SR_RXNE)==0)//wait rx buffer not empty
    {
        if(++chRetry>200)return 0;//timeout
    }
    chTemp=*(__IO uint8_t *) spixbase;//SPI1->DR;
    //wait until spi not busy
    while((SPI1->SR  & SPI_SR_BSY)!=0){}
    
    return chTemp;
}
