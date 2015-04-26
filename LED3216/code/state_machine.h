#ifndef _STATE_MACHINE_
#define _STATE_MACHINE_

#define SM_INIT         10
#define SM_TIME         20
#define SM_MD_HOUR      30
#define SM_MD_MIN       40
#define SM_MD_YEAR      50
#define SM_MD_MONTH     60
#define SM_MD_DAY       70
#define SM_SHOW_TEMP    80
#define SM_SHOW_DAY     90
#define SM_SHOW_LOVE    100

#ifndef LED3216_SM
#define SM_EXTERN  extern
#else
#define SM_EXTERN
#endif

SM_EXTERN unsigned char active_sm;
void start_new_mode(char new_sm);
void handle_active_mode();
void start_init_mode();
void handle_init_mode();
void start_time_mode();
void handle_time_mode();
void start_md_hour_mode();
void handle_md_hour_mode();
void start_md_min_mode();
void handle_md_min_mode();
void start_md_year_mode();
void handle_md_year_mode();
void start_md_month_mode();
void handle_md_month_mode();
void start_md_day_mode();
void handle_md_day_mode();
void start_show_temp_mode();
void handle_show_temp_mode();
void start_show_day_mode();
void handle_show_day_mode();
void start_show_love_mode();
void handle_show_love_mode();
#endif