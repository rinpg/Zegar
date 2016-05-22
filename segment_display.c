#include "segment_display.h"

uint8_t cyfry[]={
	0xC0,	// 0
	0xF9,	// 1
	0xA4,	// 2
	0xB0,	// 3
	0x99,	// 4
	0x92,	// 5
	0x82,	// 6
	0xD8,	// 7
	0x80,	// 8
	0x90,	// 9
	0xFF,	// null 	(10)
	0x7F,	// dot 		(11)
	0xBF,	// - 		(12)
	0x89,	// H 		(13)
	0x9C,	// stopien 	(14)
	0xC6,	// C		(15)

};


void seg_int(){
	   //Set up the timer1 as described in the
	   //tutorial

	   TCCR1B=(1<<WGM12)|(1<<CS11)|(1<<CS10);
	   OCR1A=250;

	   //Enable the Output Compare A interrupt
	   TIMSK|=(1<<OCIE1A);

		DDRB |= 0xFF ;
		DDRD |= 0x0F ;
		PORTB &=~0xFF;
		PORTD |=0x0F;
	   //Enable interrupts globally

}

ISR(TIMER1_COMPA_vect)
{
	static uint8_t licznik=1;					// zmienna do przełączania kolejno digd wyrwietlacza
	PORTD = (~PORTD & 0xF0) | (licznik & 0x0F);
	if(licznik==1) 		PORTB = cyfry[w1];		// gdy zapalony wyśw.1 podaj stan zmiennej c1
	else if(licznik==2) PORTB = cyfry[w2];		// gdy zapalony wyśw.2 podaj stan zmiennej c2
	else if(licznik==4) PORTB = cyfry[w3];		// gdy zapalony wyśw.3 podaj stan zmiennej c3
	else if(licznik==8) PORTB = cyfry[w4];		//gdy zapalony wyśw.4 podaj stan zmiennej c4
	licznik <<= 1;								// przesunięcie zawartości bitów licznika o 1 w lewo
	if(licznik>8) licznik = 1;					// jeśli licznik większy niż 8 to ustaw na 1
}
