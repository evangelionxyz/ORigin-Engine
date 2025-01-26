// Copyright (c) Evangelion Manuhutu | Origin Engine

#ifndef MESH_HPP
#define MESH_HPP

#include "Origin/Renderer/Materials/Material.hpp"
#include "Origin/Renderer/Shader.h"
#include "Origin/Asset/Asset.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace origin {

class VertexArray;
class VertexBuffer;
class Texture2D;

#define MAX_NUM_BONES_PER_VERTEX (4)

struct VertexBoneData
{
    VertexBoneData() = default;

    void AddBoneData(u32 bone_id, f32 weight)
    {
        for (u32 i = 0; i < std::size(bone_ids); ++i)
        {
            if (weights[i] == 0.0f)
            {
                bone_ids[i] = bone_id;
                weights[i] = weight;
                return;
            }
        }
    }

    u32 bone_ids[MAX_NUM_BONES_PER_VERTEX] = { 0 };
    f32 weights[MAX_NUM_BONES_PER_VERTEX] = { 0.0f };
};

struct MeshVertexData
{
    glm::vec3 position;
    glm::vec3 normals;
    glm::vec3 color;
    glm::vec2 texcoord;
    VertexBoneData bone;
};

class Mesh
{
public:
    std::string name;
    MeshMaterial material;
    i32 material_index = -1;
    Ref<VertexArray> vertex_array;
    Ref<VertexBuffer> vertex_buffer;

    std::vector<MeshVertexData> vertices;
    std::vector<u32> indices;
    std::vector<glm::mat4> bone_transforms;

    glm::mat4 transform = glm::mat4(1.0f);
};

}

#endif