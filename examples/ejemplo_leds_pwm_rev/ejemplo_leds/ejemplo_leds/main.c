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
#define registro_de_comparacion2  OCR1A

void iniciaTimer1() {
	// connect led to pin PB7
	//	DDRB = 0x80;
	//set up timer in PWM, Phase and frequency Correct mode	
//	TCCR1A |= (1 << WGM10);	
//	TCCR1B |= (1 << WGM13) | (1 << CS11) | (1 << CS10);	// set up timer with prescaler = 64 and PWM, Phase and frequency correct mode
	TCNT1 = 0; // initialize counter

  //TCCR1B:  ICNC1  ICES1  –  WGM13   WGM12   CS12 CS11 CS10     Seccion 17.11.5
  TCCR1B =  0b00011001; // |0|0|0|WGM(3:2)=11|CS1(2:0)=001 (no prescaler, seccion 17.11.8)
  //TCCR1B =  0b00010001; // probar este para modo 10 (PWM phase correct) o modo 8 (PWM phase-frequency correct)
  
  //TCCR1A:  COM1A1 COM1A0|COM1B1 COM1B0|COM1C1 COM1C0|WGM11 WGM10	Seccion 17.11.1
  TCCR1A = 0b10000010; // |COM1A(1:0)=11|COM1B(1:0)=00|COM1C(1:0)=00|WGM(1:0)=10
  //TCCR1A = 0b10000000; // probar este para modo 8 (PWM phase-frequency correct) en combinacion con TCCR1B=0b00010001
					 // Ver tabla 17-4. Pin OC1A se enciende en Bottom y se apaga en Compare Match de Timer1 vs TOP
					 // Pines OC1B y OC1C desactivados (00 y 00)
					 // Bits WGMn(3:2) de TCCR1B en '11' y bits WGMn(1:0) en '10' es modo '1110' (14)
					 // Bits WGMn(3:2) de TCCR1B en '10' y bits WGMn(1:0) en '10' es modo '1010' (10)
					 // Modos de operacion. Tabla 17-2 y seccion 17.9.3. 
					 // Modo 14 (fast PWM). El registro ICRn define el periodo  y el registro OCRnA define el 
					 // duty cycle.
					 // Modo 10 (PWM phase correct). El registro ICRn define el semiperiodo y el registro OCRnA define
					 // el duty cycle.

/*
Nota1: Tambien podes hacer 
TCCR1A=1<<COM1A1|1<<WGM11;
TCCR1B=1<<WGM13|1<<WGM12|1<<CS10; */
/*
Nota2: Habilitando 1<<COM1B1 y 1<<COM1C1 podes manejar los pines OC1B y OC1C (compartido con OC0A)
con PWM escribiendo los registros OCR1B y OCR1C respectivamente. El periodo es el mismo definido por ICR1
*/	
	ICR1 = 25000;  
				   // puede ser hasta 0xFFFF (resolucion PWM de 16 bits)
				   // En modo 14 (Fast PWM), con 25000 da un periodo 25000/16MHz=1,56 ms
				   // EN modo 10 (PWM phase correct), con 25000 da un periodo 50000/16MHz=3,12ms
				   
	// initialize compare value
	registro_de_comparacion2 = (dutyCycle2/100.0)*24999.0;
	// enable compare interrupt
	//TIMSK1 |= (1 << OCIE1A)|(1<<TOIE1); // This is wrong!!!!
	TIMSK1 |= (1 << OCIE1A); // This works

}

// this ISR is fired whenever a match occurs
// hence, toggle led here itself..
ISR( TIMER1_COMPA_vect) {
	registro_de_comparacion2 = (dutyCycle2/100.0)*24999.0;
	OCR1B=(dutyCycle1/100.0)*24999.0;
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



