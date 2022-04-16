#ifndef __SENSOR_H__
#define __SENSOR_H__
#define DHT_COUNT 6
#define DHT_MAXTIMINGS 85
struct dht11 {
  GPIO_TypeDef* GPIOx;
  uint32_t GPIO_Pin;
  uint8_t data[5];    /* data from sensor store here */
};
void DHT11_Init(struct dht11 dh);
uint8_t DHT_Read(struct dht11 *dht);

void GPIOA_ADC1_Init();
long map(long x, long in_min, long in_max, long out_min, long out_max);
#endif