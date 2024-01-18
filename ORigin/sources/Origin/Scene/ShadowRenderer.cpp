#include "pch.h"
#include "ShadowRenderer.h"
#include "Origin\Renderer\Renderer.h"

#include "Lighting.h"

#include "glm\glm.hpp"

namespace origin {

	ShadowRenderer::ShadowRenderer(const std::shared_ptr<Shader>& depthShader, LightingType type)
		: m_DepthShader(depthShader)
	{
		m_DepthUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4), 11);
		Invalidate(type);
	}

	void ShadowRenderer::Invalidate(LightingType type)
	{
		m_LightingType = type;

		if (m_Framebuffer)
			m_Framebuffer.reset();

		FramebufferSpecification fbSpec;
		fbSpec.Width = 2048;
		fbSpec.Height = 2048;
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

	void ShadowRenderer::OnAttachTexture(const std::shared_ptr<Shader> objectShader)
	{
		OGN_CORE_ASSERT(m_Framebuffer, "ShadowRenderer: Invalid Framebuffer");
		glBindTextureUnit(2, m_Framebuffer->GetDepthAttachmentRendererID());
		objectShader->SetInt("u_ShadowMap", 2);
	}

	void ShadowRenderer::BindFramebuffer()
	{
		if (m_Framebuffer)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);

			m_Framebuffer->Bind();
			glClear(GL_DEPTH_BUFFER_BIT);
		}
	}

	void ShadowRenderer::UnbindFramebuffer()
	{
		m_Framebuffer->Unbind();
	}

	void ShadowRenderer::OnRenderBegin(const TransformComponent& tc, const glm::mat4& modelTransform)
	{
		m_DepthShader->Enable();
		if (m_LightingType == LightingType::Directional)
		{
			glm::mat4 projection = glm::ortho(-Size, Size, -Size, Size, Near, Far);
			glm::mat4 view = glm::lookAt(glm::vec3(0.0f), glm::radians(-tc.GetUp()), glm::vec3(0.0f, 1.0f, 0.0));

			m_LightViewProjection = projection * view * modelTransform;
			m_DepthUniformBuffer->Bind();
			m_DepthUniformBuffer->SetData(&m_LightViewProjection, sizeof(glm::mat4));
		}
	}

	void ShadowRenderer::OnRenderEnd()
	{
		m_DepthUniformBuffer->Unbind();
		m_DepthShader->Disable();
	}

	std::shared_ptr<ShadowRenderer> ShadowRenderer::Create(const std::shared_ptr<Shader>& depthShader, LightingType type)
	{
		return std::make_shared<ShadowRenderer>(depthShader, type);
	}
}