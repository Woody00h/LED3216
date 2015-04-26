#define LED3216_SM
#include <msp430.h>
#include "timer.h"
#include "io.h"
#include "clock.h"
#include "data.h"
#include "8025T.h"
#include "state_machine.h"
#include "Hardware.h"
#include "DS18B20.h"
#include "beeper.h"

extern const unsigned char *buf[4];
extern const unsigned char *digit[10];

unsigned char Year=14;
unsigned char Month=2;
unsigned char Day=16;
unsigned char Week=1;
unsigned char WeekDay;
unsigned char Hour=22;
unsigned char Min=47;
unsigned char Second=11;

unsigned char KeyS1 = 0;
unsigned char KeyS2 = 0;
unsigned char KeyS3 = 0;
unsigned char KeyS4 = 0;
unsigned char s1_toggle = 0;
unsigned char s2_toggle = 0;
unsigned char s3_toggle = 0;
unsigned char s4_toggle = 0;

unsigned int Temperature;
unsigned char MeasureStep;
void ScanInput()
{
    if(S1_PxIN & S1)
    {
        KeyS1 = 0;
        s1_toggle = 1;
        s1_delay_timer = ONE_HUNDRED_MS_TIMER_RELOAD;
    }
    else
    {
        if(!s1_delay_timer)
            KeyS1 = 1;
    }
    
    if(S2_PxIN & S2)
    {
        KeyS2 = 0;
        s2_toggle = 1;
        s2_delay_timer = ONE_HUNDRED_MS_TIMER_RELOAD;
    }
    else
    {
        if(!s2_delay_timer)
            KeyS2 = 1;
    }
    
    if(S3_PxIN & S3)
    {
        KeyS3 = 0;
        s3_toggle = 1;
        s3_delay_timer = ONE_HUNDRED_MS_TIMER_RELOAD;
    }
    else
    {
        if(!s3_delay_timer)
            KeyS3 = 1;
    }
    
    if(S4_PxIN & S4)
    {
        KeyS4 = 0;
        s4_toggle = 1;
        s4_delay_timer = ONE_HUNDRED_MS_TIMER_RELOAD;
    }
    else
    {
        if(!s4_delay_timer)
            KeyS4 = 1;
    }
}
void start_new_mode(char new_sm)
{
    active_sm = new_sm;
    switch(active_sm)
    {
    case SM_INIT:
        start_init_mode();
        break;
        
    case SM_TIME:
        start_time_mode();
        break;
        
    case SM_MD_HOUR:
        start_md_hour_mode();
        break;
        
    case SM_MD_MIN:
        start_md_min_mode();
        break;
        
    case SM_MD_YEAR:
        start_md_year_mode();
        break;
        
    case SM_MD_MONTH:
        start_md_month_mode();
        break;
        
    case SM_MD_DAY:
        start_md_day_mode();
        break;
        
    case SM_SHOW_TEMP:
        start_show_temp_mode();
        break;
        
    case SM_SHOW_DAY:
        start_show_day_mode();
        break;
        
    case SM_SHOW_LOVE:
        start_show_love_mode();
        break;
    }
}
void handle_active_mode()
{
	if(KeyS2 && s2_toggle)
	{
		s2_toggle = 0;
		if(beeper_on)
		{
			beeper_on = 0;
			BeeperCancel();
			beeper_delay_timer = 0;
		}
		else
		{
			beeper_on = 1;
			BeeperInit();
			beeper_cnt = 0;
			beeper_delay_timer = 0;
            //			tune_length = sizeof(tune)/sizeof(tune[0]);
		}
	}
	
	if(beeper_on)
	{
		if(!beeper_delay_timer)
		{
			if(beeper_cnt)
			{
				//insert some break between the same tune
				if(old_tune==tune[beeper_cnt])
				{
					TA0CCR0 = 0;
					old_tune = 0;
					//beeper_delay_timer = 1;
					goto Next;
				}
			}
			TA0CCR0 = tune[beeper_cnt];
			old_tune = tune[beeper_cnt];
			beeper_delay_timer = QUARTER_SECOND_TIMER_RELOAD*durt[beeper_cnt];
			beeper_cnt++;
			if(beeper_cnt >= tune_length)
			{
				beeper_cnt = 0;
			}
        Next:			_NOP();
		}
	}
	
    switch(active_sm)
    {
    case SM_INIT:
        handle_init_mode();
        break;
        
    case SM_TIME:
        handle_time_mode();
        break;
        
    case SM_MD_HOUR:
        handle_md_hour_mode();
        break;
        
    case SM_MD_MIN:
        handle_md_min_mode();
        break;
        
    case SM_MD_YEAR:
        handle_md_year_mode();
        break;
        
    case SM_MD_MONTH:
        handle_md_month_mode();
        break;
        
    case SM_MD_DAY:
        handle_md_day_mode();
        break;
        
    case SM_SHOW_TEMP:
        handle_show_temp_mode();
        break;
        
    case SM_SHOW_DAY:
        handle_show_day_mode();
        break;
        
    case SM_SHOW_LOVE:
        handle_show_love_mode();
        break;
    }
}
void start_init_mode()
{
    general_purpose_timer = ONE_SECOND_TIMER_RELOAD;
}
void handle_init_mode()
{
    if(!general_purpose_timer)
    {
        start_new_mode(SM_TIME);
    }
}
void start_time_mode()
{
    buf[0] = Blank;
    buf[1] = Blank;
    buf[2] = Blank;
    buf[3] = Blank;
    blink_flag = 1;
    dot_on = 0;
    point_on = 0;
    general_purpose_timer = 0;
    key_hold_timer = ONE_SECOND_TIMER_RELOAD;
}
void handle_time_mode()
{
    char temp;
    if(!general_purpose_timer)
    {
        dot_on ^= 1; //分割点闪烁
        general_purpose_timer = HALF_SECOND_TIMER_RELOAD;
        
        ReadTime();
        
        temp = bin2bcd(Hour);
        buf[0] = digit[temp>>4];
        buf[1] = digit[temp&0xf];
        
        temp = bin2bcd(Min);
        buf[2] = digit[temp>>4];
        buf[3] = digit[temp&0xf];
    }
    
    if(KeyS1)
    {
        s1_toggle = 0;
        _NOP();
        
        if(!key_hold_timer)
        start_new_mode(SM_MD_HOUR);
    }
    else
    {
        key_hold_timer = ONE_SECOND_TIMER_RELOAD;
    }
    
    if(KeyS3 && s3_toggle)
    {
        s3_toggle = 0;
        _NOP();
        start_new_mode(SM_SHOW_TEMP);
    }
    
    if(KeyS4 && s4_toggle)
    {
        s4_toggle = 0;
        _NOP();
        start_new_mode(SM_SHOW_DAY);
    }
}
void start_md_hour_mode()
{
    blink_flag = 1;
    dot_on = 1;
    show_hour_min = 0;
    point_on = 0;
}
void handle_md_hour_mode()
{
    char temp;
    if(!general_purpose_timer)
    {
        general_purpose_timer = HALF_SECOND_TIMER_RELOAD;
        if(buf[0] != Blank)
        {            
            buf[0] = Blank;
            buf[1] = Blank;
        }
        else
        {
            temp = bin2bcd(Hour);
            buf[0] = digit[temp>>4];
            buf[1] = digit[temp&0xf];
        }
    }
    if(KeyS3 && s3_toggle)
    {
        s3_toggle = 0;
        Hour++;
        if(Hour>23)
        {
            Hour = 0;
        }
        
        temp = bin2bcd(Hour);
        buf[0] = digit[temp>>4];
        buf[1] = digit[temp&0xf];
        general_purpose_timer = HALF_SECOND_TIMER_RELOAD;
    }
    
    if(KeyS4 && s4_toggle)
    {
        s4_toggle = 0;
        if(Hour)
        {
            Hour--;
        }
        else
        {
            Hour= 23;
        }
        temp = bin2bcd(Hour);
        buf[0] = digit[temp>>4];
        buf[1] = digit[temp&0xf];
        general_purpose_timer = HALF_SECOND_TIMER_RELOAD;
    }
    
    if(KeyS1 && s1_toggle)
    {
        s1_toggle = 0;
        _NOP();
        start_new_mode(SM_MD_MIN);
    }
}

void start_md_min_mode()
{
    char temp = bin2bcd(Hour);
    buf[0] = digit[temp>>4];
    buf[1] = digit[temp&0xf];
    blink_flag = 1;
    dot_on = 1;
    show_hour_min = 0;
    point_on = 0;
}
void handle_md_min_mode()
{
    char temp;
    if(!general_purpose_timer)
    {
        general_purpose_timer = HALF_SECOND_TIMER_RELOAD;
        if(buf[2] != Blank)
        {            
            buf[2] = Blank;
            buf[3] = Blank;
        }
        else
        {
            temp = bin2bcd(Min);
            buf[2] = digit[temp>>4];
            buf[3] = digit[temp&0xf];
        }
    }
    if(KeyS3 && s3_toggle)
    {
        s3_toggle = 0;
        Min++;
        if(Min>59)
        {
            Min = 0;
        }
        temp = bin2bcd(Min);
        buf[2] = digit[temp>>4];
        buf[3] = digit[temp&0xf];
        general_purpose_timer = HALF_SECOND_TIMER_RELOAD;
    }
    
    if(KeyS4 && s4_toggle)
    {
        s4_toggle = 0;
        if(Min)
        {
            Min--;
        }
        else
        {
            Min= 59;
        }
        temp = bin2bcd(Min);
        buf[2] = digit[temp>>4];
        buf[3] = digit[temp&0xf];
        general_purpose_timer = HALF_SECOND_TIMER_RELOAD;
    }
    
    if(KeyS1 && s1_toggle)
    {
        s1_toggle = 0;
        _NOP();
        //        WriteTime();
        //        start_new_mode(SM_TIME);
        start_new_mode(SM_MD_YEAR);
    }
}
void start_md_year_mode()
{
    char temp = bin2bcd(Year)%100;
    buf[0] = digit[2];
    buf[1] = digit[0];
    buf[2] = digit[temp>>4];
    buf[3] = digit[temp&0xf];
    blink_flag = 0;
    dot_on = 0;
    show_hour_min = 0;
    point_on = 0;
}
void handle_md_year_mode()
{
    char temp;
    if(!general_purpose_timer)
    {
        general_purpose_timer = HALF_SECOND_TIMER_RELOAD;
        if(buf[0] != Blank)
        {   
            buf[0] = Blank;
            buf[1] = Blank;
            buf[2] = Blank;
            buf[3] = Blank;
        }
        else
        {
            temp = bin2bcd(Year)%100;
            buf[0] = digit[2];
            buf[1] = digit[0];            
            buf[2] = digit[temp>>4];
            buf[3] = digit[temp&0xf];
        }
    }
    if(KeyS3 && s3_toggle)
    {
        s3_toggle = 0;
        Year++;
        if(Year>99)
        {
            Year = 0;
        }
        temp = bin2bcd(Year)%100;
        buf[0] = digit[2];
        buf[1] = digit[0];            
        buf[2] = digit[temp>>4];
        buf[3] = digit[temp&0xf];
        general_purpose_timer = HALF_SECOND_TIMER_RELOAD;
    }
    
    if(KeyS4 && s4_toggle)
    {
        s4_toggle = 0;
        if(Year)
        {
            Year--;
        }
        else
        {
            Year= 99;
        }
        temp = bin2bcd(Year)%100;
        buf[0] = digit[2];
        buf[1] = digit[0];            
        buf[2] = digit[temp>>4];
        buf[3] = digit[temp&0xf];
        general_purpose_timer = HALF_SECOND_TIMER_RELOAD;
    }
    
    if(KeyS1 && s1_toggle)
    {
        s1_toggle = 0;
        _NOP();
        //        WriteTime();
        //        start_new_mode(SM_TIME);
        start_new_mode(SM_MD_MONTH);
    }
}

void start_md_month_mode()
{
    char temp = bin2bcd(Month);
    buf[0] = digit[temp>>4];
    buf[1] = digit[temp&0xf];
    
    temp = bin2bcd(Day);
    buf[2] = digit[temp>>4];
    buf[3] = digit[temp&0xf];
    blink_flag = 1;
    dot_on = 0;
    show_hour_min = 0;
    point_on = 1;
}
void handle_md_month_mode()
{
     char temp;
    if(!general_purpose_timer)
    {
        general_purpose_timer = HALF_SECOND_TIMER_RELOAD;
        if(buf[0] != Blank)
        {   
            buf[0] = Blank;
            buf[1] = Blank;
        }
        else
        {
            temp = bin2bcd(Month);          
            buf[0] = digit[temp>>4];
            buf[1] = digit[temp&0xf];
        }
    }
    if(KeyS3 && s3_toggle)
    {
        s3_toggle = 0;
        Month++;
        if(Month>12)
        {
            Month = 1;
        }
        temp = bin2bcd(Month);          
        buf[0] = digit[temp>>4];
        buf[1] = digit[temp&0xf];
        general_purpose_timer = HALF_SECOND_TIMER_RELOAD;
    }
    
    if(KeyS4 && s4_toggle)
    {
        s4_toggle = 0;
        if(Month>1)
        {
            Month--;
        }
        else
        {
            Month= 12;
        }
        temp = bin2bcd(Month);          
        buf[0] = digit[temp>>4];
        buf[1] = digit[temp&0xf];
        general_purpose_timer = HALF_SECOND_TIMER_RELOAD;
    }
    
    if(KeyS1 && s1_toggle)
    {
        s1_toggle = 0;
        _NOP();
        //        WriteTime();
        //        start_new_mode(SM_TIME);
        start_new_mode(SM_MD_DAY);
    }
}
void start_md_day_mode()
{
    char temp = bin2bcd(Month);
    buf[0] = digit[temp>>4];
    buf[1] = digit[temp&0xf];
    
    temp = bin2bcd(Day);
    buf[2] = digit[temp>>4];
    buf[3] = digit[temp&0xf];
    blink_flag = 1;
    dot_on = 0;
    show_hour_min = 0;
    point_on = 1;
}
void handle_md_day_mode()
{
    char temp;
    if(!general_purpose_timer)
    {
        general_purpose_timer = HALF_SECOND_TIMER_RELOAD;
        if(buf[2] != Blank)
        {   
            buf[2] = Blank;
            buf[3] = Blank;
        }
        else
        {
            temp = bin2bcd(Day);          
            buf[2] = digit[temp>>4];
            buf[3] = digit[temp&0xf];
        }
    }
    if(KeyS3 && s3_toggle)
    {
        s3_toggle = 0;
        Day++;
        if(Day > day_per_month[is_leap_year(Year)][Month-1])
        {
            Day = 1;
        }
        temp = bin2bcd(Day);          
        buf[2] = digit[temp>>4];
        buf[3] = digit[temp&0xf];
        general_purpose_timer = HALF_SECOND_TIMER_RELOAD;
    }
    
    if(KeyS4 && s4_toggle)
    {
        s4_toggle = 0;
        if(Day>1)
        {
            Day--;
        }
        else
        {
            Day = day_per_month[is_leap_year(Year)][Month-1];
        }
        temp = bin2bcd(Day);          
        buf[2] = digit[temp>>4];
        buf[3] = digit[temp&0xf];
        general_purpose_timer = HALF_SECOND_TIMER_RELOAD;
    }
    
    if(KeyS1 && s1_toggle)
    {
        s1_toggle = 0;
        _NOP();
        WriteTime();
        start_new_mode(SM_TIME);
    }
}
void start_show_temp_mode()
{
    blink_flag = 1;
    dot_on = 0;
    show_hour_min = 0;
    point_on = 0;
    buf[0] = Blank;
    buf[1] = Blank;
    buf[2] = Blank;
    buf[3] = Blank;
    MeasureStep = 0;
    general_purpose_timer = 0;
}

void handle_show_temp_mode()
{
    //    char temp;
    //    Temperature = Read_Temperature();
    //    _NOP();
    if(KeyS3 && s3_toggle)
    {
        s3_toggle = 0;
        _NOP();
        start_new_mode(SM_SHOW_LOVE);
    }
    
    if(KeyS4 && s4_toggle)
    {
        s4_toggle = 0;
        _NOP();
        start_new_mode(SM_TIME);
    }
    
    if(!general_purpose_timer)
    {
        if(MeasureStep == 2)
            MeasureStep = 0;
    }
    
    if(!MeasureStep)
    {
        StartMeasure();
        measure_delay_timer = ONE_SECOND_TIMER_RELOAD;
        MeasureStep = 1;
    }
    
    if(MeasureStep == 1)
    {
        if(!measure_delay_timer)
        {
            Temperature = GetTemp();
            MeasureStep = 2;
            general_purpose_timer = ONE_SECOND_TIMER_RELOAD;
            Disp_Numb(Temperature);
            
            buf[3] = degree;
            if(is_negative)//负温度
            {
                buf[0] = negative; //第一个字符 '-'
                if(dN[5])
                {
                    buf[1] = digit[dN[5]]; //十位
                    buf[2] = digit[dN[4]]; //个位
                    point_on = 0;
                }
                else
                {
                    buf[1] = digit[dN[4]]; //个位
                    buf[2] = digit[dN[3]]; //小数点后1位
                    point_on = 1;
                }
            }
            else//正温度
            {
                buf[0] = digit[dN[5]]; //十位
                buf[1] = digit[dN[4]]; //个位
                buf[2] = digit[dN[3]]; //小数点后1位
                point_on = 1;
            }
            /*
            if(Temperature & BITF) //负温度
            {
            buf[0] = negative; //第一个字符 '-'
            Temperature = Temperature ^ 0xffff;
            Temperature += 1;
            Temperature >>= 3;
            
            if(Temperature >= 20) //大于10度
            {
            point_on = 0;
            buf[1] = digit[(Temperature>>1)/10]; //十位
            buf[2] = digit[(Temperature>>1)%10]; //个位
            buf[3] = degree;
        }
                else
            {
            point_on = 1;
            buf[1] = digit[(Temperature>>1)%10]; //个位
            if(Temperature & BIT0)
            {
            buf[2] = digit[5]; //小数点后1位 .5
        }
                    else
            {
            buf[2] = digit[0]; //小数点后1位 .0
        }
            buf[3] = degree;
        }
        }
            else //正温度
            {
            Temperature >>= 3;
            if(Temperature >= 200) //大于100度
            {
            point_on = 0;
            temp = (Temperature>>1)/100;
            buf[0] = digit[temp]; //百位
            temp = ((Temperature>>1)%100)/10;
            buf[1] = digit[temp]; //十位
            temp = (Temperature>>1)%10;
            buf[2] = digit[temp]; //个位
            buf[3] = degree;
        }
                else
            {
            point_on = 1;
            temp = (Temperature>>1)/10;
            buf[0] = digit[temp]; //十位
            temp = (Temperature>>1)%10;
            buf[1] = digit[temp]; //个位
            if(Temperature & BIT0)
            {
            buf[2] = digit[5]; //小数点后1位 .5
        }
                    else
            {
            buf[2] = digit[0]; //小数点后1位 .0
        }
            buf[3] = degree;
        }
        }*/
        }
    }
}

void start_show_day_mode()
{
    buf[0] = Blank;
    buf[1] = Blank;
    buf[2] = Blank;
    buf[3] = Blank;
    blink_flag = 1;
    dot_on = 0;
    point_on = 0;
    general_purpose_timer = 0;
}
void handle_show_day_mode()
{
    char temp;
    if(!general_purpose_timer)
    {
        general_purpose_timer = HALF_SECOND_TIMER_RELOAD;
        
        ReadTime();
        
        temp = bin2bcd(Month);
        buf[0] = digit[temp>>4];
        buf[1] = digit[temp&0xf];
        
        temp = bin2bcd(Day);
        buf[2] = digit[temp>>4];
        buf[3] = digit[temp&0xf];
        
        point_on = 1;
    }
    
    //    if(KeyS1 && s1_toggle)
    //    {
    //        s1_toggle = 0;
    //        _NOP();
    //        start_new_mode(SM_MD_HOUR);
    //    }
    
    if(KeyS3 && s3_toggle)
    {
        s3_toggle = 0;
        _NOP();
        start_new_mode(SM_TIME);
    }
    
    if(KeyS4 && s4_toggle)
    {
        s4_toggle = 0;
        _NOP();
        start_new_mode(SM_SHOW_LOVE);
    }
}
void start_show_love_mode()
{
    buf[0] = Blank;
    buf[1] = Blank;
    buf[2] = Blank;
    buf[3] = Blank;
    blink_flag = 0;
    dot_on = 0;
    point_on = 0;
    general_purpose_timer = 0;
}
void handle_show_love_mode()
{
    static char invert;
    if(!general_purpose_timer)
    {
        general_purpose_timer = ONE_SECOND_TIMER_RELOAD;
        
        if(invert)
        {
            invert = 0;
            buf[0] = love_1;
            buf[1] = love_2;
            buf[2] = love_3;
            buf[3] = love_4;
        }
        else
        {
            invert = 1;
            buf[0] = love_inv_1;
            buf[1] = love_inv_2;
            buf[2] = love_inv_3;
            buf[3] = love_inv_4;
        }
    }
    
    //    if(KeyS1 && s1_toggle)
    //    {
    //        s1_toggle = 0;
    //        _NOP();
    //        start_new_mode(SM_MD_HOUR);
    //    }
    
    if(KeyS3 && s3_toggle)
    {
        s3_toggle = 0;
        _NOP();
        start_new_mode(SM_SHOW_DAY);
    }
    
    if(KeyS4 && s4_toggle)
    {
        s4_toggle = 0;
        _NOP();
        start_new_mode(SM_SHOW_TEMP);
    }
}
int main(void)
{
	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
	ClockInit();
	IOInit();
    TimerInit();
	IICInit();
    
    _EINT();
    RTCInit();
    start_new_mode(SM_INIT);
    
    while(1)
    {
//        WDTCTL = WDT_ARST_1000;
        ScanInput();
        handle_active_mode();
    }
	
}

