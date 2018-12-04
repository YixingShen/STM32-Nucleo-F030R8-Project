#include "usart.h"

#define BUFFERSIZE   8
__IO uint8_t Buffer[BUFFERSIZE];

/**
  * 函数功能: 串口引脚配置.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void USART_GPIO_Init(void)
{
    /*PA9-USART1_TX*/
    RCC->AHBENR |= USART_GPIO_RCC; //打开GPIO时钟
    USART_GPIO_PORT->MODER |= GPIO_MODER_MODER9_1; //复用功能
    USART_GPIO_PORT->OTYPER &= (~GPIO_OTYPER_OT_9); //推挽输出
    USART_GPIO_PORT->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR9; //高速
    USART_GPIO_PORT->PUPDR |= GPIO_PUPDR_PUPDR9_0; //上拉
    USART_GPIO_PORT->AFR[1] |= (1<<4);//AF1
    
    /*PA10-USART1_RX*/
    RCC->AHBENR |= USART_GPIO_RCC; //打开GPIO时钟
    USART_GPIO_PORT->MODER |= GPIO_MODER_MODER10_1; //复用功能
    USART_GPIO_PORT->OTYPER &= (~GPIO_OTYPER_OT_10); //推挽输出
    USART_GPIO_PORT->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR10; //高速
    USART_GPIO_PORT->PUPDR |= GPIO_PUPDR_PUPDR10_0; //上拉    
    USART_GPIO_PORT->AFR[1] |= (1<<8);//AF1
}


/**
  * 函数功能: 串口参数配置.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void USART_Config(void)
{
    //115200-8-N-1
    RCC->APB2ENR |= USART_RCC; //打开USART外设时钟
    RCC->CFGR3 |= RCC_CFGR3_USART1SW_SYSCLK;//USART1时钟源选择SYSCLK
    
    //oversampling by 16, Tx/Rx baud = fCK/USARTDIV, BRR[15:0] = USARTDIV[15:0]; 
    //oversampling by 8, Tx/Rx baud = (2*fCK)/USARTDIV, BRR[15:4] = USARTDIV[15:4], BRR[3]=0, BRR[2:0]=USARTDIV[3:0]>>1; 
    USART->CR1 &= (~USART_CR1_OVER8); //oversampling by 16
    USART->BRR = (SystemCoreClock/BaudRate);
    USART->CR1 &= (~USART_CR1_M); //1 Start bit, 8 data bits, n stop bits
    USART->CR2 &= (~USART_CR2_STOP); //1 stop bit
    USART->CR1 &= (~USART_CR1_PCE); //无校验
    USART->CR1 |= (USART_CR1_TE | USART_CR1_RE); //使能发送和接收
    USART->ICR |= USART_ICR_TCCF; //清除接收完成标志
    USART->CR1 |= USART_CR1_RXNEIE; //使能接收非空中断
    USART->CR1 |= USART_CR1_UE; //使能USART
    
    /* 配置串口中断并使能，需要放在UART Init函数后执行修改才有效 */    
    NVIC_SetPriority(USART1_IRQn, 0);
    NVIC_EnableIRQ(USART1_IRQn);     
}

/**
  * 函数功能: 串口中断服务函数.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void USART1_IRQHandler(void)
{
    if((USART->ISR & USART_ISR_ORE) != RESET)
    {
       USART->ICR |= USART_ICR_ORECF;
    }
    if((USART->ISR & USART_ISR_RXNE) != RESET)
    {
        Buffer[0]=USART->RDR;
    }
}

/**
  * 函数功能: 重定向c库函数printf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART->TDR = (uint8_t) ch;
    /* Loop until the end of transmission */
    while ((USART->ISR & USART_ISR_TC) == RESET)
    {}
    return ch;
}

/**
  * 函数功能: 重定向c库函数getchar,scanf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：重定向到Buffer,使用中断接收数据到Buffer,然后读取.防止
  * 来不及数据接收导致丢失数据.
  */
int fgetc(FILE * f)
{
  static uint8_t i = 0;
  uint8_t ch = 0;
  while(Buffer[i] == 0)
  {
    i++;
    if(i==BUFFERSIZE)
      i = 0;
  }
  ch = Buffer[i];
  Buffer[i] = 0;
  return ch;
}


