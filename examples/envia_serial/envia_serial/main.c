/*
 * envia_serial.c
 *
 * Created: 19/01/2017 17:56:53
 * Author : usuario
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#define F_CPU	16000000
#define BUAD	9600
#define BRC		((F_CPU/16/BUAD) - 1)
#define TX_BUFFER_SIZE	128

#include <util/delay.h>

char serialBuffer[TX_BUFFER_SIZE];
uint8_t serialReadPos = 0;
uint8_t serialWritePos = 0;

void appendSerial(char c);
void serialWrite(char  c[]);

int main(void)
{	
	UBRR3H = (BRC >> 8);
	UBRR3L =  BRC;
	
	UCSR3B = (1 << TXEN3)  | (1 << TXCIE3);

	UCSR3C = (1 << UCSZ31) | (1 << UCSZ30);
	
	sei();
	
	serialWrite("HELLO\n\r");
	serialWrite("woRLd\n\r");
	
	
	
    while(1)
    {
		_delay_ms(3000);
		
		serialWrite("GOODByE\n\r");
    }
}

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
	
	if(UCSR3A & (1 << UDRE3))
	{
		UDR3 = 0;
	}
}

ISR(USART_TX_vect)
{
	if(serialReadPos != serialWritePos)
	{
		UDR3 = serialBuffer[serialReadPos];
		serialReadPos++;
		
		if(serialReadPos >= TX_BUFFER_SIZE)
		{
			serialReadPos++;
		}
	}
}
