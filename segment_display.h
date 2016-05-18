#ifndef SEGMENT_DISPLAY_H_
#define SEGMENT_DISPLAY_H_

#include <avr/io.h>
#include <avr/interrupt.h>


extern uint8_t cyfry[];
volatile uint8_t w1,w2,w3,w4;

void seg_int(void);

#endif /* SEGMENT_DISPLAY_H_ */
