#ifndef LCD_H_
#define LCD_H_
void LCD_init(void);
void LCD_nibble_write(unsigned char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void delayMs(int n);
void write_LCD_str(char str[]);



#endif

