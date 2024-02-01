#include "render.hpp"
#include "utils.hpp"
#include <cmath>
#include <cstddef>
#include <limits>
#include <raylib.h>


Color cast_ray(const Vector3& camera_position, const Ray& ray, const Sphere& sphere) {
    float intersection_distance = std::numeric_limits<float>::max();
    if (sphere.intersect(ray, intersection_distance)) {
        return sphere.color;
    } else {
        return Color(145, 179, 217, 255);
    }
}

void render(const Sphere& sphere) {
    Vector3 camera_position = {0, 0, 0};
    float width = GetRenderWidth();
    float height = GetRenderHeight();
    float focal_length = 1;
    float fov = PI / 2;
    float render_density = 1;

    for (int _y = 0; _y < height / render_density; _y++) {
        for (int _x = 0; _x < width / render_density; _x++) {
            float x =  (2 * _x / (float) width - 1) * tan(fov / 2.0) * width / (float) height;
            float y = -(2 * _y / (float) height - 1) * tan(fov / 2.0);

            Ray r = {
                camera_position, utils::normalize({
                    x,
                    y,
                    1 / -focal_length
                })
            };

            DrawPixel(_x, _y, cast_ray(camera_position, r, sphere));
        }
    }
}
