#include <avr/io.h>
#include <avr/interrupt.h>

#include "segment_display.h"
#include "rtc.h"
int main()
{
	seg_int();
	sei();
	twi_init_master();

w1=12;
w2=12;
w3=12;
w4=12;

while(1){
	rtc_get_time_s(&hour,&min,&sec);
	if((sec%15==0)|(sec%15==1)){
		rtc_get_temp(&i,&f);
		w1=i/10;
		w2=i%10;
		w3=14;
		w4=15;
	}else{
	w1=hour/10;
	if(w1==0)w1=10;
	w2=hour%10;
	w3=min/10;
	w4=min%10;
}
}
}





