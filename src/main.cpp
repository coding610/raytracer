#include <cstdint>
#include <vector>
#include "render.hpp"
#include "objects.hpp"
#include "defines.hpp"

// https://github.com/ssloy/tinyraytracer/wiki/Part-1:-understandable-raytracing
int main() {
    _Material light_metallic_brown = {
        .albedo = {0.6, 0.6, 0.5},
        .ambient_reflection = 1,
        .diffuse_reflection = 0.6,
        .specular_reflection = 0.3,
        .specular_exponent = 80
    };

    _Material dark_matt_brown = {
        .albedo = {0.3, 0.1, 0.1},
        .ambient_reflection = 1,
        .diffuse_reflection = 0.9,
        .specular_reflection = 0.1,
        .specular_exponent = 50
    };

    const std::vector<Light> lights {
        {{-20, 20, 20}, 1.5, 1.5},
        {{30, 20, 30}, 1.7, 1.7}
    };

    const std::vector<Sphere> spheres = {
        {{-3, 0, -16},          2,      light_metallic_brown    },
        {{-1, -1.5, -12},       2,      dark_matt_brown         },
        {{1.5, -0.5, -18},      3,      dark_matt_brown         },
        {{7, 5, -18},           4,      light_metallic_brown    },
    };

    render(spheres, lights);
}
