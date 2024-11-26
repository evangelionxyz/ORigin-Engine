#include "pch.h"
#include "Model.h"

#include "Buffer.h"
#include "Renderer.h"
#include "VertexArray.h"

#include <assimp/Importer.hpp>

#include <stb_image.h>

#include "Texture.h"

namespace origin {

static std::vector<Ref<Texture2D>> textures_loaded;
static std::unordered_map<aiTextureType, Ref<Texture2D>> MLoadTextures(const aiScene *scene, aiMaterial *material, const std::string &filepath, aiTextureType type)
{
	std::unordered_map<aiTextureType, Ref<Texture2D>> textures;

	// textures[type] = Renderer::WhiteTexture;

    const i32 tex_count = material->GetTextureCount(type);
    for (i32 i = 0; i < tex_count; ++i)
    {
        aiString texture_filename;
        material->GetTexture(type, i, &texture_filename);

        bool skip_import = false;
        for (i32 j = 0; j < textures_loaded.size(); ++j)
        {
            if (std::strcmp(textures_loaded[j]->GetName().c_str(), texture_filename.C_Str()) == 0)
            {
                textures[type] = textures_loaded[j];
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
                // Handle external texture
                size_t end = filepath.find_last_of('/');
                auto texture_dir = filepath.substr(0, end);
				textures[type] = Texture2D::Create(texture_dir + "/" + texture_filename.C_Str());
            }
            textures_loaded.push_back(textures[type]);
        }
    }

	return textures;
}
static void ExtractBoneWeightForVertices(Ref<Mesh> &data, aiMesh *mesh, const aiScene *scene)
{
    // load bones weight and apply it to vertices
    for (u32 boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {
        i32 boneId = -1;
        aiBone *bone = mesh->mBones[boneIndex];
        std::string boneName = bone->mName.data;

        if (data->bone_info_map.find(boneName) == data->bone_info_map.end())
        {
            BoneInfo newBoneInfo;
            newBoneInfo.ID = data->bone_count;
            newBoneInfo.offset_matrix = Math::AssimpToGlmMatrix(bone->mOffsetMatrix);

            data->bone_info_map[boneName] = newBoneInfo;
            boneId = data->bone_count;
            data->bone_count++;
        }
        else
        {
            boneId = data->bone_info_map[boneName].ID;
        }

        auto weights = bone->mWeights;
        for (u32 weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex)
        {
            i32 vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;

            OGN_CORE_ASSERT(vertexId < data->vertices.size(), "Invalid vertex id");
            MeshVertexData &vertex = data->vertices[vertexId];

            // Find the first available slot for bone influence
            i32 availableSlot = -1;
            for (i32 i = 0; i < MAX_BONE_INFLUENCE; ++i)
            {
                if (vertex.IDs[i] < 0)
                {
                    availableSlot = i;
                    break;
                }
            }

            // If a slot is found, assign the bone weight
            if (availableSlot != -1)
            {
                vertex.IDs[availableSlot] = boneId;
                vertex.Weights[availableSlot] = weight;
            }
            else
            {
                // If no available slot, find the smallest weight and replace it if the current weight is larger
                i32 smallestWeightIndex = 0;
                for (i32 i = 1; i < MAX_BONE_INFLUENCE; ++i)
                {
                    if (vertex.Weights[i] < vertex.Weights[smallestWeightIndex])
                    {
                        smallestWeightIndex = i;
                    }
                }

                // Replace if the current weight is larger
                if (vertex.Weights[smallestWeightIndex] < weight)
                {
                    vertex.IDs[smallestWeightIndex] = boneId;
                    vertex.Weights[smallestWeightIndex] = weight;
                }
            }
        }
    }

    // Normalize the weights so that they sum to 1 for each vertex
    for (auto &vertex : data->vertices)
    {
        float totalWeight = 0.0f;
        for (i32 i = 0; i < MAX_BONE_INFLUENCE; ++i)
        {
            totalWeight += vertex.Weights[i];
        }

        if (totalWeight > 0.0f)
        {
            for (i32 i = 0; i < MAX_BONE_INFLUENCE; ++i)
            {
                vertex.Weights[i] /= totalWeight;
            }
        }
    }
}

std::vector<Ref<Mesh>> Model::ProcessNode(aiNode* node, const aiScene* scene, const std::string &filepath)
{
	std::vector<Ref<Mesh>> meshes(node->mNumMeshes);

	for (u32 meshIndex = 0; meshIndex < node->mNumMeshes; ++meshIndex)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[meshIndex]];
		meshes[meshIndex] = LoadMeshData(scene, mesh, filepath);
	}

	for (u32 i = 0; i < node->mNumChildren; ++i)
	{
		std::vector<Ref<Mesh>> childMeshes = ProcessNode(node->mChildren[i], scene, filepath);
		meshes.insert(meshes.end(), childMeshes.begin(), childMeshes.end());
	}
	return meshes;
}

std::vector<ModelAnimation> Model::LoadAnimations(const std::vector<Ref<Mesh>> &meshes, const aiScene *scene)
{
    std::vector<ModelAnimation> animations;

    if (scene->HasAnimations())
    {
        for (u32 anim_index = 0; anim_index < scene->mNumAnimations; ++anim_index)
        {
            aiAnimation *ai_anim = scene->mAnimations[anim_index];
            ModelAnimation model_animation(meshes, ai_anim, scene);
            animations.push_back(model_animation);
        }
    }
    return animations;
}

Model::Model(const std::string &filepath)
    : m_Filepath(filepath)
{
    OGN_CORE_ASSERT(std::filesystem::exists(filepath), "[Static Model] File does not exist '{}'", filepath);

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filepath.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::string err = importer.GetErrorString();
        OGN_CORE_ERROR("[Static Model] {}", err);
        return;
    }

	aiNode* node = scene->mRootNode;
	m_Meshes = ProcessNode(node, scene, filepath);
    m_Animations = LoadAnimations(m_Meshes, scene);
}

const std::vector<ModelAnimation> &Model::GetAnimations()
{
    return m_Animations;
}

const std::vector<Ref<Mesh>> &Model::GetMeshes()
{
    return m_Meshes;
}

const std::string &Model::GetFilepath()
{
    return m_Filepath;
}

Ref<Model> Model::Create(const std::string &filepath)
{
    return CreateRef<Model>(filepath);
}

Ref<Mesh> Model::LoadMeshData(const aiScene *scene, aiMesh *mesh, const std::string &filepath)
{
    Ref<Mesh> mesh_data = CreateRef<Mesh>();

    mesh_data->final_bone_matrices.resize(100, glm::mat4(1.0f));

    mesh_data->name = mesh->mName.C_Str();

    aiColor4D base_color(1.0f, 1.0f, 1.0f, 1.0f); // Default color
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

        auto texture_map = MLoadTextures(scene, material, filepath, aiTextureType_DIFFUSE);
        mesh_data->material.textures.push_back(texture_map);
    }

    // Vertices
    mesh_data->vertices.resize(mesh->mNumVertices);
    for (u32 i = 0; i < mesh->mNumVertices; ++i)
    {
        MeshVertexData vertex;

        // Position
        vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

        // Normals
        if (mesh->HasNormals())
            vertex.Normals = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
        else
            vertex.Normals = { 0.0f, 0.0f, 0.0f }; // Default normal

        // Colors
        vertex.Color = { base_color.r, base_color.g, base_color.b };

        // UV Coordinates
        if (mesh->mTextureCoords[0]) // Check for first UV set
        {
            vertex.UV = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
        }
        else
        {
            vertex.UV = { 0.0f, 0.0f }; // Default UV
        }

        // Additional default values
        vertex.TilingFactor = { 1.0f, 1.0f };
        vertex.AlbedoIndex = 0.0f;
        vertex.SpecularIndex = 0.0f;

        mesh_data->vertices[i] = vertex;
    }

    // Faces
    for (u32 i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (u32 in = 0; in < face.mNumIndices; ++in)
        {
            mesh_data->indices.push_back(face.mIndices[in]);
        }
    }

    ExtractBoneWeightForVertices(mesh_data, mesh, scene);

    /*for (auto i : mesh_data->bone_info_map)
    {
        OGN_CORE_INFO("Map {} {}: {}", i.first, i.second.ID, glm::to_string(i.second.offset_matrix));
    }*/

    CreateVertex(mesh_data);

    return mesh_data;
}


const aiScene *Model::LoadAiScene(const std::string &filepath)
{
    OGN_CORE_ASSERT(std::filesystem::exists(filepath), "[Static Model] File does not exist '{}'", filepath);

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filepath.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::string err = importer.GetErrorString();
        OGN_CORE_ERROR("[Static Model] {}", err);
        return nullptr;
    }

   return scene;
}

void Model::CreateVertex(const Ref<Mesh> &mesh_data)
{
    mesh_data->vertex_array = VertexArray::Create();
    mesh_data->vertex_buffer = VertexBuffer::Create(mesh_data->vertices.data(), static_cast<u32>(mesh_data->vertices.size() * sizeof(MeshVertexData)));
    mesh_data->vertex_buffer->SetLayout
    ({
        { ShaderDataType::Float3, "aPosition"     },
        { ShaderDataType::Float3, "aNormals"      },
        { ShaderDataType::Float3, "aColor"        },
        { ShaderDataType::Float2, "aUV"           },
        { ShaderDataType::Float2, "aTilingFactor" },
        { ShaderDataType::Int4,   "aBoneIDs"      },
        { ShaderDataType::Float4, "aBoneWeights"  },
        { ShaderDataType::Float,  "aAlbedoIndex"  },
        { ShaderDataType::Float,  "aSpecularIndex"},
        });
    mesh_data->vertex_array->AddVertexBuffer(mesh_data->vertex_buffer);
    Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(mesh_data->indices.data(), static_cast<u32>(mesh_data->indices.size()));
    mesh_data->vertex_array->SetIndexBuffer(indexBuffer);
}

}
