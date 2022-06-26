/*
 * generapwm.c
 *
 * Created: 30/10/2016 16:16:05
 * Author : usuario
 */ 

#define F_CPU	20000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

double dutyCycle1 = 50;
double dutyCycle2 = 50;


void USART_Init( unsigned int baud )
{
	// Set baud rate
	UBRR0H = (unsigned char)(baud>>8);
	UBRR0L = (unsigned char)baud;
	// Enable receiver and transmitter
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	// Set frame format: 8data, 2stop bit
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}
/*
void USART_Init( unsigned int baud )
{


	// Set baud rate 
	UBRRHn = (unsigned char)(baud>>8);
	UBRRLn = (unsigned char)baud;
	// Enable receiver and transmitter 
	UCSRnB = (1<<RXENn)|(1<<TXENn);
	// Set frame format: 8data, 2stop bit 
	UCSRnC = (1<<USBSn)|(3<<UCSZn0);
}

*/

//OCR0A es el registro de comparacion
#define registro_de_comparacion1  OCR0A
#define registro_de_comparacion2 OCR0B



int main(void) {
	//poner el pin como salida
	DDRB = (1 << PB7); 
	
	//(1 << COM0A1) clear OC0A on Compare Match
	//(1 << WGM00) | (1 << WGM01) establecer el fast pwm
	TCCR0A = (1 << COM0A1) | (1 << WGM00) | (1 << WGM01);

	//(1 << TOIE0) indicar el tipo de interrupcion-----Timer/Counter0 Overflow Interrupt Enable----The corresponding interrupt is executed if an overflow in Timer/Counter0 occurs
	TIMSK0 = (1 << TOIE0);

	//el dutycycle va entre 0 y 255
	registro_de_comparacion1 = (dutyCycle1/100.0)*255.0;
	registro_de_comparacion2 = (dutyCycle2/100.0)*255.0;

	
	//activar interrupciones globalmente
	sei();
	
	// cuando se usa solamente (1 << CS00), no se utiliza prescaler
	//con ambos registros se usa (clkI/O)/1024 (From prescaler)
	TCCR0B = (1 << CS00) | (1 << CS02);

	//agregar nuevp pwm
	DDRB |= (1 << PB6);
	TCCR0A |= (1 << COM0B1);
	

/*	// PB6
	//
	//
	PB1
	PB2
	PB3

	PD3
	PD5
	

	*/



	
	while(1)
	{
		
		_delay_ms(100);
		
		//primer pwm
		dutyCycle1 += 10;
		
		if(dutyCycle1 > 100)
		{
			dutyCycle1 = 0;
		}

	/*	//segundo pwm
		dutyCycle2 += 10;
		
		if(dutyCycle2 > 100)
		{
			dutyCycle2 = 0;
		}
*/






	}//while
}

ISR(TIMER0_OVF_vect)
{
	registro_de_comparacion1 = (dutyCycle1/100.0)*255;
	//registro_de_comparacion2 = (dutyCycle2/100.0)*255;
}

