//���������ʱ�䣺2013/1/17
/**
  ******************************************************************************
  * @file /src/main.c
  * @author  xd.wu
  * @brief  Main program body
  ******************************************************************************
  *
  */

/* Includes ------------------------------------------------------------------*/

#include "includes.h"
#include "gpio.h"
#include "system_time.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//xGlobal_TypeDef glvar;
//CanRxMsg RxMessage;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @brief RTOS Tasks
  * @{
  */
	




void Display_Serve(void* pvParam)
{
	while(1)
	{
		GPIO_Write(GPIOA,GPIO_BSRR_BS_5);
		vTaskDelay(1000);
	}
}

void Display_Serve1(void* pvParam)
{	
	while(1)
	{
		GPIO_Write(GPIOA,0x0020);
		vTaskDelay(500);
        GPIO_Write(GPIOA,0x0000);
        vTaskDelay(1000);
	}
}
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */ 
int main(void)
{
	GPIO_Config();

	/* Create tasks �������� */
	xTaskCreate(Display_Serve1, (char const*)"Display1", 100, NULL, 2, NULL);	
	//xTaskCreate(Display_Serve, (char const*)"Display", 100, NULL, 1, NULL);


	/* Start the FreeRTOS scheduler ���������� */
	vTaskStartScheduler(); //�������Զ�����������


	/* ���һ��������main()������Ӧ�û�ִ�е���������ִ�е�����ܿ������ڴ�ѿռ䲻�㵼�¿���
	�����޷��������������н�����������ڴ���������Ϣ */
	for(;;);
}

#ifdef  USE_FULL_ASSERT
/** ��#ifdef DEBUG�滻Ϊ#ifdef USE_FULL_ASSERT
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  //#ifdef __DEBUG_Example
    //printf("Wrong parameters value: file %s on line %d\r\n", file, line);
  //#else
    /* Infinite loop */
    while (1)
    {
    }
  //#endif
}
#endif
/**
  * @}
  */


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
