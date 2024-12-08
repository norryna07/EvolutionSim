#include "lives.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int pid[HEIGHT*WIDTH];
int num_children = 0;
int nr_plants, nr_herbivores, nr_carnivores;

void init_participants() 
{
    srand(time(NULL));

    //spawn the plants first
    for (int i = 0; i < nr_plants; ++i)
        place_participant(PLANT, PLANT_LIFE);

    for (int i = 0; i < nr_herbivores; ++i)
    {
        place_participant(MALE_HERBIVORE, HERBIVORE_LIFE);
        place_participant(FEMALE_HERBIVORE, HERBIVORE_LIFE);
    }

    for (int i = 0; i < nr_carnivores; ++i) 
    {
        place_participant(MALE_CARNIVORE, CARNIVORE_LIFE);
        place_participant(FEMALE_CARNIVORE, CARNIVORE_LIFE);
    }
    
}

void place_participant(enum Species type, int energy) 
{
    int x, y;
    do {
        x = rand() % HEIGHT;
        y = rand() % WIDTH;
    } while (grid[x][y].type != EMPTY_CELL);
    spawn_participant(type, energy, x, y);
}


void spawn_participant(enum Species type, int energy, int x, int y) 
{
    lock_cell(x, y);
    grid[x][y].type = type;
    grid[x][y].energy = energy;
    grid[x][y].gestation = -1;
    unlock_cell(x, y);

    int val = fork();
    
    if (val == 0) {
        int level = 0;
        while (grid[x][y].energy > 0) {
            level ++;
            switch (type) {
                case PLANT:
                    plant_action(&x, &y, level);
                    break;
                case MALE_HERBIVORE:
                    male_herbivore_action(&x, &y);
                    break;
                case FEMALE_HERBIVORE:
                    female_herbivore_action(&x, &y);
                    break;
                case MALE_CARNIVORE:
                    male_carnivore_action(&x, &y);
                    break;
                case FEMALE_CARNIVORE:
                    female_carnivore_action(&x, &y);
                    break;
                default:
                    break;
            }
            usleep(1000000);
        }
        grid[x][y].type = EMPTY_CELL;
        grid[x][y].energy = 0;
        grid[x][y].gestation = -1;
        exit(0);
    } else {
        pid[num_children ++] = val;
    }
}

void plant_action(int *x, int *y, int level) 
{
    // each level the energy - 1
    lock_cell(*x, *y);
    grid[*x][*y].energy --;
    if (level % 10 == 0) {
        int nx, ny;
        free_neighbour(*x, *y, &nx, &ny);
        if (nx != -1 && ny != -1) // we have a place for a new plant
            spawn_participant(PLANT, PLANT_LIFE, nx, ny); 
    }
    unlock_cell(*x, *y);
}

void male_herbivore_action(int *x, int *y) 
{
    lock_cell(*x, *y);
    grid[*x][*y].energy --;
    // seach for a plant
    int nx, ny;
    find_neighbour(PLANT, *x, *y, &nx, &ny);
    if (nx != -1 && ny != -1) // I found a plant
    {
        grid[*x][*y].energy += 2;
        lock_cell(nx, ny);
        grid[nx][ny].energy --;
        unlock_cell(nx, ny);
    }
    int energy = grid[*x][*y].energy;
    if (energy > 15) 
    {
        find_neighbour(FEMALE_HERBIVORE, *x, *y, &nx, &ny);
        if (nx != -1 && ny != -1 && grid[*x][*y].gestation == -1) // I found a girl
        {
            grid[*x][*y].energy -= 5;
            lock_cell(nx, ny);
            grid[nx][ny].gestation = 20;
            unlock_cell(nx, ny);
        }
    }
    if (energy < 10) // I need to find food
    {
        find_close_participant(PLANT, *x, *y, &nx, &ny);
        if (nx != -1 && ny != -1) // I found a plant
        {
            move_participant(x, y, nx, ny);
        }
    }
    if (energy > 20) // I need to find a girl
    {
        find_close_participant(FEMALE_HERBIVORE, *x, *y, &nx, &ny);
        if (nx != -1 && ny != -1) // I found a girl
        {
            move_participant(x, y, nx, ny);
        }
    }
    unlock_cell(*x, *y);
}

void female_herbivore_action(int *x, int *y)
{
    lock_cell(*x, *y);
    grid[*x][*y].energy --;
    // seach for a plant
    int nx, ny;
    find_neighbour(PLANT, *x, *y, &nx, &ny);
    if (nx != -1 && ny != -1) // I found a plant
    {
        grid[*x][*y].energy += 2;
        lock_cell(nx, ny);
        grid[nx][ny].energy --;
        unlock_cell(nx, ny);
    }
    int energy = grid[*x][*y].energy;
    if (energy < 10) // I need to find food
    {
        find_close_participant(PLANT, *x, *y, &nx, &ny);
        if (nx != -1 && ny != -1) // I found a plant
        {
            move_participant(x, y, nx, ny);
        }
    }
    if (grid[*x][*y].gestation != -1) {
        grid[*x][*y].gestation --;
        if (grid[*x][*y].gestation == 0) {
            free_neighbour(*x, *y, &nx, &ny);
            if (nx != -1 && ny != -1) // we have a free spot
            {
                int sex = rand() % 2;
                spawn_participant(sex ? MALE_HERBIVORE : FEMALE_HERBIVORE, HERBIVORE_LIFE, nx, ny);
            }
            grid[*x][*y].gestation = -1;
        }
    }
    unlock_cell(*x, *y);
}

void male_carnivore_action(int *x, int *y) 
{
    lock_cell(*x, *y);
    grid[*x][*y].energy --;
    int nx, ny;
    find_neighbour(MALE_HERBIVORE, *x, *y, &nx, &ny);
    if (nx != -1 && ny != -1) // we found food
    {
        lock_cell(nx, ny);
        grid[*x][*y].energy += grid[nx][ny].energy;
        grid[nx][ny].energy = 0;
        unlock_cell(nx, ny);
    }
    int energy = grid[*x][*y].energy;
    if (energy > 15) 
    {
        find_neighbour(FEMALE_CARNIVORE, *x, *y, &nx, &ny);
        if (nx != -1 && ny != -1 && grid[*x][*y].gestation == -1)
        {
            grid[*x][*y].energy -= 5;
            lock_cell(nx, ny);
            grid[nx][ny].gestation = 20;
            unlock_cell(nx, ny);
        }
    }
    if (energy < 20) // go to food
    {
        find_close_participant(MALE_HERBIVORE, *x, *y, &nx, &ny);
        if (nx != -1 && ny != -1)
        {
            move_participant(x, y, nx, ny);
        }
    }
    if (energy > 30) // go to girl
    {
        find_close_participant(FEMALE_CARNIVORE, *x, *y, &nx, &ny);
        if (nx != -1 && ny != -1)
        {
            move_participant(x, y, nx, ny);
        }
    }
    unlock_cell(*x, *y);
}

void female_carnivore_action(int *x, int *y)
{
    lock_cell(*x, *y);
    grid[*x][*y].energy --;
    int nx, ny;
    find_neighbour(MALE_HERBIVORE, *x, *y, &nx, &ny);
    if (nx != -1 && ny != -1) // we found food
    {
        lock_cell(nx, ny);
        grid[*x][*y].energy += grid[nx][ny].energy;
        grid[nx][ny].energy = 0;
        unlock_cell(nx, ny);
    }
    int energy = grid[*x][*y].energy;
    if (energy < 20) // go to food
    {
        find_close_participant(MALE_HERBIVORE, *x, *y, &nx, &ny);
        if (nx != -1 && ny != -1)
        {
            move_participant(x, y, nx, ny);
        }
    }
    if (grid[*x][*y].gestation != -1) 
    {
        grid[*x][*y].gestation --;
        if (grid[*x][*y].gestation == 0)
        {
            free_neighbour(*x, *y, &nx, &ny);
            if (nx != -1 && ny != -1) 
            {
                int sex = rand() % 2;
                spawn_participant(sex ? MALE_CARNIVORE : FEMALE_CARNIVORE, CARNIVORE_LIFE, nx, ny);
            }
            grid[*x][*y].gestation = -1;
        }
    }
    unlock_cell(*x, *y);
}

char inside(int x, int y) 
{
    return x >= 0 && x < HEIGHT &&
           y >= 0 && y < WIDTH;
}

void free_neighbour(int x, int y, int *nx, int *ny) 
{
    *nx = -1;
    *ny = -1;
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};
    for (int k = 0; k < 4; ++k) 
    {
        int lx = x + dx[k];
        int ly = y + dy[k];
        if (inside(lx, ly) && grid[lx][ly].type == EMPTY_CELL) {
            *nx = lx;
            *ny = ly;
            return;
        }
    }
}

void find_neighbour(enum Species type, int x, int y, int *nx, int *ny)
{
    *nx = -1;
    *ny = -1;
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};
    for (int k = 0; k < 4; ++k) 
    {
        int lx = x + dx[k];
        int ly = y + dy[k];
        if (inside(lx, ly)) 
        {
            if (type == MALE_HERBIVORE) {
                if (grid[lx][ly].type == MALE_HERBIVORE || grid[lx][ly].type == FEMALE_HERBIVORE) {
                    *nx = lx;
                    *ny = ly;
                    return;
                }
            } else if (grid[lx][ly].type == type) {
                *nx = lx;
                *ny = ly;
                return;
            }
        }
    }
}

void find_close_participant(enum Species type, int x, int y, int *nx, int *ny) 
{
    *nx = -1;
    *ny = -1;
    int distance = __INT_MAX__;
    int values = 0;
    for (int dx = -5; dx <= 5; ++dx)
        for (int dy = -5; dy <= 5; ++dy) 
        {
            int lx = x + dx;
            int ly = y + dy;
            if (inside(lx, ly)) {
                if ((type == MALE_HERBIVORE && (grid[lx][ly].type == MALE_HERBIVORE || grid[lx][ly].type == FEMALE_HERBIVORE)) ||
                    grid[lx][ly].type == type) {
                        int ld = dx + dy;
                        if (ld < distance) {
                            distance = ld;
                            *nx = lx;
                            *ny = ly;
                            values = 0;
                        } else if (ld == distance) {
                            values = 1;
                        }
                }
            }
        }
    if (values) {
        *nx = -1;
        *ny = -1;
    }
}

void move_participant(int *x, int *y, int nx, int ny)
{
    // check on x dimension
    if (nx > *x) // it's lower
    {
        if (grid[*x + 1][*y].type == EMPTY_CELL)
        {
            //move there
            lock_cell(*x + 1, *y);
            grid[*x + 1][*y] = grid[*x][*y];
            grid[*x][*y].type = EMPTY_CELL;
            grid[*x][*y].energy = 0;
            grid[*x][*y].gestation = -1;
            unlock_cell(*x, *y);
            *x = *x + 1;
            return;
        }
    } else if (nx < *x) 
    {
        if (grid[*x - 1][*y].type == EMPTY_CELL)
        {
            //move there
            lock_cell(*x -1, *y);
            grid[*x - 1][*y] = grid[*x][*y];
            grid[*x][*y].type = EMPTY_CELL;
            grid[*x][*y].energy = 0;
            grid[*x][*y].gestation = -1;
            unlock_cell(*x, *y);
            *x = *x - 1;
            return;
        }
    }
        // check on y dimension
    if (ny > *y) // it's lower
    {
        if (grid[*x][*y + 1].type == EMPTY_CELL)
        {
            //move there
            lock_cell(*x, *y + 1);
            grid[*x][*y + 1] = grid[*x][*y];
            grid[*x][*y].type = EMPTY_CELL;
            grid[*x][*y].energy = 0;
            grid[*x][*y].gestation = -1;
            unlock_cell(*x, *y);
            *y = *y + 1;
            return;
        }
    } else if (ny < *y) 
    {
        if (grid[*x][*y - 1].type == EMPTY_CELL)
        {
            //move there
            lock_cell(*x, *y - 1);
            grid[*x][*y - 1] = grid[*x][*y];
            grid[*x][*y].type = EMPTY_CELL;
            grid[*x][*y].energy = 0;
            grid[*x][*y].gestation = -1;
            unlock_cell(*x, *y);
            *y = *y - 1;
            return;
        }
    }
}