#include "PL1167.h"
extern void delay(__IO uint32_t delay_cnt);
//-----------------------------------------------------------------------------
//变量定义
//-----------------------------------------------------------------------------
#ifdef Send_Mode
unsigned int address_code;
unsigned char go_sleep,fun_code,data_code,flag_RFsend;
#endif
#ifdef Receive_Mode
unsigned char fun_code_rec,data_code_rec,flag_recv;
unsigned int address_code_rec;
#endif

unsigned char DAT[gLength+1]={0,};          // First Byte:HW length. Runtime code must set this first byte accordingly.
unsigned char gReg7_high,gReg7_low;    // 寄存器7读写暂存变量
unsigned char Auto_Ack_Select=0;            // 自动应答开关
unsigned int tempreg;
unsigned char timeout_cnt;
_Bool update;
//unsigned char t_1ms;
uint8_t rx[6],tx[6],send_size,recv_size;
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
    while((SPI2->CR1 & SPI_CR1_SPE)!=0){};//等待发送接收完
        tx[0]=addr;
        tx[1]=v1;
        tx[2]=v2;
        send_size=3;
        recv_size=3;
    SPICS_L;		
	//使能SPI2
	SPI2->CR1 |= SPI_CR1_SPE;        
}

//-----------------------------------------------------------------------------
// SPI 16位寄存器读操作
//-----------------------------------------------------------------------------
unsigned int Reg_read16(unsigned char addr)
{
        unsigned int value =0;
        
    Reg_write16(addr | REG_RD, 0xff, 0xff);
    
    while((SPI2->CR1 & SPI_CR1_SPE)!=0){};//等待发送接收完
        value = rx[1];
        value<<=8;
        value|=rx[2];
        
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
//-----------------------------------------------------------------------------------
// PL1167 频道设置函数
// Frequency = 2402 + channel;
// 最大值Frequency < 2480 MHz
//-----------------------------------------------------------------------------------
void Set_RF_Channel(unsigned char channel)
{
        GetReg7FromChip();
        gReg7_low &= 0x80;
        gReg7_low |= channel;
        Reg_write16(0x07, gReg7_high, gReg7_low);
}

//-----------------------------------------------------------------------------
// RF 初始化
//-----------------------------------------------------------------------------
void PL1167_Init(void)
{

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
    
    while((SPI2->CR1 & SPI_CR1_SPE)!=0){};//等待发送接收完
        tx[0]=0x32;
        for(j=0; j<(bytes+1); j++)
        {
                tx[j+1]=*ptr++;
        }
        send_size=bytes+2;
        recv_size=bytes+2;
    SPICS_L;		
	//使能SPI2
	SPI2->CR1 |= SPI_CR1_SPE;         
        
        Reg_write16(0x07, (0x00|0x01), gReg7_low&0x7f);//TX Enable

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
        flag_recv=0;
    
        Reg_write16(0x34,0,0x80);                   // reset RX FIFO point
        Reg_write16(0x07, 0x00, (gReg7_low|0x80));  //enter RX mode
		
        while((SPI2->CR1 & SPI_CR1_SPE)!=0){};//等待发送接收完
        //SPICS_H;
        timeout_cnt=0;//Set timeout;
        while (PKT_IS_LOW)
        {
						msec(1);
						timeout_cnt++;
						if(timeout_cnt>=10)
						{
                                LED2_OFF;
								timeout=1;
								goto time_out;
						}
						else timeout=0;
        };
        LED2_ON;

Had_Rec:
        //Read FIFO datas
    //while((SPI2->CR1 & SPI_CR1_SPE)!=0){};//等待发送接收完
        flag_recv=1;
        tx[0]=0x32 | REG_RD;
        tx[1]=0xff;
        send_size=2;
        recv_size=2;
    SPICS_L;		
	//使能SPI2
	SPI2->CR1 |= SPI_CR1_SPE;

    while((SPI2->CR1 & SPI_CR1_SPE)!=0){};//等待发送接收完
        flag_recv=0;//加上它，接收完DAT[0]个数据后SCK拉高，MISO为低电平；否则，接收完DAT[0]个数据后SCK依然保持低电平，而MISO为高电平
        DAT[0] = rx[1];
        if(DAT[0]>20) DAT[0]=20;            //Maybe first Byte is error;
        for(j=0; j<DAT[0]; j++)             // SPIF0 take 200us to read/write 33 bytes
        {
            tx[j] = 0xff;
        }
        send_size=DAT[0];
        recv_size=DAT[0];        
    SPICS_L;		
	//使能SPI2
	SPI2->CR1 |= SPI_CR1_SPE; 
 
    while((SPI2->CR1 & SPI_CR1_SPE)!=0){};//等待发送接收完  
        for(j=1; j<(DAT[0]+1); j++)             // SPIF0 take 200us to read/write 33 bytes
        {
            DAT[j] = rx[j-1];
        }      

time_out:
        if(!timeout)                        //Check if the recevied data is valid;
        {
                delay_msec(10);
        }
        else update =0;
}

//void PL1167_Init(void)
//{
//	unsigned char channel_temp;
//	RF_init();									// 初始化 RF module;
//	channel_temp=0x30;
//	Set_RF_Channel(channel_temp);
//	Auto_Ack();
//	SetDatasetFirstByte();
//}

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
