#pragma once
#include "Origin\Renderer\Model.h"
#include "Origin\Renderer\Mesh.h"
#include "Origin\Renderer\Texture.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include <vector>

namespace Origin
{
	class OpenGLModel : public Model
	{
	public:
		OpenGLModel() = default;
		OpenGLModel(const std::string& filepath, std::shared_ptr<Shader>& shader);
		OpenGLModel(std::shared_ptr<Shader>& shader);

		void Draw(const glm::mat4& viewProjection) override;
		std::shared_ptr<Shader>& GetShader() override { return m_Shader; }

	private:
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;

		std::vector<std::shared_ptr<Mesh>> m_Meshes;
		std::vector<std::shared_ptr<Texture2D>> m_Textures;
		std::shared_ptr<Shader> m_Shader;

		void ProcessNode(aiNode* node, const aiScene* scene);
		std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);
	};
}


