// Copyright (c) Evangelion Manuhutu | Origin Engine

#ifndef MESH_HPP
#define MESH_HPP

#include "MeshMaterial.hpp"
#include "Origin/Renderer/Shader.h"
#include "Origin/Asset/Asset.h"
#include "Origin/Animation/ModelAnimation.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace origin {

class VertexArray;
class VertexBuffer;
class Texture2D;

struct MeshVertexData
{
    glm::vec3 position;
    glm::vec3 normals;
    glm::vec3 color;
    glm::vec2 texcoord;
    VertexBoneData bone;
};

struct MeshEntry
{
    MeshEntry()
        : num_indices(0),
        base_vertex(0),
        base_index(0)
    {
    }

    u32 num_indices;
    u32 base_vertex;
    u32 base_index;
};

class Mesh
{
public:
    bool is_active = true;

    std::string name;
    MeshMaterial material;
    MeshEntry mesh_entry;

    Ref<VertexArray> vertex_array;
    Ref<VertexBuffer> vertex_buffer;

    std::vector<MeshVertexData> vertices;
    std::vector<u32> indices;
    std::vector<glm::mat4> bone_transforms;

    glm::mat4 transform = glm::mat4(1.0f);
};

}

#endif