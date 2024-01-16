#pragma once

#include "Components.h"
#include "Origin\Renderer\Framebuffer.h"
#include "Origin\Renderer\UniformBuffer.h"

namespace origin 
{
	enum class LightingType;

	class ShadowRenderer
	{
	public:
		ShadowRenderer(const std::shared_ptr<Shader>& depthShader, LightingType type);

		void Invalidate(LightingType type);
		void OnAttachTexture(const std::shared_ptr<Shader>& objectShader);

		void BindFramebuffer();
		void UnbindFramebuffer();

		void OnRenderBegin(const TransformComponent& tc);
		void OnRenderEnd();

		static std::shared_ptr<ShadowRenderer> Create(const std::shared_ptr<Shader>& depthShader, LightingType type);

		std::shared_ptr<Framebuffer>& GetFramebuffer() { return m_Framebuffer; }

		struct DepthBufferData
		{
			glm::mat4 LightViewProjection;
			glm::mat4 ModelTransform;
		};

		DepthBufferData m_DepthBufferData;

		glm::mat4 ShadowProjection;

		std::vector<glm::mat4> ShadowTransforms;

		float Size = 10.0f;
		float Near = 0.1f;
		float Far = 1000.5f;

	private:
		LightingType m_LightingType;
		std::shared_ptr<Framebuffer> m_Framebuffer;
		std::shared_ptr<UniformBuffer> m_DepthUniformBuffer;
		std::shared_ptr<Shader> m_DepthShader;
	};


}

