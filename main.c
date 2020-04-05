/*
 *  Example using SPI to connect to MCP 4921
 *  P2.6  Port2.6   CS
 *  P1.5  UCB0CLK   SCK
 *  P1.6  UCB0SIMO  SDI
 *
 *
 *  LDAC - ground to always set low, no buffering
 *  MISO / SIMO is not needed because no data coming from DAC
 *
 *  Write values 0 to 4095 to
 */

#include "msp.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "delay.h"
#include "keypad.h"
#include "LCD.h"

#define GAIN BIT5
#define SHDN BIT4
#define DAC_CS  BIT6//4

#define CURRENT_CLK_FREQ FREQ_6_MHZ

// data to send to SPI
uint16_t data = 0;

// create values for wave selection, initialized to a 100 Hz, 50% duty cycle square wave
uint16_t FREQ = 100;
uint8_t key_val = 0xFF;
uint8_t WAVE_TYPE = 0;


// initialize Timer compare values for square wave
uint32_t ccr0on  = 15000; // for 100 hz 50% duty cycle, value gets reset anyways
uint32_t ccr0off = 15000;

// initialize timer compare values for sine wave
uint16_t ccr0sin = 117*24;

// variables for duty cycle
uint8_t DUTY = 50; // start up duty cycle at 50 %
uint8_t DUTY_ON,DUTY_OFF;// = 50;



// initialize stuff for LCD
char buffer[50] = "";

// create sin table
uint16_t index = 0;
int sin_table[256] = {2048,2098,2148,2198,2248,2298,2348,2398,
                      2447,2496,2545,2594,2642,2690,2737,2784,
                      2831,2877,2923,2968,3013,3057,3100,3143,
                      3185,3226,3267,3307,3346,3385,3423,3459,
                      3495,3530,3565,3598,3630,3662,3692,3722,
                      3750,3777,3804,3829,3853,3876,3898,3919,
                      3939,3958,3975,3992,4007,4021,4034,4045,
                      4056,4065,4073,4080,4085,4089,4093,4094,
                      4095,4094,4093,4089,4085,4080,4073,4065,
                      4056,4045,4034,4021,4007,3992,3975,3958,
                      3939,3919,3898,3876,3853,3829,3804,3777,
                      3750,3722,3692,3662,3630,3598,3565,3530,
                      3495,3459,3423,3385,3346,3307,3267,3226,
                      3185,3143,3100,3057,3013,2968,2923,2877,
                      2831,2784,2737,2690,2642,2594,2545,2496,
                      2447,2398,2348,2298,2248,2198,2148,2098,
                      2048,1997,1947,1897,1847,1797,1747,1697,
                      1648,1599,1550,1501,1453,1405,1358,1311,
                      1264,1218,1172,1127,1082,1038,995,952,
                      910,869,828,788,749,710,672,636,
                      600,565,530,497,465,433,403,373,
                      345,318,291,266,242,219,197,176,
                      156,137,120,103,88,74,61,50,
                      39,30,22,15,10,6,2,1,
                      0,1,2,6,10,15,22,30,
                      39,50,61,74,88,103,120,137,
                      156,176,197,219,242,266,291,318,
                      345,373,403,433,465,497,530,565,
                      600,636,672,710,749,788,828,869,
                      910,952,995,1038,1082,1127,1172,1218,
                      1264,1311,1358,1405,1453,1501,1550,1599,
                      1648,1697,1747,1797,1847,1897,1947,1997};

void keypad_logic(void);
void wave_params_to_LCD(void);
int main(void)
{


    set_DCO(CURRENT_CLK_FREQ); //set DCO frequency
    keypad_init(); // initialize keypad

    LCD_init(); // set up LCD

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;      // Stop watchdog timer

    // SPI Functionality
    P1->SEL0 |= BIT5 | BIT6 | BIT7;     // Set P1.5, P1.6, and P1.7 as
                                        // SPI pins functionality
    P1->DIR |= BIT0; //Make LED output PROJECT 1 1a
    P2->DIR |= DAC_CS;                      // set P2.6 as output for CS

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST; // Put eUSCI state machine in reset

    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST    |  // keep eUSCI in reset
                      EUSCI_B_CTLW0_MST      |  // Set as SPI master
                      EUSCI_B_CTLW0_SYNC     |  // Set as synchronous mode
                      EUSCI_B_CTLW0_CKPL     |  // Set clock polarity high
                      EUSCI_B_CTLW0_UCSSEL_2 |  // SMCLK
                      EUSCI_B_CTLW0_MSB;        // MSB first

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;  // Initialize USCI state machine


    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS; // SMCLK, continuous mode
    TIMER_A0->CCR[0] = ccr0on;   // set CCR0 count for startup
    keypad_logic(); // calculate stuff to put in square


 /*   // write numbers 0-7 to SPI. Use TXIFG to verify TXBUF is empty
            for(data=0; data<4096; data++)
            {
    // set the low and high bytes of data


    }*/


    // set up P5 interrupt for keypad (inputs only). Rows 1-4 are input on P5.0,5.1,5.2,and 5.7
    P5->IE |= ROW1 | ROW2 | ROW3 | ROW4;
    // we want the keypad getting function to happen when rows go high to low transition
    P5->IES |= ROW1 | ROW2 | ROW3 | ROW4;
    P5->IES = 0;

    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);   // set NVIC interrupt for ta0_0
    NVIC->IP[2] = 0x0001; //set TA0_0 interrupt to priority 1.

    NVIC->ISER[1] = 1 << ((PORT5_IRQn) & 31);   // set NVIC interrupt for Port 5
    __enable_irq();

    // main code loop. should contain nothing, because all events in this code are interrupt triggered
    while(1) {

       }

}
void send_data(uint16_t data) {

                     uint8_t loByte = 0xFF & data;         // mask just low 8 bits
                     uint8_t hiByte = 0x0F & (data >> 8);  // shift and mask bits for D11-D8
                     hiByte |= (GAIN | SHDN);      // set the gain / shutdown control bits

                     P2->OUT &= ~DAC_CS; // Set CS low

                     // wait for TXBUF to be empty before writing high byte
                     while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
                     EUSCI_B0->TXBUF = hiByte;

                     // wait for TXBUF to be empty before writing low byte
                     while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
                     EUSCI_B0->TXBUF = loByte;

                     // wait for RXBUF to be empty before changing CS
                     while(!(EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG));

                     P2->OUT |= DAC_CS; // Set CS high
 }


// function to handle logic for keypad press. Also handles getting the input
void keypad_logic(void) {
    switch(key_val) {
    case 1: FREQ = 100; break;
    case 2: FREQ = 200; break;
    case 3: FREQ = 300; break;
    case 4: FREQ = 400; break;
    case 5: FREQ = 500; break;
    case 7: WAVE_TYPE = 0; break;
    case 8: WAVE_TYPE = 1; break;
    case 9: WAVE_TYPE = 2; break;
    case 10: if ((DUTY > 10) && (WAVE_TYPE == 0)) {DUTY -= 10;} break;
    case 0: DUTY = 50; break;
    case 12: if ((DUTY < 90) && (WAVE_TYPE == 0)) {DUTY +=10;} break;
    default: break;
    }
    // math that needs to be run on this data
    if (WAVE_TYPE == 0) { // square wave
        TIMER_A0->CTL = 0x0226; // continuous mode, no divider
        DUTY_ON = DUTY;
        DUTY_OFF = 100-DUTY;
        ccr0on = (CURRENT_CLK_FREQ*DUTY_ON)/(100*FREQ);
        ccr0off = (CURRENT_CLK_FREQ*DUTY_OFF)/(100*FREQ);
        }
    else if (WAVE_TYPE == 1) { // if sine, use continuous mode
        TIMER_A0->CTL = 0x0226; // continuous mode, no divider
        if (FREQ == 100) {
            ccr0sin = (CURRENT_CLK_FREQ)/(FREQ*256);
        }
        else if (FREQ == 200) {
            ccr0sin = (CURRENT_CLK_FREQ)/(FREQ*128);
        }
        else if (FREQ == 300) {
            ccr0sin = (CURRENT_CLK_FREQ)/(FREQ*64);
        }
        else if (FREQ == 400) {
            ccr0sin = (CURRENT_CLK_FREQ)/(FREQ*32);
        }
        else if (FREQ == 500) {
            ccr0sin = (CURRENT_CLK_FREQ)/(FREQ*16);
        }
    }
    else if (WAVE_TYPE == 2) { // if sawtooth, make adjustments to CCR
            TIMER_A0->CTL = 0x0216; // up mode, no divider
            if (FREQ == 100) {
                TIMER_A0->CCR[0] = 1500;//3200;////174;
            }
            else if (FREQ == 200) {
                TIMER_A0->CCR[0] = 1200;//75;
            }
            else if (FREQ == 300) {
                TIMER_A0->CCR[0] = 900;//35;
            }
            else if (FREQ == 400) {
                TIMER_A0->CCR[0] = 600;//1280;//25;
            }
            else if (FREQ == 500) {
                TIMER_A0->CCR[0] = 300;//640;//10;
            }
        }
}



// keypad interrupt service routine. Here, the port used is P5.
void PORT5_IRQHandler (void) {
    P5->IFG = 0;
    key_val = keypad_getkey(); // capture key pressed
    while ((P5->IN & (ROW1 | ROW2 | ROW3 | ROW4)) != 0) { // only let go when get not pressed
    }
    keypad_logic(); // compute values from key pressed
    wave_params_to_LCD(); // write those values to LCD
}

// Timer A0_0 interrupt service routine. Used for controlling when DAC outputs.
 void TA0_0_IRQHandler(void) {
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  // Clear the CCR0 interrupt

    // if wave is a square wave
    if (WAVE_TYPE == 0) {
        if (P1->OUT & BIT0) { // if Bit 0 of P1 (LED) is high
            P1->OUT &= ~BIT0; // make it low
            send_data(4095);
            TIMER_A0->CCR[0] += ccr0on;
        }
        else { // if Bit 0 of P1 (LED) is low
            P1 ->OUT |= BIT0; // make it high
            send_data(0);
            TIMER_A0->CCR[0] += ccr0off;
        }
    }
    else if (WAVE_TYPE == 1) { // sine wave
        if (FREQ == 100) { // 100 hz sine wave
            send_data((int)sin_table[(int)index]);
            index +=1;
            if (index >= 255){
                index = 0;
            }
        }
        else if (FREQ == 200) {
            send_data((int)sin_table[(int)index]);
            index +=2;
            if (index >= 255){
                index = 0;
            }
        }
        else if (FREQ == 300) {
             send_data((int)sin_table[(int)index]);
             index +=4;
             if (index >= 255){
                index = 0;
             }
        }
        else if (FREQ == 400) {
             send_data((int)sin_table[(int)index]);
             index +=8;
             if (index >= 255){
                index = 0;
             }
        }
        else if (FREQ == 500) {
             send_data((int)sin_table[(int)index]);
             index +=16;
             if (index >= 255){
                index = 0;
             }
        }
        TIMER_A0->CCR[0] += ccr0sin;

    }
    else if (WAVE_TYPE == 2) { // sawtooth wave
        if (FREQ == 100) {
            send_data(data);
            data += 105;
            if (data > 4095) {
                data = 0;
            }
        }
        else if (FREQ == 200) {
            send_data(data);
            data += 165;
            if (data > 4095) {
                data = 0;
            }
        }
        else if (FREQ == 300) {
            send_data(data);
            data += 187;
            if (data > 4095) {
                data = 0;
            }
        }
        else if (FREQ == 400) {
            send_data(data);
            data += 165;
            if (data > 4095) {
                data = 0;
            }
        }
        else if (FREQ == 500) {
            send_data(data);
            data += 105;
            if (data > 4095) {
                data = 0;
            }
        }
    }
    }
/* function to write to LCD with our (global var) wave type, frequency, and duty cycle.
 * Takes care of string formatting, clearing scree
 */
void wave_params_to_LCD(void) {
        LCD_command(0x01); // clear LCD and start at top left
        switch (WAVE_TYPE) {
        case 0: write_LCD_str("Wave: Square"); break;
        case 1: write_LCD_str("Wave: Sine"); break;
        case 2: write_LCD_str("Wave: Triangle"); break;
        }
        LCD_command(0xC0); // go to 2nd line
        switch (FREQ) {
        case 100: write_LCD_str("f:100Hz"); break;
        case 200: write_LCD_str("f:200Hz"); break;
        case 300: write_LCD_str("f:300Hz"); break;
        case 400: write_LCD_str("f:400Hz"); break;
        case 500: write_LCD_str("f:500Hz"); break;
        }
        write_LCD_str(" ");
        if (WAVE_TYPE == 0) { // only show duty cycle if square wave selected
            switch (DUTY) {
            case 10: write_LCD_str("Duty:10%"); break;
            case 20: write_LCD_str("Duty:20%"); break;
            case 30: write_LCD_str("Duty:30%"); break;
            case 40: write_LCD_str("Duty:40%"); break;
            case 50: write_LCD_str("Duty:50%"); break;
            case 60: write_LCD_str("Duty:60%"); break;
            case 70: write_LCD_str("Duty:70%"); break;
            case 80: write_LCD_str("Duty:80%"); break;
            case 90: write_LCD_str("Duty:90%"); break;
            }
        }

}


