#pragma once

#include "Components/Components.h"
#include "Origin/Renderer/Framebuffer.h"
#include "Origin/Renderer/UniformBuffer.h"

namespace origin 
{
	enum class LightingType;

	class ShadowRenderer
	{
	public:
		ShadowRenderer(const std::shared_ptr<Shader>& depthShader, LightingType type);

		void Invalidate(LightingType type);
		void OnAttachTexture(const std::shared_ptr<Shader> objectShader);

		void BindFramebuffer();
		void UnbindFramebuffer();

		void OnRenderBegin(const TransformComponent& tc, const glm::mat4& transform);
		void OnRenderEnd();

		static std::shared_ptr<ShadowRenderer> Create(const std::shared_ptr<Shader>& depthShader, LightingType type);

		std::shared_ptr<Framebuffer>& GetFramebuffer() { return m_Framebuffer; }

		glm::mat4 Transform;
		glm::mat4 ViewProjection;
		glm::mat4 ShadowProjection;

		float Size = 20.0f;
		float Near = -10.0f;
		float Far = 10.0f;

	private:
		LightingType m_LightingType;
		std::shared_ptr<Framebuffer> m_Framebuffer;
		std::shared_ptr<UniformBuffer> m_DepthUniformBuffer;
		std::shared_ptr<Shader> m_DepthShader;

		friend class Lighting;
	};


}

