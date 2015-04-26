#ifndef DS18B20_H
#define DS18B20_H
#include <msp430.h>
#define DQ1      DQ_PxOUT |= DQ
#define DQ0      DQ_PxOUT &= ~DQ
#define DQ_PxDIR P7DIR
#define DQ_PxIN  P7IN
#define DQ_PxOUT P7OUT
#define DQ       BIT4
#define uchar unsigned char
#define uint  unsigned int

#ifdef _DS18B20_
#define DS18B20_EXTERN
#else
#define DS18B20_EXTERN extern
#endif

DS18B20_EXTERN uchar dN[6];
DS18B20_EXTERN uchar is_negative;

void DelayNus(unsigned int n);
unsigned char Init_18B20(void);
void Write_18B20(unsigned char wdata);
unsigned char Read_18B20(void);
void Skip(void);
void Convert(void);
void Read_SP(void);
unsigned int ReadTemp(void);
unsigned int Do1Convert(void);
uint Read_Temperature(void);
void WriteOneChar(unsigned char dat);
uchar ReadOneChar(void);
uchar Init_DS18B20(void);
void Read_ID(char *data);
void StartMeasure();
uint GetTemp();
void Disp_Numb(uint temper);
#endif