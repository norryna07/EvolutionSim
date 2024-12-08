#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <ncurses.h>
#include "init.h"
#include "lives.h"

// Function prototypes
void cleanup();
void display_world();
void init_ncurses();

char just_plants;

int main(int argv, char * argc[]) 
{
    if (argv != 1 && argv != 4) 
    {
        //the call is incorrect
        perror("The correct way to call this is: ./simulation [nr_plants] [nr_herbivores] [nr_carnivores]");
        return 1;
    } else 
    {
        if (argv == 1) 
        {
            nr_plants = 400;
            nr_herbivores = 90;
            nr_carnivores = 35;
        } else 
        {
            nr_plants = atoi(argc[1]);
            nr_herbivores = atoi(argc[2]);
            nr_carnivores = atoi(argc[3]);
            if (nr_plants == 0 || nr_herbivores == 0 || nr_carnivores == 0)
            {
                perror("The correct way to call this is: ./simulation [nr_plants] [nr_herbivores] [nr_carnivores]");
                return 1;
            }
        }
    }
    // Set up signal handler for cleanup on program termination
    signal(SIGINT, cleanup);

    // Initialize shared memory and semaphores
    init_shared_memory();
    init_semaphores();

    // Initialize ncurses interface
    init_ncurses();

    // Populate the grid with participants
    init_participants();

    // Run the simulation
    while (1) 
    {
        clear(); // Clear the screen for redrawing
        display_world();
        refresh(); // Refresh the ncurses screen
        usleep(1000000); // Sleep for 100ms to control the update rate
    }

    return 0;
}

// Function to initialize ncurses
void init_ncurses() 
{
    initscr(); // Start ncurses mode
    noecho(); // Don't show typed characters
    curs_set(0); // Hide the cursor
    cbreak(); // Disable line buffering

    start_color();
    init_pair(PLANT, COLOR_GREEN, COLOR_BLACK);  // Plants
    init_pair(MALE_HERBIVORE, COLOR_BLUE, COLOR_BLACK);  // Male Herbivore
    init_pair(FEMALE_HERBIVORE, COLOR_CYAN, COLOR_BLACK);  // Female Herbivore
    init_pair(MALE_CARNIVORE, COLOR_RED, COLOR_BLACK);   // Male Carnivore
    init_pair(FEMALE_CARNIVORE, COLOR_MAGENTA, COLOR_BLACK); // Female Carnivore
}

// Function to display the world grid
void display_world() 
{
    just_plants = 1;
    for (int i = 0; i < HEIGHT; i++) 
    {
        for (int j = 0; j < WIDTH; j++) 
        {
            switch (grid[i][j].type) 
            {
                case PLANT:
                    attron(COLOR_PAIR(PLANT));
                    mvprintw(i, j * 2, "P"); // Plant
                    attroff(COLOR_PAIR(PLANT));
                    break;
                case MALE_HERBIVORE:
                    attron(COLOR_PAIR(MALE_HERBIVORE));
                    mvprintw(i, j * 2, "I"); // Herbivore
                    attroff(COLOR_PAIR(MALE_HERBIVORE));
                    just_plants = 0;
                    break;
                case FEMALE_HERBIVORE:
                    attron(COLOR_PAIR(FEMALE_HERBIVORE));
                    mvprintw(i, j * 2, "i");
                    attroff(COLOR_PAIR(FEMALE_HERBIVORE));
                    just_plants = 0; 
                    break;
                case MALE_CARNIVORE:
                    attron(COLOR_PAIR(MALE_CARNIVORE));
                    mvprintw(i, j * 2, "C"); // Carnivore
                    attroff(COLOR_PAIR(MALE_CARNIVORE));
                    just_plants = 0;
                    break;
                case FEMALE_CARNIVORE:
                    attron(COLOR_PAIR(FEMALE_CARNIVORE));
                    mvprintw(i, j * 2, "c"); // Carnivore
                    attroff(COLOR_PAIR(FEMALE_CARNIVORE));
                    just_plants = 0;
                    break;
                case EMPTY_CELL:
                default:
                    mvprintw(i, j * 2, " "); // Empty cell
                    break;
            }
        }
    }
    if (just_plants) {
        cleanup();
        exit(0);
    }
}

// Cleanup function to release resources
void cleanup() 
{
    // End ncurses mode
    endwin();

    printf("\nTerminating simulation...\n");

     // Terminate all child processes
    for (int i = 0; i < num_children; i++) {
        kill(pid[i], SIGKILL);  // Send SIGKILL to each child
    }
    
    // Wait for all child processes to terminate
    for (int i = 0; i < num_children; i++) {
        waitpid(pid[i], NULL, 0);
    }

    // Detach shared memory
    if (shmdt(grid) < 0) {
        perror("shmdt");
    }

    // Remove shared memory segment
    if (shmctl(shm_id, IPC_RMID, NULL) < 0) {
        perror("shmctl");
    }

    // Remove semaphores
    if (semctl(sem_id, 0, IPC_RMID) < 0) {
        perror("semctl");
    }

    exit(0);
}
