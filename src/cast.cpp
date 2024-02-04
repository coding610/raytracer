#include <cstdlib>
#include <limits>
#include <optional>
#include <raylib.h>
#include <utility>
#include <vector>
#include "render.hpp"
#include "objects.hpp"
#include "utils.hpp"

[[nodiscard]] bool Renderer::scene_intersect(const Ray& ray, const Sphere*& h_sphere, Vector3& hit) const {
    float intersect_distance = std::numeric_limits<float>::max();
    float dummy_distance;

    for (auto& sphere : _spheres) {
        if (sphere.intersect(ray, dummy_distance)) {
            if (intersect_distance > dummy_distance) {
                intersect_distance = dummy_distance;
                hit = ray.position + intersect_distance * ray.direction;
                h_sphere = &sphere;
            }
        }
    }

    return intersect_distance < _camera->render_distance;
}


[[nodiscard]] Vector3 Renderer::cast_ray(
    const Ray& ray,
    const int& reflection_depth
) const {
    // LERPING
    Vector3 color_from = {0.5, 0.7, 1.0};
    Vector3 color_to = {0.8, 0.8, 1.0};
    float lerp = 0.5 - ray.direction.y;

    Vector3 ambient_color = (1 - lerp) * color_from + lerp * color_to;
    Vector3 reflect_color = {0, 0, 0};

    Vector3 hit;              // This is the potential point of intesection
    const Sphere* hit_sphere; // This is the potential sphere of intersection
    if (!scene_intersect(ray, hit_sphere, hit)) return ambient_color;
    else ambient_color = hit_sphere->material.albedo;

    const Vector3 hit_normal = utils::normalize(hit - hit_sphere->center);                              // N^     (variables from wiki)
    const Vector3 viewing_direction = utils::normalize(_camera->position - hit);                        // V^     (variables from wiki)

    /// REFLECTION ///
    if (hit_sphere->material.scattering_constant != 0 && reflection_depth < _camera->max_reflection_depth) {
        Vector3 ray_direction = utils::normalize(utils::reflect(viewing_direction, hit_normal));
        Vector3 ray_origin = utils::dot(ray_direction, hit_normal) < 0
                             ? hit - 0.001 * hit_normal : hit + 0.001 * hit_normal;

        Vector3 rand3 = {utils::random_num(0, 1), utils::random_num(0, 1), utils::random_num(0, 1)}; rand3 *= (1 - hit_sphere->material.scattering_constant);
        ray_direction += utils::dot(ray_direction, hit_normal) < 0 ? ray_direction - rand3: ray_direction + rand3;
        ray_direction = utils::normalize(ray_direction);

        reflect_color = cast_ray({
                .position = ray_origin,
                .direction = ray_direction
            },
            reflection_depth + 1
        );
    }

    ////////////////// PHONG LIGHTING MODEL //////////////////
        /* https://en.wikipedia.org/wiki/Phong_reflection_model */
        /* Explenation in my words: */
        // When you take the dot product of two vectors a and b, denoted as a⋅ba⋅b,
        // the result is a scalar value equal to the product of the magnitudes of the
        // two vectors and the cosine of the angle between them. This scalar value
        // can be seen as a measure of "how much" of one vector lies in the direction
        // of the other vector.
        //
        // For example, if the dot product is positive, it means that the vectors are
        // pointing in roughly the same direction. If it's negative, they're pointing
        // in roughly opposite directions. And if it's zero, it means the vectors are
        // orthogonal (perpendicular) to each other.
        //
        // postive dot product -> same direction
        // negative dot product -> oppsite directions
        // 0 dot product -> perpendicular to each other
        //
        // So in our case, if a the dot product between the light_direction and
        // the hit normal is positive, the result should be a lit surface
        // because they are poitning in roughly the same direction
        // and otherwise, if it is below zero, nothing should happen because 
        // that surface is not lit, thus the std::max function
        //
        // The same logic can be used for the specular reflection. Just use the same
        // logic on the dotproduct as the reflection_direction. You can think of it as
        // shrinking down the entire circles normal lighting into a small point. And that
        // points size is detirmind by how large the exponent is. The higher the exponent,
        // the more concentration
        //
        // The same logic can be applied for anything using the dot product. Think
        // along these lines when ever you encounter a dot product in a geometry
        // aspect, and you will succeed in understanding it. there are some
        // technichalities to it tho, so have a read of that, and the different
        // ways to interpret a dot product
    ////////////////// PHONG LIGHTING MODEL //////////////////

    float diffuse_lighting_intensity  = (hit_sphere->material.ambient_reflection - 1) + (_camera->scene_lighting - 1);
    float specular_lighting_intensity = (hit_sphere->material.ambient_reflection - 1) + (_camera->scene_lighting - 1);
    for (auto& light : _lights) {
        float light_length = utils::length(light.position - hit);
        const Vector3 light_direction = utils::normalize(light.position - hit);                     // L_m^   (variables from wiki)
        const Vector3 reflection_direction = utils::reflect(light_direction, hit_normal);           // R_m^   (variables from wiki)
        
        ////// SHADOWS //////
        Vector3 shadow_origin = utils::dot(light_direction, hit_normal) < 0 ? hit - 1e-3 * hit_normal : hit + 1e-3 * hit_normal; // Is hit in shadow
        Ray shadow_ray = {shadow_origin, light_direction };
        const Sphere* tmps; Vector3 shadow_hit;
        if (scene_intersect(shadow_ray, tmps, shadow_hit) && utils::length(shadow_hit - shadow_origin) < light_length)
            continue;

        diffuse_lighting_intensity +=  hit_sphere->material.diffuse_reflection * light.diffuse_component * std::max(0.0f, utils::dot(light_direction, hit_normal));
        float pow = std::pow(std::max(0.0f, utils::dot(reflection_direction, viewing_direction)), hit_sphere->material.specular_exponent);
        specular_lighting_intensity += hit_sphere->material.specular_reflection * light.specular_component * pow;
    }

    return utils::vecminmax((diffuse_lighting_intensity + specular_lighting_intensity) * ambient_color + reflect_color);
}
