#include "objects.hpp"
#include "defines.hpp"
#include "utils.hpp"
#include <cmath>
#include <cstddef>
#include <limits>
#include <raylib.h>
#include <vector>


Vector3 cast_ray(
    const Vector3& camera_position,
    const Ray& ray,
    const std::vector<Sphere>& spheres,
    const std::vector<Light>& lights
) {
    // Background color
    Vector3 color_from = {0.5, 0.7, 1.0};
    Vector3 color_to = {0.8, 0.8, 1.0};
    float lerp = 0.5 - ray.direction.y;
    Vector3 ambient_color = (1 - lerp) * color_from + lerp * color_to;

    float intersect_distance = std::numeric_limits<float>::max();
    float dummy_distance;
    bool collided = false;
    const Sphere* sphere_hit;

    // Collision Spheres
    for (auto& sphere : spheres) {
        if (sphere.intersect(ray, dummy_distance)) {
            if (intersect_distance > dummy_distance) {
                intersect_distance = dummy_distance; collided = true;
                ambient_color = sphere.material.albedo;
                sphere_hit = &sphere;
            }
        }
    }

    // https://en.wikipedia.org/wiki/Phong_reflection_model
    // all of the logic is shown on the wiki page above
    float total_illumenation = 1;
    const float scene_lighting_constant = 1;
    if (collided) {
        // Scene lighting constant could is described in the wiki as ia,
        // It is basically how bright the overall scene will be
        // In other words, how much ambient lighting there is
        // Sometimes it can be defined by the total lighting in the scene

        total_illumenation = sphere_hit->material.ambient_reflection * scene_lighting_constant;
        for (auto& light : lights) {
            // My explenation of Difuse Reflection
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
                // the hit normal is positive, the result should be a lit surface
                // because they are poitning in roughly the same direction
                // and otherwise, if it is below zero, nothing should happen because 
                // that surface is not lit, thus the std::max function
                //
                // but now I use the phong reflection model, which is way more complex
                // So you just have to buy it. But in previous commits i did just use
                // diffuse lighting

            _Material material = sphere_hit->material;
            Vector3 hit = ray.position + intersect_distance * ray.direction;
            Vector3 hit_normal = utils::normalize(hit - sphere_hit->center);                      // N^
            Vector3 light_direction = utils::normalize(light.position - hit);                     // L_m^
            Vector3 viewing_direction = utils::normalize(camera_position - hit);                  // V^
            Vector3 reflection_direction = utils::calculate_norm_rd(light_direction, hit_normal); // R_m^

            // Difuse reflection
            total_illumenation += material.diffuse_reflection
                                  * utils::dot(light_direction, hit_normal)
                                  * light.diffuse_component;
            
            // Specular reflection
            // total_illumenation += material.specular_reflection
            //                       * std::pow(std::max(0.0f, utils::dot(reflection_direction, viewing_direction)), material.shininess)
            //                       * light.specular_component;
        }
    }

    return utils::max(0, utils::min(1, total_illumenation * ambient_color));
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
    const float width  = GetRenderWidth();
    const float height = GetRenderHeight();

    T_PIXEL pixels;
    std::vector<Vector3> slice;
    for (int _y = 0; _y < static_cast<int>(height / render_density); _y++) {
        slice.clear();
        for (int _x = 0; _x < static_cast<int>(width / render_density); _x++) {
            const float x =  (2 * (render_density * _x) / width - 1) * tan(fov / 2.0) * width / height;
            const float y = -(2 * (render_density * _y) / height - 1) * tan(fov / 2.0);
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

Texture2D create_texture(T_PIXEL pixels) {
    T_COLOR colors = utils::adjust_pixels(pixels);
    Image image;
    image.data = colors.data();
    image.width = static_cast<int>(pixels[0].size());
    image.height = static_cast<int>(pixels.size());
    image.mipmaps = 1;
    image.format =  PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

    return LoadTextureFromImage(image);
}

void render(const std::vector<Sphere>& spheres, const std::vector<Light>& lights) {
    // Camera
    const float ratio = 16.0 / 9.0;
    const int height = 1000;
    const int width = ratio * height;

    // Setup Raylib
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(width, height, "");
    SetTargetFPS(60);

    // Raytracing constants
    Vector3 camera_position = {0, 0, 0};
    const float focal_length = 1;
    const float fov = PI / 2;
    const float render_density = 1;
    const bool render_squares = true;

    // Pre-Render scene
    T_PIXEL pixels = render_scene(camera_position, focal_length, render_density, fov, spheres, lights);
    Texture2D texture = create_texture(pixels);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(texture, 0, 0, WHITE);
            DrawFPS(10, 10);
        EndDrawing();
    }

    UnloadTexture(texture);
}
