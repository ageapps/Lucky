/*
 * players.c
 *
 *  Created on: 16 de may. de 2016
 *      Author: adricacho
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // memcpy()
#include "players.h"

//This are the method used to manage the dynamic array:

void freeArray(struct Players *pArray) {
	free(pArray->array);
	pArray->array = NULL;
	pArray->player_number = pArray->numPlayers = 0;
}

void initArray(struct Players *pArray) {
	if ( NULL == (pArray->array = malloc(sizeof(struct player)))) { // then malloc failed
		freeArray(pArray);
		exit( EXIT_FAILURE);
	}

	// implied else, malloc successful

	pArray->player_number = 0;
	pArray->numPlayers = 1;
}

size_t sizeof_array(size_t size) {
	return size * sizeof(struct player);
}

void insertArray(struct Players *pArray, struct player *element) {
	if (pArray->player_number == pArray->numPlayers) { // then currently allocated memory for array of players is full

		struct player *temp = NULL;
		if ( NULL
				== (temp = realloc(pArray->array,
						sizeof_array(pArray->numPlayers) * 2))) { // then, realloc failed
			freeArray(pArray);
			exit( EXIT_FAILURE);
		}

		// implied else, realloc successful

		pArray->numPlayers *= 2;
		pArray->array = temp;
	}

	memcpy(&(pArray->array[pArray->player_number]), element,
			sizeof(struct player));
	pArray->player_number++;
}

int disqualified(struct Players *pArray, int errors) {
	int i = 0;
	for (i = 0; i < pArray->player_number; i++) {
		if (pArray->array[i].errors >= errors) {
			return 1;
		}
	}
	return 0;
}
