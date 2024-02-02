#pragma once

#include <raylib.h>
#include "utils.hpp"


// For more information:
// https://en.wikipedia.org/wiki/Phong_reflection_model
struct _Material {
    Vector3 albedo;                         // The base color for a material
    const float ambient_reflection;         // How bright the material is on the ambient side. Basically how bright it overall is
    const float diffuse_reflection;         // albedo 0 in raytracing series. How noticable the difuse shadows are
    const float specular_reflection;        // albedo 1 in raytracing series. How noticable the specular highlights are
    const float specular_exponent;          // How dense the specular highlights are. Higher values -> more dense
    const float reflective_constant;        // How much the object is like a mirror. 1 is a perfect mirror. 0.5 is a little blurred mirror
};

struct Light {
    const Vector3 position;
    const float specular_component; // Is float intensity in raytracing series. Acts the same as specular_reflection but it's global on all objects hit by this light
    const float diffuse_component;  // Is float intensity in raytracing series. Acts the same as diffuse_reflection but it's global on all objects hit by this light
};

struct Sphere {
    const Vector3 center;
    const float radius;
    const _Material material;

    inline Sphere(const Vector3& c, const float& r, const _Material& m) : center(c), radius(r), material(m) { }

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
