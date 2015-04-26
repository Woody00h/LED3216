#ifndef BEEPER_H
#define BEEPER_H

#define D_TUNE

#define NTD0 0

#ifdef D_TUNE
//Dµ÷
#define NTD1 20408
#define NTD2 18182
#define NTD3 17143
#define NTD4 15267
#define NTD5 13605
#define NTD6 12121
#define NTD7 10791

#define NTDL1 40816
#define NTDL2 36364
#define NTDL3 34286
#define NTDL4 30612
#define NTDL5 27149
#define NTDL6 24194
#define NTDL7 21583

#define NTDH1 10187
#define NTDH2 9077
#define NTDH3 8571
#define NTDH4 7634
#define NTDH5 6803
#define NTDH6 6061
#define NTDH7 5396
#endif

#ifdef F_TUNE
//Fµ÷
#define NTD1 17143
#define NTD2 15267
#define NTD3 13605
#define NTD4 12121
#define NTD5 10791
#define NTD6 9615
#define NTD7 9077

#define NTDL1 34286
#define NTDL2 30612
#define NTDL3 27149
#define NTDL4 25641
#define NTDL5 22901
#define NTDL6 20408
#define NTDL7 18182

#define NTDH1 8571
#define NTDH2 7634
#define NTDH3 6803
#define NTDH4 6417
#define NTDH5 5720
#define NTDH6 5093
#define NTDH7 4539
#endif

#ifdef _BEEPER_
#define BEEPER_EXTERN
#else
#define BEEPER_EXTERN extern
#endif

#define TUNE_LENGTH (sizeof(tune)/sizeof(tune[0]))
BEEPER_EXTERN unsigned char beeper_on;
BEEPER_EXTERN unsigned char tune_length;
BEEPER_EXTERN unsigned int old_tune;
extern const unsigned int tune[];
extern const unsigned char durt[];
BEEPER_EXTERN unsigned char beeper_cnt;
void BeeperInit();
void BeeperCancel();

#endif