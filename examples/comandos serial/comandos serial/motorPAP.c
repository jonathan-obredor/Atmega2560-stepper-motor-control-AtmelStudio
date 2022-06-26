/*
 * motorPAP.c
 *
 * Created: 23/01/2017 19:16:56
 *  Author: usuario
 */ 

#include "motorPAP.h"


int consignaX;			// variable consigna de posición (se actualizará a través de mensaje por UART
int PosActualX;	// variable posición actual. Refleja la posición actual del motor, se incrementa o decrementa) en cada pulso enviado al driver.
int rpmX;		// tiempo de retardo entre pulsos enviados al driver.
float pasosPorSegundoX;
//int enableX;
int DireccionX;
int IndicePasoX;
int PosicionOkX;
int Xmax;
int HalfStep;
int MotorBipolar;
int pasosPorVuelta;