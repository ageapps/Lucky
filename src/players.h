/*
 * players.c
 *
 *  Created on: 16 de may. de 2016
 *      Author: adricacho
 */
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

struct player
{
    int played_time;
    int min_time;
    int max_time;
    int errors;
    int color;
};

struct Players
{
    struct player *array;
    size_t player_number;
    size_t numPlayers;
};

typedef struct Players Player_list;

void initArray(Player_list *a);
void insertArray(Player_list *a, struct player *element);
void freeArray(Player_list *a);
int disqualified(Player_list *a, int errors);
size_t sizeof_array(size_t size);
