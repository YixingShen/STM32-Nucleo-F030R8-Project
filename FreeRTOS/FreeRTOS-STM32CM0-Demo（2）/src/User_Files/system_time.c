#include "system_time.h"

/* Init SystemTick Timer ��ʼ��ϵͳ�δ�ʱ�� */
void Init_SystemTick_Timer(u16 time)
{
	if (SysTick_Config(SystemCoreClock / 1000))
	{
		/* Capture error */
		while (1);
	}
}



