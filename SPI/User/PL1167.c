#include "main.h"
extern void delay(__IO uint32_t delay_cnt);

unsigned int address_code;
#ifdef Send_Mode
unsigned char go_sleep,fun_code,data_code,flag_RFsend;
#endif
#ifdef Receive_Mode
unsigned char fun_code_rec,data_code_rec;
unsigned int address_code_rec;
#endif

unsigned char DAT[gLength+1]={0,};          // First Byte:HW length. Runtime code must set this first byte accordingly.
unsigned char gReg7_high,gReg7_low;    // 寄存器7读写暂存变量
unsigned char Auto_Ack_Select=0;            // 自动应答开关
unsigned int tempreg;
unsigned char timeout_cnt;
_Bool update;
unsigned char tx[5],rx[5];
unsigned char send_size,receive_size;
//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// 微秒延时
//------------------------------------------------------------------------------------
void msec(unsigned int x)
{
        delay(x);
}

void pdelay (unsigned char t)
{
        while(t!=0)
                t--;
}

void delay_msec(unsigned int x)
{
        unsigned char i,j;
        while(x--)
        {
                i=11,j=50;
                while(i--)
                        while(j--) ;
        }
}


//-----------------------------------------------------------------------------
// SPI 16位寄存器写操作
//-----------------------------------------------------------------------------
void Reg_write16(unsigned char addr, unsigned char v1, unsigned char v2)
{
				CS_L;
	
        //等待上一次发送完
        while((SPI2->SR & SPI_SR_FTLVL)!=0);
        while((SPI2->SR & SPI_SR_BSY)!=0);

        tx[0] = addr;
        tx[1] = v1;
        tx[2] = v2;
        send_size=3; 

        SPI2->CR2 |= SPI_CR2_TXEIE;
	
				CS_H;
   
        if(addr < 0x20)
                pdelay(RF_GAP); 
}
//-----------------------------------------------------------------------------
// SPI 16位寄存器读操作
//-----------------------------------------------------------------------------
unsigned int Reg_read16(unsigned char addr)
{
        //unsigned char clr;
        unsigned int value =0;
	
				CS_L;
	
        //等待上一次发送完
        while((SPI2->SR & SPI_SR_FTLVL)!=0);
        while((SPI2->SR & SPI_SR_BSY)!=0);   
        tx[0] = addr | REG_RD;
        send_size=1; 
        SPI2->CR2 |= SPI_CR2_TXEIE;
   
        if(addr < 0x20)
        {
                pdelay(RF_GAP);
        }
        //等待发送完毕
        while((SPI2->CR2 & SPI_CR2_TXEIE)!=0);
        while((SPI2->SR & SPI_SR_FTLVL)!=0);
        while((SPI2->SR & SPI_SR_BSY)!=0);           
        
        //清RXFIFO
        //while((SPI2->SR & SPI_SR_FRLVL)!=0)clr=SPI2->DR;
        //接收
        SPI2->CR2 |= SPI_CR2_RXNEIE;
        receive_size=2;
        //等待接收完
        while((SPI2->CR2 & SPI_CR2_RXNEIE)!=0);
        value = rx[0];
        value<<=8;
        value |= rx[1];
				
				CS_H;

        return value;
}
//获取寄存器 7 的值
void GetReg7FromChip(void)
{
        unsigned int temp;

        temp= Reg_read16(0x07);
        gReg7_high= (unsigned char) ((temp & 0xff00) >> 8);
        gReg7_low = (unsigned char) (temp & 0x00ff);
}
//-----------------------------------------------------------------------------
// RF 初始化
//-----------------------------------------------------------------------------
void PL1167_Init(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  //打开Port A时钟
	GPIOA->MODER &= ~GPIO_MODER_MODER1;//PA1输入
	GPIOA->MODER |= GPIO_MODER_MODER0_0;//PA0输出
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_1 | GPIO_OTYPER_OT_0);//PA1 PA0推挽输出
	//GPIOA->PUPDR |= GPIO_PUPDR_PUPDR1_1 | GPIO_PUPDR_PUPDR0_1;//PA1 PA0下拉
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR1_0 | GPIO_PUPDR_PUPDR0_0;//PA1 PA0上拉
   
        RFRST_H;
        msec(10);
        RFRST_L;
        msec(10);
        RFRST_H;               //Enable PL1167
        msec(10);               //delay 5ms to let chip stable
        Reg_write16(0x00,0x6f,0xe0);
        Reg_write16(0x01,0x56,0x81);
        Reg_write16(0x02,0x66,0x17);
        Reg_write16(0x04,0x9c,0xc9);
        Reg_write16(0x05,0x66,0x37);
        Reg_write16(0x07,0x00,0x30);
        Reg_write16(0x08,0x6c,0x90);
        Reg_write16(0x09,0x18,0x40);
        Reg_write16(0x0a,0x7f,0xfd);
        Reg_write16(0x0b,0x00,0x08);
        Reg_write16(0x0c,0x00,0x00);
        Reg_write16(0x0d,0x48,0xbd);
        Reg_write16(0x16,0x00,0xff);
        Reg_write16(0x17,0x80,0x05);
        Reg_write16(0x18,0x00,0x67);
        Reg_write16(0x19,0x16,0x59);
        Reg_write16(0x1a,0x19,0xe0);
        Reg_write16(0x1b,0x13,0x00);
        Reg_write16(0x1c,0x18,0x00);
        Reg_write16(0x20,0x48,0x00);
        Reg_write16(0x21,0x3f,0xc7);
        Reg_write16(0x22,0x20,0x00);
        Reg_write16(0x23,0x03,0x80);
//Begin Syncword					//不同的Syncword可以用以区分不同的设备
        Reg_write16(0x24,0x42,0x31);
        Reg_write16(0x25,0x86,0x75);
        Reg_write16(0x26,0x9a,0x0b);
        Reg_write16(0x27,0xde,0xcf);
//End syncword
        Reg_write16(0x28,0x44,0x01);
        Reg_write16(0x29,0xb0,0x00);
        Reg_write16(0x2a,0xfd,0xb0);
        Reg_write16(0x2b,0x00,0x0f);
        msec(100);              //delay 100ms to let chip for operation
        GetReg7FromChip();
}
void RF_EnterSleep(void)
{
        Reg_write16(0x23,0x43,0x80);
}

//*================================================================================
//*Fuction  : TX packet via FIFO
//*Parameter: packet data and length of payload sent
//return    : none
//*================================================================================
void TX_packet(unsigned char *ptr,unsigned char bytes) //only tx loop
{

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////Because MCU is too fast,MCU write 60 bytes every time instead of 64 bytes lest the FIFO isn't over written
///////Pls refer to register56[15:12]:TX_FIFO_threshold
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned char j;

        Reg_write16(0x34,0x80,0);// reset TX FIFO point
				
				CS_L;
	
        //等待上一次发送完
        while((SPI2->SR & SPI_SR_FTLVL)!=0);
        while((SPI2->SR & SPI_SR_BSY)!=0);   
        tx[0] = 0x32;     //Fill data to FIFO;
        send_size=1; 
        SPI2->CR2 |= SPI_CR2_TXEIE;
        
        //等待发送完
        while((SPI2->CR2 & SPI_CR2_TXEIE)!=0);
        while((SPI2->SR & SPI_SR_FTLVL)!=0);
        while((SPI2->SR & SPI_SR_BSY)!=0);   
   
        for(j=0; j<(bytes+1); j++)
        {
                tx[j] = *ptr++;
        }
        SPI2->CR2 |= SPI_CR2_TXEIE;
				
        CS_H;
				
        Reg_write16(0x07, (0x00|0x01), gReg7_low&0x7f);//TX Enable
//	pdelay(10);
        //refresh_watchdog;
        while(PKT_IS_LOW) {LED2_ON;}
				LED2_OFF;
				
        //refresh_watchdog;
        Reg_write16(0x07, (0x00&0xfe), gReg7_low&0x7f); //TX Disable
}
//*=============================================================================================================
//* PS: First byte received may be wrong, so FIFO_flag may not turn high as expectation
//* Function: RX packet from FIFO
//* Parameter: two;EnableTimeout=1, enable timeout function; bytes, how much bytes you wanted to receive
//* return   :
//*=============================================================================================================
void RX_packet(void)
{
        unsigned char j;
        unsigned char timeout=0;

        Reg_write16(0x34,0,0x80);                   // reset RX FIFO point
        Reg_write16(0x07, 0x00, (gReg7_low|0x80));  //enter RX mode
				
				CS_H;
	
        timeout_cnt=0;//Set timeout;
        while (PKT_IS_LOW)
        {
						msec(1);
						timeout_cnt++;
						if(timeout_cnt>=10)
						{
								timeout=1;
								goto time_out;
						}
						else timeout=0;
        };

Had_Rec:
        //等待上一次发送完
        while((SPI2->SR & SPI_SR_FTLVL)!=0);
        while((SPI2->SR & SPI_SR_BSY)!=0);   
        tx[0] = 0x32 | REG_RD;     //Read FIFO datas
        send_size=1; 
        SPI2->CR2 |= SPI_CR2_TXEIE;        
        
        //等待发送完
        while((SPI2->CR2 & SPI_CR2_TXEIE)!=0);
        while((SPI2->SR & SPI_SR_FTLVL)!=0);
        while((SPI2->SR & SPI_SR_BSY)!=0);   
        
        //接收
        SPI2->CR2 |= SPI_CR2_RXNEIE;
        receive_size=2;
        //等待接收完
        while((SPI2->CR2 & SPI_CR2_RXNEIE)!=0);        
        DAT[0] = rx[0];
        if(DAT[0]>20) DAT[0]=20;            //Maybe first Byte is error;
        for(j=1; j<(DAT[0]+1); j++)             // SPIF0 take 200us to read/write 33 bytes
                DAT[j] = rx[j];

time_out:
        if(!timeout)                        //Check if the recevied data is valid;
        {
                delay_msec(10);
        }
        else update =0;
}
#ifdef Send_Mode
void Send_DATA(unsigned char fun_code, unsigned char data_code)
{
        unsigned char j;
        if(go_sleep==1) PL1167_Init();
        go_sleep=0;

        //4字节：2字节地址码+1字节功能+1字节数据
        DAT[0]=4;
        DAT[1]=address_code>>8;
        DAT[2]=address_code;
        DAT[3]=fun_code;    //0风扇控制，1音量控制
        DAT[4]=data_code;    //风扇或音量挡位

        TX_packet(DAT, DAT[0]);

        for(j=0; j<(DAT[0]+1); j++)
        {
                DAT[j]=0;
        }
        //delay_msec(20);
}
#endif

#ifdef Receive_Mode
void Receive_DATA(void)
{
        RX_packet();
        address_code_rec=DAT[1]<<8;
        address_code_rec+=DAT[2];
        fun_code_rec=DAT[3];//功能码
        data_code_rec=DAT[4];//数据
}
#endif
