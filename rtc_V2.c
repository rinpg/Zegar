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