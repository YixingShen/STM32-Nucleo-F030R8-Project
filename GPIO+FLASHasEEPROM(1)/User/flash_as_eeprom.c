#include "declare.h"

#define EEPROM_ADR_INI				(uint32_t)0x0800FC00//必须大于等于0x08000000 实际数据从0x0800FC02开始存储，0x0800FC00作为存储的有效数据个数

#define FLASH_FKEY1    (uint32_t)0x45670123
#define FLASH_FKEY2    (uint32_t)0xCDEF89AB

/** FLASH_Timeout_definition */ 
#define FLASH_ER_PRG_TIMEOUT         ((uint32_t)0x000B0000)


uint16_t read_flash(uint32_t flash_addr)
{
	uint16_t data=0xffff;
	//flash_addr必须是2的整数倍
	if((flash_addr%2)==0)
	{
		flash_addr=flash_addr+EEPROM_ADR_INI;
		data=*(__IO uint16_t*)(flash_addr);
	}
	return data;
}



uint8_t flash_erasspage(uint32_t page_addr,uint32_t Timeout)
{
	uint8_t flag_error=0;
		//页擦除 page64
		FLASH->CR |= FLASH_CR_PER;//使能页擦除
		FLASH->AR = page_addr;//页地址
		FLASH->CR |= FLASH_CR_STRT;//执行页擦除
		while((FLASH->SR&FLASH_SR_BSY)!=0 && Timeout>0)//等待BSY=0，flash操作完成
		{
			Timeout--;
		}
		if((FLASH->SR&FLASH_SR_BSY)!=0 && Timeout==0)flag_error=1;
		if((FLASH->SR&FLASH_SR_EOP)!=0)	//EOP=1,页擦除结束
		{
			FLASH->SR |= FLASH_SR_EOP;//写1，清零EOP
		}	
		else flag_error=1;
		FLASH->CR &= (~FLASH_CR_PER);//
		return flag_error;
}


uint16_t ReadFlash_before_Erass(void)
{
	uint16_t i,cnt;
	uint32_t addr;
	
	//在擦除前读出，存储的数据
	cnt=read_flash(0);
	if(cnt==0xffff)cnt=0;
	for(i=0;i<cnt;i++)
	{
		addr=2*i+2;
		p_data[i]=read_flash(addr);
	}	
	
	return cnt;
}


void write_flash(uint32_t flash_addr, uint16_t data)//flash_addr必须是2的整数倍
{
	uint8_t erass_error;
	uint16_t i;
	uint32_t addr;
	
	//flash_addr必须是2的整数倍，且>=2，因为0位置用来存储写入的数据个数
	if(flash_addr>=2&&(flash_addr%2)==0)
	{
//在擦除前读出，存储的数据
//	p_count=ReadFlash_before_Erass();

		while((FLASH->SR&FLASH_SR_BSY)!=0);//等待BSY=0，flash操作完成
		while((FLASH->CR&FLASH_CR_LOCK)!=0)//LOCK=1,flash被锁定，保护；需要解锁，才能进行flash操作
		{
			FLASH->KEYR = FLASH_FKEY1;
			FLASH->KEYR = FLASH_FKEY2;
		};
		if((FLASH->CR&FLASH_CR_LOCK)==0)//LOCK=0,flash未锁定，可进行操作
		{
			erass_error=flash_erasspage(EEPROM_ADR_INI,FLASH_ER_PRG_TIMEOUT);
			if(erass_error==0)
			{
				FLASH->CR |= FLASH_CR_PG;//使能flash编程
				
				addr=EEPROM_ADR_INI+2*p_count+2;
				flash_addr=flash_addr+EEPROM_ADR_INI;
				if(addr<=flash_addr)//有新数据写入，存储数据个数增加
				{
					//*(__IO uint16_t*)(flash_addr) = data;//向地址flash_addr处写半字数据  
					//while((FLASH->SR&FLASH_SR_BSY)!=0);//等待BSY=0，flash操作完成			

					i=(flash_addr-EEPROM_ADR_INI-2)/2;
					p_data[i]=data;//新瓶装新酒
					
					p_count=1+i;//新瓶的位置，前面的瓶子都认为装了酒（不管事实上是否真装了酒）(写入的数据，地址未必是连续的)
				}
				else //旧存储数据值改变，存储数据个数没有增加
				{
					i=(flash_addr-EEPROM_ADR_INI-2)/2;
					p_data[i]=data;//旧瓶装新酒
				}
				
				
				//写入存储数据个数
				*(__IO uint16_t*)(EEPROM_ADR_INI) = p_count;//
				while((FLASH->SR&FLASH_SR_BSY)!=0);//等待BSY=0，flash操作完成				
				
				//把所有数据写入（原来的数据（可能有新值）+新数据）
				for(i=0;i<p_count;i++)
				{
					addr=EEPROM_ADR_INI+2*i+2;
					*(__IO uint16_t*)(addr) = p_data[i];
					while((FLASH->SR&FLASH_SR_BSY)!=0);//等待BSY=0，flash操作完成
				}

				if((FLASH->SR&FLASH_SR_EOP)!=0)	//EOP=1,编程结束
				{
					FLASH->SR |= FLASH_SR_EOP;//写1，清零EOP
				}
				FLASH->CR &= (~FLASH_CR_PG);//关闭flash编程使能
				FLASH->CR |= FLASH_CR_LOCK;//锁定FLASH
			}
		}
	}
}

