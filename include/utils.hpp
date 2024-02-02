#pragma once


#include <raylib.h>
#include <iostream>
#include <math.h>
#include <vector>
#include "defines.hpp"


////// VECTOR OPERATOR OVERLOADING //////
inline Vector3 operator + (const Vector3& v1, const Vector3& v2) {
    return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

inline Vector3 operator - (const Vector3& v1, const Vector3& v2) {
    return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

inline Vector3 operator * (const float& f, const Vector3& v1) {
    return { v1.x * f, v1.y * f, v1.z * f };
}

inline Vector3 operator / (const Vector3& v1, const float& f) {
    return { v1.x / f, v1.y / f, v1.z / f };
}


////// DEBUG UTILS //////
template <typename T>
inline void DEB(T& m) {
    std::cout << m << "\n";
}

inline void DEB(const Vector3& m) {
    std::cout << m.x << " " << m.y << " " << m.z << "\n";
}
////// NAMESPACE UTILS //////
namespace utils {

    inline float dot(const Vector3& v1, const Vector3& v2) {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    inline float length(const Vector3& v) {
        return std::sqrt(utils::dot(v, v));
    }

    inline Vector3 project_onto_ray(const Ray& ray, const Vector3& point) {
        Vector3 line_vector = ray.direction - ray.position;
        float line_length_squared = utils::dot(line_vector, line_vector);
        float t = utils::dot(point - ray.position, line_vector) / line_length_squared;
        return ray.position + t * ray.direction;
    }

    inline Vector3 normalize(const Vector3& v) {
        return v / utils::length(v);
    }

    inline Color vec_to_color(const Vector3& v) {
        return Color(255 * v.x, 255 * v.y, 255 * v.z, 255);
    }

    inline Vector3 min(const float& f, const Vector3& v) {
        Vector3 c = v;
        c.x = std::min(v.x, f);
        c.y = std::min(v.y, f);
        c.z = std::min(v.z, f);

        return c;
    }

    inline Vector3 max(const float& f, const Vector3& v) {
        Vector3 c = v;
        c.x = std::max(v.x, f);
        c.y = std::max(v.y, f);
        c.z = std::max(v.z, f);

        return c;
    }

    // This function does the following
    // * Flatten the vector to a lol instead of a lolol
    // * Convert a Vector3 into Color
    inline T_COLOR adjust_pixels(T_PIXEL pixels) {
        std::vector<Color> colors;

        for (const auto& row : pixels) {
            for (const auto& p : row) {
                colors.push_back(utils::vec_to_color(p));
            }
        }

        return colors;
    }

    inline Vector3 calculate_norm_rd(Vector3 light_direction, Vector3 hit_normal) {
        return utils::normalize(2 * utils::dot(light_direction, hit_normal) * hit_normal - light_direction);
    }

}
