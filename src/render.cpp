#include "objects.hpp"
#include "utils.hpp"
#include <cmath>
#include <cstddef>
#include <limits>
#include <raylib.h>
#include <vector>

#define T_PIXEL std::vector<std::vector<Vector3>>


Vector3 cast_ray(
    const Vector3& camera_position,
    const Ray& ray,
    const std::vector<Sphere>& spheres,
    const std::vector<Light>& lights
) {
    // Variables
    Vector3 ambient_color = {0.2, 0.7, 0.8}; // Background Color
    float intersect_distance = std::numeric_limits<float>::max();
    float dummy_distance;
    bool collided = false;
    Vector3 hit;
    Vector3 normalized_edge;

    // Collision Spheres
    for (auto& sphere : spheres) {
        if (sphere.intersect(ray, dummy_distance)) {
            if (intersect_distance > dummy_distance) {
                intersect_distance = dummy_distance; collided = true;
                ambient_color = sphere.material.color;
                hit = ray.position + intersect_distance * ray.direction;
                normalized_edge = utils::normalize(hit - sphere.center);
            }
        }
    }

    // Lighting
    float diffuse_light_intensity = 1;
    if (collided) {
        diffuse_light_intensity = 0;
        for (auto& light : lights) {
            Vector3 light_dir = utils::normalize((light.position - hit));
            diffuse_light_intensity += light.intensity * std::max(0.0f, utils::dot(light_dir, normalized_edge));
        }
    }

    return utils::min(diffuse_light_intensity * ambient_color);
}

void render_pixels(const T_PIXEL pixels, const float render_density, const bool render_squares) {
    for (int y = 0; y < GetRenderHeight() / render_density; y++) {
        for (int x = 0; x < GetRenderWidth() / render_density; x++) {
            if (render_squares) {
                DrawRectangleRec(
                    Rectangle(x * render_density, y * render_density, render_density, render_density),
                    utils::vec_to_color(255 * pixels[y][x])
                );
            } else {
                DrawPixel(
                    x * render_density, y * render_density,
                    utils::vec_to_color(255 * pixels[y][x])
                );
            }
        }
    }
}

void render(const std::vector<Sphere>& spheres, const std::vector<Light>& lights) {
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
    const bool render_squares = true;

    // Calculate Pixels
    T_PIXEL pixels;
    for (int _y = 0; _y < (int) height / render_density; _y++) {
        std::vector<Vector3> slice;
        for (int _x = 0; _x < (int) width / render_density; _x++) {
            const float x =  (2 * (render_density * _x) / (float) width - 1) * tan(fov / 2.0) * width / (float) height;
            const float y = -(2 * (render_density * _y) / (float) height - 1) * tan(fov / 2.0);
            Ray r = {
                camera_position,
                utils::normalize({ x, y, -1 / focal_length })
            };

            slice.push_back(cast_ray(camera_position, r, spheres, lights));
        } pixels.push_back(slice);
    }

    // Render Vector3s
    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            render_pixels(pixels, render_density, render_squares);
        EndDrawing();
    }
}
