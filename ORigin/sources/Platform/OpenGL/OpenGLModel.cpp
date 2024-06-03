// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Origin\Project\Project.h"
#include "Origin\Renderer\Renderer.h"
#include "OpenGLModel.h"
#include "OpenGLMesh.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm\gtx\quaternion.hpp>

#define ALBEDO_MAP		"u_AlbedoMap"
#define SPECULAR_MAP	"u_SpecularMap"

namespace origin
{
	OpenGLModel::OpenGLModel(const std::filesystem::path & filepath)
	{
		OGN_PROFILER_RENDERING();

		Assimp::Importer importer;

		const aiScene *scene = importer.ReadFile(filepath.generic_string().c_str(),
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

		int binding = 1;
		m_Uniformbuffer = UniformBuffer::Create(sizeof(ModelBufferData), binding);

		m_Material = Material::Create();
	}

	OpenGLModel::~OpenGLModel()
	{
	}

	void OpenGLModel::Draw(int entityID)
	{
		OGN_PROFILER_RENDERING();

		for (const std::shared_ptr<Mesh>& mesh : m_Meshes)
		{
			m_ModelBufferData.EntityID = entityID;
			m_Uniformbuffer->Bind();
			m_Uniformbuffer->SetData(&m_ModelBufferData, sizeof(ModelBufferData));

			if (m_Material)
			{
				m_Material->Bind();

				for (auto& t : m_Textures)
				{
					if (t.find(aiTextureType_DIFFUSE) != t.end())
					{
						t.at(aiTextureType_DIFFUSE)->Bind(0);
						m_Material->m_Shader->SetInt(ALBEDO_MAP, 0);
					}

					if (t.find(aiTextureType_SPECULAR) != t.end())
					{
						t.at(aiTextureType_SPECULAR)->Bind(1);
						m_Material->m_Shader->SetInt(SPECULAR_MAP, 1);
					}
				}

				if (m_Textures.empty())
				{
					Renderer::WhiteTexture->Bind(0);
					m_Material->m_Shader->SetInt(ALBEDO_MAP, 0);
					Renderer::WhiteTexture->Bind(1);
					m_Material->m_Shader->SetInt(SPECULAR_MAP, 1);
				}

				mesh->Draw();
				m_Material->Unbind();
			}
			else
			{
				mesh->Draw();
			}
			m_Uniformbuffer->Unbind();
		}
	}

	void OpenGLModel::Draw()
	{
		OGN_PROFILER_RENDERING();

		for (const std::shared_ptr<Mesh>& mesh : m_Meshes)
		{
			if (m_Material)
			{
				m_Material->Bind();

				for (auto& t : m_Textures)
				{
					if (t.find(aiTextureType_DIFFUSE) != t.end())
					{
						t.at(aiTextureType_DIFFUSE)->Bind(0);
						m_Material->m_Shader->SetInt(ALBEDO_MAP, 0);
					}

					if (t.find(aiTextureType_SPECULAR) != t.end())
					{
						t.at(aiTextureType_SPECULAR)->Bind(1);
						m_Material->m_Shader->SetInt(SPECULAR_MAP, 1);
					}
				}

				if (m_Textures.empty())
				{
					Renderer::WhiteTexture->Bind(0);
					m_Material->m_Shader->SetInt(ALBEDO_MAP, 0);
					Renderer::WhiteTexture->Bind(1);
					m_Material->m_Shader->SetInt(SPECULAR_MAP, 1);
				}

				mesh->Draw();
				m_Material->Unbind();
			}
			else
			{
				mesh->Draw();
			}
		}
	}

	void OpenGLModel::SetTransform(const glm::mat4& transform)
	{
		m_ModelBufferData.Transform = transform;
	}

	void OpenGLModel::SetMaterial(std::shared_ptr<Material> mat)
	{
		m_Material = mat;
	}

	void OpenGLModel::RemoveMaterial()
	{
		OGN_PROFILER_RENDERING();

		m_Material = Renderer::GetMaterial("DefaultMesh");
	}

	void OpenGLModel::ProcessNode(aiNode *node, const aiScene *scene)
	{
		OGN_PROFILER_RENDERING();

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
		OGN_PROFILER_RENDERING();

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
		
		if (m_Material)
		{
			if (mesh->mMaterialIndex > 0)
			{
				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
				std::unordered_map<aiTextureType, std::shared_ptr<Texture2D>> diffuseMaps = m_Material->LoadTextures(m_Filepath.generic_string(), material, aiTextureType_DIFFUSE);
				m_Textures.push_back(diffuseMaps);
				std::unordered_map<aiTextureType, std::shared_ptr<Texture2D>> specMaps = m_Material->LoadTextures(m_Filepath.generic_string(), material, aiTextureType_SPECULAR);
				m_Textures.push_back(specMaps);
			}
		}

		return std::make_shared<OpenGLMesh>(vertices, indices);
	}
}