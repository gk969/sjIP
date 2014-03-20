#ifndef __TIM_H
#define __TIM_H

// 定时器周期 ms
#define TIM_PERIOD  10

typedef struct
{
    void (*task)(void);  // 任务
    u32  count;          // 计数器
    u32  reload;         // 计数初值
} tim_task;

void tim_init(void);
void ip_10ms_timer(void);
void ip_tim_proc(void);

#endif
