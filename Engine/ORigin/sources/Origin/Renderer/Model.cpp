#include "pch.h"
#include "Model.h"

#include "Buffer.h"
//#include "Renderer.h"
#include "VertexArray.h"

#include <assimp/Importer.hpp>

namespace origin
{
	static void ProcessStaticMesh(const Ref<StaticMeshData> &meshData)
	{
		meshData->vertexArray = VertexArray::Create();
		meshData->vertexBuffer = VertexBuffer::Create(meshData->vertices.data(), meshData->vertices.size() * sizeof(StaticMeshVertexData));
		meshData->vertexBuffer->SetLayout
		({
			{ ShaderDataType::Float3, "aPosition"     },
			{ ShaderDataType::Float3, "aNormals"      },
			{ ShaderDataType::Float3, "aColor"        },
			{ ShaderDataType::Float2, "aUV"           },
			{ ShaderDataType::Float2, "aTilingFactor" },
			{ ShaderDataType::Float,  "aAlbedoIndex"  },
			{ ShaderDataType::Float,  "aSpecularIndex"},
			});
		meshData->vertexArray->AddVertexBuffer(meshData->vertexBuffer);
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(meshData->indices.data(), meshData->indices.size());
		meshData->vertexArray->SetIndexBuffer(indexBuffer);
	}

	static std::vector<Ref<StaticMeshData>> ProcessStaticMeshNode(aiNode* node, const aiScene* scene)
	{
		std::vector<Ref<StaticMeshData>> meshes(node->mNumMeshes);
		for (u32 meshIndex = 0; meshIndex < node->mNumMeshes; ++meshIndex)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[meshIndex]];

			Ref<StaticMeshData> meshData = CreateRef<StaticMeshData>();
			meshData->Name = mesh->mName.C_Str();

			// Vertices
			meshData->vertices.resize(mesh->mNumVertices);
			for (u32 i = 0; i < mesh->mNumVertices; ++i)
			{
				StaticMeshVertexData vertex;
				vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
				vertex.Normals = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
				if (mesh->mTextureCoords[0]) vertex.UV = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
				else vertex.UV = { 1.0f, 1.0f };
				vertex.TilingFactor = glm::vec2(1.0f);
				meshData->vertices[i] = vertex;
			}

			// Faces
			for (u32 i = 0; i < mesh->mNumFaces; ++i)
			{
				aiFace face = mesh->mFaces[i];
				for (u32 in = 0; in < face.mNumIndices; in++)
					meshData->indices.push_back(face.mIndices[in]);
			}

			ProcessStaticMesh(meshData);
			meshes[meshIndex] = meshData;
		}

		for (u32 i = 0; i < node->mNumChildren; ++i)
		{
			std::vector<Ref<StaticMeshData>> childMeshes = ProcessStaticMeshNode(node->mChildren[i], scene);
			meshes.insert(meshes.end(), childMeshes.begin(), childMeshes.end());
		}

		return meshes;
	}


	StaticModel::StaticModel(const std::filesystem::path& filepath)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath.generic_string().c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

		aiNode* node = scene->mRootNode;
		m_Meshes = ProcessStaticMeshNode(node, scene);
	}


	// ============
	// ============ Skeletal Model

	static void ExtractBoneWeightForVertices(Ref<MeshData> &data, aiMesh* mesh, const aiScene* scene)
	{
		// load bones weight and apply it to vertices
		for (u32 boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
		{
			i32 boneId = -1;
			aiBone* bone = mesh->mBones[boneIndex];
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
				MeshVertexData& vertex = data->vertices[vertexId];

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
		for (auto& vertex : data->vertices)
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

	static void ProcessMesh(const Ref<MeshData> &data)
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
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(data->indices.data(), data->indices.size());
		data->vertexArray->SetIndexBuffer(indexBuffer);
	}

	static std::vector<Ref<MeshData>> ProcessMeshNode(aiNode* node, const aiScene* scene)
	{
		std::vector<Ref<MeshData>> meshes(node->mNumMeshes);
		for (u32 meshIndex = 0; meshIndex < node->mNumMeshes; ++meshIndex)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[meshIndex]];

			Ref<MeshData> meshData = CreateRef<MeshData>();
			meshData->Name = mesh->mName.C_Str();

			// Vertices
			meshData->vertices.resize(mesh->mNumVertices);
			for (u32 i = 0; i < mesh->mNumVertices; ++i)
			{
				MeshVertexData vertex;
				vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
				vertex.Normals = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
				if (mesh->mTextureCoords[0]) vertex.UV = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
				else vertex.UV = { 1.0f, 1.0f };
				meshData->vertices[i] = vertex;
			}

			// Faces
			for (u32 i = 0; i < mesh->mNumFaces; ++i)
			{
				aiFace face = mesh->mFaces[i];
				for (u32 in = 0; in < face.mNumIndices; in++)
					meshData->indices.push_back(face.mIndices[in]);
			}

			//if (scene->HasTextures())
			//{
			//	for (u32 i = 0; i < scene->mNumTextures; ++i)
			//	{
			//		aiTexture* aiTex = scene->mTextures[i];
			//		meshData->diffuseTexture = LoadEmbeddedTexture(aiTex);
			//		//Ref<Texture2D> texture = LoadEmbeddedTexture(aiTex);
			//		//data->textures.push_back(texture);
			//	}
			//}
			//else
			{
				//meshData->diffuseTexture = Renderer::WhiteTexture;
			}

			ExtractBoneWeightForVertices(meshData, mesh, scene);

			ProcessMesh(meshData);
			meshes[meshIndex] = meshData;
		}

		for (u32 i = 0; i < node->mNumChildren; ++i)
		{
			std::vector<Ref<MeshData>> childMeshes = ProcessMeshNode(node->mChildren[i], scene);
			meshes.insert(meshes.end(), childMeshes.begin(), childMeshes.end());
		}

		return meshes;
	}

	SkeletalModel::SkeletalModel(const std::filesystem::path& filepath)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath.generic_string().c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
		aiNode* node = scene->mRootNode;
		m_Meshes = ProcessMeshNode(node, scene);

		if (scene->HasAnimations())
		{
			for (u32 animIndex = 0; animIndex < scene->mNumAnimations; ++animIndex)
			{
				ModelAnimation anim = ModelAnimation(m_Meshes, scene->mAnimations[animIndex], scene);
				m_Animations.push_back(anim);
			}
		}
	}
}
