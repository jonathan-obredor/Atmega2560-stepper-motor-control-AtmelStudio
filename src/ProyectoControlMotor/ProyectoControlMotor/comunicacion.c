/*
* comunicacion.c
*
* Created: 24/01/2017 15:38:06
*  Author: usuario
*/ 
#include "comunicacion.h"

char rxBuffer[RX_BUFFER_SIZE];
uint8_t rxReadPos = 0;
uint8_t rxWritePos = 0;
char serialBuffer[TX_BUFFER_SIZE];
uint8_t serialReadPos = 0;
uint8_t serialWritePos = 0;
char comando[LONGI_BUF]; 	// array para recibir las cadenas de caracteres de los comandos
unsigned int indCom;  	//indice para apuntar a los elementos dentro del array comando[]
unsigned int HayComando;	// -Flag para indicar que se hay recepción de mensaje en curso (para evitar interpretar tramas incompletas




 //rutina que prepara el puerto serie numero 3 para enviar y recibir datos
void iniciarSerial()
{
	//guardar el valor de ubrr en los registros adecuados
	UBRR3H = (BRC >> 8);
	UBRR3L =  BRC;
	//habilitar recepcion (1 << RXEN3)  ,  habilitar flag de recepcion completa (1 << RXCIE3) ; transmision y  transmision completa
	UCSR3B = (1 << RXEN3)  | (1 << RXCIE3) | (1 << TXEN3)  | (1 << TXCIE3);
	//usart character size,  (1 << UCSZ31) | (1 << UCSZ30) para 8 bit ;(0 <<UMSEL31) (0 <<UMSEL30) usart asincrona; (0 <<UPM31) |  (0 <<UPM30) sin paridad;    (0 <<USBS3) 1 bit de stop
	UCSR3C = (1 << UCSZ31) | (1 << UCSZ30) | (0 <<UMSEL31) | (0 <<UMSEL30) | (0 <<UPM31) |  (0 <<UPM30) | (0 <<USBS3) ;
}
//////////////////////////////recepcion serial////////////////////////////////////////////////////////////

// Toma el caracter recibido y lo evalúa para llenar el buffer comando[]
//cuando se termina de recibir un comando llama a la funcion interpretaComando
ISR(USART3_RX_vect)
{
	char dato=UDR3; //almacenar el dato recibido
	switch(dato)
	{
		case ':':      //si es delimitador de inicio
		indCom=0;        //inicializa contador i
		HayComando=1;      //y activa bandera que indica que hay comando en curso
		break;
		case 13:       //si es delimitador de final
		if (HayComando==1)    // si habia comando en curso
		{
			comando[indCom]=0;  //termina cadena de comando con caracter null
			interpretaComando();  //va a interpretar el comando
			HayComando=0;         //desactiva bandera de comando en curso
			serialWrite("ok\n\r");
		}
		break;
		default:
		if (indCom<LONGI_BUF)	//si contador menor que longitud del buffer
		{
			comando[indCom]=dato;  	//pone caracter en cadena
			indCom++;              	//incrementa contador i
		}
		else
		{
			serialWrite("err:buffer lleno, reiniciar\n\r");
		}
		break;
	}
}


//////////////////////////////transmision serial////////////////////////////////////////////////////////////


//funcion que sirve para enviar datos por el puerto serie
//coloca todos los bytes que se desean enviar en un buffer: serialBuffer
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

//guarda un byte en el buffer y mantiene actualizado el indice para recorrerlo
void appendSerial(char c)
{
	serialBuffer[serialWritePos] = c;
	serialWritePos++;
	
	if(serialWritePos >= TX_BUFFER_SIZE)
	{
		serialWritePos = 0;
	}
}

//interrupcion por buffer de envio vacio
//envia el proximo dato que esta en el buffer por el puerto serie y actualiza el indice para recorrerlo
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

//analiza la instruccion guardada en el arreglo comando[] y llama a una funcion, segun corresponda
void interpretaComando()
{
	unsigned int auxInterpreta=2;
	switch(comando[0])
	{
		case 'x':
		case 'X':
		if (comando[1]=='?')
		{
			serialWrite("x:");
			EnviarIntComoString(getPosicionActualX());
			serialWrite("\n\r");
		}
		else
		{
			auxInterpreta=StringADecimal(1);
			setConsignaX(auxInterpreta);
		}
		break;
		case 'v':
		case 'V':
		if (comando[1]=='?')
		{
			serialWrite("v:");
			EnviarIntComoString(getVelocidadX());
			serialWrite("\n\r");
		}
		else
		{
			auxInterpreta=StringADecimal(1);
			if(auxInterpreta<60000) setVelocidadX(auxInterpreta);
		}
		break;
		case 'E':
		case 'e':
		setEnableMotor(StringADecimal(1));
		break;
		case 'D':
		case 'd':
		verEjemplo();
		break;
		default:
		serialWrite("err com") ;
		break;
	}
}

//convierte los valores de enteros a string y luego los envia por el puerto serie
void EnviarIntComoString(int dato){
	char buffer[20];
	itoa(dato,buffer,10);
	serialWrite(buffer);
}


//Función auxiliar que devuelve el valor numérico de una cadena decimal a partir del elemento k hasta encontrar el caracter null (similar a atoi o atol)
unsigned int StringADecimal(int k)
{
	unsigned int aux16=0;
	while(comando[k]!=0)  // después de la cadena decimal hay un caracter null (ASCII 0)
	{
		aux16=aux16*10+comando[k]-'0'; //'0' es 48 en decimal o 0x30 en hexa
		k++;
	}
	return aux16;
}

//muestra un pequeno menu de opciones en la pantalla
void mostrarAyuda(){
	serialWrite("proyecto de control para motor PAP Jonathan Obredor 2017\n\r");
	_delay_ms(100);
	serialWrite("iniciar mensaje con ':'\n\rpara:                     enviar:\n\r");
	_delay_ms(100);
	serialWrite("consultar v___________________:v?\n\rasignar v_______________:v[valor]\n\r");
	_delay_ms(100);
	serialWrite("consultar x___________________:x?\n\rasignar x_______________:x[valor]\n\r");
	_delay_ms(100);
	serialWrite("activar motor(enable)_________:e1\n\rdesactivar motor______________:e0\n\r");
	serialWrite("ver ejemplo                   :d\n\r");
}