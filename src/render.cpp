#include "objects.hpp"
#include "utils.hpp"
#include <cmath>
#include <cstddef>
#include <limits>
#include <raylib.h>
#include <vector>

// I know this is bad practise
// but I do not feel like writing
// this type in every damn function parameter
#define T_PIXEL std::vector<std::vector<Vector3>>


Vector3 cast_ray(
    const Vector3& camera_position,
    const Ray& ray,
    const std::vector<Sphere>& spheres,
    const std::vector<Light>& lights
) {
    // Variables
    Vector3 ambient_color = {0.0, 0.0, 0.0}; // Background Color
    float intersect_distance = std::numeric_limits<float>::max();
    float dummy_distance;
    bool collided = false;
    Vector3 hit;
    Vector3 hit_direction;

    // Collision Spheres
    for (auto& sphere : spheres) {
        if (sphere.intersect(ray, dummy_distance)) {
            if (intersect_distance > dummy_distance) {
                intersect_distance = dummy_distance; collided = true;
                ambient_color = sphere.material.color;

                // Lighting
                hit = ray.position + intersect_distance * ray.direction;
                hit_direction = utils::normalize(hit - sphere.center);
            }
        }
    }

    // Lighting
    float diffuse_light_intensity = 1;
    if (collided) { diffuse_light_intensity = 0;
        for (auto& light : lights) {
            // When you take the dot product of two vectors a and b, denoted as a⋅ba⋅b,
            // the result is a scalar value equal to the product of the magnitudes of the
            // two vectors and the cosine of the angle between them. This scalar value
            // can be seen as a measure of "how much" of one vector lies in the direction
            // of the other vector.
            //
            // For example, if the dot product is positive, it means that the vectors are
            // pointing in roughly the same direction. If it's negative, they're pointing
            // in roughly opposite directions. And if it's zero, it means the vectors are
            // orthogonal (perpendicular) to each other.
            //
            // postive dot product -> same direction
            // negative dot product -> oppsite directions
            // 0 dot product -> perpendicular to each other
            //
            // So in our case, if a the dot product between the light_direction and
            // the hit direction is positive, the result should be a lit surface
            // because they are poitning in roughly the same direction
            // and otherwise, if it is below zero, nothing should happen because 
            // that surface is not lit, thus the std::max function

            Vector3 light_direction = utils::normalize(light.position - hit);
            diffuse_light_intensity += light.intensity * std::max(0.0f, utils::dot(light_direction, hit_direction));
        }
    }

    return utils::min(1, diffuse_light_intensity * ambient_color);
}

T_PIXEL render_scene(
    const Vector3& camera_position,
    const float& focal_length,
    const float& render_density,
    const float& fov,
    const std::vector<Sphere> spheres,
    const std::vector<Light> lights
) {
    // Constants
    const float width = GetRenderWidth();
    const float height = GetRenderHeight();

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

    return pixels;
}

void render_pixels(const T_PIXEL& pixels, const float& render_density, const bool& render_squares) {
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
    Vector3 camera_position = {0, 0, 0};
    const float focal_length = 0.5;
    const float fov = PI / 2;
    const float render_density = 1;
    const bool render_squares = true;

    // Pre-Render scene
    T_PIXEL pixels = render_scene(camera_position, focal_length, render_density, fov, spheres, lights);

    // Render Vector3s
    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            render_pixels(pixels, render_density, render_squares);
            DrawFPS(10, 10);
        EndDrawing();
    }
}
