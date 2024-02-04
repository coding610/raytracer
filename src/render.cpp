#include <fstream>
#include <raylib.h>
#include <unistd.h>
#include <cstddef>
#include <cmath>
#include <limits>
#include <vector>
#include <cstdlib>
#include "camera.hpp"
#include "objects.hpp"
#include "defines.hpp"
#include "render.hpp"
#include "utils.hpp"

[[nodiscard]] T_PIXEL Renderer::render_scene() const {
    float x, y;

    T_PIXEL pixels;
    std::vector<Vector3> slice;
    for (int _y = 0; _y < _upsampled_height; _y++) {
        utils::progress_bar("Rendering scene", _y, _upsampled_height - 1, 50);
        slice.clear();
        for (int _x = 0; _x < _upsampled_width; _x++) {
            x =  (2.0f * _x / _upsampled_width  - 1) * std::tan(_camera->fov / 2.0f) * _displayed_ratio;
            y = -(2.0f * _y / _upsampled_height - 1) * std::tan(_camera->fov / 2.0f);

            slice.push_back(cast_ray({
                .position = _camera->position,
                .direction = utils::normalize({x, y, -1 / _camera->focal_length})
            }, 0 ));
        }
        pixels.push_back(slice);
    }

    return pixels;
}

[[nodiscard]] Texture2D Renderer::form_texture(T_COLOR colors, int width, int height) const {
    Image image;
    image.data = colors.data();
    image.width = static_cast<int>(width);
    image.height = static_cast<int>(height);
    image.mipmaps = 1;
    image.format =  PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

    return LoadTextureFromImage(image);
}

void Renderer::write_ppm(const std::string PATH, const T_PIXEL& pixels) const {
    std::ofstream ofs(PATH);
    ofs << "P3\n" << _displayed_width << " " << _displayed_height << "\n1\n";
    for (auto& row : pixels) {
        for (auto& p : row) {
            ofs << static_cast<int>(p.x) << " "
                << static_cast<int>(p.y) << " "
                << static_cast<int>(p.z) << "\n";
        }
    }
    ofs.close();
}

void Renderer::check_ppm(const std::string PATH) const {
    if (utils::file_exists(PATH)) { // ERROR
        std::cerr << "FATAL ERROR: Path {" << PATH << "} already exists\n";
        std::exit(0);
    }
}

void Renderer::render() {
    const std::string FILE_PATH = "history/image0.ppm";
    check_ppm(FILE_PATH);

    T_PIXEL pixels = render_scene();
    pixels = utils::upscale(pixels, _pixel_spacing);
    T_COLOR colors = utils::adjust_pixels(pixels, _SSAA_factor);
    Texture2D rendered_scene = form_texture(colors, pixels.front().size() / _SSAA_factor, pixels.size() / _SSAA_factor);

    write_ppm("history/image0.ppm", pixels);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            DrawTextureV(rendered_scene, {0, 0}, WHITE);
        EndDrawing();
    }

    UnloadTexture(rendered_scene);
}


Renderer::Renderer(
    const std::vector<Sphere>& sphs,
    const std::vector<Light>& lhts
) : _spheres(sphs), _lights(lhts)  {
    _camera = new _Camera {
        .position = {0, 0, 0},
        .focal_length = 0.6,
        .fov = PI / 2,
        .render_distance = 30,
        .scene_lighting = 1,
        .max_reflection_depth = 5
    };

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(_displayed_width, _displayed_height, "raytracer");
    SetTargetFPS(60);

    std::srand(static_cast<unsigned>(std::time(NULL)));
}
