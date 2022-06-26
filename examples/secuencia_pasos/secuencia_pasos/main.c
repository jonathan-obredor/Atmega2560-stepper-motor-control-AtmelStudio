/*
 * secuencia_pasos.c
 *
 * Created: 18/01/2017 20:24:20
 * Author : usuario
 */ 
#define F_CPU	20000000
#include <avr/io.h>
#include <util/delay.h>


//#define pin1 47
#define pin2 PE4
#define pin3 PE5

#define pin4 PG5
#define pin5 PE3
#define pin6 PH3
#define pin7 PH4

#define pin8 PH5
#define pin9 PH6
#define pin10 PH4
#define pin11 PB5
#define pin12 PB6
#define pin13 PB7



int main(void)
{
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

