#define _BEEPER_
#include <msp430.h>
#include "beeper.h"

//#define NTD0 0
/*
//虫儿飞
const unsigned int tune[]=
{
	NTD3,NTD3,NTD3,NTD4,NTD5,
	NTD3,NTD2,
	NTD1,NTD1,NTD1,NTD2,NTD3,
	NTD3,NTDL7,NTDL7,
	NTDL6,NTD3,NTD2,
	NTDL6,NTD3,NTD2,
	NTDL6,NTD3,NTD2,NTD1,
	NTD1,
	
	NTD3,NTD2,NTD5,
	NTD4,NTD3,NTD2,
	
	NTD5,NTD4,NTD3,NTD2,
	NTD5,NTD3,NTD2,
	NTDL6,NTD3,NTD2,
	NTDL6,NTD3,NTD2,
	NTD4,NTD3,NTD4,NTD3,NTD1,
	NTD4,NTD3,NTD4,NTD3,NTD1,NTD2,NTD1,	
};

const unsigned char durt[]=
{
	2,1,1,2,2,
	4,4,
	2,1,1,2,2,
	3,1,4,
	2,2,4,
	2,2,4,
	2,2,3,1,
	8,
	
	2,2,4,
	1,1,4,
	
	1,1,1,1,
	3,1,4,
	2,2,4,
	2,2,4,
	1,1,1,1,4,
	1,1,1,2,3,4,
};
*/
//欢乐颂
const unsigned int tune[]=                 //根据简谱列出各频率
{
  NTD3,NTD3,NTD4,NTD5,
  NTD5,NTD4,NTD3,NTD2,
  NTD1,NTD1,NTD2,NTD3,
  NTD3,NTD2,NTD2,
  
  NTD3,NTD3,NTD4,NTD5,
  NTD5,NTD4,NTD3,NTD2,
  NTD1,NTD1,NTD2,NTD3,
  NTD2,NTD1,NTD1,
  
  NTD2,NTD2,NTD3,NTD1,
  NTD2,NTD3,NTD4,NTD3,NTD1,
  NTD2,NTD3,NTD4,NTD3,NTD2,
  NTD1,NTD2,NTDL5,NTD3,
  NTD3,NTD3,NTD4,NTD5,
  NTD5,NTD4,NTD3,NTD4,NTD2,
  NTD1,NTD1,NTD2,NTD3,
  NTD2,NTD1,NTD1,
};
const unsigned char durt[]=                   //根据简谱列出各节拍
{
  2,2,2,2,
  2,2,2,2,
  2,2,2,2,
  3,1,4,
	  
  2,2,2,2,
  2,2,2,2,
  2,2,2,2,
  3,1,4,
  
  2,2,2,2,
  2,1,1,2,2,
  2,1,1,2,2,
  2,2,4,2,
  2,2,2,2,
  2,2,2,1,1,
  2,2,2,2,
  3,1,4,
};

void BeeperInit()
{
    P8SEL |= BIT0;
    P8DIR |= BIT0;
    TA0CCR0 = 0;	                      // PWM Period
    TA0CCTL0 = OUTMOD_4;                      // CCR0 toggle
    TA0CTL = TASSEL_2 + MC_1 + TACLR;         // SMCLK, up mode, clear TAR
    tune_length = sizeof(tune)/sizeof(tune[0]);
}

void BeeperCancel()
{
    P8SEL &= ~BIT0;
    P8DIR &= ~BIT0;
    TA0CCR0 = 0;
    TA0CCTL0 = 0;
    TA0CTL = 0;
}