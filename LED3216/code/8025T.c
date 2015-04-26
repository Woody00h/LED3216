#include <msp430.h>
#include "8025T.h"
unsigned char iicBuf[10];
unsigned char *PTxData;                     // Pointer to TX data
unsigned char TXByteCtr;
unsigned char *PRxData;                     // Pointer to RX data
unsigned char RXByteCtr;

extern unsigned char Year;
extern unsigned char Month;
extern unsigned char Day;
extern unsigned char Week;
extern unsigned char WeekDay;
extern unsigned char Hour;
extern unsigned char Min;
extern unsigned char Second;
void IICInit()
{
    P3SEL |= 0x06;                            // Assign I2C pins to USCI_B0
    
    UCB0CTL1 |= UCSWRST;                      // Enable SW reset
    
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
    
    UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
    
    UCB0BR0 = 120;                            // fSCL = SMCLK/120 = ~100kHz
    
    UCB0BR1 = 0;
    
    UCB0I2CSA = 0x64>>1;                         // Slave Address is 048h
    
    UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
    
    UCB0IE |= UCTXIE + UCRXIE;                 //INT Enable
}
char is_leap_year(unsigned int year)
{
    return (!(year % 4) && (year % 100)) || !(year % 400);
}
unsigned char bcd2bin(char val)
{
    return ((val&0x0f)+(val >> 4)*10);
}
unsigned char bin2bcd(char val)
{
    return ((val/10) << 4) + val%10;
}

unsigned char Code2Week(char data)
{
    return data;
}
void RTCInit()
{
    PTxData = iicBuf;
    TXByteCtr = 2;
    iicBuf[0] = 0xf;
    iicBuf[1] = 0xc0;
    
      while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    UCB0CTL1 |= UCTR + UCTXSTT;             // I2C TX, start condition
    
     __bis_SR_register(LPM0_bits + GIE);     // Enter LPM0, enable interrupts
    __no_operation();                       // Remain in LPM0 until all data
                                            // is TX'd
    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    __delay_cycles(100);
}
void ReadTime()
{
    PTxData = iicBuf;
    TXByteCtr = 1;
    iicBuf[0] = 0;
    
    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    UCB0CTL1 |= UCTR + UCTXSTT;             // I2C TX, start condition
    
     __bis_SR_register(LPM0_bits + GIE);     // Enter LPM0, enable interrupts
    __no_operation();                       // Remain in LPM0 until all data
                                            // is TX'd
    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    
    __delay_cycles(100);
    
    PRxData = iicBuf;
    RXByteCtr = 7;
    
    UCB0CTL1 &= ~UCTR;
    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    UCB0CTL1 |= UCTXSTT;                    // I2C RX, start condition
    
    //while(UCB0CTL1 & UCTXSTT);
     __bis_SR_register(LPM0_bits + GIE);     // Enter LPM0, enable interrupts
    __no_operation();                       // Remain in LPM0 until all data
                                            // is RX'd
    
    
    /*
    while(!(UCB0IFG & UCTXIFG));
    UCB0TXBUF = 0;
    while(!(UCB0IFG & UCTXIFG));
    
    UCB0CTL1 &= ~UCTR;
    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    UCB0CTL1 |= UCTXSTT;             // I2C TX, start condition
    while(UCB0CTL1 & UCTXSTT);
    
    for(i=0;i<7;i++)
    {
        while(!(UCB0IFG&UCRXIFG));
        iicBuf[i] = UCB0RXBUF;
        if(i==5)
        {
            UCB0CTL1 |= UCTXSTP;                // Generate I2C stop condition
        }
    }
    */
    Year = bcd2bin(iicBuf[6]);
    Month = bcd2bin(iicBuf[5]);
    Day = bcd2bin(iicBuf[4]);
    Week = bcd2bin(iicBuf[3]);
    Hour = bcd2bin(iicBuf[2]);
    Min = bcd2bin(iicBuf[1]);
    Second = bcd2bin(iicBuf[0]);
}

void WriteTime()
{
    PTxData = iicBuf;
    TXByteCtr = 8;
    iicBuf[0] = 0;
    iicBuf[1] = bin2bcd(Second);
    iicBuf[2] = bin2bcd(Min);
    iicBuf[3] = bin2bcd(Hour);
    iicBuf[4] = bin2bcd(Week);
    iicBuf[5] = bin2bcd(Day);
    iicBuf[6] = bin2bcd(Month);
    iicBuf[7] = bin2bcd(Year);
    
     while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    UCB0CTL1 |= UCTR + UCTXSTT;             // I2C TX, start condition
    
     __bis_SR_register(LPM0_bits + GIE);     // Enter LPM0, enable interrupts
    __no_operation();                       // Remain in LPM0 until all data
                                            // is TX'd
    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    /*
    while(UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    UCB0CTL1 |= UCTR + UCTXSTT;             // I2C TX, start condition
    //while(UCB0CTL1 & UCTXSTT);
    
    while(!(UCB0IFG & UCTXIFG));
    UCB0TXBUF = 0;
    while(!(UCB0IFG & UCTXIFG));
    
    for(i=0;i<7;i++)
    {
        while(!(UCB0IFG&UCTXIFG));
        UCB0TXBUF = iicBuf[i];
    }
    
    UCB0CTL1 |= UCTXSTP;                // Generate I2C stop condition
    */
}

#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
{
    switch(__even_in_range(UCB0IV,12))
    {
    case  0: break;                           // Vector  0: No interrupts
    case  2: break;                           // Vector  2: ALIFG
    case  4: break;                           // Vector  4: NACKIFG
    case  6: break;                           // Vector  6: STTIFG
    case  8: break;                           // Vector  8: STPIFG
    case 10:                                    // Vector 10: RXIFG
        if (RXByteCtr)
        {
            if (RXByteCtr == 1)                   // Only one byte left?
            {
                UCB0CTL1 |= UCTXSTP;                // Generate I2C stop condition
                __bic_SR_register_on_exit(LPM0_bits); // Exit active CPU
            }
            RXByteCtr--;
            *PRxData++ = UCB0RXBUF;               // Move RX data to address PRxData
        }
        else
        {
            *PRxData = UCB0RXBUF;                 // Move final RX data to PRxData
            __bic_SR_register_on_exit(LPM0_bits); // Exit active CPU
        }
        break;                           
    case 12:                                  // Vector 12: TXIFG
        if (TXByteCtr)                          // Check TX byte counter
        {
            UCB0TXBUF = *PTxData++;               // Load TX buffer
            TXByteCtr--;                          // Decrement TX byte counter
        }
        else
        {
            UCB0CTL1 |= UCTXSTP;                  // I2C stop condition
            UCB0IFG &= ~UCTXIFG;                  // Clear USCI_B0 TX int flag
            __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0
        }
    default: break;
    }
}
