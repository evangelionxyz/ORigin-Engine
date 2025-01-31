#pragma once

#include "Origin/Core/Types.h"
namespace origin {

struct MaterialBufferData
{
    glm::vec4 base_color;
    glm::vec4 diffuse_color;
    glm::vec4 specular_color;
    glm::vec2 tiling_factor{1.0f, 1.0f};
    float rougness = 1.0f;
    float emissive = 0.5f;
};

}