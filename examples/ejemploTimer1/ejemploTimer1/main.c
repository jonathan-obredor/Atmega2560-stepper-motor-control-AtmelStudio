/*
 * ejemploTimer1.c
 *
 * Created: 23/01/2017 12:04:50
 * Author : usuario
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>



int main(void)
{
	DDRB = 0x01;
	//poner el timer en modo CTC: clear timer on compare
	TCCR1B = (1 << WGM12);
	//registro de comparacion, guarda el numero de ticks en los que se produce el match //cada 1 segundo
	OCR1A = 19531;
	//TIMSK es el registro de interrupciones; (1 << OCIE1A) activar interrupcion cuando se produzca match con OCR1A //activar sei()!!!!!
	TIMSK1 = (1 << OCIE1A);
	
	sei();
	
	//configurar el prescaler en 1024
	//al configurar el prescaler, el timer empieza la cuenta
	TCCR1B |= (1 << CS12) | (1 << CS10); 
	
	while(1)
	{
		//TODO:: Please write your application code
	}
}

ISR(TIMER1_COMPA_vect)
{
	PORTB ^= (1 << PORTB0);

}