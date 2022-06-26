/*
 * ejemplo_leds.c
 *
 * Created: 30/10/2016 18:35:08
 * Author : usuario
 */ 

 #define F_CPU	20000000
#include <avr/io.h>
 #include <avr/interrupt.h>
 #include <util/delay.h>


#define pin2 PE4
#define pin3 PE5
#define pin4 PG5
#define pin5 PE3
#define pin6 PH3
#define pin7 PH4
#define pin8 PH5
#define pin9 PH6 //Pins 9 and10: controlled by Timer2
#define pin10 PH4
#define pin11 PB5 //Pins 11 and 12: controlled by Timer1
#define pin12 PB6
#define pin13 PB7 //Pins 4 and 13: controlled by Timer0
   

/* 
Pin 2, 3 and 5: controlled by timer 3
Pin 6, 7 and 8: controlled by timer 4
Pin 46, 45 and 44:: controlled by timer 5
*/



double dutyCycle1 = 50;
double dutyCycle2 = 80;

//OCR0A es el registro de comparacion
#define registro_de_comparacion1  OCR0A
#define registro_de_comparacion2 OCR1A



void iniciaTimer1() {

	// connect led to pin PB7
	//	DDRB = 0x80;

	//set up timer in PWM, Phase and frequency Correct mode
	TCCR1A |= (1 << WGM10);

	// set up timer with prescaler = 64 and PWM, Phase and frequency correct mode
	TCCR1B |= (1 << WGM13) | (1 << CS11) | (1 << CS10);

	// initialize counter
	TCNT1 = 0;

	// initialize compare value
	registro_de_comparacion2 = (dutyCycle2/100.0)*24999.0;

	// enable compare interrupt
	//TIMSK1 |= (1 << OCIE1A)|(1<<TOIE1); // This is wrong!!!!
	TIMSK1 |= (1 << OCIE1A); // This works

}

// this ISR is fired whenever a match occurs
// hence, toggle led here itself..
ISR( TIMER1_COMPA_vect) {

	// toggle led here
	PORTB ^= (1 << pin11);
	
	registro_de_comparacion2 = (dutyCycle2/100.0)*24999.0;
}



void iniciaTimer0() {
	
	//(1 << COM0A1) clear OC0A on Compare Match
	//(1 << WGM00) | (1 << WGM01) establecer el fast pwm
	TCCR0A = (1 << COM0A1) | (1 << WGM00) | (1 << WGM01);

	//(1 << TOIE0) indicar el tipo de interrupcion-----Timer/Counter0 Overflow Interrupt Enable----The corresponding interrupt is executed if an overflow in Timer/Counter0 occurs
	TIMSK0 = (1 << TOIE0);

	//el dutycycle va entre 0 y 255
	registro_de_comparacion1 = (dutyCycle1/100.0)*255.0;
	
	// cuando se usa solamente (1 << CS00), no se utiliza prescaler
	//con ambos registros se usa (clkI/O)/1024 (From prescaler)
	TCCR0B = (1 << CS00) | (1 << CS02);

}

ISR(TIMER0_OVF_vect)
{
	registro_de_comparacion1 = (dutyCycle1/100.0)*255;
}






int main(void) {

	//poner pines como salida
	DDRB |= (1 << pin11) | (1 << pin12) | (1 << pin13);
	DDRH |= (1 << pin8) | (1 << pin9) | (1 << pin10);

	iniciaTimer0();

	// initialize timer
	iniciaTimer1();

	

	//activar interrupciones globalmente
	sei();

	while(1)
	{
		
		_delay_ms(100);
		
		//primer pwm
		dutyCycle1 += 10;
		if(dutyCycle1 > 100)
		{
			dutyCycle1 = 0;
		}

		//segundo pwm
		dutyCycle2 += 10;
		if(dutyCycle2 > 100)
		{
			dutyCycle2 = 0;
		}

	}//while
}



