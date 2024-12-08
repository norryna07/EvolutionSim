#include "init.h"
#include "lives.h"
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>

int shm_id, sem_id;
struct World_Cell (*grid)[WIDTH];

void init_shared_memory() 
{
    // get the shared memory segment
    shm_id = shmget(IPC_PRIVATE, sizeof(struct World_Cell) * WIDTH * HEIGHT, 
    IPC_CREAT | 0666);
    // check if the shared memory was created
    if (shm_id < 0) {
        perror("shmget");
        exit(1);
    }
    // return a pointer from shared memory
    grid = shmat(shm_id, NULL, 0);
    // check for error
    if (grid == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    // init the grid will all empty cells
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            grid[i][j].type = EMPTY_CELL;
            grid[i][j].energy = 0;
            grid[i][j].gestation = -1;
        }
    }
}

void init_semaphores() 
{
    // get the semaphores set
    sem_id = semget(IPC_PRIVATE, WIDTH * HEIGHT, IPC_CREAT | 0666);
    // check for errors
    if (sem_id < 0) {
        perror("semget");
        exit(1);
    }

    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        semctl(sem_id, i, SETVAL, 1);
    }
}

void lock_cell(int x, int y)
{
    struct sembuf op = {x * WIDTH + y, -1, 0};
    semop(sem_id, &op, 1);
}

void unlock_cell(int x, int y)
{
    struct sembuf op = {x * WIDTH + y, 1, 0};
    semop(sem_id, &op, 1);
}
