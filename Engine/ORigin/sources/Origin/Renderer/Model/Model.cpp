// Copyright (c) Evangelion Manuhutu | Origin Engine

#include "pch.h"
#include "Model.hpp"

#include "Origin/Renderer/Buffer.h"
#include "Origin/Renderer/Renderer.h"
#include "Origin/Renderer/VertexArray.h"
#include "Origin/Renderer/Texture.h"

#include "Origin/Renderer/TextureType.hpp"

#include <assimp/Importer.hpp>
#include <stb_image.h>

namespace origin {

static std::vector<Ref<Texture2D>> loaded_textures_cache;

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
	LoadMeshes(m_Scene, filepath);
}

void Model::GetBoneTransforms(f32 time_in_sec, std::vector<glm::mat4> &transforms, const u32 anim_index)
{
    glm::mat4 identity(1.0f);

    const f32 time_in_ticks = time_in_sec * m_Animations[anim_index].GetTicksPerSecond();
    const f32 animation_time_in_ticks = fmod(time_in_ticks, static_cast<f32>(m_Animations[anim_index].GetDuration()));

    UpdateAnimation(animation_time_in_ticks, m_Scene->mRootNode, identity, anim_index);

    transforms.resize(m_BoneInfo.size());
    for (size_t i = 0; i < m_BoneInfo.size(); ++i)
    {
        transforms[i] = m_BoneInfo[i].final_transformation;
    }
}

void Model::LoadMeshes(const aiScene *scene, const std::string &filepath)
{
    m_Meshes.resize(scene->mNumMeshes);

    // count vertices and indices
    u32 num_vertices = 0;
    u32 num_indices = 0;

    for (size_t i = 0; i < m_Meshes.size(); ++i)
    {
        m_Meshes[i] = CreateRef<Mesh>();

        m_Meshes[i]->mesh_entry.base_vertex = num_vertices;
        m_Meshes[i]->mesh_entry.base_index = num_indices;
        m_Meshes[i]->mesh_entry.num_indices = scene->mMeshes[i]->mNumFaces * 3;

        num_vertices += scene->mMeshes[i]->mNumVertices;
        num_indices += m_Meshes[i]->mesh_entry.num_indices;
    }

    // reserve space
    for (size_t i = 0; i < m_Meshes.size(); ++i)
    {
        m_Meshes[i]->vertices.reserve(num_vertices);
        m_Meshes[i]->indices.reserve(num_indices);
    }

    m_GlobalBones.resize(num_vertices);

    // load meshes
    for (u32 mesh_index = 0; mesh_index < scene->mNumMeshes; ++mesh_index)
    {
        aiMesh *mesh = scene->mMeshes[mesh_index];
        LoadSingleMesh(mesh_index, mesh, filepath);
    }
}

void Model::LoadSingleMesh(const u32 mesh_index, aiMesh *mesh, const std::string &filepath)
{
    m_Meshes[mesh_index]->name = mesh->mName.C_Str();
    m_Meshes[mesh_index]->bone_transforms.resize(100, glm::mat4(1.0f));

    // Vertices
    MeshVertexData vertex;
    m_Meshes[mesh_index]->vertices.resize(mesh->mNumVertices);
    for (u32 i = 0; i < mesh->mNumVertices; ++i)
    {
        vertex.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
        if (mesh->HasNormals())
        {
            vertex.normals = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
        }
        else
        {
            vertex.normals = { 0.0f, 1.0f, 0.0f }; // Default normal
        }

        vertex.color = { 1.0f, 1.0f, 1.0f };
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

    LoadMaterials(m_Meshes[mesh_index], mesh, filepath);
    LoadVertexBones(mesh_index, m_Meshes[mesh_index], mesh);
    CreateVertex(m_Meshes[mesh_index]);
}

void Model::LoadAnimations()
{
    m_Animations.resize(m_Scene->mNumAnimations);

    for (u32 i = 0; i < m_Scene->mNumAnimations; ++i)
    {
        aiAnimation *anim = m_Scene->mAnimations[i];
        m_Animations[i] = Anim(anim);
    }
}

void Model::UpdateAnimation(f32 time_in_ticks, const aiNode *node, const glm::mat4 &parent_transform, const u32 anim_index)
{
    std::string node_name(node->mName.C_Str());

    glm::mat4 node_transform = Math::AssimpToGlmMatrix(node->mTransformation);
    auto &channel_map = m_Animations[anim_index].GetChannelMap();
    if (channel_map.contains(node_name))
    {
        AnimationNode &anim_node = channel_map[node_name];
        anim_node.Update(time_in_ticks);
        node_transform = anim_node.local_transform;
    }

    glm::mat4 global_transform = parent_transform * node_transform;
    if (m_BoneNameToIndexMap.contains(node_name))
    {
        const u32 bone_index = m_BoneNameToIndexMap[node_name];
        m_BoneInfo[bone_index].final_transformation = global_transform * m_BoneInfo[bone_index].offset_matrix;
    }

    for (u32 i = 0; i < node->mNumChildren; ++i)
    {
        UpdateAnimation(time_in_ticks, node->mChildren[i], global_transform, anim_index);
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
        m_BoneInfo.push_back(new_bone_info);
    }

    // assign bone's ids and weights
    for (u32 i = 0; i < bone->mNumWeights; ++i)
    {
        const aiVertexWeight &v_weight = bone->mWeights[i];

        // assign to mesh's vertices
        OGN_CORE_ASSERT(v_weight.mVertexId < m_Meshes[mesh_index]->vertices.size(), "Invalid vertex id");
        m_Meshes[mesh_index]->vertices[v_weight.mVertexId].bone.AddBoneData(bone_id, v_weight.mWeight);

        // add the bone's weight and id to the model's global bones
        const u32 global_vertex_id = m_Meshes[mesh_index]->mesh_entry.base_vertex + bone->mWeights[i].mVertexId;
        m_GlobalBones[global_vertex_id].AddBoneData(bone_id, v_weight.mWeight);
    }
}

void Model::LoadMaterials(Ref<Mesh> mesh_data, aiMesh *mesh, const std::string &filepath)
{
    aiColor4D base_color(1.0f, 1.0f, 1.0f, 1.0f);
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = m_Scene->mMaterials[mesh->mMaterialIndex];

        aiColor4D diffuse_color(1.0f, 1.0f, 1.0f, 1.0f);
        ai_real metallic_factor = 1.0f;
        material->Get(AI_MATKEY_BASE_COLOR, base_color);
        material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse_color);
        material->Get(AI_MATKEY_METALLIC_FACTOR, mesh_data->material.metallic_factor);
        material->Get(AI_MATKEY_ROUGHNESS_FACTOR, mesh_data->material.roughness_factor);

        mesh_data->material.diffuse_color = { diffuse_color.r, diffuse_color.g, diffuse_color.b };

        auto texture_map = LoadTextures(m_Scene, material, filepath, TextureType::DIFFUSE);
        if (!texture_map.empty())
        {
            mesh_data->material.textures.push_back(texture_map);
        }

        if (mesh_data->material.textures.empty())
        {
            texture_map[TextureType::DIFFUSE] = Renderer::WhiteTexture;
            mesh_data->material.textures.push_back(texture_map);
        }
    }
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

TextureTypeMap Model::LoadTextures(const aiScene *scene, aiMaterial *material, const std::string &filepath, TextureType type)
{
    TextureTypeMap textures;

    const i32 tex_count = material->GetTextureCount(ToAssimpTexture(type));
    for (i32 i = 0; i < tex_count; ++i)
    {
        aiString texture_filename;
        material->GetTexture(ToAssimpTexture(type), i, &texture_filename);

        bool skip_import = false;
        for (i32 j = 0; j < loaded_textures_cache.size(); ++j)
        {
            if (std::strcmp(loaded_textures_cache[j]->GetName().c_str(), texture_filename.C_Str()) == 0)
            {
                textures[type] = loaded_textures_cache[j];
                skip_import = true;
                break;
            }
        }

        if (!skip_import)
        {
            const aiTexture *embedded_texture = scene->GetEmbeddedTexture(texture_filename.C_Str());
            if (embedded_texture)
            {
                textures[type] = Texture2D::Create(embedded_texture);
            }
            else
            {
                // handle external texture
                size_t end = filepath.find_last_of('/');
                auto texture_dir = filepath.substr(0, end);
                textures[type] = Texture2D::Create(texture_dir + "/" + texture_filename.C_Str());
            }
            loaded_textures_cache.push_back(textures[type]);
        }
    }

    return textures;
}

void Model::CreateVertex(Ref<Mesh> &mesh_data)
{
    mesh_data->vertex_array = VertexArray::Create();
    mesh_data->vertex_buffer = VertexBuffer::Create(mesh_data->vertices.data(), static_cast<u32>(mesh_data->vertices.size() * sizeof(MeshVertexData)));

    mesh_data->vertex_buffer->SetLayout
    ({
        { ShaderDataType::Float3, "position"      },
        { ShaderDataType::Float3, "normals"       },
        { ShaderDataType::Float3, "color"         },
        { ShaderDataType::Float2, "texcoord"      },
        { ShaderDataType::Int4,   "bone_ids"      },
        { ShaderDataType::Float4, "weights"       },
    });

    mesh_data->vertex_array->AddVertexBuffer(mesh_data->vertex_buffer);
    Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(mesh_data->indices.data(), static_cast<u32>(mesh_data->indices.size()));
    mesh_data->vertex_array->SetIndexBuffer(indexBuffer);
}

Ref<Model> Model::Create(const std::string &filepath)
{
    return CreateRef<Model>(filepath);
}

const std::vector<Anim> &Model::GetAnimations()
{
    return m_Animations;
}

const std::vector<Ref<Mesh>> &Model::GetMeshes()
{
    return m_Meshes;
}

}
