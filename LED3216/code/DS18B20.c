#define _DS18B20_
#include "DS18B20.h"
#include "timer.h"
/*******************************************************************/
/*                                                                 */
/*us����ʱ����                                                     */
/*                                                                 */
/*******************************************************************/

void Delay(unsigned int num)
{
  while( --num );
}
/*******************************************************************/
/*                                                                 */
/*ms����ʱ����                                                     */
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
/*��ʼ��ds1820                                                     */
/*                                                                 */
/*******************************************************************/
uchar Init_DS18B20(void)
{  
    char presence;
    
     DQ1;      //DQ��λ
     __delay_cycles(100);

     DQ0;      //��Ƭ����DQ����
     
     __delay_cycles(6000);   //��ȷ��ʱ ���� 480us
     
//     while(!DeadZoneFlag);//�������ﲻҪ���жϣ���ֹ��ʾ��
     
     _DINT();
     DQ1;       //��������
     DQ_PxDIR &= ~DQ;
     __delay_cycles(720);        //�ȴ�60us
     presence = DQ_PxIN & DQ;    //���=0���ʼ���ɹ� =1���ʼ��ʧ��
     _EINT();
     
     __delay_cycles(3000);
     DQ_PxDIR |= DQ;
     DQ1; 
     

     return(presence); //�����źţ�0=presence,1= no presence
}

/*******************************************************************/
/*                                                                 */
/* ��һ���ֽ�                                                      */
/*                                                                 */
/*******************************************************************/
uchar ReadOneChar(void)
{
unsigned char i = 0;
unsigned char dat = 0;
    
    for (i = 8; i > 0; i--)
    {
//        while(!DeadZoneFlag);//�������ﲻҪ���жϣ���ֹ��ʾ��
         
        _DINT();
        DQ0; // �������ź�
        dat >>= 1;
        __delay_cycles(20); //1us
        DQ1; // �������ź�
        
        DQ_PxDIR &= ~DQ;
        __delay_cycles(180); //15us
        if(DQ_PxIN & DQ)
            dat |= 0x80;
        __delay_cycles(800); // ����60us
        DQ_PxDIR |= DQ;
        _EINT();
    }
   
    return (dat);
}

/*******************************************************************/
/*                                                                 */
/* дһ���ֽ�                                                      */
/*                                                                 */
/*******************************************************************/
void WriteOneChar(unsigned char dat)
{
  unsigned char i = 0;
   
  for (i = 8; i > 0; i--)
  {
//      while(!DeadZoneFlag);//�������ﲻҪ���жϣ���ֹ��ʾ��
      
      _DINT();
      DQ0;
      __delay_cycles(40); //3us
      if(dat&0x01)
      {
          //1ʱ�� low level time <15us
          DQ1;
          __delay_cycles(800);
      }
      else
      {        
          __delay_cycles(800); //0ʱ��  low level time [60us,120us]
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
/* ��ȡ�¶�                                                        */
/*                                                                 */
/*******************************************************************/
uint Read_Temperature(void)
{
     uint result;
     uint temp;
     while(Init_DS18B20());
     delayMs(10);
     WriteOneChar(0xCC);  // ����������кŵĲ���
     WriteOneChar(0x44);  // �����¶�ת��
     
     delayMs(9600);
     
     while(Init_DS18B20());
     delayMs(10);
     WriteOneChar(0xCC);  //����������кŵĲ���
     WriteOneChar(0xBE);  //��ȡ�¶ȼĴ���
     
     result = ReadOneChar();   //�¶ȵ�8λ
     temp = ReadOneChar();   //�¶ȸ�8λ 
     result |= (temp<<8);
     return result;
}

void StartMeasure()
{
    while(Init_DS18B20());
    WriteOneChar(0xCC);  // ����������кŵĲ���
    WriteOneChar(0x44);  // �����¶�ת��
}
uint GetTemp()
{
    uint result;
    uint temp;
    while(Init_DS18B20());
    WriteOneChar(0xCC);  //����������кŵĲ���
    WriteOneChar(0xBE);  //��ȡ�¶ȼĴ���
    
    result = ReadOneChar();   //�¶ȵ�8λ
    temp = ReadOneChar();   //�¶ȸ�8λ 
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
�������ƣ�Disp_Numb
��    �ܣ�����DS18B20��ȡ��11bit�¶�����ת��
          ���������ʾ���¶�����
��    ����temper--11bit�¶�����
����ֵ  ����
********************************************/
void Disp_Numb(uint temper)
{
    uchar i;
    
    for(i = 0;i < 6;i++) dN[i] = 0; //��ʼ����ʾ����
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
    //��ֵת��
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