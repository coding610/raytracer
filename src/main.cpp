#include <vector>
#include "render.hpp"
#include "objects.hpp"


int main () {
    const std::vector<Sphere> spheres = {
        {{-3, 0, -16},          2,      Color(102, 102, 77, 255)},
        {{-1, -1.5, -12},       2,      Color(77, 26, 26, 255)},
        {{1.5, -0.5, -18},      2,      Color(77, 26, 26, 255)},
        {{7, 5, -18},           2,      Color(102, 102, 77, 255)},
    };

    render(spheres);
}
