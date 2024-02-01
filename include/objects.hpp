#pragma once

#include <raylib.h>
#include "utils.hpp"

struct _Material {
    Color color;
};


struct Sphere {
    const Vector3 center;
    const float radius;
    const Color color;

    inline Sphere(const Vector3& c, const float& r, const Color clr) : center(c), radius(r), color(clr) { }

    // http://www.lighthouse3d.com/tutorials/maths/ray-sphere-intersection/
    // ray_t is the mulitplier for the direction
    // ray = origin + ray_t * direction
    // t is basically intersection distance
    inline bool intersect(const Ray& ray, float& t) const {
        Vector3 vector_to_center  = center - ray.position;
        float direction_projection = utils::dot(vector_to_center, ray.direction);

        float discriminant  = utils::dot(vector_to_center, vector_to_center) - direction_projection * direction_projection;
        if (discriminant > radius * radius) return false;

        float half_cord_length = std::sqrt(radius*radius - discriminant);
        t = direction_projection - half_cord_length;
        
        if (t < 0) {
            t = direction_projection + half_cord_length;
            return false;
        } else {
            return true;
        }
    }
};
