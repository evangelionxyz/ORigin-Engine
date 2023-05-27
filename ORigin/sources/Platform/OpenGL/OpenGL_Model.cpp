// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "OpenGL_Model.h"
#include "OpenGL_Mesh.h"

#include "Origin\Project\Project.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm\gtx\quaternion.hpp>

namespace Origin
{
	OpenGLModel::OpenGLModel(const std::string& filepath, std::shared_ptr<Shader>& shader)
		: m_Shader(shader)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath.c_str(), aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);
		OGN_CORE_INFO("MODEL: Trying to load \"{}\"", filepath);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			OGN_CORE_ERROR("MESH:ASSIMP: {}", importer.GetErrorString());
			return;
		}

		m_Texture = Texture2D::Create("SandboxProject/Assets/Textures/checkerboard.jpg");
		m_Texture->Bind();

		m_Shader->Bind();
		ProcessNode(scene->mRootNode, scene);
	}

	OpenGLModel::OpenGLModel(std::shared_ptr<Shader>& shader)
	{
	}

	OpenGLModel::~OpenGLModel()
	{
		m_Indices.clear();
		m_Vertices.clear();

		for (auto& mesh : m_Meshes)
			mesh.reset();

		m_Meshes.clear();
	}

	void OpenGLModel::Draw()
	{
		m_Texture->Bind();
		for (auto& mesh : m_Meshes)
		{
			if(mesh->IsLoaded())
				mesh->Draw();
		}

		m_Shader->Unbind();
	}

	void OpenGLModel::Draw(const EditorCamera& camera)
	{
		m_Shader->Bind();
		glm::mat4 transform = camera.GetViewProjection() * transform;

		m_Shader->SetMatrix("uModel", transform);
		m_Shader->SetMatrix("uView", camera.GetViewMatrix());
		m_Shader->SetMatrix("uProjection", camera.GetProjection());
		m_Shader->SetVector("uCameraPosition", camera.GetPosition());
		Draw();
	}

	void OpenGLModel::Draw(const glm::mat4& transform, const EditorCamera& camera, int entityID)
	{
		m_EntityID = entityID;

		m_Shader->Bind();
		m_Shader->SetMatrix("uModel", transform);
		m_Shader->SetMatrix("uView", camera.GetViewMatrix());
		m_Shader->SetMatrix("uProjection", camera.GetProjection());

		m_Shader->SetInt("uEntityID", m_EntityID);

		Draw();
	}

	void OpenGLModel::Draw(const glm::mat4& transform, const Camera* camera, int entityID)
	{
		m_EntityID = entityID;

		m_Shader->Bind();
		m_Shader->SetMatrix("uModel", transform);
		m_Shader->SetMatrix("uView", camera->GetViewMatrix());
		m_Shader->SetMatrix("uProjection", camera->GetProjection());

		m_Shader->SetInt("uEntityID", m_EntityID);

		Draw();
	}

	void OpenGLModel::LoadLighting(const glm::vec3& position, const glm::vec4& color, float ambient)
	{
		m_LightPosition = position;
		m_LightColor = color;
		m_Ambient = ambient;
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
		for (uint32_t i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			if (mesh->mTextureCoords[0])
				vertex.TexCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			else vertex.TexCoord = glm::vec2(0.0f);

			m_Vertices.push_back(vertex);
		}

		for (uint32_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (uint32_t j = 0; j < face.mNumIndices; j++)
			{
				m_Indices.push_back(face.mIndices[j]);
			}
		}

		return std::make_shared<OpenGLMesh>(m_Vertices, m_Indices);
	}

}