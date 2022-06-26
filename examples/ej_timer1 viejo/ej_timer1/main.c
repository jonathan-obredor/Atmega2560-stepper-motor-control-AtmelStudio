/*
 * ej_timer1.c
 *
 * Created: 30/10/2016 21:02:35
 * Author : usuario
 */ 
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>




// initialize timer, interrupt and variable
void timer1_init() {

	//set up timer in PWM, Phase and frequency Correct mode
	TCCR1A |= (1 << WGM10);

	// set up timer with prescaler = 64 and PWM, Phase and frequency correct mode
	TCCR1B |= (1 << WGM13) | (1 << CS11) | (1 << CS10);

	// initialize counter
	TCNT1 = 0;

	// initialize compare value
	OCR1A = 24999;

	// enable compare interrupt
	//TIMSK1 |= (1 << OCIE1A)|(1<<TOIE1); // This is wrong!!!!
	TIMSK1 |= (1 << OCIE1A); // This works

	// enable global interrupts
	sei();
}

// this ISR is fired whenever a match occurs
// hence, toggle led here itself..
ISR( TIMER1_COMPA_vect) {

	// toggle led here
	PORTB ^= (1 << 7);

}

int main(void) {

	// connect led to pin PB7
	DDRB = 0x80;

	// initialize timer
	timer1_init();

	// loop forever
	while (1) {
		// do nothing
		// whenever a match occurs, ISR is fired
		// toggle the led in the ISR itself
		// no need to keep track of any flag bits here
		// done!
	}
}