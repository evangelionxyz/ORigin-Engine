// Copyright (c) Evangelion Manuhutu | Origin Engine

#include "pch.h"
#include "Model.hpp"

#include "Origin/Renderer/Buffer.h"
#include "Origin/Renderer/Renderer.h"
#include "Origin/Renderer/VertexArray.h"
#include "Origin/Renderer/Texture.h"
#include "Origin/Renderer/Materials/MaterialManager.hpp"
#include "Origin/Renderer/TextureType.hpp"

#include <assimp/Importer.hpp>
#include <stb_image.h>

namespace origin {

static std::vector<std::pair<TextureType, Ref<Texture2D>>> loaded_textures_cache;

Model::Model(const std::string &filepath)
{
    OGN_CORE_ASSERT(std::filesystem::exists(filepath), "[Model] File does not exist '{}'", filepath);

    m_Scene = m_Importer.ReadFile(filepath.c_str(), aiProcess_Triangulate
        | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

    if (!m_Scene || m_Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_Scene->mRootNode)
    {
        OGN_CORE_ASSERT(false, "[Model] {}", m_Importer.GetErrorString());
        return;
    }

    LoadAnimations();

    m_Meshes.resize(m_Scene->mNumMeshes);

    // count vertices and indices
    u32 num_vertices = 0;
    u32 num_indices = 0;

    for (size_t i = 0; i < m_Meshes.size(); ++i)
    {
        num_vertices += m_Scene->mMeshes[i]->mNumVertices;
        num_indices += m_Scene->mMeshes[i]->mNumFaces * 3;
    }

    // reserve space
    for (size_t i = 0; i < m_Meshes.size(); ++i)
    {
        m_Meshes[i] = CreateRef<Mesh>();
        m_Meshes[i]->vertices.reserve(num_vertices);
        m_Meshes[i]->indices.reserve(num_indices);
    }

    ProcessNode(m_Scene->mRootNode, glm::mat4(1.0f), filepath);

    loaded_textures_cache.clear();
}

void Model::ProcessNode(aiNode *node, const glm::mat4 &transform, const std::string &filepath)
{
    glm::mat4 current_transform = m_Scene->mNumAnimations == 0 ? Math::AssimpToGlmMatrix(node->mTransformation) * transform : glm::mat4(1.0f);

    for (u32 i = 0; i < node->mNumMeshes; ++i)
    {
        i32 mesh_index = node->mMeshes[i];
        aiMesh *mesh = m_Scene->mMeshes[mesh_index];
        LoadSingleMesh(mesh_index, mesh, current_transform, filepath);
    }

    for (u32 i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNode(node->mChildren[i], current_transform, filepath);
    }
}

void Model::UpdateAnimation(f32 delta_time, const u32 anim_index)
{
    if (!HasAnimations())
    {
        return;
    }

    SkeletalAnimation &current_anim = m_Animations[anim_index];
    current_anim.UpdateTime(delta_time);
    CalculateBoneTransforms(m_Scene->mRootNode, glm::mat4(1.0f), anim_index);

    m_FinalBoneTransforms.resize(m_BoneInfo.size(), glm::mat4(1.0f));
    for (size_t i = 0; i < m_BoneInfo.size(); ++i)
    {
        m_FinalBoneTransforms[i] = m_BoneInfo[i].transform;
    }
}

const std::vector<glm::mat4> &Model::GetBoneTransforms() const
{
    return m_FinalBoneTransforms;
}

void Model::LoadSingleMesh(const u32 mesh_index, aiMesh *mesh, const glm::mat4 &transform, const std::string &filepath)
{
    m_Meshes[mesh_index]->name = mesh->mName.C_Str();
    m_Meshes[mesh_index]->bone_transforms.resize(100, glm::mat4(1.0f));
    m_Meshes[mesh_index]->transform = transform;

    // Vertices
    MeshVertexData vertex;
    m_Meshes[mesh_index]->vertices.resize(mesh->mNumVertices);
    for (u32 i = 0; i < mesh->mNumVertices; ++i)
    {
        vertex.color = { 1.0f, 1.0f, 1.0f };
        vertex.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
        if (mesh->HasNormals())
        {
            vertex.normals = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
        }
        else
        {
            vertex.normals = { 0.0f, 1.0f, 0.0f }; // Default normal
        }

        if (mesh->mTextureCoords[0])
        {
            vertex.texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
        }
        else
        {
            vertex.texcoord = { 0.0f, 0.0f };
        }

        m_Meshes[mesh_index]->vertices[i] = vertex;
    }

    for (u32 i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        m_Meshes[mesh_index]->indices.push_back(face.mIndices[0]);
        m_Meshes[mesh_index]->indices.push_back(face.mIndices[1]);
        m_Meshes[mesh_index]->indices.push_back(face.mIndices[2]);
    }

    if (m_Scene->HasAnimations())
    {
        LoadVertexBones(mesh_index, m_Meshes[mesh_index], mesh);
    }
    
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *ai_material = m_Scene->mMaterials[mesh->mMaterialIndex];
        std::string mat_name(ai_material->GetName().C_Str());

        LoadMaterials(m_Meshes[mesh_index]->material, ai_material, filepath);
        m_Meshes[mesh_index]->material_index = static_cast<i32>(MaterialManager::AddMaterial(m_Meshes[mesh_index]->material.buffer_data));
        MaterialManager::UpdateMaterial(m_Meshes[mesh_index]->material_index, m_Meshes[mesh_index]->material.buffer_data);
    }
    
    CreateVertex(m_Meshes[mesh_index]);
}

void Model::LoadAnimations()
{
    m_Animations.resize(m_Scene->mNumAnimations);

    for (u32 i = 0; i < m_Scene->mNumAnimations; ++i)
    {
        aiAnimation *anim = m_Scene->mAnimations[i];
        m_Animations[i] = SkeletalAnimation(anim);
    }
}

void Model::CalculateBoneTransforms(const aiNode *node, const glm::mat4 &transform, const u32 anim_index)
{
    std::string node_name(node->mName.C_Str());

    glm::mat4 current_transform = Math::AssimpToGlmMatrix(node->mTransformation);

    SkeletalAnimation &anim = m_Animations[anim_index];
    auto &channel_map = anim.GetChannelMap();
    if (channel_map.contains(node_name))
    {
        AnimationNode &anim_node = channel_map[node_name];
        anim_node.CalculateTransform(anim.GetTimeInTicks());
        current_transform = anim_node.transform; // use animated local transform
    }

    glm::mat4 global_transform = transform * current_transform;

    if (m_BoneNameToIndexMap.contains(node_name))
    {
        const u32 bone_index = m_BoneNameToIndexMap[node_name];
        // Final bone transform: global_transform * offset_matrix
        m_BoneInfo[bone_index].transform = global_transform * m_BoneInfo[bone_index].offset_matrix;
    }

    for (u32 i = 0; i < node->mNumChildren; ++i)
    {
        CalculateBoneTransforms(node->mChildren[i], global_transform, anim_index);
    }
}

void Model::CalculateAnimationTransforms(const aiNode *node, const u32 anim_index, std::unordered_map<std::string, AnimationNode> &anim_nodes, const glm::mat4 &parent_transform)
{
    std::string node_name(node->mName.C_Str());

    glm::mat4 node_transform = Math::AssimpToGlmMatrix(node->mTransformation);

    SkeletalAnimation &anim = m_Animations[anim_index];
    auto &channel_map = anim.GetChannelMap();

    if (channel_map.contains(node_name))
    {
        // Updating animation's node transformation
        AnimationNode &anim_node = channel_map[node_name];
        anim_node.CalculateTransform(anim.GetTimeInTicks());
        anim_nodes[node_name] = anim_node;
        anim_nodes[node_name].parent_transform = parent_transform;

        // set node transform
        node_transform = anim_nodes[node_name].transform;
    }
    
    glm::mat4 global_transform = parent_transform * node_transform;

    for (u32 i = 0; i < node->mNumChildren; ++i)
    {
        CalculateAnimationTransforms(node->mChildren[i], anim_index, anim_nodes, global_transform);
    }
}

void Model::LoadVertexBones(const u32 mesh_index, Ref<Mesh> &mesh_data, aiMesh *mesh)
{
    for (u32 i = 0; i < mesh->mNumBones; ++i)
    {
        const aiBone *bone = mesh->mBones[i];
        LoadSingleVertexBone(mesh_index, mesh_data, bone);
    }
}

void Model::LoadSingleVertexBone(const u32 mesh_index, Ref<Mesh> &data, const aiBone *bone)
{
    // get bone id by its index
    i32 bone_id = GetBoneID(bone);

    // check if the bone id (index) is large then it is a new bone
    if (bone_id == m_BoneInfo.size())
    {
        BoneInfo new_bone_info(Math::AssimpToGlmMatrix(bone->mOffsetMatrix));
        new_bone_info.name = bone->mName.data;
        m_BoneInfo.push_back(new_bone_info);
    }

    // assign bone's ids and weights
    for (u32 i = 0; i < bone->mNumWeights; ++i)
    {
        const aiVertexWeight &v_weight = bone->mWeights[i];

        // assign to mesh's vertices
        OGN_CORE_ASSERT(v_weight.mVertexId < m_Meshes[mesh_index]->vertices.size(), "Invalid vertex id");
        m_Meshes[mesh_index]->vertices[v_weight.mVertexId].bone.AddBoneData(bone_id, v_weight.mWeight);
    }
}

void Model::LoadMaterials(MeshMaterial &material, aiMaterial *ai_material, const std::string &filepath)
{
    aiColor4D base_color(1.0f, 1.0f, 1.0f, 1.0f);
    aiColor4D diffuse_color(1.0f, 1.0f, 1.0f, 1.0f);
    aiColor4D emmisive_color(0.0f, 0.0f, 0.0f, 0.0f);
        
    ai_material->Get(AI_MATKEY_BASE_COLOR, base_color);
    ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse_color);
    ai_material->Get(AI_MATKEY_ROUGHNESS_FACTOR, material.buffer_data.rougness);
    ai_material->Get(AI_MATKEY_COLOR_EMISSIVE, emmisive_color);

    material.buffer_data.base_color = {base_color.r, base_color.g, base_color.b, 1.0f};
    material.buffer_data.diffuse_color = {diffuse_color.r, diffuse_color.g, diffuse_color.b, 1.0f};
    material.buffer_data.emissive = emmisive_color.r / diffuse_color.r;

    // load textures
    material.diffuse_texture = LoadTexture(m_Scene, ai_material, filepath, TextureType::DIFFUSE);
    material.specular_texture = LoadTexture(m_Scene, ai_material, filepath, TextureType::SPECULAR);
    material.roughness_texture = LoadTexture(m_Scene, ai_material, filepath, TextureType::DIFFUSE_ROUGHNESS);
}

i32 Model::GetBoneID(const aiBone *bone)
{
    // register each bone's name with index
    std::string bone_name(bone->mName.C_Str());
    if (!m_BoneNameToIndexMap.contains(bone_name))
    {
        const i32 bone_index = static_cast<i32>(m_BoneNameToIndexMap.size());
        m_BoneNameToIndexMap[bone_name] = bone_index;
        return bone_index;
    }

    return m_BoneNameToIndexMap[bone_name];
}

const bool Model::HasAnimations() const
{
    return !m_Animations.empty();
}

Ref<Texture2D> Model::LoadTexture(const aiScene *scene, aiMaterial *material, const std::string &filepath, TextureType type)
{
    Ref<Texture2D> texture;

    const i32 tex_count = material->GetTextureCount(ToAssimpTexture(type));
    for (i32 i = 0; i < tex_count; ++i)
    {
        aiString texture_filename;
        material->GetTexture(ToAssimpTexture(type), i, &texture_filename);

        bool skip_import = false;
        
        for (size_t loaded_texture_index = 0; loaded_texture_index < loaded_textures_cache.size(); ++loaded_texture_index)
        {
            if (std::strcmp(loaded_textures_cache[loaded_texture_index].second->GetName().c_str(), texture_filename.C_Str()) == 0)
            {
                texture = loaded_textures_cache[loaded_texture_index].second;
                skip_import = true;
                break;
            }
        }

        if (!skip_import)
        {
            const aiTexture *embedded_texture = scene->GetEmbeddedTexture(texture_filename.C_Str());
            if (embedded_texture)
            {
                texture = Texture2D::Create(embedded_texture);
            }
            else
            {
                // handle external texture
                size_t end = filepath.find_last_of('/');
                auto texture_dir = filepath.substr(0, end);
                texture = Texture2D::Create(texture_dir + "/" + texture_filename.C_Str());
            }
            loaded_textures_cache.emplace_back(type, texture);
        }
    }

    if (!texture)
        texture = Renderer::white_texture;
    return texture;
}

void Model::CreateVertex(Ref<Mesh> &mesh_data)
{
    mesh_data->vertex_array = VertexArray::Create();
    mesh_data->vertex_buffer = VertexBuffer::Create(mesh_data->vertices.data(), static_cast<u32>(mesh_data->vertices.size() * sizeof(MeshVertexData)));

    mesh_data->vertex_buffer->SetLayout
    ({
        { ShaderDataType::Float3, "position"},
        { ShaderDataType::Float3, "normals" },
        { ShaderDataType::Float3, "color"   },
        { ShaderDataType::Float2, "texcoord"},
        { ShaderDataType::Int4,   "bone_ids"},
        { ShaderDataType::Float4, "weights" },
    });

    mesh_data->vertex_array->AddVertexBuffer(mesh_data->vertex_buffer);
    Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(mesh_data->indices.data(), static_cast<u32>(mesh_data->indices.size()));
    mesh_data->vertex_array->SetIndexBuffer(indexBuffer);
}

Ref<Model> Model::Create(const std::string &filepath)
{
    return CreateRef<Model>(filepath);
}

std::vector<SkeletalAnimation> &Model::GetAnimations()
{
    return m_Animations;
}

SkeletalAnimation *Model::GetAnimation(u32 index)
{
    return &m_Animations[index];
}

const std::vector<Ref<Mesh>> &Model::GetMeshes()
{
    return m_Meshes;
}

}
