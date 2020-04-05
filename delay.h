#ifndef DELAY_H_
#define DELAY_H_

void delay_ms (int ms, int freq); //function declaration for delay_ms
void delay_us (int us, int freq); //function declaration for delay_us
void set_DCO(int freq);

#define FREQ_1_5_MHZ 1500000
#define FREQ_3_MHZ 3000000
#define FREQ_6_MHZ 6000000
#define FREQ_12_MHZ 12000000
#define FREQ_24_MHZ 24000000
#define FREQ_48_MHZ 48000000

#endif /* DELAY_H_ */
