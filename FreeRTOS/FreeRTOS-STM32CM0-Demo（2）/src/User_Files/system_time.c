#include "system_time.h"

/* Init SystemTick Timer 初始化系统滴答定时器 */
void Init_SystemTick_Timer(u16 time)
{
	if (SysTick_Config(SystemCoreClock / 1000))
	{
		/* Capture error */
		while (1);
	}
}



