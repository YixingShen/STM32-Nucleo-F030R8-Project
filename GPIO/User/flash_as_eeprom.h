#include "stm32f0xx.h"  

#define EEPROM_ADR_INI				(uint32_t)0x0800FC00 //0x0800FC00 - 0x0800FFFF 1Kbyte Page63 作为eeprom使用

/******************  FLASH Keys  **********************************************/
#define FLASH_FKEY1                          ((uint32_t)0x45670123)        /*!< Flash program erase key1 */
#define FLASH_FKEY2                          ((uint32_t)0xCDEF89AB)        /*!< Flash program erase key2: used with FLASH_PEKEY1
                                                                                to unlock the write access to the FPEC. */
/** FLASH_Timeout_definition */ 
#define FLASH_ER_PRG_TIMEOUT         ((uint32_t)0x000B0000)

/** 
  * @brief  FLASH Status
  */ 
typedef enum
{
  FLASH_BUSY = 1,
  FLASH_ERROR_WRP,
  FLASH_ERROR_PROGRAM,
  FLASH_COMPLETE,
  FLASH_TIMEOUT
}FLASH_Status;

