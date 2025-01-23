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

void Model::LoadMeshes(const aiScene *scene, const std::string &filepath)
{
	m_Meshes.resize(scene->mNumMeshes);

    for (u32 mesh_index = 0; mesh_index < scene->mNumMeshes; ++mesh_index)
    {
        aiMesh *mesh = scene->mMeshes[mesh_index];
        m_Meshes[mesh_index] = LoadMeshData(scene, mesh, filepath);
    }
}

aiNode *Model::FindMeshNode(aiNode *node, const aiScene *scene, aiMesh *target_mesh)
{
    for (u32 i = 0; i < node->mNumMeshes; i++)
    {
        if (scene->mMeshes[node->mMeshes[i]] == target_mesh)
            return node;
    }

    for (u32 i = 0; i < node->mNumChildren; i++)
    {
        aiNode *found_node = FindMeshNode(node->mChildren[i], scene, target_mesh);
        if (found_node)
            return found_node;
    }

    return nullptr;
}

void Model::LoadAnimations(const aiScene *scene)
{
    for (u32 i = 0; i < scene->mNumAnimations; ++i)
    {
        const aiAnimation *anim = scene->mAnimations[i];
        m_Animations.emplace_back(anim, global_bones);
    }

    if (!m_Animations.empty())
    {
        current_animation = &m_Animations[0];
    }
}

void Model::GetFinalBoneTransforms(std::vector<glm::mat4> &out_bone_transforms)
{
    out_bone_transforms.resize(global_bones.size(), glm::mat4(1.0f));
    if (!current_animation)
        return;

    const f32 anim_time = fmod(current_time * current_animation->ticks_per_second, current_animation->duration);
    std::function<void(const std::string &, const glm::mat4 &)> CalculateBoneTransforms;
    CalculateBoneTransforms = [&](const std::string &bone_name, const glm::mat4 &parent_transform)
    {
        Bone &bone = global_bones[bone_name];
        const aiNodeAnim *node_anim = current_animation->FindNodeAnim(bone_name);

        if (node_anim)
        {
            bone.Update(anim_time);
        }

        bone.global_transform = parent_transform * bone.local_transform;
        out_bone_transforms[bone.id] = bone.global_transform * bone.offset_matrix;

        for (const auto &child_name : bone.children)
        {
            CalculateBoneTransforms(child_name, bone.global_transform);
        }
    };

    CalculateBoneTransforms("_rootJoint", glm::mat4(1.0f));
}

void Model::UpdateAnimation(const f32 dt, const f32 speed)
{
    if (current_animation)
    {
        current_time += dt * speed;
    }
}

void Model::BuildBoneHierarchy(const aiNode *node, std::map<std::string, Bone> &global_bones)
{
    std::string node_name(node->mName.C_Str());

    if (global_bones.contains(node_name))
    {
        Bone &parent_bone = global_bones[node_name];
        for (u32 i = 0; i < node->mNumChildren; ++i)
        {
            const aiNode *child_node = node->mChildren[i];
            std::string child_name(child_node->mName.C_Str());

            if (global_bones.contains(child_name))
            {
                parent_bone.children.push_back(child_name);
            }
        }
    }

    for (u32 i = 0; i < node->mNumChildren; ++i)
    {
        BuildBoneHierarchy(node->mChildren[i], global_bones);
    }
}

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

	LoadMeshes(m_Scene, filepath);
    LoadAnimations(m_Scene);
}

Ref<Mesh> Model::LoadMeshData(const aiScene *scene, aiMesh *mesh, const std::string &filepath)
{
    Ref<Mesh> mesh_data = CreateRef<Mesh>();
    mesh_data->name = mesh->mName.C_Str();
    mesh_data->bone_transforms.resize(100, glm::mat4(1.0f));

    aiNode *node = FindMeshNode(scene->mRootNode, scene, mesh);

    if (node)
    {
        mesh_data->transform = Math::AssimpToGlmMatrix(node->mTransformation);

        aiColor4D base_color(1.0f, 1.0f, 1.0f, 1.0f);
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

            aiColor4D diffuse_color(1.0f, 1.0f, 1.0f, 1.0f);
            ai_real metallic_factor = 1.0f;
            material->Get(AI_MATKEY_BASE_COLOR, base_color);
            material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse_color);
            material->Get(AI_MATKEY_METALLIC_FACTOR, mesh_data->material.metallic_factor);
            material->Get(AI_MATKEY_ROUGHNESS_FACTOR, mesh_data->material.roughness_factor);

            mesh_data->material.diffuse_color = { diffuse_color.r, diffuse_color.g, diffuse_color.b };

            auto texture_map = LoadTextures(scene, material, filepath, TextureType::DIFFUSE);
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

        // Vertices
        MeshVertexData vertex;
        mesh_data->vertices.resize(mesh->mNumVertices);
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

            vertex.color = { base_color.r, base_color.g, base_color.b };
            if (mesh->mTextureCoords[0])
            {
                vertex.texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
            }
            else
            {
                vertex.texcoord = { 0.0f, 0.0f };
            }

            mesh_data->vertices[i] = vertex;
        }

        for (u32 i = 0; i < mesh->mNumFaces; ++i)
        {
            aiFace face = mesh->mFaces[i];
            mesh_data->indices.push_back(face.mIndices[0]);
            mesh_data->indices.push_back(face.mIndices[1]);
            mesh_data->indices.push_back(face.mIndices[2]);
        }

        LoadBones(mesh_data, mesh);
        CreateVertex(mesh_data);
    }

    return mesh_data;
}

void Model::LoadBones(Ref<Mesh> &data, aiMesh *mesh)
{
    for (u32 bone_index = 0; bone_index < mesh->mNumBones; ++bone_index)
    {
        aiBone *mesh_bone = mesh->mBones[bone_index];

        std::string bone_name(mesh_bone->mName.data);

        i32 bone_id;
        auto it = global_bones.find(bone_name);
        if (it != global_bones.end())
        {
            // bone already exists, reuse its ID
            bone_id = it->second.id;
        }
        else
        {
            // new bone, assign a new ID and add it to the map
            Bone new_bone;
            new_bone.name = bone_name;
            new_bone.id = data->bones.size();
            new_bone.offset_matrix = Math::AssimpToGlmMatrix(mesh_bone->mOffsetMatrix);
            global_bones[bone_name] = new_bone;
            bone_id = new_bone.id;
        }

        // store the bone in the mesh's local bones map
        if (data->bones.find(bone_name) == data->bones.end())
        {
            data->bones[bone_name] = global_bones[bone_name];
        }

        // assign weights to the vertices
        for (u32 w_idx = 0; w_idx < mesh_bone->mNumWeights; ++w_idx)
        {
            aiVertexWeight vertex_weight = mesh_bone->mWeights[w_idx];
            const i32 &vertex_id = vertex_weight.mVertexId;
            const f32 &weight = vertex_weight.mWeight;

            OGN_CORE_ASSERT(vertex_id < data->vertices.size(), "Invalid vertex id");
            MeshVertexData &vertex = data->vertices[vertex_id];
            vertex.bone.AddBoneData(bone_id, weight);
        }
    }

    BuildBoneHierarchy(m_Scene->mRootNode, global_bones);
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
