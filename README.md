# EvolutionSim: A Game of Life Simulation

**EvolutionSim** is an educational simulation that explores how a simple ecosystem evolves over time. The project models interactions among plants, herbivores, and carnivores on a shared grid, showcasing complex behavior in a simple, visual manner. This project serves as an educational tool to understand concurrency, process management, and resource sharing in programming.

---

## Features

- **Dynamic Ecosystem**: Includes plants, herbivores (male and female), and carnivores (male and female).
- **Concurrency**: Each entity is a separate process, managed with shared memory and semaphores.
- **Interactive Visuals**: Uses `ncurses` for a real-time, terminal-based interface.
- **Evolving Behaviors**:
  - Plants grow and die based on interactions.
  - Herbivores eat plants and reproduce.
  - Carnivores hunt herbivores and reproduce.
- **Customizable**: Adjustable initial conditions (number of entities) via command-line arguments.

---

## How It Works

The simulation runs on a 100x100 grid where each cell can contain one of the following entities:

| Entity              | Symbol | Description               | Initial Energy | Behavior Summary                                  |
|---------------------|--------|---------------------------|----------------|--------------------------------------------------|
| Plant               | `P`    | Stationary, grows/dies    | 100            | Loses energy over time, reproduces every 10 steps |
| Herbivore (Male)    | `I`    | Male herbivore            | 40             | Eats plants, moves towards mates or food         |
| Herbivore (Female)  | `i`    | Female herbivore          | 40             | Eats plants, gives birth when gestation ends     |
| Carnivore (Male)    | `C`    | Male carnivore            | 60             | Hunts herbivores, moves towards mates or food    |
| Carnivore (Female)  | `c`    | Female carnivore          | 60             | Hunts herbivores, gives birth when gestation ends|

### Entity Behaviors

- **Plants**:
  - Lose 1 energy per step; die if energy reaches 0.
  - Reproduce every 10 steps if adjacent space is available.
  - Eaten by herbivores, losing additional energy.

- **Herbivores**:
  - Lose 1 energy per step; die if energy reaches 0.
  - Eat adjacent plants to gain energy.
  - Reproduce when energy conditions are met and a suitable mate is nearby.

- **Carnivores**:
  - Lose 1 energy per step; die if energy reaches 0.
  - Hunt adjacent herbivores to gain energy.
  - Reproduce when energy conditions are met and a suitable mate is nearby.

The simulation ends when only plants remain or the grid is empty.

---

## Installation and Setup

### Prerequisites

Ensure the following are installed on your system:
- **GCC compiler**
- **ncurses library**

To install `ncurses` on Fedora:
```bash
sudo dnf install ncurses ncurses-devel
```

### Building the Project
Clone the repository and navigate to the project directory:
```bash
git clone https://github.com/norryna07/EvolutionSim.git
cd EvolutionSim
```
Build the project using the provided Makefile:
```bash
make
```
### Running the Simulation
The program can be run with or without arguments:
```bash
./simulation [nr_plants] [nr_herbivores] [nr_carnivores]
```
- Default values: If no arguments are provided, default values will be used.
- Example:
```bash
./simulation 20 10 5
```

## Usage
### Controls
- The simulation runs automatically once started.
- Use Ctrl+C to terminate the program manually.

## Technical Details
- **Shared Memory**: The grid is stored in shared memory to allow concurrent processes to access it.
- **Semaphores**: Protect concurrent write operations to prevent data corruption.
- **Processes**: Each entity (plant, herbivore, carnivore) runs as a separate process.
- **Makefile**: Simplifies compilation and linking of the project.

### File Structure
- `main.c`: Contains the main logic and entry point for the simulation.
- `init.c`: Handles grid initialization and spawning of entities.
- `lives.c`: Implements entity behaviors and interactions.
- **Headers** (`init.h`, `lives.h`): Declare shared functions and constants.

## License
This project is licensed under the MIT License. See the LICENSE file for details.