//          Universidad Politecnica de Madrid (UPM)
//
// File: main.c
// File history:
//      v0.0: 13/05/2016: Creation
//
// Description:   
//
// Fichero principal que contiene al juego.
// Authors: Karen Flores Yánez y Adrián García Espinosa 
//------------------------------------------------------------------------------

/*Includes-------------------------------------------------------*/
#include <stdlib.h> 
#include <wiringPi.h>
#include <stdio.h>
#include "fsm.h"
#include "tmr.h"
#include "tipos.h"
#include "led_control.h"
#include "players.h"

/*Posibles estados en los que puede estar la máquina de estados---*/
enum fsm_state {
	 START, READY, PLAYING
};

/*Variables globales de juego-------------------------------------*/
int TIMEOUT_S = 3000; //timer del led START
int TIMEOUT_P = 5000; //timer del resto de leds del tablero
int MAX_RONDAS = 10;
int PENALTY = 5000; //tiempo de penalización 5seg
int MAX_ERRORES = 3;
int NUM_LEDS = 0;
char GAME_MODE; //guarda el comando del menú display
unsigned int tiempo_start = 0; //tiempo de comienzo de juego
int num_ronda = 0;
int flags = 0;
int boton_pulsado = 0;
int num_jugadores = 1;
int indice = 0;
int PLAYERS_COLORS[] = { PLAYER_GREEN, PLAYER_BLUE, PLAYER_RED };
int current_player = 0;
Player_list players; //lista de jugadores
Player_list *point_players = &players; //puntero a la lista de jugadores

/*
 @brief: Establece una configuración de juego por defecto
 */
void defaultSetUp() {
	TIMEOUT_S = 3000;
	TIMEOUT_P = 5000; //timer de cada led no start
	MAX_RONDAS = 10;
	PENALTY = 5000;
	MAX_ERRORES = 3;
	NUM_LEDS = 4;
	num_jugadores = 1;
	initArray(point_players); //inicializa la lista de jugadores con 1 solo jugador
	struct player myPlayer;
	myPlayer.color = PLAYERS_COLORS[0]; //le da un color por defecto al jugador
	myPlayer.errors = 0;
	myPlayer.max_time = 0;
	myPlayer.min_time = TIMEOUT_P; //tiempo mínimo=5seg
	myPlayer.played_time = 0;
	insertArray(point_players, &myPlayer); //introduce el jugador a la lista
}

/**
 * @brief: Rutina de interrupción a la excepcion de haber pulsado el botón START.
 */
void button_s_isr(void) {
	if (debounce(GPIO_BUTTON_S) == 1) {
		flags |= FLAG_BOTON_S;
	}
}

/**
 * @brief: Rutina de interrupción a la excepcion de haber pulsado el botón P1.
 */
void button_p1_isr(void) {
	if (debounce(GPIO_BUTTON_P1) == 1 && ((flags & FLAG_BOTON_P) == 0)) {
		boton_pulsado = 1;
		flags |= FLAG_BOTON_P;
	}
}

/**
 * @brief: Rutina de interrupción a la excepcion de haber pulsado el botón P2.
 */
void button_p2_isr(void) {
	if (debounce(GPIO_BUTTON_P2) == 1 && ((flags & FLAG_BOTON_P) == 0)) {
		boton_pulsado = 2;
		flags |= FLAG_BOTON_P;
	}
}
/**
 * @brief: Rutina de interrupción a la excepcion de haber pulsado el botón P3.
 */
void button_p3_isr(void) {
	if (debounce(GPIO_BUTTON_P3) == 1 && ((flags & FLAG_BOTON_P) == 0)) {
		boton_pulsado = 3;
		flags |= FLAG_BOTON_P;
	}
}
/**
 * @brief: Rutina de interrupción a la excepcion de haber pulsado el botón P4.
 */
void button_p4_isr(void) {
	if (debounce(GPIO_BUTTON_P4) == 1 && ((flags & FLAG_BOTON_P) == 0)) {
		boton_pulsado = 4;
		flags |= FLAG_BOTON_P;
	}
}

/**
 * @brief: Rutina de interrupción a la excepcion de timeout del timer.
 */
void timer_isr(union sigval value) {
	flags |= FLAG_TIMER;
}

/**
 * @brief: Este método se usa por su el jugador elige el modo "survival"
 El método comprueba que el timeout del led sea mayor de 0,5seg
 para poder ir restando 0,5seg segun avanza las rondas
 */
void checkSurvival() {
	if (GAME_MODE == 's' && TIMEOUT_P > 500) {
		TIMEOUT_P = TIMEOUT_P - 500;
	}
}

/*
 @brief: Al finalizar cada ronda este método calcula el tiempo que ha tardado el jugador en apagar el led.
 Acumula ese valor para que al finalizar el nº de rondas establecidas se puede hallar el tiempo medio.
 Por otro lado compara el tiempo con 2 valores concretos para saber si es el tiempo min o max.
 Comprueba si al led apagado le correspondía el pulsador que el jugador ha pulsado, si falla lo infica
 mediante el led de estado y suma tanto errores como penalización.
 */
void finishRound(int ok) {
	printf("Final de ronda: %d\n", num_ronda);
	int tiempo = millis() - tiempo_start;

	if (point_players->array[current_player].max_time < tiempo) {
		point_players->array[current_player].max_time = tiempo;
	}
	if (point_players->array[current_player].min_time > tiempo) {
		point_players->array[current_player].min_time = tiempo;
	}

	switch (ok) {

	case BOTON_OK: {
		turnGreen(0);
		printf("Has pulsado correctamente\n");
		break;
	}
	case BOTON_TIMEOUT: {
		turnRed(0);
		point_players->array[current_player].errors += 1;
		printf("No has pulsado nada\n");
		break;
	}
	case BOTON_MAL: {
		turnRed(0);
		point_players->array[current_player].errors += 1;
		tiempo = tiempo + PENALTY;
		printf(
				"Has pulsado un boton erroneo, has sido penalizado con : %d ms\n",
				PENALTY);
		break;
	}
	}

	checkSurvival();
	point_players->array[current_player].played_time += tiempo;
	//printf("Has tardado: %u\n", tiempo);
	//printf("Llevas un tiempo acumulado de: %u\n",
	//	point_players->array[current_player].played_time);

}

/*
 @brief: comprueba si se ha pulsado el botón de START
 @return: el flag correspondiente al boton START
 */

int STATE_PRESSED(fsm_t* this) {
	return (flags & FLAG_BOTON_S);
}

/*
 @brief: comprueba si el timeout de un led del tablero ha llegado a cero
 En el caso de que esto suceda porque el jugador ha estado lento
 llama al método correspondiente para penalizar.
 @return: el flag correspondiente al timeout.
 */
int TIMEOUT(fsm_t* this) {
	int result = (flags & FLAG_TIMER);
	if (result && num_ronda > 0) {
		finishRound(BOTON_TIMEOUT);
	}
	return result;
}

/*
 @brief: Comprueba si algún jugador es descalificado
 @return: devuelve 1 en caso afirmativo y 0 en caso contrario
 */
int DESQUALIFIED(fsm_t* this) {
	int result = disqualified(point_players, MAX_ERRORES);

	return result;
}

/**
 * @brief: Atiende la interrupción que causada por cualquier botón/sensor
 @return: el flag correspondiente al botón/sensor
 */
int BUTTON_PRESSED(fsm_t* this) {
	int result = (flags & FLAG_BOTON_P);

	if (result) {
		//printf("Has pulsado el botón: %d\n", boton_pulsado);
		if (boton_pulsado == indice) {
			finishRound(BOTON_OK);
		} else {
			finishRound(BOTON_MAL);
		}
		boton_pulsado = 0;
	}
	return result;
}

int BUTTON_PRESS(fsm_t* this) {
	return (flags & FLAG_BOTON_P);
}

/*
 @brief: comprueba si se ha llegado al final de las rondas de una partida
 @return: si se ha llegado al final devuelve 1 en caso contrario 0.
 */
int FINISHED_ROUNDS(fsm_t* this) {
	return num_ronda > MAX_RONDAS;
}

/**
 * @brief: Apagamos todos los leds
 */
void turnOff() {
	flags = 0;
	allOff();
}

/**
 * @brief: Método que enciende un led aleatoriamente a traves de enciende_color
 */
void turnLed(fsm_t* this) {

		tiempo_start = 0;
		turnOff();
		int nuevoindice = rand() % (NUM_LEDS) + 1; // numero aleatorio de 0 a 3
		while (nuevoindice == indice) {
			nuevoindice = rand() % (NUM_LEDS) + 1;
		}
		indice = nuevoindice;
		if (GAME_MODE == 'm') {
			current_player = rand() % (num_jugadores);
		}
		if (num_ronda < MAX_RONDAS) {
		turnColor(point_players->array[current_player].color, indice);
		}
		tiempo_start = millis();
		num_ronda += 1;
		tmr_startms((tmr_t*) (this->user_data), TIMEOUT_P);

}

/*
 @brief: Se encarga de hacer cambiar de estado y por consiguiente de color
 al led START
 */
void turnState(fsm_t* this) {
	flags = 0; //flags &= (~FLAG_BOTON_S);
	turnGreen(0);
	tmr_startms((tmr_t*) (this->user_data), TIMEOUT_S);
}

/*
 @brief: método auxiliar usado en el menu display para marcar el inicio de juego
 */
void startGame(fsm_t* this) {
	num_ronda = 0;
	printf("DEBES PULSAR LOS BOTONES A TIEMPO\n");
	turnLed(this);
	offSTATE();
}


/*
 @brief: Permite establecer la configuración del modo multijugador
 */
void setUpMultiplayer() {
	printf("  Introduce el número de jugadores: ");
	scanf("%d", &num_jugadores);
	while (num_jugadores <= 0 || num_jugadores > MAX_PLAYERS) {
		printf("  Introduzca un numero correcto de jugadores: ");
		scanf("%d", &num_jugadores);
	}

	int i;
	for (i = 1; i < num_jugadores; i++) {
		struct player myPlayer;
		myPlayer.color = PLAYERS_COLORS[i];
		myPlayer.errors = 0;
		myPlayer.max_time = 0;
		myPlayer.min_time = TIMEOUT_P;
		myPlayer.played_time = 0;
		insertArray(point_players, &myPlayer);
		printf("  Añadido Jugador: %d \n", i + 1);
	}

	MAX_RONDAS = num_jugadores * MAX_RONDAS;
	turnOff();
}

/*
 @brief: permite establecer la configuración del jugador en el modo "personalizado"
 */
void configurate() {

	printf("Introduce el tiempo máximo de pulsacion (seg): ");
	scanf("%d", &TIMEOUT_P);
	printf("Introduce el número máximo de rondas: ");
	scanf("%d", &MAX_RONDAS);
	printf("Introduce el tiempo de penalizacion (seg): ");
	scanf("%d", &PENALTY);
	printf("Introduce el numero máximo de errores: ");
	scanf("%d", &MAX_ERRORES);
	printf("Numero leds para jugar: ");
	scanf("%d", &NUM_LEDS);

	while (NUM_LEDS <= 0 || NUM_LEDS > MAX_LEDS) {
		printf("Introduzca un numero correcto de leds: ");
		scanf("%d", &NUM_LEDS);
	}

	printf("\nSE HA GUARDADO LA CONFIGURACIÓN\n");
	printf(
			"_______________________________________________________________\n\n");
	printf("  TU CONFIGURACIÓN ES: \n");
	printf("\tTiempo máximo de pulsacion: %d \n", TIMEOUT_P);
	printf("\tNúmero máximo de rondas: %d \n", MAX_RONDAS);
	printf("\tTiempo de penalización: %d \n", PENALTY);
	printf("\tNumero máximo de errores: %d \n", MAX_ERRORES);
	printf("\tNumero leds para jugar: %d \n", NUM_LEDS);
	printf(
			"_______________________________________________________________\n\n");
	// Pasamos a segundos las variables de tiempo
	TIMEOUT_P = TIMEOUT_P * 1000;
	PENALTY = PENALTY * 1000;

}
/*
 @brief: Menú display, muestra todos los modos de juego disponibles
 */
void displayMainMenu() {

	printf(
			"_______________________________________________________________\n\n");

	printf(
			"  Bienvenidos a LUCKY, el juego donde puedes medir tus reflejos!\n\n");

	printf(
			"_______________________________________________________________\n\n");

	printf("  Modos de juego disponibles:\n\n");

	printf("\tr\tRápido\n");

	printf("\ts\tSurvival\n");

	printf("\tp\tPersonalizado\n");

	printf("\tm\tMultijugador\n");

	printf(
			"_______________________________________________________________\n\n");

	printf("  Por favor, introduzca un comando: ");

}
/*
 @brief: Permite limpiar la pantalla después de escribir cualquier comando.
 */
void clearScrn(void) {
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
}

/*
 @brief: Comprueba si el comando introducido por el jugador es correcto
 */
void checkCmdLine(char *cmd_line) {
	if (cmd_line[1] == '\n') {
		if (cmd_line[0] != 'r' && cmd_line[0] != 's' && cmd_line[0] != 'p'
				&& cmd_line[0] != 'm')
			cmd_line[0] = 'z'; // Wrong command
	}
	// Wrong command lines
	else
		cmd_line[0] = 'z'; // Tag as a wrong command
}

/*
 @brief: Este método se encarga de sacar las estadísticas calculadas durante la partida,
 si se ha jugado una partida multijugadr, se imprime por pantalla todos los datos correspondientes
 a cada jugador. Y si no, la del jugador que haya realizado la partida.
 */
void printStatistics() {
	printf(
			"_______________________________________________________________\n\n");
	printf("  SE HA ACABADO LA PARTIDA\n");
	printf("  ESTOS SON LOS RESULTADOS: \n");
	printf(
			"_______________________________________________________________\n\n");

	if (num_ronda - 1 > 0) {
		if (num_jugadores > 1) {
			printf("  MODO: MULTIJUGADOR \n");
			printf("\t%d Rondas cada jugador \n",
					(num_ronda - 1) / num_jugadores);
			int i;
			for (i = 0; i < num_jugadores; i++) {
				printf(
						"_______________________________________________________________\n\n");
				printf("  Jugador %d: \n", i + 1);
				printf(
						"_______________________________________________________________\n\n");
				int errors = point_players->array[i].errors;
				if (errors == MAX_ERRORES) {
					printf(
							"  El jugador %d ha sido descalificado por cometer %d \n",
							i + 1, MAX_ERRORES);
				}
				int tiempo_medio = point_players->array[i].played_time
						/ (num_ronda - 1);
				printf("\tErrores cometidos: %d\n", errors);
				printf("\tTiempo medio de pulsacion (ms): %d\n", tiempo_medio);
				printf("\tTiempo maximo de pulsacion (ms): %d\n",
						point_players->array[i].max_time);
				printf("\tTiempo minimo de pulsacion (ms): %d\n",
						point_players->array[i].min_time);
			}
		} else {
			// solo 1 jugador
			printf("\t%d Rondas\n", num_ronda - 1);

			int errors = point_players->array[0].errors;
			if (errors == MAX_ERRORES) {
				printf("  Has sido descalificado por cometer %d \n",
						MAX_ERRORES);
			}
			int tiempo_medio = point_players->array[0].played_time
					/ (num_ronda - 1);
			printf("\tErrores cometidos: %d\n", errors);
			printf("\tTiempo medio de pulsacion (ms): %d\n", tiempo_medio);

			printf("\tTiempo maximo de pulsacion (ms): %d\n",
					point_players->array[0].max_time);
			printf("\tTiempo minimo de pulsacion (ms): %d\n",
					point_players->array[0].min_time);
		}
	} else {
		printf("  NO SE HA JUGADO\n");
	}

}



/*
 @brief: Si durante la partida algún jugador pulsa el botón START,
 este método se encarga de resetear el juego y empezar de nuevo.
 */
void resetGame() {
	freeArray(point_players);
	tiempo_start = 0;
	num_ronda = 0;
	turnOff();
}

/*
 @brief: Se encarga de resetear el juego, limpiar la pantalla y
 establecer una configuración predeterminada.
 */
void initiaize() {
	resetGame();
	offSTATE();
	clearScrn();
	defaultSetUp();
}
/*
 @brief: Este método reune tanto el mostrar por pantalla los comandos disponibles,
 como el procesamiento del comando introducido y la ejecución de una rutina concreta
 relacionada con dicho comando
 */
void startMenu(int firstTime) {
	char cmd_line[3];
	clearScrn();
	turnBlue(0);
	while (1) {
		if (firstTime == 0) {
			printStatistics();
		}
		displayMainMenu();
		if (fgets(cmd_line, 3, stdin) != NULL) {
			checkCmdLine(cmd_line);
			if (sscanf(cmd_line, "%c", &GAME_MODE) != EOF) {
				switch (GAME_MODE) {
				case 'p':
					initiaize();
					configurate();
					return;
				case 'r':
					initiaize();
					printf("INICIANDO MODO RÁPIDO\n");
					return;
				case 's':
					initiaize();
					printf("INICIANDO MODO RÁPIDO\n");
					return;
				case 'm':
					initiaize();
					setUpMultiplayer();
					printf("INICIANDO MODO MULTIJUGADOR\n");
					return;

				default:
					clearScrn();
					if (num_ronda > 0) {
						printStatistics();
					}
					printf("ERROR: El comando introducido no es correcto.\n\n");
					break;
				} /* end of switch */
			} /* end of if sscanf */
		} /* end of while fgets */
	} /* end of while (1) */

	offSTATE();
}
/*
 @brief: Cuando se terminado la partida, este método nos permite volver a empezar a jugar
 */
void gameOver(fsm_t* this) {
	turnOff();
	startMenu(0);
	printf("PULSA START PARA EMPEZAR DE NUEVO\n");
	//encender_todo();
}

void end(fsm_t* this){
	tiempo_start = 0;
		num_ronda = 0;
		turnOff();
	printStatistics();
	turnOff();
	offSTATE();

	printf("PULSA START PARA EMPEZAR DE NUEVO\n");
}

/*
 @brief: wait until next_activation (absolute time)
 */
void delay_until(unsigned int next) {
	unsigned int now = millis();
	if (next > now) {
		delay(next - now);
	}
}



void startGameMode( char mode){

	switch (mode) {
					case 'p':
						resetGame();
						offSTATE();
						configurate();
						return;
					case 'r':
						resetGame();
							offSTATE();
						printf("INICIANDO MODO RÁPIDO\n");
						return;
					case 's':
						resetGame();
							offSTATE();
						printf("INICIANDO MODO RÁPIDO\n");
						return;
					case 'm':
						resetGame();
						offSTATE();
						setUpMultiplayer();
						printf("INICIANDO MODO MULTIJUGADOR\n");
						return;

					default:
						printf("ERROR\n\n");
						break;
					} /* end of switch */
}

void restartMenu(fsm_t* this){
	turnOff();
	startMenu(1);
	printf("PULSA START PARA EMPEZAR DE NUEVO\n");

}



/*
 @brief: programa principal para ejecutar
 */
int main() {
	tmr_t* tmr = tmr_new(timer_isr); //creamos el temporizador asociado a la máquina de estados
	// Máquina de estados: lista de transiciones:{EstadoOrigen, CondicionDeDisparo, EstadoFinal, AccionesSiTransicion }
	fsm_trans_t pul_tmr[] = {
			{ START, STATE_PRESSED, READY, turnState },
			{ START, BUTTON_PRESS, START, restartMenu },
			{ READY, TIMEOUT, PLAYING, startGame },
			{ PLAYING, TIMEOUT, PLAYING,turnLed },
			{ PLAYING, BUTTON_PRESSED, PLAYING, turnLed },
			{ PLAYING, DESQUALIFIED, START,  end },
			{ PLAYING, FINISHED_ROUNDS, START, end },
			{ PLAYING, STATE_PRESSED, START, gameOver},
			{ -1, NULL,
			-1, NULL }, };
	fsm_t* pul_fsm = fsm_new(START, pul_tmr, tmr); //creamos la máquina de estados acorde de nuestro tabla de transiciones
	unsigned int next;

	wiringPiSetupGpio(); //Configuramos los pines que vamos a usar
	pinMode(GPIO_BUTTON_ENABLE, OUTPUT);
	digitalWrite(GPIO_BUTTON_ENABLE, 1);
	pinMode(GPIO_BUTTON_S, INPUT);
	pinMode(GPIO_BUTTON_P1, INPUT);
	pinMode(GPIO_BUTTON_P2, INPUT);
	pinMode(GPIO_BUTTON_P3, INPUT);
	pinMode(GPIO_BUTTON_P4, INPUT);
	pullUpDnControl(GPIO_BUTTON_S, PUD_DOWN); //establecemos resistencias pull down para los botones/sensores
	pullUpDnControl(GPIO_BUTTON_P1, PUD_DOWN);
	pullUpDnControl(GPIO_BUTTON_P2, PUD_DOWN);
	pullUpDnControl(GPIO_BUTTON_P3, PUD_DOWN);
	pullUpDnControl(GPIO_BUTTON_P4, PUD_DOWN);
	wiringPiISR(GPIO_BUTTON_S, INT_EDGE_RISING, button_s_isr); //sensibles a flanco de subida
	wiringPiISR(GPIO_BUTTON_P1, INT_EDGE_RISING, button_p1_isr);
	wiringPiISR(GPIO_BUTTON_P2, INT_EDGE_RISING, button_p2_isr);
	wiringPiISR(GPIO_BUTTON_P3, INT_EDGE_RISING, button_p3_isr);
	wiringPiISR(GPIO_BUTTON_P4, INT_EDGE_RISING, button_p4_isr);

	// Configuramos los LEDS RGB como pines de salida
	pinMode(GPIO_LIGHT_P1_R, OUTPUT);
	pinMode(GPIO_LIGHT_P1_G, OUTPUT);
	pinMode(GPIO_LIGHT_P1_B, OUTPUT);

	pinMode(GPIO_LIGHT_P2_R, OUTPUT);
	pinMode(GPIO_LIGHT_P2_G, OUTPUT);
	pinMode(GPIO_LIGHT_P2_B, OUTPUT);

	pinMode(GPIO_LIGHT_P3_R, OUTPUT);
	pinMode(GPIO_LIGHT_P3_G, OUTPUT);
	pinMode(GPIO_LIGHT_P3_B, OUTPUT);

	pinMode(GPIO_LIGHT_P4_R, OUTPUT);
	pinMode(GPIO_LIGHT_P4_G, OUTPUT);
	pinMode(GPIO_LIGHT_P4_B, OUTPUT);

	// Led RGB de Estado
	pinMode(GPIO_STATE_BLUE, OUTPUT);
	pinMode(GPIO_STATE_GREEN, OUTPUT);
	pinMode(GPIO_STATE_RED, OUTPUT);

	turnOff(); //Apagamos el tablero antes de empezar a mostrar el menu por pantalla
	startMenu(1);
	turnOff();

	printf("COMIENZA EL JUEGO, PULSA START PARA EMPEZAR\n");
	next = millis();
	while (1) //Bucle infinito que se encarga de comprobar periódicamente si se ha cambiado o no de estado
	{
		fsm_fire(pul_fsm);
		next += CLK_MS;
		delay_until(next);
	}

	tmr_destroy((tmr_t*) (pul_fsm->user_data));
	fsm_destroy(pul_fsm);
}
