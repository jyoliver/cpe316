#include "msp.h"
#include "keypad.h"


void keypad_init(void) {
    P5->DIR = 0;        // make all pins an input
    P5->REN |= (ROW1 | ROW2 | ROW3 | ROW4);  // enable resistor for row pins
    P5->OUT &= ~(ROW1 | ROW2 | ROW3 | ROW4); // make row pins pull-down
}

/*
 * This is a non-blocking function to read the keypad.
 * If a key is pressed, it returns that key value 0-9. * is 10, # is 12
 * If no key is pressed, it returns 0xFF
 * Port 5.0 - 5.2 + 5.7 are used as inputs and connected to the rows. Pull-down
 * resistors are enabled so when no key is pressed, these pins are pulled low
 *
 * The Port 5.4 - 5.6 are used as outputs that drives the keypad columns.
 * First all columns are driven high and the input pins are read. If no key is
 * pressed, they will read zero because of the pull-down resistors. If no key
 * is pressed, return 0xFF. If the value is non-zero, determine which key is
 * being pressed.
 * To determine which key is being pressed, the program proceeds to drive one
 * column high at a time and read the input pins (rows). Knowing which row is
 * high and which column is active, the program can decide which key is pressed
 *
 */
uint8_t keypad_getkey(void) {
    uint8_t row, col, key;

    /* check to see any key pressed */
    P5->DIR |= (COL1 | COL2 | COL3);  // make the column pins outputs
    P5->OUT |= (COL1 | COL2 | COL3);  // drive all column pins high
    _delay_cycles(25);                // wait for signals to settle

    row = P5->IN & (ROW1 | ROW2 | ROW3 | ROW4);   // read all row pins

    if (row == 0)           // if all rows are low, no key pressed
        return 0xFF;

    /* If a key is pressed, it gets here to find out which key.
     * It activates one column at a time and reads the input to see
     * which row is active. */

    for (col = 0; col < 3; col++) {
        // zero out bits 6-4
        P5->OUT &= ~(COL1 | COL2 | COL3);

        // shift a 1 into the correct column depending on which to turn on
        P5->OUT |= (COL1 << col);
        _delay_cycles(25);            // wait for signals to settle

        row = P5->IN & (ROW1 | ROW2 | ROW3 | ROW4); // mask only the row pins

        if (row != 0) break;      // if the input is non-zero, key detected
    }

    P5->OUT &= ~(COL1 | COL2 | COL3);   // drive all columns low
    P5->DIR &= ~(COL1 | COL2 | COL3);   // disable the column outputs

    if (col == 3)   return 0xFF;        // if we get here, no key was detected

    // rows are read in binary, so powers of 2 (1,2,4,8)
    if (row == 4) row = 3;
    if (row == 128) row = 4;

    /*******************************************************************
     * IF MULTIPLE KEYS IN A COLUMN ARE PRESSED THIS WILL BE INCORRECT *
     *******************************************************************/

    // calculate the key value based on the row and columns where detected
    if (col == 0) key = row*3 - 2;
    if (col == 1) key = row*3 - 1;
    if (col == 2) key = row*3;


    if (key == 11)  key = 0; // fix for 0 key

    return key;
}




