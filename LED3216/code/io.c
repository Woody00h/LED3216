#include <msp430.h>
#include "Hardware.h"
#include "DS18B20.h"
void IOInit()
{
	PAOUT = 0xffff;
	PADIR = 0xffff; //row
	
	P4OUT = 0;
	P6OUT = 0;
	P9OUT = 0;
	P10OUT = 0; //column
	
	P4DIR = 0xff;
	P6DIR = 0xff;
	P9DIR = 0xff;
	P10DIR = 0xff;
        
        S1_PxDIR &= ~S1;
        S2_PxDIR &= ~S2;
        S3_PxDIR &= ~S3;
        S4_PxDIR &= ~S4;
        
        //ÉèÖÃDS18B20µÄIO×´Ì¬
        DQ_PxDIR |= DQ;
        DQ_PxOUT |= DQ;
}