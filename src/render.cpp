#include "objects.hpp"
#include "utils.hpp"
#include <cmath>
#include <cstddef>
#include <limits>
#include <raylib.h>
#include <vector>

#define T_PIXEL std::vector<std::vector<Color>>


Color cast_ray(
    const Vector3& camera_position,
    const Ray& ray,
    const std::vector<Sphere>& spheres
) {
    Color color = Color(145, 179, 217, 255); // Background Color
    float dummy_distance;
    float intersect_distance = std::numeric_limits<float>::max();

    for (auto& sphere : spheres) {
        if (sphere.intersect(ray, dummy_distance)) {
            if (intersect_distance > dummy_distance) {
                intersect_distance = dummy_distance;
                color = sphere.color;
            }
        }
    }

    return color;
}

void render_pixels(const T_PIXEL pixels, const float render_density) {
    for (int y = 0; y < GetRenderHeight() / render_density; y++) {
        for (int x = 0; x < GetRenderWidth() / render_density; x++) {
            DrawPixel(x * render_density, y * render_density, pixels[y][x]);
            DrawRectangleRec(
                Rectangle(
                    x * render_density, y * render_density,
                    render_density, render_density
                ),
                pixels[y][x]
            );
        }
    }
}

void render(const std::vector<Sphere>& spheres) {
    // Camera
    const float ratio = 16.0 / 9.0;
    const float height = 1000;
    const float width = ratio * height;

    // Setup Raylib
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(width, height, "");
    SetTargetFPS(60);

    // Raytracing constants
    const Vector3 camera_position = {0, 0, 0};
    const float focal_length = 0.5;
    const float fov = PI / 2;
    const float render_density = 1;

    T_PIXEL pixels;
    for (int _y = 0; _y < (int) height / render_density; _y++) {
        std::vector<Color> slice;
        for (int _x = 0; _x < (int) width / render_density; _x++) {
            const float x =  (2 * (render_density * _x) / (float) width - 1) * tan(fov / 2.0) * width / (float) height;
            const float y = -(2 * (render_density * _y) / (float) height - 1) * tan(fov / 2.0);
            Ray r = {
                camera_position,
                utils::normalize({ x, y, -1 / focal_length })
            };

            slice.push_back(cast_ray(camera_position, r, spheres));
        } pixels.push_back(slice);
    }

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            render_pixels(pixels, render_density);
        EndDrawing();
    }
}
