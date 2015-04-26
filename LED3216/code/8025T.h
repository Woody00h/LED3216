#ifndef _8025T_
#define _8025T_

#define Code2Sec(a) Code2Min(a)
#define Code2Day(a) Code2Hour(a)
unsigned char Code2Year(char data);
unsigned char Code2Month(char data);
unsigned char Code2Day(char data);
unsigned char Code2Week(char data);
unsigned char Code2Min(char data);

#define Sec2Code(a) Min2Code(a)
#define Day2Code(a) Hour2Code(a)
unsigned char Min2Code(char data);
unsigned char Hour2Code(char data);
unsigned char Week2Code(char data);
unsigned char Month2Code(char data);
unsigned char Year2Code(char data);

char is_leap_year(unsigned int year);
unsigned char bcd2bin(char val);
unsigned char bin2bcd(char val);
void IICInit();
void ReadTime();
void WriteTime();
void RTCInit();
#endif