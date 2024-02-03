#pragma once


#include <raylib.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <iomanip>
#include "defines.hpp"


////// VECTOR OPERATOR OVERLOADING //////
inline Vector3 operator + (const Vector3& v1, const Vector3& v2) {
    return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

inline void operator += (Vector3& v1, const Vector3& v2) {
    v1.x += v2.x; v1.y += v2.y; v1.z += v2.z;
}

inline Vector3 operator - (const Vector3& v1, const Vector3& v2) {
    return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

inline Vector3 operator * (const float& f, const Vector3& v1) {
    return { v1.x * f, v1.y * f, v1.z * f };
}

inline void operator *= (Vector3& v1, const float& f) {
    v1.x *= f; v1.y *= f; v1.z *= f;
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

    inline Vector3 minmax(const Vector3& v) {
        Vector3 c = v;
        c.x = std::min(std::max(v.x, 0.0f), 1.0f);
        c.y = std::min(std::max(v.y, 0.0f), 1.0f);
        c.z = std::min(std::max(v.z, 0.0f), 1.0f);

        return c;
    }

    inline Vector3 reflect(const Vector3& light_direction, const Vector3& hit_normal) {
        return utils::normalize(2 * utils::dot(light_direction, hit_normal) * hit_normal - light_direction);
    }

    inline void progress_bar(const float value, const float max_value, const int bar_length) {
        double percentage = static_cast<double>(value) / max_value;
        int pos = static_cast<int>(bar_length * percentage);

        std::cout << "[";
        for (int i = 0; i < bar_length; ++i) {
            if (i < pos) std::cout << "#";
            else std::cout << "-";
        }
        std::cout << "] " << std::fixed << std::setprecision(2) << percentage * 100.0 << "%\r";
        std::cout.flush();
    }

    inline float random_num(float min, float max) {
        double random = ((double) rand() / (RAND_MAX));
        double range = max - min;
        return (random * range + min);
    }

    inline Vector3 average_color(std::vector<Vector3> pixels) {
        Vector3 clr = {0, 0, 0};
        for (auto& p : pixels) clr += p / pixels.size();
        return clr;
    }

    // Anti aliasing
    // Flatten
    // Convert Vec3 to color
    inline T_COLOR adjust_pixels(const T_PIXEL& pixels, const int& downscale) {
        int new_width = static_cast<int>(pixels.front().size() / downscale);
        int new_height = static_cast<int>(pixels.size() / downscale);
        T_PIXEL AA_pixels(new_height, std::vector<Vector3>(new_width));

        std::vector<Vector3> averaging_pixels;
        for (int y = 0; y < new_height; y++) {
            for (int x = 0; x < new_width; x++) {
                averaging_pixels.clear();
                for (int i = 0; i < downscale; i++) {
                    for (int j = 0; j < downscale; j++) {
                        averaging_pixels.push_back(pixels[y * downscale + i][x * downscale + j]);
                    }
                }

                AA_pixels[y][x] = utils::average_color(averaging_pixels);
            }
        }

        T_COLOR adjusted_pixels;
        for (auto& row : AA_pixels) {
            for (auto& p : row) {
                adjusted_pixels.push_back(utils::vec_to_color(p));
            }
        }

        return adjusted_pixels;
    }
}
