#include <cstdint>
#include <vector>
#include "render.hpp"
#include "objects.hpp"
#include "defines.hpp"

// https://github.com/ssloy/tinyraytracer/wiki/Part-1:-understandable-raytracing
int main() {
    _Material mirror = {
        .albedo = {1, 1, 1},
        .ambient_reflection = 0,
        .diffuse_reflection = 0,
        .specular_reflection = 0,
        .specular_exponent = 0,
        .reflective_constant = 1
    };

    _Material light_metallic_brown = {
        .albedo = {0.6, 0.6, 0.5},
        .ambient_reflection = 1,
        .diffuse_reflection = 0.6,
        .specular_reflection = 0.3,
        .specular_exponent = 80,
        .reflective_constant = 0
    };

    _Material dark_matt_brown = {
        .albedo = {0.3, 0.1, 0.1},
        .ambient_reflection = 1,
        .diffuse_reflection = 0.9,
        .specular_reflection = 0.1,
        .specular_exponent = 50,
        .reflective_constant = 0
    };

    const std::vector<Light> lights {
        {{-20, 20, 20}, 1.5, 1.5},
        {{30, 20, 30}, 1.7, 1.7}
    };

    const std::vector<Sphere> spheres = {
        {{-3, 0, -16},          2,      light_metallic_brown    },
        {{-1, -1.5, -12},       2,      mirror                  },
        {{1.5, -0.5, -18},      3,      dark_matt_brown         },
        {{7, 10, -18},          4,      mirror                  },
        {{15, 3, -18},          4,      mirror                  },
    };

    render(spheres, lights);
}
