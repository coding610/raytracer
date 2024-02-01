#include <vector>
#include "render.hpp"
#include "objects.hpp"

// https://github.com/ssloy/tinyraytracer/wiki/Part-1:-understandable-raytracing
int main() {
    _Material brown1 = {{0.4, 0.4, 0.3}};
    _Material brown2 = {{0.3, 0.1, 0.1}};

    const std::vector<Light> lights = {
        {{-20, 20, 20}, 3},
    };

    const std::vector<Sphere> spheres = {
        {{-3, 0, -16},          2,      brown1},
        {{-1, -1.5, -12},       2,      brown2},
        {{1.5, -0.5, -18},      2,      brown2},
        {{7, 5, -18},           2,      brown1},
    };

    render(spheres, lights);
}
