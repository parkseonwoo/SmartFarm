#include "stm32f4xx.h"
#include "color.h"

// delay ÇÔ¼ö
static void Delay(const uint32_t Count)
{
  __IO uint32_t index = 0; 
  for(index = (16800 * Count); index != 0; index--);
}

void Color_Init()
{
   GPIO_InitTypeDef   GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
 }
  
 void Color_Start(int mapVal)
  {
    
      if(mapVal < 10)
      {
        GPIO_Write(GPIOB, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14);   // Black ==> R=1, G=1, B=1
      }
      else if(mapVal >= 10  && mapVal <20)
      {
         GPIO_Write(GPIOB,  GPIO_Pin_12|GPIO_Pin_13);                                    
      }
      else if(mapVal >= 20 && mapVal <30)
     {
         GPIO_Write(GPIOB, GPIO_Pin_12);                                   
      }
      else if(mapVal >= 30 && mapVal <50)
      {
         GPIO_Write(GPIOB, GPIO_Pin_13); 
      }
      else if(mapVal >= 50)
      {
         GPIO_Write(GPIOB, 0);  
      }
    
}