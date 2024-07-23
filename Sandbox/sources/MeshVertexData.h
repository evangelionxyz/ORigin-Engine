#pragma once
#include <vector>
#include <glm/glm.hpp>

namespace origin
{
    struct MeshVertexData
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec4 Color;
        glm::vec2 TexCoord;
        float TexIndex;
        int EntityID;
    };

    struct MeshData
    {
        std::vector<MeshVertexData> vertices;
        std::vector<uint32_t> indices;
    };
}

