// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "ModelLoader.h"

#include "Origin/Math/Math.h"

namespace origin
{
	static void ProcessStaticMeshNode(aiNode *node, const aiScene *scene, StaticMeshData *data)
	{
		for (uint32_t meshIndex = 0; meshIndex < node->mNumMeshes; ++meshIndex)
		{
			aiMesh *mesh = scene->mMeshes[node->mMeshes[meshIndex]];

			// Vertices
			for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
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
            for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
            {
                aiFace face = mesh->mFaces[i];
                for (uint32_t in = 0; in < face.mNumIndices; in++)
                {
                    data->indices.push_back(face.mIndices[in]);
                }
            }
		}

        for (uint32_t i = 0; i < node->mNumChildren; ++i)
        {
			ProcessStaticMeshNode(node->mChildren[i], scene, data);
        }
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
	// Dynamic Mesh

    std::shared_ptr<MeshData> ModelLoader::LoadModel(const std::filesystem::path &filepath)
    {
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(filepath.generic_string().c_str(),
			  aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			OGN_CORE_ASSERT(false, importer.GetErrorString());
			return nullptr;
		}

		aiMesh *mesh = scene->mMeshes[0];
		std::shared_ptr<MeshData> data = std::make_shared<MeshData>();

        // load position, normal, uv
        for (uint32_t i = 0; i < mesh->mNumVertices; i++)
        {
            MeshVertexData vertex;

			// set default bones value
			for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
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
        for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
        {
            aiFace &face = mesh->mFaces[faceIndex];
			for (uint32_t indicesIndex = 0; indicesIndex < face.mNumIndices; indicesIndex++)
			{
                data->indices.push_back(face.mIndices[indicesIndex]);
			}
        }

		// load animations
		if (scene->HasAnimations())
		{
			for (uint32_t animIndex = 0; animIndex < scene->mNumAnimations; ++animIndex)
			{
				ModelAnimation anim = ModelAnimation(data.get(), scene->mAnimations[animIndex], scene);
				data->animations.push_back(anim);
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
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
		{
			int boneId = -1;
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

			OGN_CORE_ASSERT(boneId != -1, "Invalid bone id");

			auto weights = bone->mWeights;
			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex)
			{
				int vertexId = weights[weightIndex].mVertexId;
				float weight = weights[weightIndex].mWeight;

				OGN_CORE_ASSERT(vertexId < data->vertices.size(), "Invalid vertex id");
				MeshVertexData &vertex = data->vertices[vertexId];

				for (int boneIndex = 0; boneIndex < MAX_BONE_INFLUENCE; ++boneIndex)
				{
					if (vertex.IDs[boneIndex] < 0)
					{
						vertex.Weights[boneIndex] = weight;
						vertex.IDs[boneIndex] = boneId;
					}
				}
			}
		}
	}
}