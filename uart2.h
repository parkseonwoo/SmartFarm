#ifndef __UART2_H__
#define __UART2_H__

void UART2_Init();
void Serial2_Send(unsigned char t);
void Serial2_Send_String(char* s);
int putchar(int ch);
void print2_2d1(double number);		        /* floating-point number xx.x */

#endif