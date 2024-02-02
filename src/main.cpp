#include <cstdint>
#include <vector>
#include "render.hpp"
#include "objects.hpp"
#include "defines.hpp"

// https://github.com/ssloy/tinyraytracer/wiki/Part-1:-understandable-raytracing
int main() {
    _Material light_brown = {
        .albedo = {0.6, 0.6, 0.5},
        .ambient_reflection = 1,
        .specular_reflection = 1,
        .diffuse_reflection = 1,
        .shininess = 1
    };

    _Material dark_brown = {
        .albedo = {0.2, 0.05, 0.05},
        .ambient_reflection = 1,
        .specular_reflection = 1,
        .diffuse_reflection = 1,
        .shininess = 1
    };

    // lights.push_back(Light(Vec3f( 30, 50, -25), 1.8));
    // lights.push_back(Light(Vec3f( 30, 20,  30), 1.7));
    const std::vector<Light> lights = {{
        .position = {-20, 20, 20},
        .specular_component = 1,
        .diffuse_component = 1
    }};

    const std::vector<Sphere> spheres = {
        {{-3, 0, -16},          2,      light_brown     },
        {{-1, -1.5, -12},       2,      dark_brown      },
        {{1.5, -0.5, -18},      3,      dark_brown      },
        {{7, 5, -18},           4,      light_brown     },
    };

    render(spheres, lights);
}
