#include "flash_as_eeprom.h"

/**
  * Unlocks the FLASH control register and program memory access.
  */
void FLASH_Unlock(void)
{
  if((FLASH->CR & FLASH_CR_LOCK) != 0)//LOCK=1,flash被锁定，保护；需要解锁，才能进行flash操作
  {
    /* Unlocking the program memory access */
    FLASH->KEYR = FLASH_FKEY1;
    FLASH->KEYR = FLASH_FKEY2;
  }
}
/**
  * Locks the Program memory access.
  */
void FLASH_Lock(void)
{
  /* Set the LOCK Bit to lock the FLASH control register and program memory access */
  FLASH->CR |= FLASH_CR_LOCK;//锁定FLASH
}
/**
  * @brief  Returns the FLASH Status.
  * @param  None
  * @retval FLASH Status: The returned value can be: 
  *         FLASH_BUSY, FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP or FLASH_COMPLETE.
  */
FLASH_Status FLASH_GetStatus(void)
{
  FLASH_Status FLASHstatus = FLASH_COMPLETE;
  
  if((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY) 
  {
    FLASHstatus = FLASH_BUSY;//操作进行中
  }
  else 
  {  
    if((FLASH->SR & FLASH_SR_WRPRTERR)!= 0)
    { 
      FLASHstatus = FLASH_ERROR_WRP;//写保护错误
    }
    else 
    {
      if((FLASH->SR & FLASH_SR_PGERR) != 0)
      {
        FLASHstatus = FLASH_ERROR_PROGRAM;//编程错误
      }
      else
      {
        FLASHstatus = FLASH_COMPLETE;
      }
    }
  }
  /* Return the FLASH Status */
  return FLASHstatus;
}
/**
  * @brief  Waits for a FLASH operation to complete or a TIMEOUT to occur.
  * @param  Timeout: FLASH programming Timeout
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, 
  *         FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout)
{ 
  FLASH_Status status = FLASH_COMPLETE;
   
  /* Check for the FLASH Status */
  status = FLASH_GetStatus();
  
  /* Wait for a FLASH operation to complete or a TIMEOUT to occur */
  while((status == FLASH_BUSY) && (Timeout != 0x00))//等待BSY=0，flash操作完成
  {
    status = FLASH_GetStatus();
    Timeout--;
  }
  
  if(Timeout == 0x00 )
  {
    status = FLASH_TIMEOUT;
  }
  /* Return the operation status */
  return status;
}
/**
  * @brief  Erases a specified page in program memory.
  * @note   To correctly run this function, the FLASH_Unlock() function must be called before.
  * @note   Call the FLASH_Lock() to disable the flash memory access (recommended
  *         to protect the FLASH memory against possible unwanted operation)
  * @param  Page_Address: The page address in program memory to be erased.
  * @note   A Page is erased in the Program memory only if the address to load 
  *         is the start address of a page (multiple of 1024 bytes).
  * @retval FLASH Status: The returned value can be: 
  *         FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_ErasePage(uint32_t Page_Address)
{
  FLASH_Status status = FLASH_COMPLETE;

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);
  
  if(status == FLASH_COMPLETE)
  { 
    /* If the previous operation is completed, proceed to erase the page */
    FLASH->CR |= FLASH_CR_PER;//使能页擦除
    FLASH->AR  = Page_Address;//页地址
    FLASH->CR |= FLASH_CR_STRT;//开始页擦除
    
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);
    
    /* Disable the PER Bit */
    FLASH->CR &= ~FLASH_CR_PER;
  }
    
  /* Return the Erase Status */
  return status;
}
/**
  * @brief  Programs a half word at a specified address.
  * @note   To correctly run this function, the FLASH_Unlock() function must be called before.
  * @note   Call the FLASH_Lock() to disable the flash memory access (recommended
  *         to protect the FLASH memory against possible unwanted operation)
  * @param  Address: specifies the address to be programmed.
  * @param  Data: specifies the data to be programmed.
  * @retval FLASH Status: The returned value can be: FLASH_ERROR_PG,
  *         FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT. 
  */
FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data)
{
  FLASH_Status status = FLASH_COMPLETE;

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);
  
  if(status == FLASH_COMPLETE)
  {
    /* If the previous operation is completed, proceed to program the new data */
    FLASH->CR |= FLASH_CR_PG;//使能flash编程
  
    *(__IO uint16_t*)Address = Data;//向地址Address处写半字数据  

    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);
    
    /* Disable the PG Bit */
    FLASH->CR &= ~FLASH_CR_PG;//关闭flash编程使能
  } 
  
  /* Return the Program Status */
  return status;
}


FLASH_Status write_flash_HalfWord(uint32_t flash_addr, uint16_t data)
{
	FLASH_Status status = FLASH_COMPLETE;
	
  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);	
	
	if(status == FLASH_COMPLETE)
	{
		while((FLASH->CR&FLASH_CR_LOCK)!=0)
		{
			FLASH_Unlock();//LOCK=1,flash被锁定，保护；需要解锁，才能进行flash操作
		};
		if((FLASH->CR&FLASH_CR_LOCK)==0)//LOCK=0,flash未锁定，可进行操作
		{
			//页擦除 page64
			status = FLASH_ErasePage(EEPROM_ADR_INI);
			if(status == FLASH_COMPLETE)
			{
				flash_addr=flash_addr+EEPROM_ADR_INI;
				status = FLASH_ProgramHalfWord(flash_addr,data);//向地址flash_addr处写半字数据 
			}
		}
	}
	
	FLASH_Lock();//锁定FLASH
	
	return status;
}

uint16_t read_flash_HalfWord(uint32_t flash_addr)
{
	uint16_t data;
	flash_addr=flash_addr+EEPROM_ADR_INI;
	data=*(__IO uint16_t*)(flash_addr);
	return data;
}
