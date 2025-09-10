// Compiling Command for this specific Code: g++ 2d_ray_tracing_final.cpp -o 2d_ray_tracing_final.exe -I C:\mingw64_update\include -L C:\mingw64_update\lib -lSDL2main -lSDL2 && 2d_ray_tracing_final.exe


#define SDL_MAIN_HANDLED
#include <iostream>
#include <SDL2/SDL.h>
#include <cmath>
#include <vector>
using namespace std;

#define WIDTH 1200
#define HEIGHT 600
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000
#define COLOR_RED 0xffff0000
#define COLOR_GRAY 0xefefefef
#define COLOR_YELLOW 0xf0f763
#define RAY_NUMBER 100

struct Circle {
    double x;
    double y;
    double radius;
};

struct Ray {
    double x_start;
    double y_start;
    double angle;
};

void FillCircle(SDL_Surface* surface, struct Circle circle, Uint32 color) {
    for (double x = circle.x - circle.radius; x <= circle.x + circle.radius; x++) {
        double radius_squared = circle.radius * circle.radius;
        for (double y = circle.y - circle.radius; y <= circle.y + circle.radius; y++) {
            double dist_squared_from_center = (x - circle.x) * (x - circle.x) + (y - circle.y) * (y - circle.y);
            if (dist_squared_from_center < radius_squared) {
                SDL_Rect pixel = {(int)x, (int)y, 1, 1};
                SDL_FillRect(surface, &pixel, color);
            }
        }
    }
}

void generate_rays(struct Circle circle, struct Ray rays_arr[RAY_NUMBER]) {
    for (int i = 0; i < RAY_NUMBER; i++) {
        double angle = ((double)i / RAY_NUMBER) * 2 * M_PI;
        struct Ray ray = {circle.x, circle.y, angle};
        rays_arr[i] = ray;
    }
}

void FillRays(SDL_Surface* surface, struct Ray rays[RAY_NUMBER], Uint32 color, vector<Circle>& obstacles) {
    for (int i = 0; i < RAY_NUMBER; i++) {
        struct Ray ray = rays[i];

        double step = 1.0;
        double x_draw = ray.x_start;
        double y_draw = ray.y_start;
        bool hit = false;

        while (true) {
            x_draw += step * cos(ray.angle);
            y_draw += step * sin(ray.angle);

            if (x_draw < 0 || x_draw >= WIDTH || y_draw < 0 || y_draw >= HEIGHT)
                break;

            for (auto& object : obstacles) {
                double radius_squared = object.radius * object.radius;
                double dist_squared_from_center = (x_draw - object.x) * (x_draw - object.x) +
                                                  (y_draw - object.y) * (y_draw - object.y);
                if (dist_squared_from_center < radius_squared) {
                    hit = true;
                    break;
                }
            }

            if (hit) break;

            SDL_Rect pixel = {(int)x_draw, (int)y_draw, 1, 1};
            SDL_FillRect(surface, &pixel, color);
        }
    }
}

vector<Circle> obstacles = {
    {650.00, 350.00, 100.00},
    {300.00, 500.00, 75.00},
    {500.00, 200.00, 50.00},
    {400.00, 150.00, 35}
};

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("2D Ray Tracing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);

    Circle light_source_circle = {450.00, 350.00, 32.50};

    SDL_Rect erase_rect = {0, 0, WIDTH, HEIGHT};

    struct Ray ray_arr[RAY_NUMBER];
    bool sim_running_flag = true;
    SDL_Event event;

    bool dragging = false;
    int selectedObstacle = -1;

    while (sim_running_flag) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                sim_running_flag = false;
            }

            if (event.type == SDL_MOUSEMOTION) {
                if (dragging && selectedObstacle != -1) {
                    obstacles[selectedObstacle].x = event.motion.x;
                    obstacles[selectedObstacle].y = event.motion.y;
                } else {
                    light_source_circle.x = event.motion.x;
                    light_source_circle.y = event.motion.y;
                    generate_rays(light_source_circle, ray_arr);
                }
            }

            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                for (int i = 0; i < obstacles.size(); i++) {
                    double dx = event.button.x - obstacles[i].x;
                    double dy = event.button.y - obstacles[i].y;
                    if (dx * dx + dy * dy < obstacles[i].radius * obstacles[i].radius) {
                        dragging = true;
                        selectedObstacle = i;
                        break;
                    }
                }
            }

            if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
                dragging = false;
                selectedObstacle = -1;
            }
        }

        SDL_FillRect(surface, &erase_rect, COLOR_BLACK);
        FillCircle(surface, light_source_circle, COLOR_YELLOW);

        for (auto& circle : obstacles) {
            FillCircle(surface, circle, COLOR_RED);
        }

        FillRays(surface, ray_arr, COLOR_YELLOW, obstacles);

        SDL_UpdateWindowSurface(window);
        SDL_Delay(10);
    }

    SDL_Quit();
    return 0;
}