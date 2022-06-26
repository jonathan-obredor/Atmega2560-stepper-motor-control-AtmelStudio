/*
 * parpadea_led_timer1.c
 *
 * Created: 21/01/2017 19:05:42
 * Author : usuario
 */ 

#define F_CPU	20000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>



#define LED PB0
volatile int milis=0;

//OCR0A es el registro de comparacion
#define registro_de_comparacion1  OCR1A



void iniciaTimer1(void)
{
	TCNT1 = 0; // initialize counter

	// Timer en modo normal, sin salida por OC0 con preescaler de 1024
	//TCCR0 = _BV(CS00) | _BV(CS02);
	TCCR1B = _BV(CS00) | _BV(CS02); //revisar
	
	// Activa interrupción por comparación del timer
	TIMSK1 |= (1 << OCIE1A);
	

	
	// carga registro comparador con 79
	registro_de_comparacion1 = 79;
}



ISR(TIMER1_COMPA_vect){//revisar
	
	// reinicia el timer
	TCNT1 = 0;
	if((milis += 10) &gt;= RETARDO){
		milis=0;
		PORTB ^= 0xFF;
	}
}



int main(void)
{
	iniciaTimer1();

	// puerto PB0 como salida
	DDRB = _BV(LED);
	PORTB = 0;

	// Activa interrupciones globales
	sei();

	int pausa=50;
	//poner pines como salida
	DDRC = 0b11111111;
	//usar pines 37, 36,35,34

	while (1)
	{
		//medio paso motor bipolar
		//B A
		PORTC = 0b00000001;// PASO 1
		_delay_ms(pausa);
		PORTC = 0b00000101;// PASO 2
		_delay_ms(pausa);
		PORTC = 0b00000100;// PASO 3
		_delay_ms(pausa);
		PORTC = 0b00000110;// PASO 4
		_delay_ms(pausa);
		PORTC = 0b00000010;// PASO 5
		_delay_ms(pausa);
		PORTC = 0b00001010;// PASO 6
		_delay_ms(pausa);
		PORTC = 0b00001000;// PASO 7
		_delay_ms(pausa);
		PORTC = 0b00001001;// PASO 8
		_delay_ms(pausa);
	}
}

