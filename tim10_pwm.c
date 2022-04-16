#include "stm32f4xx.h"
#include "tim10_pwm.h"
void TIM10_PWM_Init()
{
  GPIO_InitTypeDef   GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef   NVIC_InitStructure; 
  TIM_OCInitTypeDef  TIM_OCInitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;     // TIM10_CH1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_TIM10);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;     // LED2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  TIM_TimeBaseStructure.TIM_Prescaler = 168-1;         //168Mhz/168 = 1MHz(1us)
  TIM_TimeBaseStructure.TIM_Period = 17700-1;          //1us * 17700 = 17.7ms
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM10, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 7080-1;         //40% duty
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC1Init(TIM10, &TIM_OCInitStructure);

  //타이머10을 동작시킨다.
  TIM_ClearITPendingBit(TIM10, TIM_IT_CC1|TIM_IT_Update);
  TIM_ITConfig(TIM10, TIM_IT_CC1|TIM_IT_Update, ENABLE);
  TIM_OC1PreloadConfig(TIM10, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM10, ENABLE);
  TIM_Cmd(TIM10, ENABLE);
}

void MotorStop()
{
  GPIO_WriteBit(GPIOB,GPIO_Pin_9,Bit_RESET);  //motor stop
  GPIO_WriteBit(GPIOB,GPIO_Pin_10,Bit_RESET);//motor stop  
}
void MotorRight()
{
  GPIO_ResetBits(GPIOB,GPIO_Pin_9);
  GPIO_SetBits(GPIOB,GPIO_Pin_10);
}
void MotorLeft()
{
  GPIO_SetBits(GPIOB,GPIO_Pin_9);
  GPIO_ResetBits(GPIOB,GPIO_Pin_10);
}



void TIM1_UP_TIM10_IRQHandler(void) //17.7ms
{
  if(TIM_GetITStatus(TIM10, TIM_IT_CC1) != RESET)
  {
    TIM_ClearITPendingBit(TIM10, TIM_IT_CC1);
  }
  if(TIM_GetITStatus(TIM10, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM10, TIM_IT_Update);
  }
}