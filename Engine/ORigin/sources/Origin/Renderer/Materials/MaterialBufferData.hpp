#pragma once

#include "Origin/Core/Types.h"
namespace origin {

struct MeshMaterialBufferData
{
    f32 metallic_factor;
    f32 roughness_factor;
    glm::vec2 tiling_factor;
    glm::vec3 diffuse_color;
    glm::vec3 base_color;
};


}