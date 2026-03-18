#pragma once

#include <glm/glm.hpp>
#include <algorithm>
#include <limits>

namespace Engine::Systems::BVHUtils {
    struct AABB {
        glm::vec3 min;
        glm::vec3 max;

        // ------------------------------------------------------------
        // Constructors
        // ------------------------------------------------------------

        // Invalid / empty AABB
        AABB()
            : min(std::numeric_limits<float>::infinity()),
            max(-std::numeric_limits<float>::infinity()) {}

        // From min/max
        AABB(const glm::vec3& min_, const glm::vec3& max_)
            : min(min_), max(max_) {}

        // From a single point
        explicit AABB(const glm::vec3& p)
            : min(p), max(p) {}

        // ------------------------------------------------------------
        // State checks
        // ------------------------------------------------------------

        inline bool isValid() const {
            return min.x <= max.x &&
                min.y <= max.y &&
                min.z <= max.z;
        }

        // ------------------------------------------------------------
        // Expansion
        // ------------------------------------------------------------

        // Expand to include a point
        inline void expand(const glm::vec3& p) {
            min = glm::min(min, p);
            max = glm::max(max, p);
        }

        // Expand to include another AABB
        inline void expand(const AABB& other) {
            min = glm::min(min, other.min);
            max = glm::max(max, other.max);
        }

        // ------------------------------------------------------------
        // Construction helpers
        // ------------------------------------------------------------

        // Build from triangle
        static AABB fromTriangle(const glm::vec3& a,
                                const glm::vec3& b,
                                const glm::vec3& c)
        {
            AABB box;
            box.expand(a);
            box.expand(b);
            box.expand(c);
            return box;
        }

        // Build from points array
        template<typename Iterator>
        static AABB fromPoints(Iterator begin, Iterator end) {
            AABB box;
            for (auto it = begin; it != end; ++it)
                box.expand(*it);
            return box;
        }

        // ------------------------------------------------------------
        // Geometry queries
        // ------------------------------------------------------------

        inline glm::vec3 center() const {
            return (min + max) * 0.5f;
        }

        inline glm::vec3 extent() const {
            return max - min;
        }

        inline float surfaceArea() const {
            glm::vec3 e = extent();
            return 2.0f * (e.x * e.y + e.y * e.z + e.z * e.x);
        }

        inline float volume() const {
            glm::vec3 e = extent();
            return e.x * e.y * e.z;
        }

        // Largest axis (0 = X, 1 = Y, 2 = Z)
        inline int maxExtentAxis() const {
            glm::vec3 e = extent();
            if (e.x > e.y && e.x > e.z) return 0;
            if (e.y > e.z) return 1;
            return 2;
        }

        // ------------------------------------------------------------
        // Containment / overlap
        // ------------------------------------------------------------

        inline bool contains(const glm::vec3& p) const {
            return p.x >= min.x && p.x <= max.x &&
                p.y >= min.y && p.y <= max.y &&
                p.z >= min.z && p.z <= max.z;
        }

        inline bool intersects(const AABB& other) const {
            return (min.x <= other.max.x && max.x >= other.min.x) &&
                (min.y <= other.max.y && max.y >= other.min.y) &&
                (min.z <= other.max.z && max.z >= other.min.z);
        }

        // ------------------------------------------------------------
        // Ray intersection (slab method)
        // ------------------------------------------------------------

        inline bool intersectRay(const glm::vec3& rayOrigin,
                                const glm::vec3& rayDir,
                                float tMin,
                                float tMax) const
        {
            for (int axis = 0; axis < 3; ++axis) {
                float invD = 1.0f / rayDir[axis];
                float t0 = (min[axis] - rayOrigin[axis]) * invD;
                float t1 = (max[axis] - rayOrigin[axis]) * invD;

                if (invD < 0.0f)
                    std::swap(t0, t1);

                tMin = t0 > tMin ? t0 : tMin;
                tMax = t1 < tMax ? t1 : tMax;

                if (tMax <= tMin)
                    return false;
            }
            return true;
        }
    };
}