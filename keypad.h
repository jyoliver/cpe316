/* Keypad.c: Matrix keypad scanning
 *
 * This program scans a 4x3 matrix keypad and returns the number of the
 * key that is pressed. The getkey() function is non-blocking, so it will
 * return 0xFF if no key is pressed to allow 0 to be used for key 0.
 * * key will return 10 and # will return 12.
 *
 * The getkey() function assumes that only a single key is pressed. If keys
 * on multiple columns are pressed, the key on the farthest left column will
 * be detected and the others will be ignored. If multiple keys in the same
 * column are pressed, the function will return an incorrect value.
 *
*/

/* Column 1 5.4
 * Column 2 5.5
 * Column 3 5.6
 * Row 1 5.0
 * Row 2 5.1
 * Row 3 5.2
 * Row 4 5.7
 */
#ifndef KEYPAD_H_
#define KEYPAD_H_

void keypad_init(void);
uint8_t keypad_getkey(void);

#define COL1  BIT4
#define COL2  BIT5
#define COL3  BIT6
#define ROW1  BIT0
#define ROW2  BIT1
#define ROW3  BIT2
#define ROW4  BIT7


#endif /* KEYPAD_H_ */
