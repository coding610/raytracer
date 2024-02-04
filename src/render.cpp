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

[[nodiscard]] Texture2D Renderer::render_scene() const {
    float x, y;

    T_PIXEL pixels;
    std::vector<Vector3> slice;
    for (int _y = 0; _y < static_cast<int>(_upsampled_height / _pixel_spacing); _y++) {
        utils::progress_bar("Rendering scene", _y, static_cast<int>(_upsampled_height / _pixel_spacing) - 1, 50);
        slice.clear();
        for (int _x = 0; _x < static_cast<int>(_upsampled_width / _pixel_spacing); _x++) {
            x =  (2 * (_pixel_spacing * _x) / _upsampled_width - 1 ) * std::tan(_camera->fov / 2.0) * _upsampled_width / _upsampled_height;
            y = -(2 * (_pixel_spacing * _y) / _upsampled_height - 1) * std::tan(_camera->fov / 2.0);
            slice.push_back(cast_ray({
                _camera->position,
                utils::normalize({x, y, -1 / _camera->focal_length})
            }, 0 ));
        }
        pixels.push_back(slice);
    }

    T_COLOR adjusted_pixels = utils::adjust_pixels(pixels, _SSAA_factor);
    return form_texture(adjusted_pixels, _displayed_width / _pixel_spacing, _displayed_height / _pixel_spacing);
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

void Renderer::render() {
    // Pre-Render scene
    Texture2D rendered_scene = render_scene();

    while (!WindowShouldClose()) {
        // if (IsKeyDown(KEY_LEFT))         { _camera->position.x -= 0.1; moved = true;                    }
        // if (IsKeyDown(KEY_RIGHT))        { _camera->position.x += 0.1; moved = true;                    }
        // if (IsKeyDown(KEY_UP))           { _camera->position.z -= 0.1; moved = true;                    }
        // if (IsKeyDown(KEY_DOWN))         { _camera->position.z += 0.1; moved = true;                    }
        // if (IsKeyDown(KEY_SPACE))        { _camera->position.y += 0.1; moved = true;                    }
        // if (IsKeyDown(KEY_LEFT_CONTROL)) { _camera->position.y -= 0.1; moved = true;                    }
        // if (IsKeyPressed(KEY_ENTER))     { _pixel_spacing = _pixel_spacing == 1 ? 10 : 1; moved = true; }
        // if (moved) { moved = false; rendered_scene = render_scene(); }

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(
                rendered_scene,
                Rectangle( 0, 0, _upsampled_width / _pixel_spacing, _upsampled_height / _pixel_spacing),
                Rectangle( 0, 0, _upsampled_width, _upsampled_height),
                {0, 0}, 0, WHITE
            );
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
    InitWindow(_displayed_width, _displayed_height, "");
    SetTargetFPS(60);

    std::srand(static_cast<unsigned>(std::time(NULL)));
}
