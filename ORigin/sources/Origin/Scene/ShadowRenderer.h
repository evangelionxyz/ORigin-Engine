#pragma once

#include "Components.h"
#include "Origin\Renderer\Framebuffer.h"

namespace origin 
{
	enum class LightingType;

	class ShadowRenderer
	{
	public:
		ShadowRenderer(LightingType type);

		void Invalidate(LightingType type);

		void OnAttachTexture(const std::shared_ptr<Material>& mat);
		void Setup(const TransformComponent& tc, float size, float n, float f);

		static std::shared_ptr<ShadowRenderer> Create(LightingType type);

		std::shared_ptr<Framebuffer>& GetFramebuffer() { return m_Framebuffer; }

		glm::mat4 LightProjection;
		glm::mat4 LightSpaceMatrix;
		glm::mat4 LightViewMatrix;

		glm::mat4 ShadowProjection;

		std::vector<glm::mat4> ShadowTransforms;

	private:
		LightingType m_LightingType;
		std::shared_ptr<Framebuffer> m_Framebuffer;

	};


}

