#ifndef _TIMER_
#define _TIMER_

#ifndef LED3216_TIMER
#define EXTERN  extern
#else
#define EXTERN
#endif

void TimerInit();
#define DIGIT_1 P9OUT
#define DIGIT_2 P10OUT
#define DIGIT_3 P4OUT
#define DIGIT_4 P6OUT
#define QUARTER_SECOND_TIMER_RELOAD 5
#define HALF_SECOND_TIMER_RELOAD    10
#define ONE_SECOND_TIMER_RELOAD     20
#define TWO_SECOND_TIMER_RELOAD     40
#define ONE_HUNDRED_MS_TIMER_RELOAD 2
EXTERN unsigned char blink_flag;
EXTERN unsigned char dot_on;  //时分间隔符s
EXTERN unsigned char point_on;//小数点
EXTERN unsigned char show_hour_min;
EXTERN unsigned int general_purpose_timer;
EXTERN unsigned int key_hold_timer;
EXTERN unsigned int s1_delay_timer;
EXTERN unsigned int s2_delay_timer;
EXTERN unsigned int s3_delay_timer;
EXTERN unsigned int s4_delay_timer;
EXTERN unsigned int measure_delay_timer;
EXTERN unsigned int beeper_delay_timer;
EXTERN unsigned char DeadZoneFlag;
#endif