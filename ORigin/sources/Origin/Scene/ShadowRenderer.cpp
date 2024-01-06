#include "pch.h"
#include "ShadowRenderer.h"

#include "Origin\Renderer\Renderer.h"

#include "glm\glm.hpp"

#include "Lighting.h"

namespace origin {

	ShadowRenderer::ShadowRenderer(LightingType type)
	{
		Invalidate(type);
	}

	void ShadowRenderer::Invalidate(LightingType type)
	{
		m_LightingType = type;

		if (m_Framebuffer)
			m_Framebuffer.reset();

		FramebufferSpecification fbSpec;
		fbSpec.Width = 1080;
		fbSpec.Height = 1080;
		fbSpec.ReadBuffer = false;
		
		switch (type)
		{
			case origin::LightingType::Spot:
				break;
			
			case origin::LightingType::Point:
				fbSpec.WrapMode = GL_CLAMP_TO_EDGE;
				fbSpec.Attachments = { FramebufferTextureFormat::DEPTH_CUBE };
				break;
			
			case origin::LightingType::Directional:
				fbSpec.WrapMode = GL_CLAMP_TO_BORDER;
				fbSpec.Attachments = { FramebufferTextureFormat::DEPTH };
				break;
		}

		m_Framebuffer = Framebuffer::Create(fbSpec);
	}

	void ShadowRenderer::OnAttachTexture(const std::shared_ptr<Material>& mat)
	{
		OGN_CORE_ASSERT(m_Framebuffer, "ShadowRenderer: Invalid Framebuffer");

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, m_Framebuffer->GetDepthAttachmentRendererID());
		mat->SetInt("material.shadow_map", 4);
	}

	void ShadowRenderer::Setup(const TransformComponent& tc, float size, float n, float f)
	{
		switch (m_LightingType)
		{
			case LightingType::Spot:
				break;
			
			case LightingType::Point:
				ShadowProjection = glm::perspective(glm::radians(90.0f), m_Framebuffer->GetWidth() / (float)m_Framebuffer->GetHeight(), n, f);
				ShadowTransforms.push_back(ShadowProjection * glm::lookAt(tc.Translation, tc.Translation + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
				ShadowTransforms.push_back(ShadowProjection *	glm::lookAt(tc.Translation, tc.Translation + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
				ShadowTransforms.push_back(ShadowProjection * glm::lookAt(tc.Translation, tc.Translation + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
				ShadowTransforms.push_back(ShadowProjection * glm::lookAt(tc.Translation, tc.Translation + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
				ShadowTransforms.push_back(ShadowProjection * glm::lookAt(tc.Translation, tc.Translation + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
				ShadowTransforms.push_back(ShadowProjection * glm::lookAt(tc.Translation, tc.Translation + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
				break;
			
			case LightingType::Directional:
				LightProjection = glm::ortho(-size, size, -size, size, n, f);
				LightViewMatrix = lookAt(radians(-tc.GetForward()), glm::vec3(0.0f), radians(-tc.GetUp())); // direction/eye, center, up
				LightSpaceMatrix = LightProjection * LightViewMatrix;
				Renderer::GetGShader("DirLightDepthMap")->SetMatrix("uLightSpaceMatrix", LightSpaceMatrix);
				break;
		}

		if (m_Framebuffer)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);

			m_Framebuffer->Bind();
			glClear(GL_DEPTH_BUFFER_BIT);
		}
	}

	std::shared_ptr<ShadowRenderer> ShadowRenderer::Create(LightingType type)
	{
		return std::make_shared<ShadowRenderer>(type);
	}
}