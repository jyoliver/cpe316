#include "delay.h"
#include "msp.h"


void delay_ms (int ms, int freq) { //delay function for milliseconds
int i, j; //initialize variables for timing
for (j=ms; j>0 ; j--) //counts clock cycles exits when condition is satisfied
    for (i=freq/10000; i>0;i--);
}


void delay_us (int us, int freq) { //delay function for microseconds case by case
int i, j;
if (freq == 1500000) {
    for (j=(us*3/20)-7; j>0 ; j--)
    for (i=freq/1000000; i>0;i--);
}
else if (freq == 3000000) {
    for (j=(us*3/15)-7; j>0 ; j--)
    for (i=freq/1000000; i>0;i--);
}

else if (freq == 6000000) {
    for (j=(us*3/13)-7; j>0 ; j--)
    for (i=freq/1000000; i>0;i--);
}

else if (freq == 12000000) {
    for (j=us/12; j>0 ; j--)
    for (i=freq/1000000; i>0;i--);
}
else if (freq == 24000000) {
    for (j=(us*3/11)-7; j>0 ; j--)
    for (i=freq/1000000; i>0;i--);
}
else if (freq == 48000000) {
   for (j=(us*3/11)-7; j>0 ; j--)
    for (i=freq/1000000; i>0;i--);
}

}

void set_DCO(int freq) { //set the digitally controlled oscillator frequency
    CS->KEY=CS_KEY_VAL; //unlock register
    CS->CTL0=0; //clears register
    if(freq==FREQ_1_5_MHZ){ //case for 1.5MHZ
        CS->CTL0=CS_CTL0_DCORSEL_0; //set DCO=1.5MHZ
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
        CS->KEY = 0; //lock CS registers
    }
    else if (freq==FREQ_3_MHZ){
        CS->CTL0=CS_CTL0_DCORSEL_1;
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
        CS->KEY = 0; //lock CS registers
    }
    else if (freq==FREQ_6_MHZ){
        CS->CTL0=CS_CTL0_DCORSEL_2;
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
        CS->KEY = 0;//lock CS registers
    }
    else if (freq==FREQ_12_MHZ){
        CS->CTL0=CS_CTL0_DCORSEL_3;
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
        CS->KEY = 0;//lock CS registers
    }
    else if (freq==FREQ_24_MHZ){
        CS->CTL0=CS_CTL0_DCORSEL_4;
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
        CS->KEY = 0; //lock CS registers
    }
    else if (freq==FREQ_48_MHZ){
        /* Transition to VCORE Level 1: AM0_LDO --> AM1_LDO */
        while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
         PCM->CTL0 = PCM_CTL0_KEY_VAL | PCM_CTL0_AMR_1;
        while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
        /* Configure Flash wait-state to 1 for both banks 0 & 1 */
        FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL &
         ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) | FLCTL_BANK0_RDCTL_WAIT_1;
        FLCTL->BANK1_RDCTL = (FLCTL->BANK0_RDCTL &
         ~(FLCTL_BANK1_RDCTL_WAIT_MASK)) | FLCTL_BANK1_RDCTL_WAIT_1;
        /* Configure DCO to 48MHz, ensure MCLK uses DCO as source*/
        CS->KEY = CS_KEY_VAL ; // Unlock CS module for register access
        CS->CTL0 = 0; // Reset tuning parameters
        CS->CTL0 = CS_CTL0_DCORSEL_5; // Set DCO to 48MHz
        /* Select MCLK = DCO, no divider */
        CS->CTL1 = CS->CTL1 & ~(CS_CTL1_SELM_MASK | CS_CTL1_DIVM_MASK) |
         CS_CTL1_SELM_3;
        CS->KEY = 0; // Lock CS module from unintended accesses
    }

    }
