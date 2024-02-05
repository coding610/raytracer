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

[[nodiscard]] RenderTexture2D Renderer::form_texture(T_COLOR colors, int width, int height) const {
    Image image;
    image.data = colors.data();
    image.width = static_cast<int>(width);
    image.height = static_cast<int>(height);
    image.mipmaps = 1;
    image.format =  PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

    Texture2D texture = LoadTextureFromImage(image);
    RenderTexture2D target = LoadRenderTexture(texture.width * _pixel_spacing, texture.height * _pixel_spacing);

    BeginTextureMode(target);
        ClearBackground(BLACK);
        DrawTextureEx(texture, {0, 0}, 0.0f, _pixel_spacing, WHITE);
    EndTextureMode();

    // https://www.reddit.com/r/raylib/comments/14lk8fx/texture_is_rendering_coordinates_with_flipped_y/
    // Basically, the texture is mirrored because of opengls origo being in the bottom left
    // The solution is that I render the texture once more with a normal DrawTextureV,
    // and bada bing bada boom, it works. But thats becuase were basically flipping the image upsidedown twice...
    RenderTexture2D target_unflipped = LoadRenderTexture(target.texture.width, target.texture.height);

    BeginTextureMode(target_unflipped);
        ClearBackground(BLACK);
        DrawTextureV(target.texture, {0, 0}, WHITE);
    EndTextureMode();

    // Cleanup
    UnloadTexture(texture);
    UnloadRenderTexture(target);

    return target_unflipped;
}

void Renderer::check_image(const std::string PATH) const {
    if (utils::file_exists(PATH)) {
        std::cerr << "Error: Path {" << PATH << "} already exists\n";
        std::exit(0);
    }
}

void Renderer::render() {
    if (_write_file) check_image(_FILE_PATH);

    T_PIXEL pixels = render_scene();
    T_COLOR colors = utils::adjust_pixels(pixels, _SSAA_factor);
    Texture2D rendered_scene = form_texture(colors, pixels.front().size() / _SSAA_factor, pixels.size() / _SSAA_factor).texture;

    if (_write_file) {
        Image image = LoadImageFromTexture(rendered_scene);
        ExportImage(image, _FILE_PATH.c_str());
        UnloadImage(image);
    }

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
