// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "OpenGL_Model.h"
#include "OpenGL_Mesh.h"

#include "Origin\Project\Project.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm\gtx\quaternion.hpp>

namespace origin
{
	

	OpenGLModel::OpenGLModel(const std::string& filepath, std::shared_ptr<Material> material)
		: m_Filepath(filepath), m_Material(material)
	{
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(filepath.c_str(),
			aiProcess_Triangulate
			| aiProcess_GenSmoothNormals
			| aiProcess_FlipUVs
			| aiProcess_JoinIdenticalVertices
		);

		OGN_CORE_INFO("MODEL: Trying to load \"{}\"", filepath);
		if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			OGN_CORE_ERROR("MESH: ASSIMP: {}", importer.GetErrorString());
			return;
		}

		ProcessNode(scene->mRootNode, scene);

		ModelUbo = UniformBuffer::Create(sizeof(ModelBuffer), 1);
	}

	OpenGLModel::~OpenGLModel()
	{
	}

	void OpenGLModel::Draw(const glm::mat4& transform, int entityID)
	{
		for (const std::shared_ptr<Mesh>& mesh : m_Meshes)
		{
			if (mesh->IsLoaded())
			{
				m_Material->OnRender();
				m_Material->m_Shader->Enable();
				m_ModelBuffer.Transform = transform;
				m_ModelBuffer.EntityID = entityID;

				ModelUbo->Bind();
				ModelUbo->SetData(&m_ModelBuffer, sizeof(ModelBuffer));
				mesh->Draw(m_Material->m_Shader);
				ModelUbo->Unbind();
				m_Material->m_Shader->Disable();
			}
		}
	}

	void OpenGLModel::Draw()
	{
		for (const std::shared_ptr<Mesh>& mesh : m_Meshes)
		{
			if (mesh->IsLoaded())
			{
				mesh->Draw();
			}
		}
	}

	void OpenGLModel::ProcessNode(aiNode* node, const aiScene* scene)
	{
		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_Meshes.push_back(ProcessMesh(mesh, scene));
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene);
		}
	}

	std::shared_ptr<Mesh> OpenGLModel::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<MeshVertex> vertices;
		std::vector<uint32_t> indices;

		for (uint32_t i = 0; i < mesh->mNumVertices; i++)
		{
			MeshVertex vertex;

			vertex.Position = glm::vec3(
				mesh->mVertices[i].x,
				mesh->mVertices[i].y,
				mesh->mVertices[i].z
			);

			if (mesh->HasNormals())
			{
				vertex.Normal = glm::vec3(
					mesh->mNormals[i].x,
					mesh->mNormals[i].y,
					mesh->mNormals[i].z
				);
			}

			if (mesh->mTextureCoords[0])
			{
				vertex.TexCoord = glm::vec2(
					mesh->mTextureCoords[0][i].x,
					mesh->mTextureCoords[0][i].y
				);
			}
			else
			{
				vertex.TexCoord = glm::vec2(0.0f);
			}

			vertices.push_back(vertex);
		}

		for (uint32_t f = 0; f < mesh->mNumFaces; f++)
		{
			aiFace face = mesh->mFaces[f];
			for (uint32_t in = 0; in < face.mNumIndices; in++)
				indices.push_back(face.mIndices[in]);
		}

		std::vector<std::unordered_map<aiTextureType, std::shared_ptr<Texture2D>>> textures;
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			std::unordered_map<aiTextureType, std::shared_ptr<Texture2D>> diffuseMaps = m_Material->LoadTextures(m_Filepath, material, aiTextureType_DIFFUSE);
			textures.push_back(diffuseMaps);
			std::unordered_map<aiTextureType, std::shared_ptr<Texture2D>> specMaps = m_Material->LoadTextures(m_Filepath, material, aiTextureType_SPECULAR);
			textures.push_back(specMaps);
		}

		return std::make_shared<OpenGLMesh>(vertices, indices, textures, m_Filepath);
	}
}