/*
 * comandos serial.c
 *
 * Created: 20/01/2017 12:50:09
 * Author : usuario
 */ 



#include "configuracion.h"

/////////////////////////////////////////timer////////////////////////////////////////////

//habilitar y configurar el timer 1
void iniciarTimer1(void)
{

//poner el timer en modo CTC: clear timer on compare
TCCR1B = (1 << WGM12);
//registro de comparacion, guarda el numero de ticks en los que se produce el match 
//usar script para calcular!!
OCR1A = 19500;
//TIMSK es el registro de interrupciones; (1 << OCIE1A) activar interrupcion cuando se produzca match con OCR1A //activar sei()!!!!!
TIMSK1 = (1 << OCIE1A);

//configurar el prescaler en 1024
//al configurar el prescaler, el timer empieza la cuenta
TCCR1B |= (1 << CS12) | (1 << CS10);
}

//interrupcion por CTC del timer 1
//cuando se produce la senal del timer1 indica que es el momento de mover el motor 1 paso
ISR(TIMER1_COMPA_vect)
{
	moverMotorX();
	//led que parpadea
	PORTB ^= (1 << PB0);
}


int main(void)
{
	//led testigo que parpadea
	DDRB = (1 << PB0);

	iniciarMotor();

	iniciarSerial();

	iniciarTimer1();

	

	//habilitar interrupciones
	sei();

	

	serialWrite("\n\rHOLA!\n\r");
	_delay_ms(1000);

	mostrarAyuda();

    while (1) 
    {

    }
}
