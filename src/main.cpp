#include <raylib.h>


int main () {
    float ratio = 16.0 / 9.0;
    float height = 1000;
    float width = ratio * height;
    InitWindow(width, height, "");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            DrawCircle(500, 500, 300, BLUE);
        EndDrawing();
    }

    return 0;
}
