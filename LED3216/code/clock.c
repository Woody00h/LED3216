#include <msp430.h>

void SetVcoreUp (unsigned int level)
{
  // Open PMM registers for write
  PMMCTL0_H = PMMPW_H;              
  // Set SVS/SVM high side new level
  SVSMHCTL = SVSHE + SVSHRVL0 * level + SVMHE + SVSMHRRL0 * level;
  // Set SVM low side to new level
  SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 * level;
  // Wait till SVM is settled
  while ((PMMIFG & SVSMLDLYIFG) == 0);
  // Clear already set flags
  PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);
  // Set VCore to new level
  PMMCTL0_L = PMMCOREV0 * level;
  // Wait till new level reached
  if ((PMMIFG & SVMLIFG))
    while ((PMMIFG & SVMLVLRIFG) == 0);
  // Set SVS/SVM low side to new level
  SVSMLCTL = SVSLE + SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level;
  // Lock PMM registers for write access
  PMMCTL0_H = 0x00;
}

void ClockInit()
{
	SetVcoreUp(PMMCOREV_1); 
	SetVcoreUp(PMMCOREV_2);                     // Set VCore to 1.8MHz for 20MHz
	
//	P11DIR = BIT1+BIT2;                       // P11.1-2 to output direction
//	P11SEL |= BIT1+BIT2;                      // P11.1-2 to output SMCLK,MCLK
	P5SEL |= 0x0C;                            // Port select XT2
	
	UCSCTL6 &= ~XT2OFF;                       // Enable XT2
	UCSCTL3 |= SELREF_2;                      // FLLref = REFO
						  // Since LFXT1 is not used,
						  // sourcing FLL with LFXT1 can cause
						  // XT1OFFG flag to set
	UCSCTL4 |= SELA_2;                        // ACLK=REFO,SMCLK=DCO,MCLK=DCO
	
	// Loop until XT1,XT2 & DCO stabilizes
	do
	{
		UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);
						  // Clear XT2,XT1,DCO fault flags
		SFRIFG1 &= ~OFIFG;                // Clear fault flags
	}while (SFRIFG1&OFIFG);                   // Test oscillator fault flag
	
	UCSCTL6 &= ~XT2DRIVE0;                    // Decrease XT2 Drive according to
						  // expected frequency
	UCSCTL4 |= SELS_5 + SELM_5 + SELA_5;      // SMCLK=MCLK=XT2
        
        UCSCTL5 |= DIVA__8; 
            
}