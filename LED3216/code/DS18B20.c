#define _DS18B20_
#include "DS18B20.h"
#include "timer.h"
/*******************************************************************/
/*                                                                 */
/*us级延时函数                                                     */
/*                                                                 */
/*******************************************************************/

void Delay(unsigned int num)
{
  while( --num );
}
/*******************************************************************/
/*                                                                 */
/*ms级延时函数                                                     */
/*                                                                 */
/*******************************************************************/
void delayMs(int ms)
{
    unsigned char y;
    while(ms--)
    {
        for(y = 0; y<250; y++)
        {
            _NOP();
            _NOP();
            _NOP();
            _NOP();
        }
    }
}
/*******************************************************************/
/*                                                                 */
/*初始化ds1820                                                     */
/*                                                                 */
/*******************************************************************/
uchar Init_DS18B20(void)
{  
    char presence;
    
     DQ1;      //DQ复位
     __delay_cycles(100);

     DQ0;      //单片机将DQ拉低
     
     __delay_cycles(6000);   //精确延时 大于 480us
     
//     while(!DeadZoneFlag);//在死区里不要关中断，防止显示闪
     
     _DINT();
     DQ1;       //拉高总线
     DQ_PxDIR &= ~DQ;
     __delay_cycles(720);        //等待60us
     presence = DQ_PxIN & DQ;    //如果=0则初始化成功 =1则初始化失败
     _EINT();
     
     __delay_cycles(3000);
     DQ_PxDIR |= DQ;
     DQ1; 
     

     return(presence); //返回信号，0=presence,1= no presence
}

/*******************************************************************/
/*                                                                 */
/* 读一个字节                                                      */
/*                                                                 */
/*******************************************************************/
uchar ReadOneChar(void)
{
unsigned char i = 0;
unsigned char dat = 0;
    
    for (i = 8; i > 0; i--)
    {
//        while(!DeadZoneFlag);//在死区里不要关中断，防止显示闪
         
        _DINT();
        DQ0; // 给脉冲信号
        dat >>= 1;
        __delay_cycles(20); //1us
        DQ1; // 给脉冲信号
        
        DQ_PxDIR &= ~DQ;
        __delay_cycles(180); //15us
        if(DQ_PxIN & DQ)
            dat |= 0x80;
        __delay_cycles(800); // 至少60us
        DQ_PxDIR |= DQ;
        _EINT();
    }
   
    return (dat);
}

/*******************************************************************/
/*                                                                 */
/* 写一个字节                                                      */
/*                                                                 */
/*******************************************************************/
void WriteOneChar(unsigned char dat)
{
  unsigned char i = 0;
   
  for (i = 8; i > 0; i--)
  {
//      while(!DeadZoneFlag);//在死区里不要关中断，防止显示闪
      
      _DINT();
      DQ0;
      __delay_cycles(40); //3us
      if(dat&0x01)
      {
          //1时序 low level time <15us
          DQ1;
          __delay_cycles(800);
      }
      else
      {        
          __delay_cycles(800); //0时序  low level time [60us,120us]
          DQ1;
      }
      __delay_cycles(100); //8us
      dat>>=1;
      _EINT();
      _NOP();
  }
  
}

/*******************************************************************/
/*                                                                 */
/* 读取温度                                                        */
/*                                                                 */
/*******************************************************************/
uint Read_Temperature(void)
{
     uint result;
     uint temp;
     while(Init_DS18B20());
     delayMs(10);
     WriteOneChar(0xCC);  // 跳过读序号列号的操作
     WriteOneChar(0x44);  // 启动温度转换
     
     delayMs(9600);
     
     while(Init_DS18B20());
     delayMs(10);
     WriteOneChar(0xCC);  //跳过读序号列号的操作
     WriteOneChar(0xBE);  //读取温度寄存器
     
     result = ReadOneChar();   //温度低8位
     temp = ReadOneChar();   //温度高8位 
     result |= (temp<<8);
     return result;
}

void StartMeasure()
{
    while(Init_DS18B20());
    WriteOneChar(0xCC);  // 跳过读序号列号的操作
    WriteOneChar(0x44);  // 启动温度转换
}
uint GetTemp()
{
    uint result;
    uint temp;
    while(Init_DS18B20());
    WriteOneChar(0xCC);  //跳过读序号列号的操作
    WriteOneChar(0xBE);  //读取温度寄存器
    
    result = ReadOneChar();   //温度低8位
    temp = ReadOneChar();   //温度高8位 
    result |= (temp<<8);
    return result;
}
void Read_ID(char *data)
{
    while(Init_DS18B20());
    __delay_cycles(2400);
    WriteOneChar(0x33);
    for(char x=0;x<8;x++)
    {
        *(data++) = ReadOneChar();
    }
}

/*******************************************
函数名称：Disp_Numb
功    能：将从DS18B20读取的11bit温度数据转换
          成数码管显示的温度数字
参    数：temper--11bit温度数据
返回值  ：无
********************************************/
void Disp_Numb(uint temper)
{
    uchar i;
    
    for(i = 0;i < 6;i++) dN[i] = 0; //初始化显示变量
    if(temper & BITB)
    {
        temper -= 1;
        temper ^= 0xffff;
        is_negative = 1;
    }
    else
    {
        is_negative = 0;
    }
    //数值转换
    if(temper & BIT0) 
    {
        dN[0] = 5;
        dN[1] = 2;
        dN[2] = 6;
    }
    if(temper&BIT1)     
    {
        dN[1] += 5;
        dN[2] += 2;
        dN[3] += 1;
    }
    if(temper & BIT2)     
    {
        dN[2] += 5;
        dN[3] += 2;
        if(dN[2] >= 10)
        {
            dN[2] -= 10;
            dN[3] += 1;
        }
    }
    if(temper&BIT3)     
    {
        dN[3] += 5;
    }
    if(temper & BIT4)
    {
        dN[4] += 1;
    }
    if(temper & BIT5)     
    {
        dN[4] += 2;
    }
    if(temper & BIT6)
    {
        dN[4] += 4;
    }
    if(temper & BIT7)     
    {
        dN[4] += 8;
        if(dN[4] >= 10)
        {
            dN[4] -= 10;
            dN[5] += 1;
        }
    }
    if(temper & BIT8)
    {
        dN[4] += 6;
        dN[5] += 1;
        if(dN[4] >= 10)
        {
            dN[4] -= 10;
            dN[5] += 1;
        }
    }
    if(temper & BIT9)
    {
        dN[4] += 2;
        dN[5] += 3;
        if(dN[4] >= 10)
        {
            dN[4] -= 10;
            dN[5] += 1;
        }
    }
    if(temper & BITA)
    {
        dN[4] += 4;
        dN[5] += 6;
        if(dN[4] >= 10)
        {
            dN[4] -= 10;
            dN[5] += 1;
        }
        if(dN[5] >= 10)
        {
            dN[5] -= 10;
        }
    }
}