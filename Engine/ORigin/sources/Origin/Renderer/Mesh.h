// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef MESH_VERTEX_DATA_H
#define MESH_VERTEX_DATA_H

#include "Shader.h"
#include "Origin/Asset/Asset.h"
#include "Origin/Animation/ModelAnimation.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

namespace origin {

#define MAX_BONE_INFLUENCE 4

class VertexArray;
class VertexBuffer;
class Texture2D;

class MeshMaterial
{
    using TextureTypeMap = std::unordered_map<aiTextureType, Ref<Texture2D>>;
public:
    std::vector<TextureTypeMap> textures;
    Ref<Shader> shader;

    glm::vec3 diffuse_color;
    float metallic_factor;
    float roughness_factor;

    MeshMaterial(): metallic_factor(1.0f), roughness_factor(0.0f), diffuse_color(1.0f)
    {
    }
};

struct MeshVertexData
{
    glm::vec3 Position;
    glm::vec3 Normals;
    glm::vec3 Color;
    glm::vec2 UV;
    glm::vec2 TilingFactor;
    i32 IDs[MAX_BONE_INFLUENCE] = { -1, -1, -1, -1 };
    f32 Weights[MAX_BONE_INFLUENCE];
    f32 AlbedoIndex = 0.0f;
    f32 SpecularIndex = 0.0f;
};

struct BoneInfo
{
    i32 ID;
    glm::mat4 offset_matrix;
};

class Mesh
{
public:
    std::string name;
    std::vector<MeshVertexData> vertices;
    std::vector<u32> indices;
    Ref<VertexArray> vertex_array;
    Ref<VertexBuffer> vertex_buffer;
    MeshMaterial material;

    glm::mat4 transformation = glm::mat4(1.0f);

    std::vector<glm::mat4> final_bone_matrices;

    std::unordered_map<std::string, BoneInfo> bone_info_map;
    i32 bone_count = 0;
    bool is_active = true;
};

}

#endif