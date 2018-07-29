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
unsigned char fun_code_rec,data_code_rec;
unsigned int address_code_rec;
#endif

unsigned char DAT[gLength+1]={0,};          // First Byte:HW length. Runtime code must set this first byte accordingly.
unsigned char gReg7_high,gReg7_low;    // 寄存器7读写暂存变量
unsigned char Auto_Ack_Select=0;            // 自动应答开关
unsigned int tempreg;
unsigned char timeout_cnt;
_Bool update;
//unsigned char t_1ms;
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
// SPI 写操作
//-----------------------------------------------------------------------------
void spi_write(unsigned char spi_bValue)
{
				uint32_t spixbase = 0x00;
				spixbase = (uint32_t)SPI2; 
				spixbase += 0x0C;		
	SPICS_L;
				while((SPI2->SR & SPI_SR_TXE)==0);//等待发送区空
				*(__IO uint8_t *) spixbase = spi_bValue;//SPI2->DR=spi_bValue;	 	  		//发送一个字节
				//while((SPI2->SR & SPI_SR_FTLVL)==0);
}
//-----------------------------------------------------------------------------
// SPI 读操作
//-----------------------------------------------------------------------------
unsigned char   spi_read(void)
{
        unsigned char spi_bValue;
	SPICS_L;
        while((SPI2->SR  & SPI_SR_RXNE)==0);//等待接收完一个字节
				spi_bValue=SPI2->DR;
        return spi_bValue;
}
//-----------------------------------------------------------------------------
// SPI 读写操作
//-----------------------------------------------------------------------------
unsigned char spi_write_read(unsigned char spi_tValue)
{
        unsigned char spi_rValue;
				uint32_t spixbase = 0x00;
				spixbase = (uint32_t)SPI2; 
				spixbase += 0x0C;	
	SPICS_L;
				while((SPI2->SR & SPI_SR_TXE)==0);//等待发送区空
	/****************************************************************************************************
	 这里如果直接使用SPI2->DR=spi_tValue;虽然spi_tValue是一个字节，但是实际发送却是2个字节0x00+spi_tValue;
	 改为*(__IO uint8_t *) spixbase = spi_tValue;发送正确。
	*****************************************************************************************************/
				*(__IO uint8_t *) spixbase = spi_tValue;//SPI2->DR=spi_tValue;	 	  		//发送一个字节	
        while((SPI2->SR  & SPI_SR_RXNE)==0);//等待接收完一个字节
				spi_rValue=*(__IO uint8_t *) spixbase;//SPI2->DR;
        return spi_rValue;
}
//-----------------------------------------------------------------------------
// SPI 16位寄存器写操作
//-----------------------------------------------------------------------------
void Reg_write16(unsigned char addr, unsigned char v1, unsigned char v2)
{
        spi_write_read(addr);//spi_write(addr);
        spi_write_read(v1);//spi_write(v1);
        spi_write_read(v2);//spi_write(v2);
        SPICS_H;
        if(addr < 0x20)
                pdelay(RF_GAP);
}

//-----------------------------------------------------------------------------
// SPI 16位寄存器读操作
//-----------------------------------------------------------------------------
unsigned int Reg_read16(unsigned char addr)
{
        unsigned int value =0;

        spi_write_read(addr | REG_RD);//spi_write(addr | REG_RD);
        if(addr < 0x20)
        {
                pdelay(RF_GAP);
        }
        value = spi_write_read(0xff);//spi_read();
        value<<=8;
        value|=spi_write_read(0xff);//spi_read();
        SPICS_H;

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
        //SPICLK_L;                 //set SPI clock to low
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
/*	SPICS_L;
    pdelay(10);
    spi_write(35);
    pdelay(5);
    spi_write(0x43);
    spi_write(0x80);
    pdelay(5);
    SPICS_H;
    pdelay(5);	*/
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

        spi_write_read(0x32);//spi_write(0x32);                                        //Fill data to FIFO;
        for(j=0; j<(bytes+1); j++)
        {
                spi_write_read(*ptr++);//spi_write(*ptr++);
        }
        SPICS_H;
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
				
        SPICS_H;
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
        spi_write_read(0x32 | REG_RD);//spi_write(0x32 | REG_RD);               //Read FIFO datas
        DAT[0] = spi_write_read(0xff);//spi_read();
        if(DAT[0]>20) DAT[0]=20;            //Maybe first Byte is error;
        for(j=1; j<(DAT[0]+1); j++)             // SPIF0 take 200us to read/write 33 bytes
                DAT[j] = spi_write_read(0xff);//spi_read();

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
