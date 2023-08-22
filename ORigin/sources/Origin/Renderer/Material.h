#pragma once
#include "Shader.h"
#include "Texture.h"

#include <assimp\scene.h>
#include <assimp\postprocess.h>

namespace origin {

	class Material
	{
	public:
		Material() = default;
		Material(std::string name);

		void LoadShader(std::shared_ptr<Shader> shader);
		void LoadShader(const std::string& filepath, bool enablerSpirv = false, bool recompile = false);

		const std::string& GetShaderFilepath() const { return m_ShaderFilepath; }
		const std::string& GetMaterialName() const { return m_Name; }

		bool RefreshShader();

		void EnableShader();
		void DisableShader();

		void SetFloat(const std::string& name, float value);
		void SetInt(const std::string& name, int value);
		void SetBool(const std::string& name, bool condition);

		void SetVector(const std::string& name, const glm::vec2& vectors);
		void SetVector(const std::string& name, const glm::vec3& vectors);
		void SetVector(const std::string& name, const glm::vec4& vectors);

		void SetMatrix(const std::string& name, const glm::mat2& matrices);
		void SetMatrix(const std::string& name, const glm::mat3& matrices);
		void SetMatrix(const std::string& name, const glm::mat4& matrices);

		void LoadTextureFromFile(const std::string& filepath);
		std::shared_ptr<Texture2D> Texture;
		glm::vec2 TilingFactor = glm::vec2(1.0f);

		glm::vec4 Color = glm::vec4(1.0f);
		float Shininess = 32.0f;

		bool HasTexture = false;
		std::shared_ptr<Shader> m_Shader;
		std::vector<std::shared_ptr<Texture2D>> LoadTextures(const std::string& modelFilepath, aiMaterial* mat, aiTextureType type, std::string typeName);

		static std::shared_ptr<Material> Create(const std::string& name);

		int m_SpotLightCount = 0;
		int m_PointLightCount = 0;

	private:
		std::string m_ShaderFilepath = "";
		bool m_ShaderEnableSpirv = false;
		bool m_ShaderRecompileSpirv = false;

		bool m_ShaderLoaded = false;
		std::string m_Name;

		std::vector<std::shared_ptr<Texture2D>> m_LoadedTextures;
	};
}

