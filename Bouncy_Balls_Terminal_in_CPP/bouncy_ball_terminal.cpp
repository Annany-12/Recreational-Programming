#include <iostream>
#include <unistd.h>
#include <cmath>
#include <vector>
#define WIDTH 30
#define HEIGHT 80
using namespace std;

struct Circle {
    double x;
    double y;
    double r;
    double v_x;
    double v_y;
};

void clear_screen() {
    cout << "\033[H";
}

void anim(const vector<Circle>& circles) {
    char frame[WIDTH][HEIGHT];

    // Initialize the frame with background (dots)
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            frame[x][y] = '.';
        }
    }

    // Update frame for each circle
    for (const auto& circle : circles) {
        for (int x = 0; x < WIDTH; x++) {
            for (int y = 0; y < HEIGHT; y++) {
                double dist = sqrt(pow(x - circle.x, 2) + pow(y - circle.y, 2));
                if (dist < circle.r) {
                    frame[x][y] = '*';
                }
            }
        }
    }

    // Render the frame
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            cout << frame[x][y];
        }
        cout << "\n";
    }
}

// Update the position of a single circle
void step(struct Circle* circle) {
    circle->x += circle->v_x;
    circle->y += circle->v_y;

    if (circle->x - circle->r < 0) {
        circle->v_x = -circle->v_x;
        circle->x = circle->r;
    }
    if (circle->x + circle->r > WIDTH) {
        circle->v_x = -circle->v_x;
        circle->x = WIDTH - circle->r;
    }
    if (circle->y - circle->r < 0) {
        circle->v_y = -circle->v_y;
        circle->y = circle->r;
    }
    if (circle->y + circle->r > HEIGHT) {
        circle->v_y = -circle->v_y;
        circle->y = HEIGHT - circle->r;
    }
}

int main() {
    // Multiple circle definations
    vector<Circle> circles = {
        {10, 10, 5, 3, 6},
        {5, 5, 3, 1, 3},
        {15, 25, 4, -2, 4}
    };
    cout << "\033[2J"; // Clearing Terminal Screen

    while (true) {
        cout << "Bouncy Ball Terminal Animation\n";
        clear_screen();
        for (auto& circle : circles) {
            step(&circle);
        }
        anim(circles);
        usleep(100000);
    }

    return 0;
}
