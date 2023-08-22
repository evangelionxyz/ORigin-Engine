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
		const aiScene* scene = importer.ReadFile(filepath.c_str(), aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);
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

	void OpenGLModel::DrawMesh()
	{
		for (const std::shared_ptr<Mesh>& mesh : m_Meshes)
		{
			if (mesh->IsLoaded())
				mesh->Draw(m_Material->m_Shader);
		}
	}

	void OpenGLModel::Draw(const glm::mat4& modelTransform, const EditorCamera& camera, int entityID)
	{
		// Applying Main Shader Uniforms
		m_Material->EnableShader();

		m_Material->SetBool("uHasOneTexture", m_Material->Texture != nullptr);
		
		if (m_Material->Texture)
		{
			m_Material->Texture->Bind(0);
			m_Material->SetInt("material.Texture", m_Material->Texture->GetIndex());
			m_Material->SetVector("material.TilingFactor", m_Material->TilingFactor);
		}

		m_Material->SetMatrix("uModel", modelTransform);
		m_Material->SetMatrix("uView", camera.GetViewMatrix());
		m_Material->SetMatrix("uProjection", camera.GetProjection());
		m_Material->SetVector("uCameraPosition", camera.GetPosition());

		m_Material->SetVector("uColor", m_Material->Color);
		m_Material->SetBool("uHasTextures", m_Material->HasTexture);
		m_Material->SetFloat("material.Shininess", m_Material->Shininess);

		m_Material->SetInt("uEntityID", entityID);

		// Draw Mesh
		DrawMesh();

		if (m_Material->Texture)
		{
			m_Material->Texture->Unbind();
		}

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
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<std::shared_ptr<Texture2D>> textures;

		for (uint32_t i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;

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

			vertices.push_back(vertex);
		}

		for (uint32_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (uint32_t j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			std::vector<std::shared_ptr<Texture2D>> diffuseMaps = m_Material->LoadTextures(m_Filepath, material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			std::vector<std::shared_ptr<Texture2D>> specularMaps = m_Material->LoadTextures(m_Filepath, material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		OGN_CORE_TRACE("MODEL: HAS TEXTURE {}", m_Material->HasTexture);

		return std::make_shared<OpenGLMesh>(vertices, indices, textures, m_Filepath);
	}
}