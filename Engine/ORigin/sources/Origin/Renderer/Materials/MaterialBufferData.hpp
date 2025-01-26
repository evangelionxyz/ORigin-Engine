#pragma once

#include "Origin/Core/Types.h"
namespace origin {

struct MeshMaterialBufferData
{
    glm::vec4 base_color;
    glm::vec4 tiling_factor{1.0f, 1.0f, 1.0f, 1.0f};
};

}