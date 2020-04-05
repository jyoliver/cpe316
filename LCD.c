#include "LCD.h"
#include "msp.h"
#include "delay.h"

#define RS 1     //P4.0 mask
#define RW 2     //P4.1 mask
#define EN 4     //P4.2 mask

#define FREQ_1_5_MHZ 1500000
#define FREQ_3_MHZ 3000000
#define FREQ_6_MHZ 6000000
#define FREQ_12_MHZ 12000000
#define FREQ_24_MHZ 24000000
#define FREQ_48_MHZ 48000000

void LCD_init(void) {
    P4->DIR = 0xFF;     /* make P4 pins output for data and controls */
    delay_ms(30, FREQ_24_MHZ);                /* initialization sequence */
    LCD_nibble_write(0x30, 0);
    delay_ms(10, FREQ_24_MHZ);
    LCD_nibble_write(0x30, 0);
    delay_ms(1, FREQ_24_MHZ);
    LCD_nibble_write(0x30, 0);
    delay_ms(1, FREQ_24_MHZ);
    LCD_nibble_write(0x20, 0);  /* use 4-bit data mode */
    delay_ms(1, FREQ_24_MHZ);

    LCD_command(0x28);      /* set 4-bit data, 2-line, 5x7 font */
    LCD_command(0x06);      /* move cursor right after each char */
    LCD_command(0x01);      /* clear screen, move cursor to home */
    LCD_command(0x0F);      /* turn on display, cursor blinking */
}

// With 4-bit mode, each command or data is sent twice with upper
 // nibble first then lower nibble.
 //
void LCD_nibble_write(unsigned char data, unsigned char control) {
    data &= 0xF0;           // clear lower nibble for control
    control &= 0x0F;        // clear upper nibble for data
    P4->OUT = data | control;       // RS = 0, R/W = 0
    P4->OUT = data | control | EN;  //pulse E
    delay_ms(0, FREQ_24_MHZ);
    P4->OUT = data;                 // clear E
    P4->OUT = 0;
}

void LCD_command(unsigned char command) {
    LCD_nibble_write(command & 0xF0, 0);    //upper nibble first
    LCD_nibble_write(command << 4, 0);      //then lower nibble

    if (command < 4)
        delay_ms(4, FREQ_24_MHZ);         //commands 1 and 2 need up to 1.64ms
    else
        delay_ms(1, FREQ_24_MHZ);         //all others 40 us
}
void LCD_data(unsigned char data) {
    LCD_nibble_write(data & 0xF0, RS);    //upper nibble first
    LCD_nibble_write(data << 4, RS);      //then lower nibble

    delay_ms(1, FREQ_24_MHZ);
}

 //delay milliseconds when system clock is at 3 MHz
/*void delayMs(int n) {
    int i, j;

    for (j = 0; j < n; j++)
        for (i = 750; i > 0; i--);      //Delay
}*/

void write_LCD_str(char str[])
{

int i=0;
        while (str[i]!='\0') {
            LCD_data(str[i++]);
        //i++;
        }
}

