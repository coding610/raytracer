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
    [[nodiscard]] T_PIXEL         render_scene() const;
    [[nodiscard]] RenderTexture2D form_texture(const T_COLOR pixels, int w, int h) const;
    [[nodiscard]] Vector3         cast_ray(const Ray& ray, const int& reflection_depth) const;
    [[nodiscard]] bool            scene_intersect(const Ray& ray, const Sphere*& h_sphere, Vector3& hit) const;
                  void            check_image(const std::string PATH) const;

private:
    const std::vector<Light>& _lights;
    const std::vector<Sphere>& _spheres;

    const bool render_spaces = false; // TODO
    const float _pixel_spacing = 10;                // Basically resolution of image
    const float _SSAA_factor = 5;                  // How much to upsample original image. more -> more time and better image
    const float _displayed_ratio = 16.0 / 9.0;
    const int   _displayed_height = 1000;
    const int   _displayed_width  = _displayed_height * _displayed_ratio;
    const float _upsampled_width  = _SSAA_factor * _displayed_width / _pixel_spacing;
    const float _upsampled_height = _SSAA_factor * _displayed_height / _pixel_spacing;

    // Format
    // history/{description}_{generation}_{scene}.png
    // Read more at history/README.md
    const std::string _FILE_PATH = "history/antialiasing(5)-pixelation(10)_0_0.png";
    const bool _write_file = true;

    _Camera* _camera;
};
