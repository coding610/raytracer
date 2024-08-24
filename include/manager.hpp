#pragma once

#include <optional>
#include <vector>
#include "objects.hpp"
#include "camera.hpp"

class RayTracingManager {
public:
    RayTracingManager(const std::vector<Sphere>& spheres, const std::vector<Light>& lights);
    ~RayTracingManager();
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

    float _pixel_spacing = 1;                // Basically resolution of image
    float _SSAA_factor = 5;                  // How much to upsample original image. more -> more time and better image
    const float _displayed_ratio = 16.0 / 9.0;
    const int   _displayed_height = 1000;
    const int   _displayed_width  = _displayed_height * _displayed_ratio;
    const float _upsampled_width  = _SSAA_factor * _displayed_width / _pixel_spacing;
    const float _upsampled_height = _SSAA_factor * _displayed_height / _pixel_spacing;

    const std::string _FILE_PATH = "imgs/25aa.png";
    const bool _write_file = false;

    _Camera* _camera;
};
