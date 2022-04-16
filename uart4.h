#ifndef __UART4_H__
#define __UART4_H__

void UART4_Init();
void Serial4_Send(unsigned char t);
void Serial4_Send_String(char* s);
//int putchar(int ch);
void print4_2d1(double number);		        /* floating-point number xx.x */

#endif