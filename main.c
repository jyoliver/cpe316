#include "msp.h"


/**
 * main.c
 */
void main(void)
{

    //comment3..
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
}
