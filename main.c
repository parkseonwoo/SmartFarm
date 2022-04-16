/******************************************************************************
  포트 연결:
    1)  ARM Cortex-M4 모듈의 포트C (PC0 ~ PC7)를 8핀 케이블를 이용해서
        LED모듈의 LED0 ~ LED7에 연결한다. (LED0가 PC0로 연결되야 한다.)
******************************************************************************/
// stm32f4xx의 각 레지스터들을 정의한 헤더파일
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f4xx.h"
#include "uart2.h"
#include "uart4.h"
#include "gpioc_key.h"
#include "tim7_fnd.h"
#include "lcd.h"
#include "sensor.h"
#include "tim10_pwm.h"
#include "servo.h"


#define CMD_SIZE 50
#define ARR_CNT 5  

extern volatile unsigned char rx2Flag;
extern char rx2Data[CMD_SIZE];
extern volatile unsigned char rx4Flag;
extern char rx4Data[CMD_SIZE];
extern int key;
extern int adc1Flag;
extern int adc_data;
extern volatile unsigned long systick_sec;

void PORTC_Led_Init();
void Serial2_Event();
void Serial4_Event();

int main()
{
//  double volt = 0;
  int mapVal = 0;
  int old_pwm=50;
  int ccr1;
  int old_systick_sec = 0;
  struct dht11 dhtInfo; 
  dhtInfo.GPIOx = GPIOB;
  dhtInfo.GPIO_Pin = GPIO_Pin_7;
  
  UART2_Init();                   //pa2,pa3 Debug
  UART4_Init();                   //pa0,pa1 Bluetooth
  TIM7_Fnd_Init();                //pc0~pc11    FND
  GPIOC_Key_Init();             //pc12~pc15  EXTI11~15
  GPIOAB_LCD_Init();          //pa5~12, pb0~2 LCD
  GPIOA_ADC1_Init();          //pa4
  DHT11_Init(dhtInfo);          //pb7
  TIM10_PWM_Init();            //pb8~10   ,pb8 : M_EN, pb9 : M_D2, pb10  : M_D1
  Servo_Init();
  Color_Init();
  
  Serial2_Send_String("Start Main()\n\r");
    
  while(1)
  {
    
    if(rx2Flag)  // '\r' 까지 입력되면  //STM32 Terminal(Putty)
      Serial2_Event();
    if(rx4Flag)  // '\n" 까지 입력되면  //STM32 Bluetooth
      Serial4_Event();
    
    if(systick_sec != old_systick_sec)  //1초마다 실행
//    if(systick_sec != old_systick_sec  && !(systick_sec % 5))   //5초마다 실행     
    {     
       if(DHT_Read(&dhtInfo))
      {
        printf("humi:%d temp:%d\r\n",dhtInfo.data[0],dhtInfo.data[2]);                                                     // ADC1 컨버전 시작
        
      }
      if(dhtInfo.data[0] > 10 || dhtInfo.data[1] > 24)
      {
        MotorRight();
      }
      else
        MotorStop();
      ADC_SoftwareStartConv(ADC1); 
      old_systick_sec = systick_sec;
    }
    
    if(key != 0) 
    {
      printf("key : %d\n\r",key);
      if(key == 1)
        //MotorLeft();
        Servo_Moter_ON();
      else if(key == 2)
        //MotorRight();
        Servo_Moter_OFF();
      else if(key == 3)    
          MotorRight();
      else if(key == 4)
        
      key=0; 
    }

    if(adc1Flag)
    {
//        volt = adc_data* 3.3/4096;
        mapVal = map(adc_data,0,4096,0,100);
//        printf("adc_data : %d, Volt : %lf, %d%%\n\r",adc_data, volt,mapVal);
        adc1Flag = 0;
     
    }
    
    if(mapVal != old_pwm)
    {
      char sendBuf[CMD_SIZE]={0}; 
        if(mapVal == 0)          
             ccr1 = 1;
        else if(mapVal == 100)
             ccr1 = 177 * 100 - 1;
        else
             ccr1 = 177 * mapVal;
        
        TIM10->CCR1 = ccr1;
        old_pwm = mapVal;
        printf("pwm : %d\n\r",mapVal);
        
        if(mapVal < 50)
        {
          Servo_Moter_OFF();
        }
        if(mapVal >= 50)
        {
          
          Servo_Moter_ON();
        }
        sprintf(sendBuf,"light:%d", mapVal);
        lcd(0,0,sendBuf);
        sprintf(sendBuf,"humi:%d temp:%d", dhtInfo.data[0],dhtInfo.data[2]);
        lcd(0,1,sendBuf);
        Color_Start(mapVal);
    }
  }
}


void Serial2_Event()
{
/*
  int i=0;
  int num = 0;
  char * pToken;
  char * pArray[ARR_CNT]={0};
  char recvBuf[CMD_SIZE]={0};       
  strcpy(recvBuf,rx2Data);
  i=0;
  rx2Flag = 0; // 다시 Rflag 를 0으로 놓는다.    
  Serial2_Send_String(recvBuf);
  Serial2_Send_String("\n\r");
  printf("rx : %s\r\n",recvBuf);
     
  pToken = strtok(recvBuf,"[@]");

  while(pToken != NULL)
  {
    pArray[i] =  pToken;
    if(++i >= ARR_CNT)
      break;
    pToken = strtok(NULL,"[@]");
  }
       
  if(!strcmp(pArray[1],"LED"))
  {
    if(pArray != NULL)              
      num = atoi(pArray[3]);
    else
      num = 0;
    if(!strcmp(pArray[2],"ON"))
    {
     GPIO_SetBits(GPIOC, 0x01 << num); 
    }
    else 
    {
      GPIO_ResetBits(GPIOC, 0x01 << num); 
    }
  }  
  */
}

void Serial4_Event()
{
  
//  Serial2_Send_String(rx4Data);
//  Serial2_Send_String("\r\n");

  int i=0;
  int num = 0;
  char * pToken;
  char * pArray[ARR_CNT]={0};
  char recvBuf[CMD_SIZE]={0};
  char sendBuf[CMD_SIZE]={0}; 
  strcpy(recvBuf,rx4Data);
  i=0;
  rx4Flag = 0; // 다시 Rflag 를 0으로 놓는다.    
//  Serial2_Send_String(recvBuf);
//  Serial2_Send_String("\n\r");
  printf("rx : %s\r\n",recvBuf);
     
  pToken = strtok(recvBuf,"[@]");

  while(pToken != NULL)
  {
    pArray[i] =  pToken;
    if(++i >= ARR_CNT)
      break;
    pToken = strtok(NULL,"[@]");
  }
       
  if(!strcmp(pArray[1],"MOTER"))
  {
    if(pArray != NULL)              
      num = atoi(pArray[3]);
    else
      num = 0;
    
    if(!strcmp(pArray[2],"ON"))
    {
      Servo_Moter_ON();
    }
    else 
    {
      Servo_Moter_OFF();
    }
  } 
  
   else if(!strcmp(pArray[1],"FAN"))
  {
   
    if(!strcmp(pArray[2],"ON"))
    {
      MotorRight();
    }
    else 
    {
      MotorStop();
    }
  } 
  else if(!strcmp(pArray[1],"LIGHT"))
  {
   if(pArray != NULL)              
      num = atoi(pArray[3]);
    else
      num = 0;
    if(!strcmp(pArray[2],"ON"))
    {
      Color_Start(num);
    }
  } 
  
  else if(!strncmp(pArray[1]," New conn",sizeof(" New conn")))
  {
      return;
  }
  else if(!strncmp(pArray[1]," Already log",sizeof(" Already log")))
  {
      return;
  }    
  sprintf(sendBuf,"[%s]%s@%s@%d\n",pArray[0],pArray[1],pArray[2],num);
  Serial4_Send_String(sendBuf);
}