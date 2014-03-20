#include "sjip.h"
#include "stm32f10x_conf.h"
#include "tim.h"
#include "arp.h"

tim_task	tim_queue[]=
{
	{
		arp_timer,	// ARP缓存生存时间定时器
		6000,		// 60S 1min
		6000
	}
};

#define TIM_TASK_NUM (sizeof(tim_queue)/sizeof(tim_task))
	

// TIM6 Basic Timer
void tim_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* TIM6 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	/* Configure the NVIC Preemption Priority Bits */  
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	/* Enable the TIM6 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
	
	/* Time base configuration */
	// The counter counts from 0 to the auto-reload value,
	// then restarts from 0 and generates a counter overflow event.
	
	// 10ms 
	TIM_TimeBaseStructure.TIM_Period = 720;
	TIM_TimeBaseStructure.TIM_Prescaler = 1000;

	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

	/* TIM IT enable */
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

	/* enable counter */
	TIM_Cmd(TIM6, ENABLE);
}

// 10ms per time 
// called by tim interrupt
void ip_10ms_timer(void)
{
	u8 i;
	
	for(i=0; i<TIM_TASK_NUM; i++)
	{
		if(tim_queue[i].count!=0)
			tim_queue[i].count--;
	}
}

// process timer task  called by yrip main thread
void ip_tim_proc(void)
{
	u8 i;
	
	for(i=0; i<TIM_TASK_NUM; i++)
	{
		// check timer
		if(tim_queue[i].count==0)
		{
			tim_queue[i].count=tim_queue[i].reload;
			tim_queue[i].task();
		}
	}
}
