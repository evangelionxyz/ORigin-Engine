#pragma once
#include "Origin\Renderer\VertexArray.h"
#include "Origin\Renderer\Buffer.h"
#include "Origin\Renderer\RenderCommand.h"
#include "Origin\Scene\Skybox.h"

#include "glm\glm.hpp"

#include <string>

namespace origin
{
	class OpenGLSkybox : public Skybox
	{
	public:
		OpenGLSkybox(const std::string& filepath);
		void Draw(const glm::mat4& viewProjection) override;
		void Draw(const EditorCamera& camera) override;
		void SetBlur(float blur) override;

	private:
		void LoadTexture(const std::string& filepath);

		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<Shader> m_Shader;
		uint32_t m_TextureID;
		const uint32_t IndicesCount = 36;
		float m_BlurFactor = 0.005f;
	};
}