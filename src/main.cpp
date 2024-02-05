#include <cstdint>
#include <vector>
#include "render.hpp"
#include "objects.hpp"
#include "defines.hpp"


// For future me:
// When rendering images,
// do it by hand. do not automate it.


// https://github.com/ssloy/tinyraytracer/wiki/Part-1:-understandable-raytracing
int main() {
    _Material mirror = {
        .albedo = {1, 1, 1},
        .ambient_reflection = 1,
        .diffuse_reflection = 0.01,
        .specular_reflection = 10,
        .specular_exponent = 1425,
        .scattering_constant = 1 // No scattering
    }; _Material scratched_mirror = {
        .albedo = {1, 1, 1},
        .ambient_reflection = 1,
        .diffuse_reflection = 0.01,
        .specular_reflection = 10,
        .specular_exponent = 1425,
        .scattering_constant = 0.3 // a lil scattering
    }; _Material light_metallic_brown = {
        .albedo = {0.6, 0.6, 0.5},
        .ambient_reflection = 1,
        .diffuse_reflection = 0.55,
        .specular_reflection = 0.2,
        .specular_exponent = 50,
        .scattering_constant = 0
    }; _Material dark_matt_brown = {
        .albedo = {0.3, 0.1, 0.1},
        .ambient_reflection = 1,
        .diffuse_reflection = 0.9,
        .specular_reflection = 0.1,
        .specular_exponent = 10,
        .scattering_constant= 0
    };

    const std::vector<Light> lights {
        {{-20, 20, 20}, 1.5, 1.5},      // A lil to the left
        {{30, 20, 30}, 1.7, 1.7},       // A lil to the right
        {{30, 50, -25}, 1.3, 1.3}       // Light high up
    };

    const std::vector<Sphere> spheres = {
        {{-3.0, 0.0, -16.0  },          2,      light_metallic_brown    },
        {{-9.0, 6.0, -18.0  },          5,      light_metallic_brown    },
        {{-1.0, -1.5, -12.0 },          2,      dark_matt_brown         },
        {{1.5, -0.5, -18    },          3,      dark_matt_brown         },
        {{-7.0, -7.0, -14.0 },          4.5,    dark_matt_brown         },
        {{7.0, 5.0, -18.0   },          4,      mirror                  },
        {{9, -7.5, -18.0    },          6,      scratched_mirror         },
    };

    Renderer* renderer = new Renderer(spheres, lights);
    renderer->render();
    delete renderer;
}
