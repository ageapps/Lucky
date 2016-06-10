//          Universidad Politecnica de Madrid (UPM)
//
// File: enciende_apaga.h
// File history:
//      v0.0: 13/05/2016: Creation
//
// Description:   
//
// Fichero .h en el cual declaramos las cabeceras de las funciones 
// que sirve para apagar o enceder los leds RGB de determinadas formas.
// Authors: Karen Flores Yánez y Adrián García Espinosa 
//------------------------------------------------------------------------------
#ifndef _LED_CONTROL_H
#define _LED_CONTROL_H

#include "tipos.h"

void turnRed(int pin);
void turnBlue(int pin);
void turnGreen(int pin);
void offSTATE();
void turnColor(int player, int pin);
int debounce(int pin);
void offAll();
void allOff();
#endif /* _LED_CONTROL_H */
