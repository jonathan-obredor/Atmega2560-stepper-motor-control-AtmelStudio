/*
 * comunicacion.h
 *
 * Created: 24/01/2017 15:42:43
 *  Author: usuario
 */ 


#ifndef COMUNICACION_H_
#define COMUNICACION_H_


#include "configuracion.h"

#define BUAD	9600 //baudios
#define BRC		((F_CPU/16/BUAD) - 1)
#define  RX_BUFFER_SIZE	128
#define  TX_BUFFER_SIZE	128
#define  LONGI_BUF	128	// Constante para definir longitud del buffer de recepción

void iniciarSerial();


void appendSerial(char c);
void serialWrite(char c[]);
void interpretaComando();
unsigned int StringADecimal(int k);
void EnviarIntComoString();
void mostrarAyuda();


#endif /* COMUNICACION_H_ */