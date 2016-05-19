
#include <avr/io.h>		// do��czenie g��wnego systemowego  pliku nag��wkowego
#include <avr/interrupt.h>
#include <util/delay.h>

#include "segment_display.h"		// do��czenie naszego pliku nag��wkowego (obs�uga LED)
#include "rtc.h"

int main(void)
{
	seg_int();
	sei();
	twi_init_master();
	while(1){
		rtc_get_time_s(&hour,&min,&sec);
		if((sec%15==0)|(sec%15==1)){
			rtc_get_temp(&i,&f);
			w1=i/10;
			w2=i%10;
			w3=14;
			w4=15;
			}
		else
			{
			w1=hour/10;
			if(w1==0)w1=10;
			w2=hour%10;
			w3=min/10;
			w4=min%10;
			}
		}
}



