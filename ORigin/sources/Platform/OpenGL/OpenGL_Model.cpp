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
		//const aiScene* scene = importer.ReadFile(filepath.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		const aiScene* scene = importer.ReadFile(filepath.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
		OGN_CORE_INFO("MODEL: Trying to load \"{}\"", filepath);
		if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			OGN_CORE_ERROR("MESH: ASSIMP: {}", importer.GetErrorString());
			return;
		}

		ProcessNode(scene->mRootNode, scene);
	}

	OpenGLModel::~OpenGLModel()
	{
		m_Material->m_Shader.reset();
		m_Material.reset();
	}

	void OpenGLModel::Draw()
	{
		for (const std::shared_ptr<Mesh>& mesh : m_Meshes)
		{
			if (mesh->IsLoaded())
				mesh->Draw(m_Material->m_Shader);
		}
	}

	void OpenGLModel::Draw(const glm::mat4& modelTransform, const Camera& camera, int entityID)
	{
		// Applying Main Shader Uniforms
		m_Material->EnableShader();

		m_Material->SetMatrix("uModelTransform", modelTransform);
		m_Material->SetMatrix("uViewProjection", camera.GetViewProjection());
		m_Material->SetVector("uCameraPosition", camera.GetPosition());

		m_Material->SetVector("uColor", m_Material->Color);
		m_Material->SetBool("uHasTextures", m_Material->HasTexture);
		m_Material->SetVector("material.TilingFactor", m_Material->TilingFactor);
		m_Material->SetFloat("material.Shininess", m_Material->Shininess);

		m_Material->SetInt("uEntityID", entityID);

		// Draw Mesh
		Draw();
		m_Material->DisableShader();
	}

	void OpenGLModel::Draw(const glm::mat4& modelTransform, const Camera& camera, const glm::mat4& cameraTransform, int entityID)
	{
		// Applying Main Shader Uniforms
		m_Material->EnableShader();
		m_Material->SetMatrix("uModelTransform", modelTransform);
		m_Material->SetMatrix("uViewProjection", camera.GetViewProjection() * glm::inverse(cameraTransform));
		m_Material->SetVector("uCameraPosition", camera.GetPosition());

		m_Material->SetVector("uColor", m_Material->Color);
		m_Material->SetBool("uHasTextures", m_Material->HasTexture);
		m_Material->SetVector("material.TilingFactor", m_Material->TilingFactor);
		m_Material->SetFloat("material.Shininess", m_Material->Shininess);

		m_Material->SetInt("uEntityID", entityID);

		// Draw Mesh
		Draw();

		m_Material->DisableShader();
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
		std::vector<std::shared_ptr<Texture2D>> textures;

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

				/*vertex.Tangents = glm::vec3(
					mesh->mTangents[i].x,
					mesh->mTangents[i].y,
					mesh->mTangents[i].z
				);

				vertex.Bitangents = glm::vec3(
					mesh->mBitangents[i].x,
					mesh->mBitangents[i].y,
					mesh->mBitangents[i].z
				);*/
			}
			else
				vertex.TexCoord = glm::vec2(0.0f);

			vertices.push_back(vertex);
		}

		for (uint32_t f = 0; f < mesh->mNumFaces; f++)
		{
			aiFace face = mesh->mFaces[f];
			for (uint32_t in = 0; in < face.mNumIndices; in++)
				indices.push_back(face.mIndices[in]);
		}

		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			std::vector<std::shared_ptr<Texture2D>> diffuseMaps = m_Material->LoadTextures(m_Filepath, material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			std::vector<std::shared_ptr<Texture2D>> specularMaps = m_Material->LoadTextures(m_Filepath, material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			//std::vector<std::shared_ptr<Texture2D>> normalMaps = m_Material->LoadTextures(m_Filepath, material, aiTextureType_AMBIENT, "texture_normal");
			//textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			// 
			//std::vector<std::shared_ptr<Texture2D>> heightMaps = m_Material->LoadTextures(m_Filepath, material, aiTextureType_HEIGHT, "texture_height");
			//textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
		}

		return std::make_shared<OpenGLMesh>(vertices, indices, textures, m_Filepath);
	}
}