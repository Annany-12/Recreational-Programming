// Compiling Command for this specific Code: g++ BouncyBall.cpp -o BouncyBall.exe -I C:\mingw64_update\include -L C:\mingw64_update\lib -lSDL2main -lSDL2

#define SDL_MAIN_HANDLED
#include <iostream>
#include <SDL2/SDL.h>
#include <cmath>
#include <vector>

#define WIDTH 900
#define HEIGHT 600
#define GRAVITY 2
#define DAMPENING 0.7

#define COLOR_WHITE  0xffffffff
#define COLOR_BLACK  0x00000000
#define COLOR_BACKGROUND  0x292929
#define COLOR_BALL  0xf0f0f0f0

using namespace std;

struct Circle{
    double x;
    double y;
    double radius;
    double velocity_x;
    double velocity_y;
};

void draw_circle(SDL_Surface* surface, struct Circle circle, Uint32 color) {
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            double dist = sqrt(pow(i - circle.x, 2) + pow(j - circle.y, 2));
            if (dist < circle.radius) {
                Uint32* pixels = (Uint32*)surface->pixels; // Treat the surface pixels as a 2D array of 32-bit colors
                pixels[j * (surface->pitch / 4) + i] = color; // Set the pixel at row j and column i to white
            }
        }
    }
}

void step(struct Circle* circle){
    circle->x = circle->x + circle->velocity_x;
    circle->y = circle->y + circle->velocity_y;
    circle->velocity_y = circle->velocity_y + GRAVITY;

    if (circle->x - circle->radius < 0) {
        circle->velocity_x = -circle->velocity_x * DAMPENING;
        circle->x = circle->radius;
    }
    if (circle->x + circle->radius > WIDTH) {
        circle->velocity_x = -circle->velocity_x * DAMPENING;
        circle->x = WIDTH - circle->radius;
    }
    if (circle->y - circle->radius < 0) {
        circle->velocity_y = -circle->velocity_y * DAMPENING;
        circle->y = circle->radius;
    }
    if (circle->y + circle->radius > HEIGHT) {
        circle->velocity_y = -circle->velocity_y * DAMPENING;
        circle->y = HEIGHT - circle->radius;
    }
}

int main(){
    cout << "Hello Bouncy Ball\n";

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Bouncy Ball", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

    SDL_Surface* surface = SDL_GetWindowSurface(window);

    struct Circle circle = {200, 200, 70, 50, 50};

    SDL_Rect erase_rect = (SDL_Rect){0, 0, WIDTH, HEIGHT};
    SDL_Event event;

    bool sim_running = true;
    while(sim_running){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                sim_running = false;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                sim_running = false;
            }
        }
        SDL_FillRect(surface, &erase_rect, COLOR_BACKGROUND);
        draw_circle(surface, circle, COLOR_BALL);
        step(&circle);
        SDL_UpdateWindowSurface(window);
    }

    SDL_Quit();
    return 0;
}