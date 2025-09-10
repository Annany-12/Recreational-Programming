// Compiling Command for this specific Code: g++ anim_shapes.cpp -o anim_shapes.exe -I C:\mingw64_update\include -L C:\mingw64_update\lib -lSDL2main -lSDL2

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <cmath>
#include <vector>

// Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Base class for 3D shapes
class Shape3D {
public:
    virtual const std::vector<std::vector<float>>& getVertices() const = 0;
    virtual const std::vector<std::vector<int>>& getEdges() const = 0;
    virtual ~Shape3D() = default;
};

// Cube shape
class Cube : public Shape3D {
private:
    std::vector<std::vector<float>> vertices = {
        {-0.25, -0.25, -0.25}, {0.25, -0.25, -0.25}, {0.25, 0.25, -0.25}, {-0.25, 0.25, -0.25}, // Back face
        {-0.25, -0.25,  0.25}, {0.25, -0.25,  0.25}, {0.25, 0.25,  0.25}, {-0.25, 0.25,  0.25}  // Front face
    };
    std::vector<std::vector<int>> edges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, // Back face
        {4, 5}, {5, 6}, {6, 7}, {7, 4}, // Front face
        {0, 4}, {1, 5}, {2, 6}, {3, 7}  // Connecting edges
    };

public:
    const std::vector<std::vector<float>>& getVertices() const override { return vertices; }
    const std::vector<std::vector<int>>& getEdges() const override { return edges; }
};

// Pyramid shape
class Pyramid : public Shape3D {
private:
    std::vector<std::vector<float>> vertices = {
        {-0.25, -0.25, -0.25}, {0.25, -0.25, -0.25}, {0.25, 0.25, -0.25}, {-0.25, 0.25, -0.25}, // Base vertices
        {0.0, 0.0, 0.25} // Apex
    };
    std::vector<std::vector<int>> edges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, // Base edges
        {0, 4}, {1, 4}, {2, 4}, {3, 4}  // Sides
    };

public:
    const std::vector<std::vector<float>>& getVertices() const override { return vertices; }
    const std::vector<std::vector<int>>& getEdges() const override { return edges; }
};

// Octahedron shape
class Octahedron : public Shape3D {
private:
    std::vector<std::vector<float>> vertices = {
        {0.0, 1.0, 0.0},  // Top vertex
        {1.0, 0.0, 0.0},  // Base vertices
        {0.0, 0.0, 1.0},
        {-1.0, 0.0, 0.0},
        {0.0, 0.0, -1.0},
        {0.0, -1.0, 0.0}  // Bottom vertex
    };
    std::vector<std::vector<int>> edges = {
        {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, // Sides to top
        {1, 2}, {2, 3}, {3, 4}, {4, 1}, // Base edges
        {1, 5}, {2, 5}, {3, 5}, {4, 5}  // Sides to bottom
    };

public:
    const std::vector<std::vector<float>>& getVertices() const override { return vertices; }
    const std::vector<std::vector<int>>& getEdges() const override { return edges; }
};

class Toroid : public Shape3D {
private:
    std::vector<std::vector<float>> vertices;
    std::vector<std::vector<int>> edges;

public:
    Toroid(int numSegments = 36, int numRings = 18, float R = 0.4f, float r = 0.2f) {
        // Generate vertices
        for (int i = 0; i < numRings; ++i) {
            float theta = 2 * M_PI * i / numRings; // Angle around the toroid
            float cosTheta = cos(theta);
            float sinTheta = sin(theta);

            for (int j = 0; j < numSegments; ++j) {
                float phi = 2 * M_PI * j / numSegments; // Angle around the tube
                float cosPhi = cos(phi);
                float sinPhi = sin(phi);

                // Calculate 3D coordinates
                float x = (R + r * cosPhi) * cosTheta;
                float y = (R + r * cosPhi) * sinTheta;
                float z = r * sinPhi;

                vertices.push_back({x, y, z});
            }
        }

        // Generate edges
        for (int i = 0; i < numRings; ++i) {
            for (int j = 0; j < numSegments; ++j) {
                int current = i * numSegments + j;
                int nextInRing = i * numSegments + (j + 1) % numSegments;
                int nextRing = ((i + 1) % numRings) * numSegments + j;

                // Edges within the same ring
                edges.push_back({current, nextInRing});
                // Edges connecting to the next ring
                edges.push_back({current, nextRing});
            }
        }
    }

    const std::vector<std::vector<float>>& getVertices() const override { return vertices; }
    const std::vector<std::vector<int>>& getEdges() const override { return edges; }
};


// Add more shapes as needed...

// Function to project 3D points onto a 2D plane
void project(float x, float y, float z, int& screenX, int& screenY) {
    float distance = 2.0f;
    float factor = distance / (distance + z);
    screenX = static_cast<int>(x * factor * 200 + SCREEN_WIDTH / 2);
    screenY = static_cast<int>(-y * factor * 200 + SCREEN_HEIGHT / 2);
}

// Function to rotate a point in 3D space
void rotate(float& x, float& y, float& z, float angleX, float angleY) {
    // Rotation around the X-axis
    float tempY = y * cos(angleX) - z * sin(angleX);
    float tempZ = y * sin(angleX) + z * cos(angleX);
    y = tempY; z = tempZ;

    // Rotation around the Y-axis
    float tempX = x * cos(angleY) + z * sin(angleY);
    z = -x * sin(angleY) + z * cos(angleY);
    x = tempX;
}

// Function to render a 3D shape
void renderShape(SDL_Renderer* renderer, const Shape3D& shape, float angleX, float angleY) {
    const auto& vertices = shape.getVertices();
    const auto& edges = shape.getEdges();
    std::vector<std::vector<int>> projected(vertices.size(), std::vector<int>(2));

    // Rotate and project each vertex
    for (size_t i = 0; i < vertices.size(); ++i) {
        float x = vertices[i][0];
        float y = vertices[i][1];
        float z = vertices[i][2];

        rotate(x, y, z, angleX, angleY);
        project(x, y, z, projected[i][0], projected[i][1]);
    }

    // Draw the edges of the shape
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color
    for (const auto& edge : edges) {
        SDL_RenderDrawLine(renderer,
                           projected[edge[0]][0], projected[edge[0]][1],
                           projected[edge[1]][0], projected[edge[1]][1]);
    }
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("3D Shapes Viewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // List of shapes
    std::vector<Shape3D*> shapes = { 
        new Toroid(),
        new Cube(), 
        new Pyramid(), 
        new Octahedron()
    };
    size_t currentShapeIndex = 0;

    float angleX = 0.0f, angleY = 0.0f; // Rotation angles
    bool running = true;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                currentShapeIndex = (currentShapeIndex + 1) % shapes.size();
            }
        }

        // Update rotation angles
        angleX += 0.01f;
        angleY += 0.02f;

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render the current shape
        renderShape(renderer, *shapes[currentShapeIndex], angleX, angleY);

        // Present the renderer
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // Cap frame rate to ~60 FPS
    }

    // Cleanup
    for (auto shape : shapes) delete shape;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
