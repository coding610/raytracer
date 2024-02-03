#pragma once

#include <raylib.h>
#include <vector>
#include "objects.hpp"
#include "utils.hpp"


struct _Camera {
    const Vector3 position;
    const float focal_length;
    const float fov;
    const float render_distance;
    const float scene_lighting;         // Basically the global lighting
    const float max_reflection_depth;
};
