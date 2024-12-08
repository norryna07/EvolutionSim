#ifndef LIVES_H
#define LIVES_H

#include "init.h"

extern int pid[HEIGHT*WIDTH];
extern int num_children;

extern int nr_plants, nr_herbivores, nr_carnivores;

void init_participants();
void place_participant(enum Species type, int energy);
void spawn_participant(enum Species type, int energy, int x, int y);
void plant_action(int *x, int *y, int level);
void male_herbivore_action(int *x, int *y);
void female_herbivore_action(int *x, int *y);
void male_carnivore_action(int *x, int *y);
void female_carnivore_action(int *x, int *y);

char inside(int x, int y);
void free_neighbour(int x, int y, int *nx, int *ny);
void find_neighbour(enum Species type, int x, int y, int *nx, int *ny);
void find_close_participant(enum Species type, int x, int y, int *nx, int *ny); 

void move_participant(int *x, int *y, int nx, int ny);

#endif