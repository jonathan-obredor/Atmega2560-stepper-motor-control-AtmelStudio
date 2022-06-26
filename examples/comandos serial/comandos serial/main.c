/*
 * comandos serial.c
 *
 * Created: 20/01/2017 12:50:09
 * Author : usuario
 */ 



#include "configuracion.h"
#include "motorPAP.h"



unsigned int convADec(int k);
void interpreta();



#define  RX_BUFFER_SIZE	128
#define  TX_BUFFER_SIZE	128
#define  LONGI_BUF	128	// Constante para definir longitud del buffer de recepción




void iniciarSerial();

char getChar(void);
char peekChar(void);


void appendSerial(char c);
void serialWrite(char  c[]);



///////////////////////////////////serial
char rxBuffer[RX_BUFFER_SIZE];
uint8_t rxReadPos = 0;
uint8_t rxWritePos = 0;
char serialBuffer[TX_BUFFER_SIZE];
uint8_t serialReadPos = 0;
uint8_t serialWritePos = 0;
 char comando[LONGI_BUF]; 	// array para recibir las cadenas de caracteres de los comandos (buffer de recepción)
unsigned int i;  	// -Índice para apuntar a los elementos dentro del array comando[]
unsigned int cmd;	// -Flag para indicar que se hay recepción de mensaje en curso (para evitar interpretar tramas incompletas








//////////////////////////////////////motor

extern int Xmax;
extern int pasosPorVuelta;
extern int rpmX;
extern float pasosPorSegundoX;
extern int MotorBipolar;
extern int HalfStep;
extern int IndicePasoX;
extern int PosActualX;
extern int PosicionOkX;
extern int consignaX;
extern int DireccionX;
//extern int enableX;
#define enableX PB1

int setEnableMotor(int enable){
	if (enable==1)
	{
		PORTB= (1<<enableX);
		return 0;
	} 
	else if(enable==0)
	{
		PORTB= (0<<enableX);
		return 0;
	}
	else{
		serialWrite("err enableMotor");
		return 1;
	}
}
int getEnableMotor(){
	if(PINB & (1<<PINB1)){
 		return 1;
	}
	return 0;
}

void iniciarMotor(){
	//establecer pines del motor como salida
	//usar pines 37, 36,35,34
	DDRC = 0b11111111;
	//poner como salida el pin enable
	DDRB ^=(1 << PB1);

	setEnableMotor(1);
	pasosPorVuelta=48;
	rpmX=60;
	pasosPorSegundoX=pasosPorVuelta*rpmX/60;
	MotorBipolar=1;
	HalfStep=1;
	IndicePasoX=0;
	
	Xmax=300;
	PosActualX=0;                  //Supone posactual=0 (en realidad debe ser lograrse mediante homing)
	consignaX=0;                        //arranca en reposo

	//escribir funcion de homing
}




//Función que interpreta los comandos contenidos en la cadena comando[]
void interpreta()
{
	unsigned int auxInter=2;
	switch(comando[0])
	{
		case 'x':
		case 'X':
			auxInter=convADec(1);
			if (0<=auxInter && auxInter<Xmax) consignaX=auxInter;
			break;
		case 'v':
		case 'V':
			auxInter=convADec(1);
			if(auxInter<60000) rpmX=auxInter;
			break;
		case 'E':
		case 'e':
			setEnableMotor(convADec(1));
			break;
		default:
		serialWrite("err: com") ;
		break;
	}
}

//reescribir la funcion delay para evitar problemas con el compilador
void delay_ms(uint16_t count) {
	while(count--) {
		_delay_ms(1);

	}
}

void delay_us(uint16_t count) {
	while(count--) {
		_delay_us(1);

	}
}








///////////////////////////////////////movimiento/////////////////////////////////////////////////////

int actualizarBobinas(){

	switch (IndicePasoX){
		case 1:
			PORTC = 0b00000001;// PASO 1
			break;
		case 2:
			PORTC = 0b00000101;// PASO 2
			break;
		case 3:
			PORTC = 0b00000100;// PASO 3
			break;
		case 4:
			PORTC = 0b00000110;// PASO 4
			break;
		case 5:
			PORTC = 0b00000010;// PASO 5
			break;
		case 6:
			PORTC = 0b00001010;// PASO 6
			break;
		case 7:
			PORTC = 0b00001000;// PASO 7
			break;
		case 8:
			PORTC = 0b00001001;// PASO 8
			break;
		default:
			serialWrite("error bobina");
			return 0;
	}
	return 1;
}

void avanzaPaso(){
	if(HalfStep){
		if (IndicePasoX==8){
			IndicePasoX=1;
		}
		else{
			IndicePasoX++;
		}
			
	}
	else{ //fullstep
		//TODO:
	}
	actualizarBobinas();
}

void retrocedePaso(){

}



void moverMotorX(){
	if (getEnableMotor())
	{
		if(PosActualX==consignaX)
		{
			PosicionOkX=1;       // cuando se iguala des-energiza el motor (correcto según la aplicación)
		}
		else if (PosActualX<consignaX && consignaX<Xmax)
		{
			PosicionOkX=0;
			DireccionX=1;
			avanzaPaso();
			PosActualX++;
			//enviar posicion x
			char buffer[20];
			itoa(PosActualX,buffer,10);
			serialWrite("x");
			serialWrite(buffer);
			serialWrite("\n\r");
		}
		else if(consignaX>=0)
		{
			PosicionOkX=0;
			DireccionX=0;            // incrementa o decrementa en 1
			retrocedePaso();
			PosActualX--;
			//enviar posicion x
			char buffer[20];
			itoa(PosActualX,buffer,10);
			serialWrite("x");
			serialWrite(buffer);
			serialWrite("\n\r");
		}
	}

}





//Función auxiliar que devuelve el valor numérico (int16) de una cadena decimal a partir del elemento k hasta encontrar el caracter null (similar a atoi o atol)
unsigned int convADec(int k)
{
	unsigned int aux16=0;
	while(comando[k]!=0)  // después de la cadena decimal hay un caracter null (ASCII 0)
	{
		aux16=aux16*10+comando[k]-'0'; //'0' es 48 en decimal o 0x30 en hexa
		k++;
	}
	return aux16;
}



///////////////////////////////////serial




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


ISR(USART3_RX_vect)// Toma el caracter recibido y lo evalúa para llenar el buffer comando[]
{
	char dato=UDR3; //almacenar el dato recibido
	switch(dato)
	{
		case ':':      //si es delimitador de inicio
		i=0;        //inicializa contador i
		cmd=1;      //y activa bandera que indica que hay comando en curso
		break;
		case 13:       //si es delimitador de final
		if (cmd==1)    // si habia comando en curso
		{
			comando[i]=0;  //termina cadena de comando con caracter null
			interpreta();  //va a interpretar el comando
			cmd=0;         //desactiva bandera de comando en curso
			serialWrite("com ok\n\r");
		}
		break;
		default:
		if (i<LONGI_BUF)	//si contador menor que longitud del buffer
		{
			comando[i]=dato;  	//pone caracter en cadena
			i++;              	//incrementa contador i
		}
		else
		{
			serialWrite("err:buffer lleno, reiniciar\n\r");
		}
		break;
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



/////////////////////////////////////////timer////////////////////////////////////////////
void iniciarTimer1(void)
{

//poner el timer en modo CTC: clear timer on compare
TCCR1B = (1 << WGM12);
//registro de comparacion, guarda el numero de ticks en los que se produce el match //cada 1 segundo
//usar script para calcular!!
OCR1A = 19531;
//TIMSK es el registro de interrupciones; (1 << OCIE1A) activar interrupcion cuando se produzca match con OCR1A //activar sei()!!!!!
TIMSK1 = (1 << OCIE1A);

//configurar el prescaler en 1024
//al configurar el prescaler, el timer empieza la cuenta
TCCR1B |= (1 << CS12) | (1 << CS10);
}

ISR(TIMER1_COMPA_vect)
{
	//mover motor x
	moverMotorX();
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

	

	serialWrite("\n\rHELLO\n\r");
	_delay_ms(1000);




    while (1) 
    {

    }
}
