//          Universidad Politecnica de Madrid (UPM)
//
// File: enciende_apaga.h
// File history:
//      v0.0: 13/05/2016: Creation
//
// Description:   
//
// Fichero .c donde implementamos las funciones que sirve 
// para apagar o enceder los leds RGB de determinadas formas.
// Authors: Karen Flores Yánez y Adrián García Espinosa 
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "tipos.h"
#include <wiringPi.h>
#include "led_control.h"


//Array de los pines correspondientes al color Rojo de cada led RGB del juego.
int leds_rojos[] = {GPIO_STATE_RED,
					GPIO_LIGHT_P1_R,
					GPIO_LIGHT_P2_R,
					GPIO_LIGHT_P3_R,
					GPIO_LIGHT_P4_R };

//Array de los pines correspondientes al color Verde de cada led RGB del juego.
int leds_verdes[] = {GPIO_STATE_GREEN,
					GPIO_LIGHT_P1_G,
					GPIO_LIGHT_P2_G,
					GPIO_LIGHT_P3_G,
					GPIO_LIGHT_P4_G };

//Array de los pines correspondientes al color Azul de cada led RGB del juego.
int leds_azules[] = {GPIO_STATE_BLUE,
					GPIO_LIGHT_P1_B,
					GPIO_LIGHT_P2_B,
					GPIO_LIGHT_P3_B,
					GPIO_LIGHT_P4_B };

//Método que se encarga de encender el color Rojo de un led RGB concreto.
void turnRed(int pin) {
	digitalWrite(leds_rojos[pin], LOW);
	digitalWrite(leds_verdes[pin], HIGH);
	digitalWrite(leds_azules[pin], HIGH);
	//printf("enciendeRojo\n");
}

//Método que se encarga de encender el color Azul de un led RGB concreto.
void turnBlue(int pin) {
	digitalWrite(leds_rojos[pin], HIGH);
	digitalWrite(leds_verdes[pin], HIGH);
	digitalWrite(leds_azules[pin], LOW);
	//printf("enciendeAzul\n");
}

//Método que se encarga de encender el color Verde de un led RGB concreto.
void turnGreen(int pin) {
	digitalWrite(leds_rojos[pin], HIGH);
	digitalWrite(leds_verdes[pin], LOW);
	digitalWrite(leds_azules[pin], HIGH);
	//printf("enciendeVerde\n");
}

//Método que se encarga de apagar el led START.
void offSTATE() {
	digitalWrite(GPIO_STATE_RED, HIGH);
	digitalWrite(GPIO_STATE_BLUE, HIGH);
	digitalWrite(GPIO_STATE_GREEN, HIGH);
	//printf("apagaSTATE\n");
}

//Método que en una partida multijugador enciende un led de un determinado color correspondiente a un jugador concreto.
void turnColor(int player, int pin) {

	switch (player) {
		case PLAYER_RED: {
		turnRed(pin);
		break;
		}
		case PLAYER_BLUE: {
		turnBlue(pin);
		break;
		}
		case PLAYER_GREEN: {
		turnGreen(pin);
		break;
		}
		default: {
		turnRed(pin);
		break; //antes no estaba el break
		}
	}
}
int debounce(int pin) {
	int contador = 0;
	int estado = 0; // guarda el estado del pin
	int estadoAnterior = 0; // guarde el ultimo estado del boton

	do {
		estado = digitalRead(pin);
		if (estado != estadoAnterior) { // se commpara el estado actual con el$
			contador = 0;
			estadoAnterior = estado;
		} else {
			contador = contador + 1;
		}
		delay(1);
	} while (contador < DEBOUNCE_TIME);

	return estado;
}
void allOff(){
	digitalWrite(GPIO_LIGHT_P1_R, 1);
		digitalWrite(GPIO_LIGHT_P1_G, 1);
		digitalWrite(GPIO_LIGHT_P1_B, HIGH);
		digitalWrite(GPIO_LIGHT_P2_R, HIGH);
		digitalWrite(GPIO_LIGHT_P2_G, HIGH);
		digitalWrite(GPIO_LIGHT_P2_B, HIGH);
		digitalWrite(GPIO_LIGHT_P3_R, HIGH);
		digitalWrite(GPIO_LIGHT_P3_G, HIGH);
		digitalWrite(GPIO_LIGHT_P3_B, HIGH);
		digitalWrite(GPIO_LIGHT_P4_R, HIGH);
		digitalWrite(GPIO_LIGHT_P4_G, HIGH);
		digitalWrite(GPIO_LIGHT_P4_B, HIGH);
}
