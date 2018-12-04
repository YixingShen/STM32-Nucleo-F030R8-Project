#include "usart.h"

#define BUFFERSIZE   8
__IO uint8_t Buffer[BUFFERSIZE];

/**
  * ��������: ������������.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void USART_GPIO_Init(void)
{
    /*PA9-USART1_TX*/
    RCC->AHBENR |= USART_GPIO_RCC; //��GPIOʱ��
    USART_GPIO_PORT->MODER |= GPIO_MODER_MODER9_1; //���ù���
    USART_GPIO_PORT->OTYPER &= (~GPIO_OTYPER_OT_9); //�������
    USART_GPIO_PORT->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR9; //����
    USART_GPIO_PORT->PUPDR |= GPIO_PUPDR_PUPDR9_0; //����
    USART_GPIO_PORT->AFR[1] |= (1<<4);//AF1
    
    /*PA10-USART1_RX*/
    RCC->AHBENR |= USART_GPIO_RCC; //��GPIOʱ��
    USART_GPIO_PORT->MODER |= GPIO_MODER_MODER10_1; //���ù���
    USART_GPIO_PORT->OTYPER &= (~GPIO_OTYPER_OT_10); //�������
    USART_GPIO_PORT->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR10; //����
    USART_GPIO_PORT->PUPDR |= GPIO_PUPDR_PUPDR10_0; //����    
    USART_GPIO_PORT->AFR[1] |= (1<<8);//AF1
}


/**
  * ��������: ���ڲ�������.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void USART_Config(void)
{
    //115200-8-N-1
    RCC->APB2ENR |= USART_RCC; //��USART����ʱ��
    RCC->CFGR3 |= RCC_CFGR3_USART1SW_SYSCLK;//USART1ʱ��Դѡ��SYSCLK
    
    //oversampling by 16, Tx/Rx baud = fCK/USARTDIV, BRR[15:0] = USARTDIV[15:0]; 
    //oversampling by 8, Tx/Rx baud = (2*fCK)/USARTDIV, BRR[15:4] = USARTDIV[15:4], BRR[3]=0, BRR[2:0]=USARTDIV[3:0]>>1; 
    USART->CR1 &= (~USART_CR1_OVER8); //oversampling by 16
    USART->BRR = (SystemCoreClock/BaudRate);
    USART->CR1 &= (~USART_CR1_M); //1 Start bit, 8 data bits, n stop bits
    USART->CR2 &= (~USART_CR2_STOP); //1 stop bit
    USART->CR1 &= (~USART_CR1_PCE); //��У��
    USART->CR1 |= (USART_CR1_TE | USART_CR1_RE); //ʹ�ܷ��ͺͽ���
    USART->ICR |= USART_ICR_TCCF; //���������ɱ�־
    USART->CR1 |= USART_CR1_RXNEIE; //ʹ�ܽ��շǿ��ж�
    USART->CR1 |= USART_CR1_UE; //ʹ��USART
    
    /* ���ô����жϲ�ʹ�ܣ���Ҫ����UART Init������ִ���޸Ĳ���Ч */    
    NVIC_SetPriority(USART1_IRQn, 0);
    NVIC_EnableIRQ(USART1_IRQn);     
}

/**
  * ��������: �����жϷ�����.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
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
  * ��������: �ض���c�⺯��printf��DEBUG_USARTx
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
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
  * ��������: �ض���c�⺯��getchar,scanf��DEBUG_USARTx
  * �������: ��
  * �� �� ֵ: ��
  * ˵    �����ض���Buffer,ʹ���жϽ������ݵ�Buffer,Ȼ���ȡ.��ֹ
  * ���������ݽ��յ��¶�ʧ����.
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


