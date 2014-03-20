#ifndef __TIM_H
#define __TIM_H

// ��ʱ������ ms
#define TIM_PERIOD  10

typedef struct
{
    void (*task)(void);  // ����
    u32  count;          // ������
    u32  reload;         // ������ֵ
} tim_task;

void tim_init(void);
void ip_10ms_timer(void);
void ip_tim_proc(void);

#endif
