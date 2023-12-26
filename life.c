#include <SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define WIDTH 1920*2   // New width
#define HEIGHT 1080*2   // New height

// Function to count alive neighbors
int countAliveNeighbors(bool (*cells)[WIDTH], int x, int y) {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue; // Skip the cell itself
            int col = (x + j + WIDTH) % WIDTH;
            int row = (y + i + HEIGHT) % HEIGHT;
            count += cells[row][col] ? 1 : 0;
        }
    }
    return count;
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Conway's Game of Life",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          WIDTH, HEIGHT,
                                          SDL_WINDOW_SHOWN);
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

    // Allocate memory for cells
    bool (*cells)[WIDTH] = malloc(HEIGHT * sizeof(*cells));
    bool (*newCells)[WIDTH] = malloc(HEIGHT * sizeof(*newCells));
    if (!cells || !newCells) {
        fprintf(stderr, "Failed to allocate memory.\n");
        free(cells); // free in case one allocation succeeded
        free(newCells);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Seed random number generator
    srand((unsigned int)time(NULL));

    // Randomly initialize cells
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            cells[y][x] = (rand() % 100) < 25; // approximately 5% chance
        }
    }

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Update the game state
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                int aliveNeighbors = countAliveNeighbors(cells, x, y);

                newCells[y][x] = cells[y][x] ? 
                    (aliveNeighbors == 2 || aliveNeighbors == 3) : 
                    (aliveNeighbors == 3);
            }
        }

        // Copy newCells to cells
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                cells[y][x] = newCells[y][x];
            }
        }

        // Render the current game state
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White background
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black color for cells

        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                if (cells[y][x]) {
                    SDL_RenderDrawPoint(renderer, x, y);
                }
            }
        }

        SDL_RenderPresent(renderer);
    }

    // Free the allocated memory
    free(cells);
    free(newCells);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
