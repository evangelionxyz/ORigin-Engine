#pragma once

#include "Origin/Core/Types.h"
#include "Origin/Renderer/Buffer.h"
#include "Origin/Renderer/VertexArray.h"

#include <map>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>

#define MAX_NUM_BONES_PER_VERTEX (4)
#define INVALID_MATERIAL (0xFFFFFFFF)
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices)

struct VertexBoneDataA
{
    VertexBoneDataA() = default;

    void AddBoneData(u32 bone_id, f32 weight)
    {
        for (u32 i = 0; i < std::size(BoneIDs); ++i)
        {
            if (Weights[i] == 0.0f)
            {
                BoneIDs[i] = bone_id;
                Weights[i] = weight;
                return;
            }
        }
    }

    u32 BoneIDs[MAX_NUM_BONES_PER_VERTEX] = { 0 };
    f32 Weights[MAX_NUM_BONES_PER_VERTEX] = { 0.0f };
};

struct BoneInfoA
{
    glm::mat4 offset_matrix;
    glm::mat4 final_transformation;

    BoneInfoA(const glm::mat4 &offset)
    {
        offset_matrix = offset;
        final_transformation = glm::mat4(0.0f);
    }
};

struct NodeInfo
{
    NodeInfo() = default;
    NodeInfo(const aiNode *n)
        : node(n)
    {
    }

    const aiNode *node = nullptr;
    bool is_requiered = false;
};

struct MeshEntryA
{
    MeshEntryA()
        : num_indices(0), base_vertex(0),
        base_index(0), material_index(INVALID_MATERIAL)
    {
    }
    u32 num_indices;
    u32 base_vertex;
    u32 base_index;
    u32 material_index;
};

struct SkinnedVertex
{
    glm::vec3 position;
    glm::vec2 texcoord;
    glm::vec3 normal;
    MeshEntryA bone;
};

struct SkinnedMeshData
{
    std::vector<SkinnedVertex> vertices;
    Ref<origin::VertexArray> vertex_array;
    Ref<origin::VertexBuffer> vertex_buffer;
    MeshEntryA mesh_entry;
    std::vector<u32> indices;
};

template<typename _VertexType>
static void CreateVertex(const std::vector<_VertexType> &vertices, Ref<origin::VertexArray> &vertex_array, Ref<origin::VertexBuffer> &vertex_buffer, const std::vector<u32> &indices, const origin::BufferLayout &layout)
{
    vertex_array = origin::VertexArray::Create();
    vertex_buffer = origin::VertexBuffer::Create((void *)vertices.data(), static_cast<u32>(vertices.size() * sizeof(_VertexType)));
    vertex_buffer->SetLayout(layout);
    vertex_array->AddVertexBuffer(vertex_buffer);
    Ref<origin::IndexBuffer> indexBuffer = origin::IndexBuffer::Create((void *)indices.data(), static_cast<u32>(indices.size()));
    vertex_array->SetIndexBuffer(indexBuffer);
}

class SkinnedMesh
{
public:
    SkinnedMesh() = default;
    ~SkinnedMesh();

    bool LoadMesh(const std::string &filename);
    void Render();
    u32 NumBones() const { return static_cast<u32>(m_BoneNameToIndexMap.size()); }

    void GetBoneTransforms(f32 time_in_sec, std::vector<glm::mat4> &transforms, u32 anim_index = 0);
    void GetBoneTransfromsBlended(f32 time_in_sec, std::vector<glm::mat4> &transforms, u32 start_anim_index, u32 end_anim_index, f32 blend_factor);

    bool IsAnimated() const;

private:
    bool InitFromScene(const aiScene *scene, const std::string &filename);
    void CountVerticesAndIndices(const aiScene *scene, u32 &num_vertices, u32 &num_indices);
    void ReserveSpace(u32 num_vertices, u32 num_indices);
    void InitAllMeshes(const aiScene *scene);
    void InitSingleMesh(u32 mesh_index, const aiMesh *mesh);
    bool InitMaterials(const aiScene *scene, const std::string &filename);
    void LoadTextures(const std::string &dir, const aiMaterial *material, i32 index);
    void LoadDiffuseTexture(const std::string &dir, const aiMaterial *material, i32 index);
    void LoadSpecularTexture(const std::string &dir, const aiMaterial *material, i32 index);
    void LoadColors(const aiMaterial *material, i32 index);
    void LoadMeshBones(u32 mesh_index, std::vector<SkinnedVertex> &skinned_vertices, const aiMesh *mesh);
    void LoadSingleBone(u32 mesh_index, std::vector<SkinnedVertex> &skinned_vertices, const aiBone *bone);
    void CalcInterpolatedScaling(aiVector3D &out, f32 anim_time, const aiNodeAnim *node_anim);
    void CalcInterpolatedRotation(aiQuaternion &out, f32 anim_time, const aiNodeAnim *node_anim);
    void CalcInterpolatedPosition(aiVector3D &out, f32 anim_time, const aiNodeAnim *node_anim);
    const aiNodeAnim *FindNodeAnim(const aiAnimation *animation, const std::string &node_name);
    void ReadNodeHierarchy(f32 time_in_ticks, const aiNode *node, const glm::mat4 &parent_transform);
    void ReadNodeHierarchyBlended(f32 start_anim_time_ticks, f32 end_anim_time_ticks, const aiNode *node, const glm::mat4 &parent_transform,
        const aiAnimation *start_animation, const aiAnimation *nd_animation, f32 blend_factor);
    void InitializeRequiredNodeMap(const aiNode *node);
    void MarkRequiredNodesForBone(const aiBone *bone);

    f32 CalcAnimationTimeTicks(f32 time_in_seconds, u32 anim_index);
    i32 GetBoneId(const aiBone *bone);
    u32 FindScaling(f32 anim_time, const aiNodeAnim *node_anim);
    u32 FindRotation(f32 anim_time, const aiNodeAnim *node_anim);
    u32 FindPosition(f32 anim_time, const aiNodeAnim *node_anim);

    struct LocalTransform
    {
        aiVector3D scaling;
        aiQuaternion rotation;
        aiVector3D translation;
    };

    void CalcLocalTransform(LocalTransform &transform, f32 anim_time_ticks, const aiNodeAnim *node_anim);

    Assimp::Importer m_Importer;
    const aiScene *m_Scene = nullptr;
    glm::mat4 m_WorldTransform;
    glm::mat4 m_GlobalInverseTransform;

    std::vector<SkinnedMeshData> m_Meshes;
    std::vector<VertexBoneDataA> m_Bones;
    std::vector<BoneInfoA> m_BoneInfo;

    std::map<std::string, u32> m_BoneNameToIndexMap;
    std::map<std::string, NodeInfo> m_RequiredNodeMap;
};