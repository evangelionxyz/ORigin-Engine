#include "pch.h"
#include "ShadowRenderer.h"

#include "Origin\Renderer\Renderer.h"

#include "glm\glm.hpp"

#include "Lighting.h"

namespace origin
{
	ShadowRenderer::ShadowRenderer(LightingType type)
	{
		Invalidate(type);
	}

	void ShadowRenderer::Invalidate(LightingType type)
	{
		if (m_Framebuffer)
			m_Framebuffer.reset();

		FramebufferSpecification fbSpec;
		fbSpec.ReadBuffer = false;

		switch (type)
		{
			case origin::LightingType::Spot:
			{
				break;
			}
			case origin::LightingType::Point:
			{
				break;
			}
			case origin::LightingType::Directional:
			{
				fbSpec.Width = 1080;
				fbSpec.Height = 1080;
				fbSpec.WrapMode = GL_CLAMP_TO_BORDER;
				fbSpec.Attachments = { FramebufferTextureFormat::DEPTH };
				break;
			}
		}

		m_Framebuffer = Framebuffer::Create(fbSpec);
	}

	void ShadowRenderer::OnAttachTexture(const std::shared_ptr<Material>& mat)
	{
		OGN_CORE_ASSERT(m_Framebuffer);

		glActiveTexture(m_Framebuffer->GetDepthAttachmentRendererID());
		glBindTexture(GL_TEXTURE_2D, m_Framebuffer->GetDepthAttachmentRendererID());
		mat->SetInt("uShadowMap", 0);
	}

	void ShadowRenderer::Setup(const TransformComponent& tc, float size, float n, float f)
	{
#if 0
		switch (m_LightingType)
		{
			case origin::LightingType::Spot:
				break;
			case origin::LightingType::Point:
				break;
			case origin::LightingType::Directional:
			{
				
				break;
			}
		}
#endif

		glm::mat4 lightProjection = glm::ortho(-size, size, -size, size, n, f);

		// direction/eye, center, up
		glm::mat4 lightView = lookAt(radians(-tc.GetForward()), glm::vec3(0.0f), radians(-tc.GetUp()));
		LightSpaceMatrix = lightProjection * lightView;

		Renderer::GetGShader("DirLightDepthMap")->SetMatrix("uLightSpaceMatrix", LightSpaceMatrix);

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