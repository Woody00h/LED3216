#define LED3216_TIMER
#include <msp430.h>
#include "timer.h"
#include "data.h"
char RowCnt = 0;
const unsigned char *buf[4];
void TimerInit()
{
    TBCCTL0 = CCIE;                           // CCR0 interrupt enabled
    TBCCR0 = 10000;				  // 5/6 ms
    TBCTL = TBSSEL_2 + MC_2 + TBCLR;          // SMCLK, contmode, clear TBR
    
    TA1CCTL0 = CCIE;                           // CCR0 int enable
    TA1CCR0 = 9375;
    TA1CTL = TASSEL__ACLK + MC_1 + TACLR + ID__8;         // ACLK, upmode, clear TAR
}

//TimerA1 interrupt service routine 20Hz
#pragma vector = TIMER1_A0_VECTOR
__interrupt void TimerA1_ISR()
{
    if(measure_delay_timer)     measure_delay_timer--;
    if(general_purpose_timer)   general_purpose_timer--;
    if(s1_delay_timer)          s1_delay_timer--;
    if(s2_delay_timer)          s2_delay_timer--;
    if(s3_delay_timer)          s3_delay_timer--;
    if(s4_delay_timer)          s4_delay_timer--;
    if(beeper_delay_timer)	    beeper_delay_timer--;
    if(key_hold_timer)          key_hold_timer--;
}

// Timer B0 interrupt service routine
#pragma vector=TIMERB0_VECTOR
__interrupt void TIMERB0_ISR (void)
{
    PAOUT = 0xffff;
    
    if(blink_flag)
    {
        DIGIT_1 = buf[0][RowCnt]>>1;
        if(dot_on && (RowCnt==5 || RowCnt==9))
            DIGIT_2 = (buf[1][RowCnt]>>1)|0x80;
        else
        {
            if(point_on && RowCnt == 11)
            {
                DIGIT_2 = (buf[1][RowCnt]>>1)|0x80;
            }
            else
            {
                DIGIT_2 = buf[1][RowCnt]>>1;
            }
        }
    }
    else
    {
        DIGIT_1 = buf[0][RowCnt];
        DIGIT_2 = buf[1][RowCnt];
    }
    
    DIGIT_3 = buf[2][RowCnt];
    DIGIT_4 = buf[3][RowCnt];
    
    PAOUT = 0xffff&(~(1<<RowCnt));
    RowCnt++;
    if(RowCnt>15)
    {
        RowCnt = 0;
    }
    
    TBCCR0 += 10000;                          // Add Offset to CCR0 [Cont mode]
}

