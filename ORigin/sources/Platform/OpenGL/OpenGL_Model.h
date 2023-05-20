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
		virtual ~OpenGLModel() override;

		void Draw() override;
		void Draw(const EditorCamera& camera) override;
		void Draw(const glm::mat4& transform, const EditorCamera& camera, int entityID = -1) override;

		void LoadLighting(const glm::vec3& position, const glm::vec4& color, float ambient) override;

		std::shared_ptr<Shader>& GetShader() override { return m_Shader; }

	private:
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;

		std::vector<std::shared_ptr<Mesh>> m_Meshes;
		std::vector<std::shared_ptr<Texture2D>> m_Textures;
		std::shared_ptr<Shader> m_Shader;

		void ProcessNode(aiNode* node, const aiScene* scene);
		std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);
		int m_EntityID = -1;

		std::shared_ptr<Texture2D> m_Texture;
		
		glm::vec4 m_LightColor;
		glm::vec3 m_CameraPosition, m_LightPosition;
		float m_Ambient;
	};
}


