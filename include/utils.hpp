#pragma once


#include <raylib.h>
#include <iostream>
#include <math.h>


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
inline void DEB(T m) {
    std::cout << m << "\n";
}

inline void DEB(const Vector2& m) {
    std::cout << m.x << " " << m.y << "\n";
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
        return Color(v.x, v.y, v.z, 255);
    }

    inline Vector3 min(const Vector3& v) {
        Vector3 c = v;
        c.x = std::min(v.x, 1.0f);
        c.y = std::min(v.y, 1.0f);
        c.z = std::min(v.z, 1.0f);

        return c;
    }
}
