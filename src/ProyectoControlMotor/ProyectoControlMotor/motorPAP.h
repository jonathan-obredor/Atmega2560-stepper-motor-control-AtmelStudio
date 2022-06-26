/*
 * motorPAP.h
 *
 * Created: 23/01/2017 19:15:16
 *  Author: usuario
 */ 


#ifndef MOTORPAP_H_
#define MOTORPAP_H_

#include "configuracion.h"


#define enableX PB1



void iniciarMotor();
int setEnableMotor(int enable);
int getEnableMotor();
int actualizarBobinas();
void avanzaPaso();
void retrocedePaso();
void moverMotorX();
void setVelocidadX(int velo);
int getVelocidadX();
void setConsignaX(int consig);
int getPosicionActualX();
void verEjemplo();

#endif /* MOTORPAP_H_ */