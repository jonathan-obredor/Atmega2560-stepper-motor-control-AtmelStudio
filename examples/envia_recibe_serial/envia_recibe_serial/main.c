/*
 * envia_recibe_serial.c
 *
 * Created: 19/01/2017 19:29:25
 * Author : usuario
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

//definir funciones de bit , estan obsoletas, se sugiere sustituir por funciones mas eficientes http://www.nongnu.org/avr-libc/user-manual/group__avr__sfr.html
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

#define F_CPU	16000000
#define BUAD	9600
#define BRC		((F_CPU/16/BUAD) - 1)

#define RX_BUFFER_SIZE	128
#define TX_BUFFER_SIZE	128
#include <util/delay.h>

char rxBuffer[RX_BUFFER_SIZE];
uint8_t rxReadPos = 0;
uint8_t rxWritePos = 0;

char serialBuffer[TX_BUFFER_SIZE];
uint8_t serialReadPos = 0;
uint8_t serialWritePos = 0;

char getChar(void);
char peekChar(void);
void appendSerial(char c);
void serialWrite(char  c[]);

int main(void)
{
	//guardar el valor de ubrr en los registros adecuados
	UBRR3H = (BRC >> 8);
	UBRR3L =  BRC;
	
	//habilitar recepcion (1 << RXEN3)  ,  habilitar flag de recepcion completa (1 << RXCIE3) ; transmision y  transmision completa
	UCSR3B = (1 << RXEN3)  | (1 << RXCIE3) | (1 << TXEN3)  | (1 << TXCIE3);

	//usart character size,  (1 << UCSZ31) | (1 << UCSZ30) para 8 bit ;
	//(0 <<UMSEL31) (0 <<UMSEL30) usart asincrona; (0 <<UPM31) |  (0 <<UPM30) sin paridad;    (0 <<USBS3) 1 bit de stop
	UCSR3C = (1 << UCSZ31) | (1 << UCSZ30) | (0 <<UMSEL31) | (0 <<UMSEL30) | (0 <<UPM31) |  (0 <<UPM30) | (0 <<USBS3) ;
	
	//habilitar interrupciones
	sei();

	serialWrite("\n\rHELLO\n\r");
	_delay_ms(1000);
	while(1)
	{
		//enviar datos
		serialWrite("GOODByE\n\r");

		//recibir datos
/*		char c = getChar();
		if(c == '1')
		{
			sbi(DDRJ, DDJ0);
		}
		else if(c =='0')
		{
			cbi(DDRJ, DDJ0);
		}
*/
		
		_delay_ms(1000);
	}
}

//////////////////////////////recepcion serial////////////////////////////////////////////////////////////
char peekChar(void)
{
	char ret = '\0';
	
	if(rxReadPos != rxWritePos)
	{
		ret = rxBuffer[rxReadPos];
	}
	
	return ret;
}

char getChar(void)
{
	char ret = '\0';
	
	if(rxReadPos != rxWritePos)
	{
		ret = rxBuffer[rxReadPos];
		
		rxReadPos++;
		
		if(rxReadPos >= RX_BUFFER_SIZE)
		{
			rxReadPos = 0;
		}
	}
	
	return ret;
}

ISR(USART3_RX_vect) 
{
	rxBuffer[rxWritePos] = UDR3; //"USART Data Register" UDRn es un registro de 16 bit, en este registro se guardan los datos que se van a enviar o que se han recibido
	
	
	rxWritePos++;
	
	if(rxWritePos >= RX_BUFFER_SIZE)
	{
		rxWritePos = 0;
	}
}

//////////////////////////////transmision serial////////////////////////////////////////////////////////////

void appendSerial(char c)
{
	serialBuffer[serialWritePos] = c;
	serialWritePos++;
	
	if(serialWritePos >= TX_BUFFER_SIZE)
	{
		serialWritePos = 0;
	}
}

void serialWrite(char c[])
{
	for(uint8_t i = 0; i < strlen(c); i++)
	{
		appendSerial(c[i]);
	}
	
	if(UCSR3A & (1 << UDRE3)) //verificar que el buffer de transmision este vacio
	{
		UDR3 = 0;
	}
}

ISR(USART3_TX_vect)
{
	if(serialReadPos != serialWritePos)
	{
		UDR3 = serialBuffer[serialReadPos]; //no verifica si el buffer esta vacio porque esta funcion la llama la interrupcion de transmision completa
		serialReadPos++;
		
		if(serialReadPos >= TX_BUFFER_SIZE)
		{
			serialReadPos=0;
		}
	}
}

