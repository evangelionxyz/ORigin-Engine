// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include <glm/glm.hpp>

namespace origin
{
    struct AABB
    {
        glm::vec3 Min = glm::vec3(0.0f);
        glm::vec3 Max = glm::vec3(0.0f);

        AABB() = default;
        AABB(const glm::vec3 &min, const glm::vec3 &max)
            : Min(min), Max(max)
        {
        }

        AABB(const AABB &) = default;
        static AABB FromCenterAndSize(const glm::vec3 &center, const glm::vec3 &size)
        {
            glm::vec3 halfSize = size * 0.5f;
            return AABB(center - halfSize, center + halfSize);
        }
    };
}


