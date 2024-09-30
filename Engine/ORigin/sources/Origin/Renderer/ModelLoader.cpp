// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "ModelLoader.h"
#include "Origin/Math/Math.h"
#include "Renderer.h"
#include "Texture.h"

#include <stb_image.h>

namespace origin
{
	static void ProcessStaticMeshNode(aiNode *node, const aiScene *scene, StaticMeshData *data)
	{
		for (u32 meshIndex = 0; meshIndex < node->mNumMeshes; ++meshIndex)
		{
			aiMesh *mesh = scene->mMeshes[node->mMeshes[meshIndex]];

			// Vertices
			for (u32 i = 0; i < mesh->mNumVertices; ++i)
			{
				StaticMeshVertexData vertex;
				vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
				vertex.Normals = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
				if (mesh->mTextureCoords[0])
				{
					vertex.UV = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
				}
				else
				{
					vertex.UV = { 1.0f, 1.0f };
				}
				vertex.TilingFactor	 = glm::vec2(1.0f);
				data->vertices.push_back(vertex);
			}

            // Faces
            for (u32 i = 0; i < mesh->mNumFaces; ++i)
            {
                aiFace face = mesh->mFaces[i];
                for (u32 in = 0; in < face.mNumIndices; in++)
                {
                    data->indices.push_back(face.mIndices[in]);
                }
            }
		}

        for (u32 i = 0; i < node->mNumChildren; ++i)
        {
			ProcessStaticMeshNode(node->mChildren[i], scene, data);
        }
	}

	Ref<Texture2D> LoadEmbeddedTexture(aiTexture *texture)
	{
		// uncompressed texture
		if (texture->mHeight == 0)
		{
			int width, height, channels;
			stbi_load_from_memory(
				reinterpret_cast<unsigned char*>(texture->pcData),
				texture->mWidth,
				&width, &height, &channels, 0);

			Buffer buffer(reinterpret_cast<void*>(texture->pcData), width * height * channels);

			TextureSpecification spec;
			spec.Width = width;
			spec.Height = height;
			spec.Format = ImageFormat::RGB8;

			return Texture2D::Create(spec, buffer);
		}

		OGN_CORE_ASSERT(false, "[Model Loader] Could not process uncompressed texture!");
		return nullptr;
	}

	std::shared_ptr<StaticMeshData> ModelLoader::LoadStaticModel(const std::filesystem::path &filepath)
	{
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(filepath.generic_string().c_str(),
			  aiProcess_Triangulate | aiProcess_GenSmoothNormals
			| aiProcess_FlipUVs     | aiProcess_JoinIdenticalVertices
		);

		aiNode *node = scene->mRootNode;
		std::shared_ptr<StaticMeshData> data = std::make_shared<StaticMeshData>();
		ProcessStaticMeshNode(node, scene, data.get());
		ProcessStaticMesh(data);

		if (scene->HasTextures())
		{
			for (u32 i = 0; i < scene->mNumTextures; ++i)
			{
				aiTexture* aiTex = scene->mTextures[i];
				Ref<Texture2D> texture = LoadEmbeddedTexture(aiTex);
				data->textures.push_back(texture);
			}
		}

		return data;
	}

	void ModelLoader::ProcessStaticMesh(const std::shared_ptr<StaticMeshData> &data)
	{
		data->vertexArray = VertexArray::Create();
		data->vertexBuffer = VertexBuffer::Create(data->vertices.data(), data->vertices.size() * sizeof(StaticMeshVertexData));
		data->vertexBuffer->SetLayout
		({
			{ ShaderDataType::Float3, "aPosition"     },
			{ ShaderDataType::Float3, "aNormals"      },
			{ ShaderDataType::Float3, "aColor"        },
			{ ShaderDataType::Float2, "aUV"           },
			{ ShaderDataType::Float2, "aTilingFactor" },
			{ ShaderDataType::Float,  "aAlbedoIndex"  },
			{ ShaderDataType::Float,  "aSpecularIndex"},
		});

		data->vertexArray->AddVertexBuffer(data->vertexBuffer);
		std::shared_ptr<IndexBuffer> indexBuffer = IndexBuffer::Create(data->indices.data(), data->indices.size());
		data->vertexArray->SetIndexBuffer(indexBuffer);
	}


	// ======================================
	// Skinned Mesh

    std::shared_ptr<MeshData> ModelLoader::LoadModel(const std::filesystem::path &filepath)
    {
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(filepath.generic_string().c_str(),
			  aiProcess_Triangulate | aiProcess_GenSmoothNormals
			| aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices
		);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			OGN_CORE_ASSERT(false, importer.GetErrorString());
			return nullptr;
		}

		aiMesh *mesh = scene->mMeshes[0];
		std::shared_ptr<MeshData> data = std::make_shared<MeshData>();
		data->DiffuseTexture = Renderer::WhiteTexture;

        // load position, normal, uv
        for (u32 i = 0; i < mesh->mNumVertices; i++)
        {
            MeshVertexData vertex;

			// set default bones value
			for (i32 i = 0; i < MAX_BONE_INFLUENCE; ++i)
			{
				vertex.IDs[i] = -1;
				vertex.Weights[i] = 0.0f;
			}

			vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
			vertex.Normals = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
			if (mesh->mTextureCoords[0])
			{
				vertex.UV = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
			}
			else
			{
				vertex.UV = { 1.0f, 1.0f };
			}

			data->vertices.push_back(vertex);
        }

		// load bones weight and apply it to vertices
		ExtractBoneWeightForVertices(data.get(), mesh, scene);

        // load faces
        for (u32 faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
        {
            aiFace &face = mesh->mFaces[faceIndex];
			for (u32 indicesIndex = 0; indicesIndex < face.mNumIndices; indicesIndex++)
			{
                data->indices.push_back(face.mIndices[indicesIndex]);
			}
        }

		if (scene->HasAnimations())
		{
			for (u32 animIndex = 0; animIndex < scene->mNumAnimations; ++animIndex)
			{
				ModelAnimation anim = ModelAnimation(data.get(), scene->mAnimations[animIndex], scene);
				data->animations.push_back(anim);
			}
		}

		if (scene->HasTextures())
		{
			for (u32 i = 0; i < scene->mNumTextures; ++i)
			{
				aiTexture* aiTex = scene->mTextures[i];
				Ref<Texture2D> texture = LoadEmbeddedTexture(aiTex);
				data->textures.push_back(texture);
			}
		}

		ProcessMesh(data);
		return data;
    }

    void ModelLoader::ProcessMesh(const std::shared_ptr<MeshData> &data)
    {
        data->vertexArray = VertexArray::Create();
        data->vertexBuffer = VertexBuffer::Create(data->vertices.data(), data->vertices.size() * sizeof(MeshVertexData));

        data->vertexBuffer->SetLayout
        ({
            { ShaderDataType::Float3, "aPosition"     },
            { ShaderDataType::Float3, "aNormals"      },
            { ShaderDataType::Float2, "aUV"           },
            { ShaderDataType::Int4,   "aBoneIDs"      },
            { ShaderDataType::Float4, "aBoneWeights"  },
        });

        data->vertexArray->AddVertexBuffer(data->vertexBuffer);
		std::shared_ptr<IndexBuffer> indexBuffer = IndexBuffer::Create(data->indices.data(), data->indices.size());
        data->vertexArray->SetIndexBuffer(indexBuffer);
    }

	void ModelLoader::ExtractBoneWeightForVertices(MeshData *data, aiMesh *mesh, const aiScene *scene)
	{
		// load bones weight and apply it to vertices
		for (u32 boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
		{
			i32 boneId = -1;
			aiBone *bone = mesh->mBones[boneIndex];
			std::string boneName = bone->mName.data;

			if (data->boneInfoMap.find(boneName) == data->boneInfoMap.end())
			{
				BoneInfo newBoneInfo;
				newBoneInfo.ID = data->boneCount;
				newBoneInfo.OffsetMatrix = Math::AssimpToGlmMatrix(bone->mOffsetMatrix);

				data->boneInfoMap[boneName] = newBoneInfo;
				boneId = data->boneCount;
				data->boneCount++;
			}
			else
			{
				boneId = data->boneInfoMap[boneName].ID;
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
}