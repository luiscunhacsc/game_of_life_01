#include <SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#define WIDTH 1920
#define HEIGHT 1080
#define NUM_THREADS 12    // Adjust based on your system's CPU cores

bool cells[HEIGHT * WIDTH];
bool newCells[HEIGHT * WIDTH];
bool *currentCells = cells;        // Pointer to current generation
bool *nextCells = newCells;        // Pointer to next generation

static inline bool get_cell(int x, int y) {
    return currentCells[(y + HEIGHT) % HEIGHT * WIDTH + (x + WIDTH) % WIDTH];
}

static inline void set_cell(int x, int y, bool state) {
    nextCells[(y + HEIGHT) % HEIGHT * WIDTH + (x + WIDTH) % WIDTH] = state;
}

int countAliveNeighbors(int x, int y) {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            count += get_cell(x + j, y + i);
        }
    }
    return count;
}

void* update_section(void* arg) {
    int thread_part = *(int*)arg;
    int section_height = HEIGHT / NUM_THREADS;
    int y_start = thread_part * section_height;
    int y_end = (thread_part + 1) * section_height;

    for (int y = y_start; y < y_end; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int aliveNeighbors = countAliveNeighbors(x, y);
            bool currentState = get_cell(x, y);
            bool nextState = currentState ? (aliveNeighbors == 2 || aliveNeighbors == 3) : (aliveNeighbors == 3);
            set_cell(x, y, nextState);
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Optimized Conway's Game of Life", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

    // Initialize cells randomly
    srand((unsigned int)time(NULL));
    for (int i = 0; i < HEIGHT * WIDTH; i++) {
        currentCells[i] = rand() % 100 < 5;
    }

    bool running = true;
    SDL_Event event;
    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Multi-threaded update
        for (int i = 0; i < NUM_THREADS; i++) {
            thread_args[i] = i;
            pthread_create(&threads[i], NULL, update_section, (void *)&thread_args[i]);
        }
        for (int i = 0; i < NUM_THREADS; i++) {
            pthread_join(threads[i], NULL);
        }

        // Swap pointers for the next generation
        bool* temp = currentCells;
        currentCells = nextCells;
        nextCells = temp;

        // Rendering code
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                if (get_cell(x, y)) {
                    SDL_RenderDrawPoint(renderer, x, y);
                }
            }
        }
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
