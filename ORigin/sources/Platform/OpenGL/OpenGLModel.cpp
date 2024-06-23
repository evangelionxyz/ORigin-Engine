// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "OpenGLModel.h"
#include "OpenGLMesh.h"
#include "Origin/Project/Project.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Scene/EntityManager.h"
#include "Origin/Renderer/Renderer.h"

#include <glm/gtx/quaternion.hpp>

namespace origin
{
	OpenGLModel::OpenGLModel(const std::filesystem::path &filepath, Scene *scene)
		: m_Scene(scene), m_Filepath(filepath)
	{
		OGN_PROFILER_RENDERING();

		Assimp::Importer importer;
		const aiScene *aiScene = importer.ReadFile(filepath.generic_string().c_str(),
			aiProcess_Triangulate | aiProcess_GenSmoothNormals
			| aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices
		);

		OGN_CORE_INFO("MODEL: Trying to load \"{}\"", filepath);
		if (aiScene == nullptr || aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiScene->mRootNode)
		{
			OGN_CORE_ERROR("MESH: ASSIMP: {}", importer.GetErrorString());
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

	void OpenGLModel::SetMaterial(AssetHandle handle)
	{
		for (auto id : m_MeshEntities)
		{
			Entity entity = m_Scene->GetEntityWithUUID(id);
			entity.GetComponent<StaticMeshComponent>().HMaterial = handle;
		}
	}

	AssetHandle OpenGLModel::GetMaterial()
	{
		Entity entity = m_Scene->GetEntityWithUUID(m_MeshEntities[0]);
		return entity.GetComponent<StaticMeshComponent>().HMaterial;
	}

	void OpenGLModel::ProcessNode(aiNode *node, const aiScene *aiScene)
	{
		OGN_PROFILER_RENDERING();

		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			// Get each meshes
			aiMesh* aiMesh = aiScene->mMeshes[node->mMeshes[i]];
			Entity entity = EntityManager::CreateEntity(node->mName.C_Str(), m_Scene);
			StaticMeshComponent &m = entity.AddComponent<StaticMeshComponent>();
			m.Name = node->mName.C_Str();
			m.Mesh = ProcessMesh(aiMesh, aiScene);
			OGN_CORE_TRACE("{}", m.Name);
			m_MeshEntities.push_back(entity.GetUUID());
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], aiScene);
		}
	}

	std::shared_ptr<Mesh> OpenGLModel::ProcessMesh(aiMesh *mesh, const aiScene *scene)
	{
		OGN_PROFILER_RENDERING();

		std::vector<MeshVertex> vertices;
		std::vector<uint32_t> indices;

		for (uint32_t i = 0; i < mesh->mNumVertices; i++)
		{
			MeshVertex vertex;

			vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

			if (mesh->HasNormals())
				vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

			if (mesh->mTextureCoords[0])
				vertex.TexCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
			else
				vertex.TexCoord = { 0.0f, 0.0f };

			vertices.push_back(vertex);
		}

		for (uint32_t f = 0; f < mesh->mNumFaces; f++)
		{
			aiFace face = mesh->mFaces[f];
			for (uint32_t in = 0; in < face.mNumIndices; in++)
				indices.push_back(face.mIndices[in]);
		}

		if (this->GetMaterial())
		{
			if (mesh->mMaterialIndex > 0)
			{
				aiMaterial *aiMat = scene->mMaterials[mesh->mMaterialIndex];
				std::shared_ptr<Material> material = AssetManager::GetAsset<Material>(GetMaterial());
				Utils::LoadMatTextures(&material->Textures, m_Filepath.generic_string(), aiMat, aiTextureType_DIFFUSE);
				Utils::LoadMatTextures(&material->Textures, m_Filepath.generic_string(), aiMat, aiTextureType_SPECULAR);
			}
		}

		return std::make_shared<OpenGLMesh>(vertices, indices);
	}
}