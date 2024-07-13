// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"

#include "OpenGLModel.h"
#include "Origin/Project/Project.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Scene/EntityManager.h"
#include "Origin/Renderer/Renderer.h"
#include "Origin/Profiler/Profiler.h"

#include <glm/gtx/quaternion.hpp>

namespace origin
{
	OpenGLModel::OpenGLModel(const char *filepath, Scene *scene)
		: m_Filepath(filepath), m_Scene(scene)
	{
		OGN_PROFILER_RENDERING();

		Assimp::Importer importer;
		const aiScene *aiScene = importer.ReadFile(filepath,
			aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices
		);

		OGN_CORE_INFO("[OpenGLModel] Trying to load {0}", filepath);
		if (aiScene == nullptr || aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiScene->mRootNode)
		{
			OGN_CORE_ERROR("[OpenGLModel] {0}", importer.GetErrorString());
			return;
		}

		ProcessNode(aiScene->mRootNode, aiScene);
	}

	OpenGLModel::~OpenGLModel()
	{
	}

	void OpenGLModel::SetTransform(const glm::mat4 &transform)
	{
	}

	void OpenGLModel::ProcessNode(const aiNode *node, const aiScene *scene)
	{
		OGN_PROFILER_RENDERING();

		for (uint32_t n = 0; n < node->mNumMeshes; n++)
		{
			// Get each meshes
			aiMesh *aiMesh = scene->mMeshes[node->mMeshes[n]];
			
			std::vector<MeshVertex> vertices;
			std::vector<uint32_t> indices;

			for (uint32_t i = 0; i < aiMesh->mNumVertices; i++)
			{
				MeshVertex vertex;
				vertex.Position = { aiMesh->mVertices[i].x, aiMesh->mVertices[i].y, aiMesh->mVertices[i].z };

				if (aiMesh->HasNormals())
					vertex.Normal = { aiMesh->mNormals[i].x, aiMesh->mNormals[i].y, aiMesh->mNormals[i].z };

				if (aiMesh->mTextureCoords[0])
					vertex.TexCoord = { aiMesh->mTextureCoords[0][i].x, aiMesh->mTextureCoords[0][i].y };
				else
					vertex.TexCoord = { 0.0f, 0.0f };

				vertices.push_back(vertex);
			}

			for (uint32_t f = 0; f < aiMesh->mNumFaces; f++)
			{
				aiFace face = aiMesh->mFaces[f];
				for (uint32_t in = 0; in < face.mNumIndices; in++)
					indices.push_back(face.mIndices[in]);
			}

#if 0
			if (this->GetMaterial())
			{
				if (aiMesh->mMaterialIndex > 0)
				{
					aiMaterial *aiMat = scene->mMaterials[aiMesh->mMaterialIndex];
					std::shared_ptr<Material> material = AssetManager::GetAsset<Material>(GetMaterial());
					Utils::LoadMatTextures(&material->Textures, m_Filepath, aiMat, aiTextureType_DIFFUSE);
					Utils::LoadMatTextures(&material->Textures, m_Filepath, aiMat, aiTextureType_SPECULAR);
				}
			}
#endif

			// Creating Entity to scene
			StaticMeshComponent m;
			m.Name = node->mName.C_Str();
			m.HMesh = Mesh::Create(vertices, indices);
			OGN_CORE_TRACE("[Model] Pushing meshses {0}", m.Name);
			
			m_Meshes.push_back(std::move(m));
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene);
		}
	}
}