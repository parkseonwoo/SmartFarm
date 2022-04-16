#include "stm32f4xx.h"
#include "sensor.h"

volatile uint16_t adc_data;
volatile int adc1Flag=0;
void ADC_IRQHandler(void)
{ 
  if(ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET) 
  {
    ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
    adc_data = ADC_GetConversionValue(ADC1);
    adc1Flag = 1;
  }
}

void GPIOA_ADC1_Init()
{
  GPIO_InitTypeDef   GPIO_InitStructure;
  ADC_InitTypeDef ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;    // single 컨버전
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_3Cycles);     // ADC1의 채널0 사용
  ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);             // ADC1 인터럽트 사용
  ADC_Cmd(ADC1, ENABLE);
//  ADC_SoftwareStartConv(ADC1);   
}
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
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

void DHT11_Init(struct dht11 dh)
{
  GPIO_InitTypeDef   GPIO_InitStructure;
  if( dh.GPIOx == GPIOA)
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  else if(dh.GPIOx == GPIOB)
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  else if(dh.GPIOx == GPIOC)
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);  
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = dh.GPIO_Pin;  
  GPIO_Init(dh.GPIOx, &GPIO_InitStructure);  
}

uint8_t DHT_Read(struct dht11 *dht)
{
    uint8_t tmp;
    uint8_t sum = 0;
    uint8_t j = 0, i;
    uint8_t last_state = 1;
    uint16_t counter = 0;
     /*
     * Pull the pin 1 and wait 250 milliseconds
     */
   GPIO_InitTypeDef   GPIO_InitStructure;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_Pin = dht->GPIO_Pin;
   GPIO_Init(dht->GPIOx, &GPIO_InitStructure);
    
   GPIO_SetBits(dht->GPIOx, dht->GPIO_Pin); 
   Delay(250);

//   dht->data[0] = dht->data[1] = dht->data[2] = dht->data[3] = dht->data[4] = 0;  
	//data배열 0으로 초기화 

   GPIO_ResetBits(dht->GPIOx, dht->GPIO_Pin);   
   Delay(20);
   GPIO_SetBits(dht->GPIOx, dht->GPIO_Pin); 
   Delay_us(30);                   					//40 나노 세컨드 딜레이
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_Pin = dht->GPIO_Pin;
   GPIO_Init(dht->GPIOx, &GPIO_InitStructure);

    /* Read the timings */
    for (i = 0; i < DHT_MAXTIMINGS; i++) {             //DHT_MAXTIMINGS = 85
        counter = 0;
        while (1) {
            tmp = GPIO_ReadInputDataBit(dht->GPIOx,dht->GPIO_Pin);
            Delay_us(3);

            if (tmp != last_state)    //last state = 1
                break;

            counter++;
            Delay_us(1);

            if (counter == 255)  //85 times
                break;
        }

        last_state = GPIO_ReadInputDataBit(dht->GPIOx,dht->GPIO_Pin);  //0000 0x00

        if (counter == 255)  //85 times
            break;

        /* Ignore first 3 transitions */
        if ((i >= 4) && (i % 2 == 0)) {
            /* Shove each bit into the storage bytes */
            dht->data[j/8] <<= 1;
            if (counter > DHT_COUNT)  //DHT_COUNT 6
                dht->data[j/8] |= 1;
            j++;
        }
    }

    sum = dht->data[0] + dht->data[1] + dht->data[2] + dht->data[3];
	//sum = check sum

    if ((j >= 40) && (dht->data[4] == (sum & 0xFF)))  //data[4]은 check sum
        return 1;
    return 0;
}
