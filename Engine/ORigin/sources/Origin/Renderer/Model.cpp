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
                // handle external texture
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
    for (u32 bone_index = 0; bone_index < mesh->mNumBones; ++bone_index)
    {
        i32 bone_id = -1;
        aiBone *bone = mesh->mBones[bone_index];
        std::string bone_name = bone->mName.data;

        if (!data->bone_info_map.contains(bone_name))
        {
            BoneInfo new_bone_info;
            new_bone_info.ID = data->bone_count;
            new_bone_info.offset_matrix = Math::AssimpToGlmMatrix(bone->mOffsetMatrix);

            data->bone_info_map[bone_name] = new_bone_info;
            bone_id = data->bone_count++;
        }
        else
        {
            bone_id = data->bone_info_map[bone_name].ID;
        }

        aiVertexWeight *weights = bone->mWeights;
        for (u32 w_idx = 0; w_idx < bone->mNumWeights; ++w_idx)
        {
            i32 vertex_id = weights[w_idx].mVertexId;
            f32 weight = weights[w_idx].mWeight;

            OGN_CORE_ASSERT(vertex_id < data->vertices.size(), "Invalid vertex id");
            MeshVertexData &vertex = data->vertices[vertex_id];

            // find the first available slot for bone influence
            i32 available_slot = -1;
            for (i32 i = 0; i < MAX_BONE_INFLUENCE; ++i)
            {
                if (vertex.IDs[i] < 0)
                {
                    available_slot = i;
                    break;
                }
            }

            // if a slot is found, assign the bone weight
            if (available_slot != -1)
            {
                vertex.IDs[available_slot] = bone_id;
                vertex.Weights[available_slot] = weight;
            }
            else
            {
                i32 smalles_weight_idx = 0;
                for (i32 i = 1; i < MAX_BONE_INFLUENCE; ++i)
                {
                    if (vertex.Weights[i] < vertex.Weights[smalles_weight_idx])
                        smalles_weight_idx = i;
                }

                // replace if the current weight is larger
                if (vertex.Weights[smalles_weight_idx] < weight)
                {
                    vertex.IDs[smalles_weight_idx] = bone_id;
                    vertex.Weights[smalles_weight_idx] = weight;
                }
            }
        }
    }
}

std::vector<Ref<Mesh>> Model::ProcessNode(aiNode* node, const aiScene* scene, const std::string &filepath)
{
	std::vector<Ref<Mesh>> meshes(node->mNumMeshes);

	for (u32 mesh_index = 0; mesh_index < node->mNumMeshes; ++mesh_index)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[mesh_index]];
		meshes[mesh_index] = LoadMeshData(scene, mesh, filepath);
	}

	for (u32 i = 0; i < node->mNumChildren; ++i)
	{
		std::vector<Ref<Mesh>> child_meshes = ProcessNode(node->mChildren[i], scene, filepath);
		meshes.insert(meshes.end(), child_meshes.begin(), child_meshes.end());
	}

	return meshes;
}

glm::mat4 Model::CalculateTransform(aiNode *node, aiMesh *mesh)
{
    return Math::AssimpToGlmMatrix(node->mTransformation);
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
    const aiScene *scene = importer.ReadFile(filepath.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals 
        | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

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
    mesh_data->node = new MeshNode();
    mesh_data->final_bone_matrices.resize(100, glm::mat4(1.0f));

    aiNode *node = FindMeshNode(scene->mRootNode, scene, mesh);

    if (node)
    {
        mesh_data->node->name = node->mName.data;
        mesh_data->name = node->mName.data;

        // store local transform of the mesh node
        mesh_data->node->local_transform = Math::AssimpToGlmMatrix(node->mTransformation);

        // handle parent node
        if (aiNode *parent_node = node->mParent)
        {
            mesh_data->node->parent = new MeshNode();
            mesh_data->node->parent->name = parent_node->mName.data;
            mesh_data->node->parent->local_transform = Math::AssimpToGlmMatrix(parent_node->mTransformation);
        }

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

            auto texture_map = MLoadTextures(scene, material, filepath, aiTextureType_DIFFUSE);
            if (!texture_map.empty())
                mesh_data->material.textures.push_back(texture_map);

            if (mesh_data->material.textures.empty())
            {
                texture_map[aiTextureType_DIFFUSE] = Renderer::WhiteTexture;
                mesh_data->material.textures.push_back(texture_map);
            }
        }

        // Vertices
        mesh_data->vertices.resize(mesh->mNumVertices);
        for (u32 i = 0; i < mesh->mNumVertices; ++i)
        {
            MeshVertexData vertex;

            vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
            if (mesh->HasNormals())
            {
                vertex.Normals = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
            }
            else
            {
                vertex.Normals = { 0.0f, 0.0f, 0.0f }; // Default normal
            }

            vertex.Color = { base_color.r, base_color.g, base_color.b };
            if (mesh->mTextureCoords[0]) // Check for first UV set
            {
                vertex.UV = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
            }
            else
            {
                vertex.UV = { 0.0f, 0.0f }; // Default UV
            }

            vertex.TilingFactor = { 1.0f, 1.0f };
            vertex.AlbedoIndex = 0.0f;
            vertex.SpecularIndex = 0.0f;
            mesh_data->vertices[i] = vertex;
        }

        for (u32 i = 0; i < mesh->mNumFaces; ++i)
        {
            aiFace face = mesh->mFaces[i];
            for (u32 in = 0; in < face.mNumIndices; ++in)
            {
                mesh_data->indices.push_back(face.mIndices[in]);
            }
        }

        ExtractBoneWeightForVertices(mesh_data, mesh, scene);
        CreateVertex(mesh_data);
    }

    return mesh_data;
}


const aiScene *Model::LoadAiScene(const std::string &filepath)
{
    OGN_CORE_ASSERT(std::filesystem::exists(filepath), "[Model] File does not exist '{}'", filepath);

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filepath.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::string err = importer.GetErrorString();
        OGN_CORE_ASSERT(false, "[Model] {}", err);
        return nullptr;
    }

   return scene;
}

void Model::CreateVertex(Ref<Mesh> &mesh_data)
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
        { ShaderDataType::Float4, "aWeights"      },
        { ShaderDataType::Float,  "aAlbedoIndex"  },
        { ShaderDataType::Float,  "aSpecularIndex"},
    });

    mesh_data->vertex_array->AddVertexBuffer(mesh_data->vertex_buffer);
    Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(mesh_data->indices.data(), static_cast<u32>(mesh_data->indices.size()));
    mesh_data->vertex_array->SetIndexBuffer(indexBuffer);
}

}
