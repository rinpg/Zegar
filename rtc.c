// DS3231 driver library

#include <avr/io.h>
#include "rtc.h"

#define TRUE 1
#define FALSE 0

struct tm _tm;		// statically allocated structure for time value

//******************konwertery DEC<->BCD ******************************
uint8_t dec2bcd(uint8_t d){
  return ((d/10 * 16) + (d % 10));
}
uint8_t bcd2dec(uint8_t b){
  return ((b/16 * 10) + (b % 16));
}
//*********************************************************************

//*******odczyt z rtc adresu "offset"**********************************
uint8_t rtc_read_byte(uint8_t offset){
	twi_begin_transmission(RTC_ADDR);
	twi_send_byte(offset);
	twi_end_transmission();
	twi_request_from(RTC_ADDR, 1);
	return twi_receive();
}
//*********************************************************************

//*******zapis do rtc pod adresem offset wartoci b ********************
void rtc_write_byte(uint8_t b, uint8_t offset){
	twi_begin_transmission(RTC_ADDR);
	twi_send_byte(offset);
	twi_send_byte(b);
	twi_end_transmission();
}
//*********************************************************************

#if GET_FULL
//********obczyt pe³nego czasu z rtc w formie struktury****************
struct tm* rtc_get_time(void){
	uint8_t rtc[7];
	uint8_t century = 0;

	twi_begin_transmission(RTC_ADDR);
	twi_send_byte(0x0);					//zaqcznij odczyt od adresu 0x00
	twi_end_transmission();
	twi_request_from(RTC_ADDR, 7);		//zarzadaj 7 kolejnych bytes

	for (uint8_t i = 0; i < 7; i++) {	// read 7 kolejnych bytes
		rtc[i] = twi_receive();
	}
	twi_end_transmission();
									//**v** zapis bytes czasu do struktury tm
	_tm.sec = bcd2dec(rtc[0]);
	_tm.min = bcd2dec(rtc[1]);
	_tm.hour = bcd2dec(rtc[2]);
	_tm.mday = bcd2dec(rtc[4]);
	_tm.mon = bcd2dec(rtc[5] & 0x1F);
	century = (rtc[5] & 0x80) >> 7;
	_tm.year = century == 1 ? 2000 + bcd2dec(rtc[6]) : 1900 + bcd2dec(rtc[6]);
	_tm.wday = bcd2dec(rtc[3]);

	return &_tm;
}
//*********************************************************************
#endif

#if GET_SHORT
//********obczyt czasu z rtc w formie trzech zmiennych ****************
void rtc_get_time_s(uint8_t* hour, uint8_t* min, uint8_t* sec)
{
	uint8_t rtc[9];

	// read 7 bytes starting from register 0
	// sec, min, hour, day-of-week, date, month, year
	twi_begin_transmission(RTC_ADDR);
	twi_send_byte(0x0);
	twi_end_transmission();
	
	twi_request_from(RTC_ADDR, 7);
	
	for(uint8_t i=0; i<7; i++) {
		rtc[i] = twi_receive();
	}
	
	twi_end_transmission();
	
	if (sec)  *sec =  bcd2dec(rtc[0]);
	if (min)  *min =  bcd2dec(rtc[1]);
	if (hour) *hour = bcd2dec(rtc[2]);
}
//*********************************************************************
#endif

#if SET_FULL
//********ustawienie pe³nego czasu rtc w formie struktury**************
void rtc_set_time(struct tm* tm_){
	twi_begin_transmission(RTC_ADDR);
	twi_send_byte(0x0);
	uint8_t century;
	if (tm_->year > 2000) {
		century = 0x80;
		tm_->year = tm_->year - 2000;
	} else {
		century = 0;
		tm_->year = tm_->year - 1900;
	}
	twi_send_byte(dec2bcd(tm_->sec)); 			// seconds
	twi_send_byte(dec2bcd(tm_->min));			// minutes
	twi_send_byte(dec2bcd(tm_->hour)); 			// hours
	twi_send_byte(dec2bcd(tm_->wday)); 			// day of week
	twi_send_byte(dec2bcd(tm_->mday)); 			// day
	twi_send_byte(dec2bcd(tm_->mon) + century); // month
	twi_send_byte(dec2bcd(tm_->year)); 			// year
	twi_end_transmission();
}
//*********************************************************************
#endif

#if SET_SHORT
//********ustawienie czasu rtc w formie trzech zmiennych **************
void rtc_set_time_s(uint8_t hour, uint8_t min, uint8_t sec){
	twi_begin_transmission(RTC_ADDR);
	twi_send_byte(0x0);
	twi_send_byte(dec2bcd(sec)); 	// seconds
	twi_send_byte(dec2bcd(min)); 	// minutes
	twi_send_byte(dec2bcd(hour)); 	// hours
	twi_end_transmission();
}
//*********************************************************************
#endif

#if GET_TEMP
//**ODCZYT TEMPERATURY w 2 zmiennych ca³kowite ze znakiem i fractals **
void rtc_get_temp(int8_t* i, uint8_t* f){
	uint8_t msb, lsb;
	*i = 0;
	*f = 0;
	twi_begin_transmission(RTC_ADDR);
	twi_send_byte(0x11);		// temp registers 0x11 and 0x12
	twi_end_transmission();
	twi_request_from(RTC_ADDR, 2);

	if (twi_available()){
		msb = twi_receive(); 	// integer part (in twos complement)
		lsb = twi_receive(); 	// fraction part
		*i = msb;				// integer part in entire byte
		*f = (lsb >> 6) * 25;	// fractional part in top two bits (increments of 0.25)
	}
	twi_begin_transmission(RTC_ADDR);		//wymuszenie conwersji temperatury (dodatkowo, wewnetdzne otwie¿anie poza tym jest co 64sekundy)
		twi_send_byte(0x0E);
		twi_end_transmission();
		twi_request_from(RTC_ADDR, 1);
		uint8_t ctrl = twi_receive();
		ctrl |= 0b00100000;
		twi_begin_transmission(RTC_ADDR);
		twi_send_byte(0x0E);
		twi_send_byte(ctrl);
	twi_end_transmission();
}
//*********************************************************************
#endif

#if SQUERE_WAVE
//******W£¥CZENIE wejscia sqw i ustawienie czestotliwoci przebiegu*****
void rtc_SQW(bool enable,enum RTC_SQW_FREQ freq){
		twi_begin_transmission(RTC_ADDR);
		twi_send_byte(0x0E);
		twi_end_transmission();
 		twi_request_from(RTC_ADDR, 1);
		uint8_t control = twi_receive();
		if (enable) {
			control |=  0b01000000; // set BBSQW to 1
			control &= ~0b00000100; // set INTCN to 0
		}
		else {
			control &= ~0b01000000; // set BBSQW to 0
		}
		twi_begin_transmission(RTC_ADDR);
		twi_send_byte(0x0E);
		twi_send_byte(control);
		twi_end_transmission();

		twi_begin_transmission(RTC_ADDR);
		twi_send_byte(0x0E);
		twi_end_transmission();
   		twi_request_from(RTC_ADDR, 1);
		control = twi_receive();
		control &= ~0b00011000; // Set to 0
		control |= (freq << 4); // Set freq bitmask
		twi_begin_transmission(RTC_ADDR);
		twi_send_byte(0x0E);
		twi_send_byte(control);
		twi_end_transmission();
}
//*********************************************************************
#endif

#if ALARM_1
void rtc_set_alarm_1(uint8_t mode, uint8_t data, uint8_t hour, uint8_t min, uint8_t sec){
	if (mode > 6) return;
	if (data > 31) return;
	if (hour > 23) return;
	if (min > 59) return;
	if (sec > 59) return;
	uint8_t a1m1=0, a1m2=0, a1m3=0, a1m4=0, d1=0;

	if(mode==1)
		a1m1=0b10000000;		 // A1M1
	if((mode==1)|(mode==2))
		a1m2=0b10000000;		 // A1M2
	if((mode==1)|(mode==2)|(mode==3))
		a1m3=0b10000000;		 // A1M3
	if((mode==1)|(mode==2)|(mode==3)|(mode==4))
		a1m4=0b10000000;		 // A1M4
	if(mode==6)
		d1=0b010000000;			 // DY/DT

	rtc_write_byte((dec2bcd(sec)|a1m1),  0x07); // second
	rtc_write_byte((dec2bcd(min)|a1m2),  0x08); // minute
	rtc_write_byte((dec2bcd(hour)|a1m3), 0x09); // hour
	rtc_write_byte((dec2bcd(data)|d1|a1m4), 	 0x0a); // data
	uint8_t val = rtc_read_byte(0x0f);
	rtc_write_byte(val & ~0b00000001, 0x0f);
}
void rtc_get_alarm_1(uint8_t* mode, uint8_t* data, uint8_t* hour, uint8_t* min, uint8_t* sec){
		*sec  = bcd2dec(rtc_read_byte(0x07) & ~0b10000000);
		*min  = bcd2dec(rtc_read_byte(0x08) & ~0b10000000);
		*hour = bcd2dec(rtc_read_byte(0x09) & ~0b10000000);
		*data = bcd2dec(rtc_read_byte(0x0a) & ~0b11000000);

		if((((rtc_read_byte(0x07)&0b10000000))==0b10000000) &
		   (((rtc_read_byte(0x08)&0b10000000))==0b10000000) &
		   (((rtc_read_byte(0x09)&0b10000000))==0b10000000) &
		   (((rtc_read_byte(0x0a)&0b10000000))==0b10000000) )
				*mode=1;
		if((((rtc_read_byte(0x07)&0b10000000))==0b00000000) &
		   (((rtc_read_byte(0x08)&0b10000000))==0b10000000) &
		   (((rtc_read_byte(0x09)&0b10000000))==0b10000000) &
		   (((rtc_read_byte(0x0a)&0b10000000))==0b10000000) )
				*mode=2;
		if((bcd2dec(((rtc_read_byte(0x07)&0b10000000)))==0b00000000) &
		   (bcd2dec(((rtc_read_byte(0x08)&0b10000000)))==0b00000000) &
		   (bcd2dec(((rtc_read_byte(0x09)&0b10000000)))==0b10000000) &
		   (bcd2dec(((rtc_read_byte(0x0a)&0b10000000)))==0b10000000) )
				*mode=3;
		if((bcd2dec(((rtc_read_byte(0x07)&0b10000000)))==0b00000000) &
		   (bcd2dec(((rtc_read_byte(0x08)&0b10000000)))==0b00000000) &
		   (bcd2dec(((rtc_read_byte(0x09)&0b10000000)))==0b00000000) &
		   (bcd2dec(((rtc_read_byte(0x0a)&0b10000000)))==0b10000000) )
				*mode=4;
		if((bcd2dec(((rtc_read_byte(0x07)&0b10000000)))==0b00000000) &
		   (bcd2dec(((rtc_read_byte(0x08)&0b10000000)))==0b00000000) &
		   (bcd2dec(((rtc_read_byte(0x09)&0b10000000)))==0b00000000) &
		   (bcd2dec(((rtc_read_byte(0x0a)&0b11000000)))==0b00000000) )
				*mode=5;
		if((bcd2dec(((rtc_read_byte(0x07)&0b10000000)))==0b00000000) &
		   (bcd2dec(((rtc_read_byte(0x08)&0b10000000)))==0b00000000) &
		   (bcd2dec(((rtc_read_byte(0x09)&0b10000000)))==0b00000000) &
		   (bcd2dec(((rtc_read_byte(0x0a)&0b11000000)))==0b01000000) )
				*mode=6;
}
uint8_t rtc_check_alarm_1(void){
		uint8_t val = rtc_read_byte(0x0f)&0b00000001;

		if (val & 1)rtc_write_byte(val & ~0b00000001, 0x0f);
				return val & 1 ? 1 : 0;
}
#endif

#if ALARM_2
void rtc_set_alarm_2(uint8_t mode, uint8_t data, uint8_t hour, uint8_t min){
	if (mode > 5) return;
	if (data > 31) return;
	if (hour > 23) return;
	if (min > 59) return;
	uint8_t a2m2=0, a2m3=0, a2m4=0, d1=0;

	if(mode==1)
		a2m2=0b10000000;		 // A1M2
	if((mode==1)|(mode==2))
		a2m3=0b10000000;		 // A1M2
	if((mode==1)|(mode==2)|(mode==3))
		a2m4=0b10000000;		 // A1M3
	if(mode==5)
		d1=0b010000000;			 // DY/DT

	rtc_write_byte((dec2bcd(min)|a2m2),  		0x0b); // minute
	rtc_write_byte((dec2bcd(hour)|a2m3), 		0x0c); // hour
	rtc_write_byte((dec2bcd(data)|d1|a2m4), 	0x0d); // data
	uint8_t val = rtc_read_byte(0x0f);
	rtc_write_byte(val & ~0b00000010, 0x0f);
}
void rtc_get_alarm_2(uint8_t* mode, uint8_t* data, uint8_t* hour, uint8_t* min){
		*min  = bcd2dec(rtc_read_byte(0x0b) & ~0b10000000);
		*hour = bcd2dec(rtc_read_byte(0x0c) & ~0b10000000);
		*data = bcd2dec(rtc_read_byte(0x0d) & ~0b11000000);

		if((((rtc_read_byte(0x0b)&0b10000000))==0b10000000) &
		   (((rtc_read_byte(0x0c)&0b10000000))==0b10000000) &
		   (((rtc_read_byte(0x0d)&0b10000000))==0b10000000) )
				*mode=1;
		if((((rtc_read_byte(0x0b)&0b10000000))==0b00000000) &
		   (((rtc_read_byte(0x0c)&0b10000000))==0b10000000) &
		   (((rtc_read_byte(0x0d)&0b10000000))==0b10000000) )
				*mode=2;
		if((((rtc_read_byte(0x0b)&0b10000000))==0b00000000) &
		   (((rtc_read_byte(0x0c)&0b10000000))==0b00000000) &
		   (((rtc_read_byte(0x0d)&0b10000000))==0b10000000) )
				*mode=3;
		if((((rtc_read_byte(0x0b)&0b10000000))==0b00000000) &
		   (((rtc_read_byte(0x0c)&0b10000000))==0b00000000) &
		   (((rtc_read_byte(0x0d)&0b11000000))==0b00000000) )
				*mode=4;
		if((((rtc_read_byte(0x0b)&0b10000000))==0b00000000) &
		   (((rtc_read_byte(0x0c)&0b10000000))==0b00000000) &
		   (((rtc_read_byte(0x0d)&0b11000000))==0b01000000) )
				*mode=5;
}
uint8_t rtc_check_alarm_2(void){
		uint8_t val = rtc_read_byte(0x0f)&0b00000010;

		if (val & 1)rtc_write_byte(val & ~0b00000010, 0x0f);
				return val & 1 ? 1 : 0;
}
#endif
