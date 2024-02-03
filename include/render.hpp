#pragma once

#include <optional>
#include <vector>
#include "objects.hpp"
#include "camera.hpp"

class Renderer {
public:
    Renderer(const std::vector<Sphere>& spheres, const std::vector<Light>& lights);
    void render();

private:
    [[nodiscard]] Texture2D render_scene() const;
    [[nodiscard]] Texture2D form_texture(const T_COLOR pixels, int w, int h) const;
    [[nodiscard]] Vector3   cast_ray(const Ray& ray, const int& reflection_depth) const;
    [[nodiscard]] bool      scene_intersect(const Ray& ray, const Sphere*& h_sphere, Vector3& hit) const;

private:
    const std::vector<Light>& _lights;
    const std::vector<Sphere>& _spheres;
    const float _pixel_spacing = 10;                 // Resolution

    const float _AA_factor = 5;                     // How much to oversample original image. more -> more time and better image
    const float _width  = _AA_factor * 1000 * 16.0 / 9.0;
    const float _height = _AA_factor * 1000;
    const float _displayed_width  = 1000 * 16.0 / 9.0;
    const float _displayed_height = 1000;

    _Camera* _camera;
};
