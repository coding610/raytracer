#include <vector>
#include "render.hpp"
#include "objects.hpp"

// https://github.com/ssloy/tinyraytracer/wiki/Part-1:-understandable-raytracing
int main() {
    _Material light_brown = {{0.4, 0.4, 0.3}};
    _Material dark_brown  = {{0.3, 0.1, 0.1}};

    const std::vector<Light> lights = {
        {{-20, 20, 20}, 2},
    };

    const std::vector<Sphere> spheres = {
        {{-3, 0, -16},          2,      light_brown     },
        {{-1, -1.5, -12},       2,      dark_brown      },
        {{1.5, -0.5, -18},      2,      dark_brown      },
        {{7, 5, -18},           2,      light_brown     },
    };

    render(spheres, lights);
}
