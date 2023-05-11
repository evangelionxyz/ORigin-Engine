#include "pch.h"
#include "OpenGL_Model.h"
#include "OpenGL_Mesh.h"

namespace Origin
{
	OpenGLModel::OpenGLModel(const std::string& filepath, std::shared_ptr<Shader>& shader)
		: m_Shader(shader)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath.c_str(), aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);
		OGN_CORE_INFO("OPENGL_MESH: LOADING MODEL: \"{}\"", filepath);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			OGN_CORE_ERROR("MESH:ASSIMP: {}", importer.GetErrorString());
			return;
		}

		shader->Bind();
		ProcessNode(scene->mRootNode, scene);
	}

	OpenGLModel::OpenGLModel(std::shared_ptr<Shader>& shader)
	{
	}

	void OpenGLModel::Draw(const glm::mat4& viewProjection)
	{
		m_Shader->Bind();
		m_Shader->SetMatrix("uViewProjection", viewProjection);

		for (auto mesh : m_Meshes)
			mesh->Draw();

		m_Shader->Unbind();
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

		// =======================================================
		// ================== Processing Vertex ==================
		// =======================================================

		for (uint32_t i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

			// Get Normals
			if (mesh->HasNormals())
			{
				vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			}
			else
			{
				vertex.Normal = glm::vec3(0.0f);
			}

			// Get TexCoords
			if (mesh->mTextureCoords[0])
			{
				vertex.TexCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			}
			else
			{
				vertex.TexCoord = glm::vec2(0.0f);
			}

			m_Vertices.push_back(vertex);
		}

		// =======================================================
		// ================== Processing Index ===================
		// =======================================================

		for (uint32_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (uint32_t j = 0; j < face.mNumIndices; j++)
				m_Indices.push_back(face.mIndices[j]);
		}

		return std::make_shared<OpenGLMesh>(m_Vertices, m_Indices);
	}

}