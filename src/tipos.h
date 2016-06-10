//          Universidad Politecnica de Madrid (UPM)
//
// File: tipos.h
// File history:
//      v0.0: 13/05/2016: Creation
//
// Description:   
//
// Conjunto de #define que usamos a lo largo del main.c.
//
// Authors: Karen Flores Yánez y Adrián García Espinosa 
//------------------------------------------------------------------------------

#ifndef _TIPOS_H
#define _TIPOS_H

//Pines correspondientes al botón START y los sensores de juego P1, P2, P3 y P4. 
#define GPIO_BUTTON_S 26
#define GPIO_BUTTON_P1  21
#define GPIO_BUTTON_P2 20
#define GPIO_BUTTON_P3 16
#define GPIO_BUTTON_P4 12

//Pines correspondientes al led RGB de "ESTADO" el cual marca el inicio de juego y si cometes fallos durante el mismo.
#define GPIO_STATE_RED	27 // Led de ESTADO
#define GPIO_STATE_BLUE 17
#define GPIO_STATE_GREEN 22

//Pines correspondientes al led RGB de P1.
#define GPIO_LIGHT_P1_R	2
#define GPIO_LIGHT_P1_B	4
#define GPIO_LIGHT_P1_G	3

//Pines correspondientes al led RGB de P2.
#define GPIO_LIGHT_P2_R	11
#define GPIO_LIGHT_P2_B	10
#define GPIO_LIGHT_P2_G	9

//Pines correspondientes al led RGB de P3.
#define GPIO_LIGHT_P3_R	7
#define GPIO_LIGHT_P3_B	8
#define GPIO_LIGHT_P3_G 25

//Pines correspondientes al led RGB de P4.
#define GPIO_LIGHT_P4_R	18
#define GPIO_LIGHT_P4_B	14
#define GPIO_LIGHT_P4_G	15

//Pin correspondiente al boton Enable.
#define GPIO_BUTTON_ENABLE	0

//Flags correspondientes al botón START,a los sensores P1, P2, P3 y P4 y al timer.
#define FLAG_BOTON_S	0x01
#define FLAG_BOTON_P	0x02
#define FLAG_TIMER		0x04

//Valores correspondientes al acierto y a los dos tipos de fallo a la hora de pulsar.
#define BOTON_TIMEOUT 0
#define BOTON_OK 1
#define BOTON_MAL 2

//Valores correspondientes al número de jugadores.
#define PLAYER_RED 0
#define PLAYER_GREEN 1
#define PLAYER_BLUE 2

//Otros valores auxiliares.
#define CLK_MS 10
#define MAX_LEDS 4 // máximo numero de leds para jugar
#define MAX_PLAYERS 3 // máximo numero de leds para jugar
#define DEBOUNCE_TIME 30 // tiempo de antirebote

#endif /* _TIPOS_H */
