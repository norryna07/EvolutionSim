#ifndef INIT_H
#define INIT_H

#define PLANT_LIFE 100
#define HERBIVORE_LIFE 40
#define CARNIVORE_LIFE 60
#define GESTATION 20
#define WIDTH 100
#define HEIGHT 100
enum Species {EMPTY_CELL, PLANT, MALE_HERBIVORE, FEMALE_HERBIVORE, MALE_CARNIVORE, FEMALE_CARNIVORE};
struct World_Cell {
    enum Species type;
    int energy;
    int gestation;
};
extern int shm_id, sem_id;
extern struct World_Cell (*grid)[WIDTH];


void init_shared_memory();
void init_semaphores();
void lock_cell(int x, int y);
void unlock_cell(int x, int y);

#endif