#include <raylib.h>
#include <unistd.h>
#include <cstddef>
#include <cmath>
#include <limits>
#include <vector>
#include "objects.hpp"
#include "defines.hpp"
#include "utils.hpp"

// RAYTRACING CONSTANTS
#define SCENE_LIGHTING 1
#define MAX_REFLECTION_DEPTH 7


[[nodiscard]] Vector3 cast_ray(
    const Vector3& camera_position,
    const Ray& ray,
    const std::vector<Sphere>& spheres,
    const std::vector<Light>& lights,
    const int& reflection_depth
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

    // Collision with Spheres
    for (auto& sphere : spheres) {
        if (sphere.intersect(ray, dummy_distance)) {
            if (intersect_distance > dummy_distance) {
                intersect_distance = dummy_distance; collided = true;
                ambient_color = sphere.material.albedo;
                sphere_hit = &sphere;
            }
        }
    }

    // Convientent constants
    const Vector3 hit = ray.position + intersect_distance * ray.direction;
    const Vector3 hit_normal = utils::normalize(hit - sphere_hit->center);                      // N^     (variables from wiki)
    const Vector3 viewing_direction = utils::normalize(camera_position - hit);                  // V^     (variables from wiki)

    if (collided && sphere_hit->material.reflective_constant != 0) {
        if (reflection_depth < MAX_REFLECTION_DEPTH) [[likely]] {
            // We have to offset the origin point of the ray to avoid
            // colliding with the object it self

            ambient_color = cast_ray(
                camera_position, {
                    hit + 0.01 * hit_normal,
                    utils::reflect(viewing_direction, hit_normal)
                },
                spheres,
                lights,
                reflection_depth + 1
            );
        }
    }

        // https://en.wikipedia.org/wiki/Phong_reflection_model
        // all of the logic is shown on the wiki page above
        // This code below is simplified and is provided by raytracing series
        // If you want to expand it, see the wiki page or previous commits
        // Variables or explenations removed will be marked as deprecated
        // (For me) NOTE: 
        //      * material.{diffuse or specular}_reflection * light.{diffuse or specular}_component
        //      * is viewed in the raytracing series as one variable: "light.intensity"
        //      * but not really, as in the return statement, they multiply by albedo[0] and [1]
        //      * which is basically a mateiral.{d and s}_reflection
        //      * but light.{d or s}_component is not to be found (its just light.intensity)
    if (collided && sphere_hit->material.reflective_constant == 0) {
            // Scene lighting is basically global lighting
            // The same as ambient_reflection but global
        float diffuse_lighting_intensity  = (sphere_hit->material.ambient_reflection - 1) + (SCENE_LIGHTING - 1);
        float specular_lighting_intensity = (sphere_hit->material.ambient_reflection - 1) + (SCENE_LIGHTING - 1);
        for (auto& light : lights) {
                // My explenation of Difuse Reflection and Specular Reflection
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
                // The same logic can be used for the specular reflection. Just use the same
                // logic on the dotproduct as the reflection_direction. You can think of it as
                // shrinking down the entire circles normal lighting into a small point. And that
                // points size is detirmind by how large the exponent is. The higher the exponent,
                // the more concentration

            // Light dependent directions
            const Vector3 light_direction = utils::normalize(light.position - hit);                     // L_m^   (variables from wiki)
            const Vector3 reflection_direction = utils::reflect(light_direction, hit_normal);           // R_m^   (variables from wiki)

            // Difuse lighting
            diffuse_lighting_intensity += sphere_hit->material.diffuse_reflection * light.diffuse_component
                                          * std::max(0.0f, utils::dot(light_direction, hit_normal));
            
            // Specular lighting
            specular_lighting_intensity += sphere_hit->material.specular_reflection * light.specular_component
                                           * std::pow(std::max(0.0f, utils::dot(reflection_direction, viewing_direction)), sphere_hit->material.specular_exponent);
        }

        return utils::minmax((diffuse_lighting_intensity + specular_lighting_intensity) * ambient_color);
    } else {
        return ambient_color;
    }
}

[[nodiscard]] T_PIXEL render_scene(
    const Vector3& camera_position,
    const float& focal_length,
    const float& render_density,
    const float& fov,
    const std::vector<Sphere> spheres,
    const std::vector<Light> lights
) {
    const float width  = GetRenderWidth();
    const float height = GetRenderHeight();

    T_PIXEL pixels;
    std::vector<Vector3> slice;
    for (int _y = 0; _y < static_cast<int>(height / render_density); _y++) {
        utils::progress_bar(_y, static_cast<int>(height / render_density), 50);

        slice.clear();
        for (int _x = 0; _x < static_cast<int>(width / render_density); _x++) {
            const float x =  (2 * (render_density * _x) / width - 1) * tan(fov / 2.0) * width / height;
            const float y = -(2 * (render_density * _y) / height - 1) * tan(fov / 2.0);
            Ray r = {
                camera_position,
                utils::normalize({ x, y, -1 / focal_length })
            };

            slice.push_back(cast_ray(camera_position, r, spheres, lights, 0));
        }
        pixels.push_back(slice);
    }

    return pixels;
}

[[nodiscard]] Texture2D create_texture(T_PIXEL pixels) {
    T_COLOR colors = utils::adjust_pixels(pixels);
    Image image;
    image.data = colors.data();
    image.width = static_cast<int>(pixels[0].size());
    image.height = static_cast<int>(pixels.size());
    image.mipmaps = 1;
    image.format =  PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

    return LoadTextureFromImage(image);
}

void render(
    const std::vector<Sphere>& spheres,
    const std::vector<Light>& lights
) {
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
    const float focal_length = 0.6;
    const float fov = PI / 2;
    const float render_density = 1;

    // Pre-Render scene
    T_PIXEL pixels = render_scene(camera_position, focal_length, render_density, fov, spheres, lights);
    Texture2D texture = create_texture(pixels);

    bool movement = false;
    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_LEFT))         { camera_position.x -= 0.1; movement = true; }
        if (IsKeyDown(KEY_RIGHT))        { camera_position.x += 0.1; movement = true; }
        if (IsKeyDown(KEY_UP))           { camera_position.z -= 0.1; movement = true; }
        if (IsKeyDown(KEY_DOWN))         { camera_position.z += 0.1; movement = true; }
        if (IsKeyDown(KEY_SPACE))        { camera_position.y += 0.1; movement = true; }
        if (IsKeyDown(KEY_LEFT_CONTROL)) { camera_position.y -= 0.1; movement = true; }
        if (movement) {
            pixels = render_scene(camera_position, focal_length, render_density, fov, spheres, lights);
            texture = create_texture(pixels);
            movement = false;
        }

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro( texture,
                Rectangle(
                    0, 0,
                    width / render_density, height / render_density
                ), Rectangle(
                    0, 0, width, height
                ), {0, 0}, 0, WHITE
            );

        EndDrawing();
    }

    UnloadTexture(texture);
}
