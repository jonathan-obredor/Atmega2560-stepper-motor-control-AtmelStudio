/*
 * configuracion.h
 *
 * Created: 23/01/2017 19:14:01
 *  Author: usuario
 */ 


#ifndef CONFIGURACION_H_
#define CONFIGURACION_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>

#define F_CPU	16000000
#define BUAD	9600
#define BRC		((F_CPU/16/BUAD) - 1)
#include <util/delay.h>



#endif /* CONFIGURACION_H_ */