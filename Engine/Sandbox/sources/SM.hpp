#pragma once

#include "Origin/Core/Types.h"

#include <map>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>

#define MAX_NUM_BONES_PER_VERTEX (4)
#define INVALID_MATERIAL (0xFFFFFFFF)
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices)


enum VERTEX_LOCATION
{
    POSITION_LOCATION    = 0,
    TEXCOORD_LOCATION    = 1,
    NORMAL_LOCATION      = 2,
    BONE_ID_LOCATION     = 3,
    BONE_WEIGHT_LOCATION = 4
};

enum BUFFER_TYPE
{
    INDEX_BUFFER = 0,
    POS_VB       = 1,
    TEXCOORD_VB  = 2,
    NORMAL_VB    = 3,
    BONE_VB      = 4,
    NUM_BUFFERS  = 5
};

struct VertexBoneData
{
    VertexBoneData() = default;

    void AddBoneData(u32 bone_id, f32 weight)
    {
        for (u32 i = 0; i < std::size(BoneIDs); ++i)
        {
            if (Weights[i] == 0.0f)
            {
                BoneIDs[i] = bone_id;
                Weights[i] = weight;
                //printf("bone %d weight %f index %i\n", bone_id, weight, i);
                return;
            }
        }
    }

    u32 BoneIDs[MAX_NUM_BONES_PER_VERTEX] = { 0 };
    float Weights[MAX_NUM_BONES_PER_VERTEX] = { 0.0f };
};

struct BasicMeshEntry
{
    BasicMeshEntry()
        : num_indices(0), base_vertex(0),
        base_index(0), material_index(INVALID_MATERIAL)
    {
    }
    u32 num_indices;
    u32 base_vertex;
    u32 base_index;
    u32 material_index;
};

class SkinnedMesh
{
public:
    SkinnedMesh() = default;
    ~SkinnedMesh();

    bool LoadMesh(const std::string &filename);
    void Render();
    u32 NumBones() { return static_cast<u32>(m_BoneNameToIndexMap.size()); }

private:
    void Clear();
    bool InitFromScene(const aiScene *scene, const std::string &filename);
    void CountVerticesAndIndices(const aiScene *scene, u32 &num_vertices, u32 &num_indices);
    void ReserveSpace(u32 num_vertices, u32 num_indices);
    void InitAllMeshes(const aiScene *scene);
    void InitSingleMesh(u32 mesh_index, const aiMesh *mesh);
    bool InitMaterials(const aiScene *scene, const std::string &filename);
    void PopulateBuffers();
    void LoadTextures(const std::string &dir, const aiMaterial *material, i32 index);
    void LoadDiffuseTexture(const std::string &dir, const aiMaterial *material, i32 index);
    void LoadSpecularTexture(const std::string &dir, const aiMaterial *material, i32 index);
    void LoadColors(const aiMaterial *material, i32 index);
    void LoadMeshBones(u32 mesh_index, const aiMesh *mesh);
    void LoadSingleBone(u32 mesh_index, const aiBone *bone);
    i32 GetBoneId(const aiBone *bone);

    u32 m_VAO = 0;
    u32 m_Buffers[NUM_BUFFERS];

    glm::mat4 m_WorldTransform;

    std::vector<BasicMeshEntry> m_Meshes;

    std::vector<glm::vec3> m_Positions;
    std::vector<glm::vec3> m_Normals;
    std::vector<glm::vec2> m_TexCoords;
    std::vector<u32> m_Indices;
    std::vector<VertexBoneData> m_Bones;

    std::map<std::string, u32> m_BoneNameToIndexMap;

};