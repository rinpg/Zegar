#define ALARM_1_CHECK	1

#define ALARM_2_SET		1
#define ALARM_2_GET		1
#define ALARM_2_CHECK	1

/*****tryby pracy alarmu 1
 v
 v mode
 v
 1	//Alarm once per second
 2	//Alarm when seconds match
 3	//Alarm when minutes and seconds match
 4	//Alarm when hours, minutes, and seconds match
 5	//Alarm when date, hours, minutes, and seconds match
 6	//Alarm when day, hours, minutes, and seconds match
*/

/*****tryby pracy alarmu 2
 v
 v mode
 v
 1	//Alarm once per minute (00 seconds of every minute)
 2	//Alarm when minutes match
 3	//Alarm when hours and minutes match
 4	//Alarm when date, hours, and minutes match
 5	//Alarm when day, hours, and minutes match
*/

// Time structure
struct tm {
	int sec;      // 0 to 59
	int min;      // 0 to 59
	int hour;     // 0 to 23
	int mday;     // 1 to 31
	int mon;      // 1 to 12
	int year;     // year-99
	int wday;
};

extern struct tm _tm;	// statically allocated
uint8_t mode, data, hour, min, sec;

#if GET_FULL
struct tm* rtc_get_time(void);
#endif
#if GET_SHORT
void rtc_get_time_s(uint8_t* hour, uint8_t* min, uint8_t* sec);
#endif
#if SET_FULL
void rtc_set_time(struct tm* tm_);
#endif
#if SET_SHORT
void rtc_set_time_s(uint8_t hour, uint8_t min, uint8_t sec);
#endif
