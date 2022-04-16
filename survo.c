#include "stm32f4xx.h"
#include "servo.h"

int t_cnt6 = 0;
int AngleCount = 0;
int i=0;

static void Delay(const uint32_t Count)
{
  __IO uint32_t index = 0; 
  for(index = (16800 * Count); index != 0; index--);
}

static void Delay_us(const uint32_t Count)
{
  __IO uint32_t index = 0; 
  for(index = (16 * Count); index != 0; index--);
}

void Servo_Init()
{
  GPIO_InitTypeDef   GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef   NVIC_InitStructure; 

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; // SERVO
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  //���ͷ�Ʈ enable �� Priority ����.
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  //system_stm32f4xx.c ����
  //TIM7 input clock (TIM7CLK) is set to 2 * APB1 clock (PCLK1),
  //since APB1 prescaler is different from 1.
  //TIM7CLK = 2 * PCLK1
  //PCLK1 = HCLK / 4  => TIM7CLK = HCLK / 2 = SystemCoreClock /2
  //TIM_CounterMode�� TIM_CounterMode_Up�� �����ϸ�, ī��Ʈ���� TIMx_ARR����
  //��������, Update �̺�Ʈ�� �߻��Ѵ�.
  //TIM_Prescaler�� 83����, TIM_ClockDivision�� TIM_CKD_DIV1(1����)��
  //�����ϸ�, Ÿ�̸�7 ī��Ʈ Ŭ���� (84MHz/1) /(83+1) = 1MHz (1us)�̴�.
  //TIM_Period�� 99���� ����������(TIMx_ARR), ī���Ͱ� �̰��� ��������,
  //(1us x 100(99+1) = 100us)
  TIM_TimeBaseStructure.TIM_Prescaler = 84-1;         //(168Mhz/2)/840 = 100KHz(10us)
  TIM_TimeBaseStructure.TIM_Period = 100-1;            //100us
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
  //Ÿ�̸�7�� ���۽�Ų��.
  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM2, ENABLE);

}

void TIM2_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    t_cnt6++;

    if(t_cnt6 <= AngleCount)
      GPIO_SetBits(GPIOB, GPIO_Pin_6);     // PWM ON Time
    else
      GPIO_ResetBits(GPIOB, GPIO_Pin_6);  // PWM OFF Time
    if(t_cnt6 >= 200) t_cnt6 = 0;                      // 20[ms] �ֱ� PWM
  }
}

void Servo_Moter_ON()
{
  for(i = 0; i <50 ; i++ ) // +90�� ����
    {
      GPIO_SetBits(GPIOB, GPIO_Pin_6);
      Delay_us(700);    // 0.7[ms]

      GPIO_ResetBits(GPIOB, GPIO_Pin_6);;
      Delay_us(19300);  // 19.3[ms]
    }
}

void Servo_Moter_OFF()
{
  for(i = 0; i <50 ; i++ ) // -90�� ����
    {
      GPIO_SetBits(GPIOB, GPIO_Pin_6);
      Delay_us(2300);     // 2.3[ms]

      GPIO_ResetBits(GPIOB, GPIO_Pin_6);;
      Delay_us(17700);    // 17.7[ms]
    }
}

void Servo(int Angle) 
{
  if (Angle == -90 )
    AngleCount = 23; // PWM ON Time 2.3[ms]
  else if (Angle == 0)
    AngleCount = 15;  // PWM ON Time 1.5[ms]
  else if(Angle == 90)
    AngleCount = 7;   // PWM ON Time 0.7[ms]
}
