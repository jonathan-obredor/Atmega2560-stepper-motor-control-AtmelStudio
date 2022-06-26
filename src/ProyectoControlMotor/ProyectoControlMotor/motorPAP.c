/*
 * motorPAP.c
 *
 * Created: 23/01/2017 19:16:56
 *  Author: usuario
 */ 

#include "motorPAP.h"


volatile int ConsignaX;			// variable consigna de posición (se actualizará a través de mensaje por UART
int PosicionActualX;	// variable posición actual. Refleja la posición actual del motor, se incrementa o decrementa) en cada pulso enviado al driver.
volatile float rpmX;		// tiempo de retardo entre pulsos enviados al driver.

int DireccionX;
int IndicePasoX;
int PosicionOkX;
int Xmax;
int HalfStep;
int MotorBipolar;
float PasosPorVuelta;



//establecer valores iniciales para algunas constantes y adoptar caracteristicas del motor PAP
void iniciarMotor(){
	//establecer pines del motor como salida
	//usar pines 37, 36,35,34
	DDRC = 0b11111111;
	//poner como salida el pin enable
	DDRB ^=(1 << PB1);

	setEnableMotor(1);
	PasosPorVuelta=48;
	setVelocidadX(100);
	
	MotorBipolar=1;
	HalfStep=1;
	IndicePasoX=0;
	
	Xmax=10000;
	PosicionActualX=0;                  //Supone posactual=0 
	ConsignaX=0;                        //arranca en reposo

	//TODO:escribir funcion de homing
}

//colocar el pin de enable en el valor que recibe como parametro
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
		return -1;
	}
}

//devolver el valor del pin de enable
int getEnableMotor(){
	if(PINB & (1<<PINB1)){
		return 1;
	}
	return 0;
}

//configura los pines de salida para establecer la secuencia de pasos
//tiene 8 pasos para permitir half step
//si se desea full step deberia recorrerse de dos en dos
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
		serialWrite("err bobina");
		return 0;
	}
	return 1;
}
//actualizar el indice del paso del motor y efectuar el movimiento
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
//funcion identica que avanzaPaso pero en sentido contrario
void retrocedePaso(){
	if(HalfStep){
		if (IndicePasoX==1){
			IndicePasoX=8;
		}
		else{
			IndicePasoX--;
		}
		
	}
	else{ //fullstep
		//TODO:
	}
	actualizarBobinas();
}

// llevar el motor hasta la posicion consigna
//esta funcion la llama la interrupcion del timer
//cuando llega al destino pone una variable en alto
void moverMotorX(){
	if (getEnableMotor())
	{
		if(PosicionActualX==ConsignaX)
		{
			PosicionOkX=1;
		}
		else if (PosicionActualX<ConsignaX && ConsignaX<Xmax)
		{
			PosicionOkX=0;
			DireccionX=1;
			avanzaPaso();
			PosicionActualX++;
// 			serialWrite("x:");
// 			EnviarIntComoString(PosicionActualX);
// 			serialWrite("\n\r");
		}
		else if(ConsignaX>=0)
		{
			PosicionOkX=0;
			DireccionX=0;            // incrementa o decrementa en 1
			retrocedePaso();
			PosicionActualX--;
// 			serialWrite("x:");
// 			EnviarIntComoString(PosicionActualX);
// 			serialWrite("\n\r");
		}
	}

}
//establecer la velocidad del eje del motor en rpm
void setVelocidadX(int velo){
	rpmX=velo;
	float SegundosPorPasoX=60/(PasosPorVuelta*rpmX); //verificar!!!
	float ticksNecesarios=(F_CPU/1024)*SegundosPorPasoX;
	OCR1A=ticksNecesarios;
	//serialWrite("ticks necesarios:");
	//EnviarIntComoString(ticksNecesarios);
}

int getVelocidadX(){
	return rpmX;
}

void setConsignaX(int consig){
	if (0<=consig && consig<Xmax) 
		ConsignaX=consig;
	else
		serialWrite("err setConsigna excede lim\n\r");
}
int getPosicionActualX(){
	return PosicionActualX;
}

void verEjemplo(){
// 	serialWrite("EJEMPLO");
// 	setVelocidadX(200);
// 	_delay_ms(200);
// 	for (int i=0;i<10;i++)
// 	{
// 		setConsignaX(2000);
// 		while(PosicionActualX!=ConsignaX){}
// 		
// 		setConsignaX(0);
// 		while(PosicionActualX!=ConsignaX){}
// 	}
// 	serialWrite("listo");
}