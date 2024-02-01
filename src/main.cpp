#include <raylib.h>
#include <unistd.h>
#include "render.hpp"
#include "sphere.hpp"


int main () {
    float ratio = 16.0 / 9.0;
    float height = 1000;
    float width = ratio * height;

    InitWindow(width, height, "");
    SetTargetFPS(60);

    const Sphere sphere = {{0, 0, -16}, 2, Color(255, 0, 0, 255)};

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            render(sphere);
        EndDrawing();
    }

    return 0;
}
