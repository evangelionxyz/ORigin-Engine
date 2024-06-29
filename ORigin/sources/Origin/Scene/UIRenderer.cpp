#include "pch.h"
#include "UIRenderer.h"
#include "Origin\Renderer\Framebuffer.h"
#include "Origin\Renderer\Renderer2D.h"

namespace origin
{
	void UIRenderer::CreateFramebuffer(uint32_t vpW, uint32_t vpH, float orthoW, float orthoH)
	{
		FramebufferSpecification spec;

		spec.Attachments = {
			FramebufferTextureFormat::RGBA8,
			FramebufferTextureFormat::RED_INTEGER,
			FramebufferTextureFormat::DEPTH24STENCIL8
		};

		spec.Width = vpW;
		spec.Height = vpH;

		for (auto &ui : m_UIs)
		{
			ui.Framebuffer = Framebuffer::Create(spec);
		}

		float vertices[] = {
			 -1.0f,  1.0f,
				1.0f,  1.0f,
			 -1.0f, -1.0f,
				1.0f, -1.0f
		};

		glCreateVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		glCreateBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(float), nullptr);

		m_ScreenShader = Shader::Create("Resources/Shaders/Screen.glsl", false);

		m_Projection = glm::ortho(-orthoW / 2.0f, orthoW / 2.0f, -orthoH / 2.0f, orthoH / 2.0f, 0.0f, 1.0f);
	}

	void UIRenderer::SetViewportSize(uint32_t width, uint32_t height, float orthoW, float orthoH)
	{
		for (UIComponent &ui : m_UIs)
		{
			ui.Framebuffer->Resize(width, height);
		}
		m_Projection = glm::ortho(-orthoW / 2.0f, orthoW / 2.0f, -orthoH / 2.0f, orthoH / 2.0f, 0.0f, 1.0f);
	}

	void UIRenderer::Unload()
	{
		glDeleteBuffers(1, &m_VBO);
		glDeleteVertexArrays(1, &m_VAO);
	}

	void UIRenderer::RenderFramebuffer()
	{
		for (auto &ui : m_UIs)
		{
			ui.Framebuffer->Bind();

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Renderer2D::Begin(m_Projection);

			for (auto &[key, value] : ui.Components)
			{
				if (ui.Is<TextComponent>(key))
				{
					UIData<TextComponent> *comp = ui.GetComponent<TextComponent>(key);
					Renderer2D::DrawString(comp->Component.TextString, comp->Transform.GetTransform(), comp->Component);
				}
				else if (ui.Is<SpriteRenderer2DComponent>(key))
				{
					UIData<SpriteRenderer2DComponent> *comp = ui.GetComponent<SpriteRenderer2DComponent>(key);
					Renderer2D::DrawSprite(comp->Transform.GetTransform(), comp->Component);
				}
			}

			Renderer2D::End();

			ui.Framebuffer->Unbind();
		}
	}

	void UIRenderer::Render()
	{
		for (auto &ui : m_UIs)
		{
			m_ScreenShader->Enable();

			glBindVertexArray(m_VAO);
			glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

			glActiveTexture(GL_TEXTURE0);
			glBindTextureUnit(0, ui.Framebuffer->GetColorAttachmentRendererID());
			m_ScreenShader->SetInt("uScreenTexture", 0);
			m_ScreenShader->SetMatrix("uViewProjection", glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)));

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			m_ScreenShader->Disable();
		}
	}

	void UIRenderer::AddUI(const UIComponent &ui)
	{
		m_UIs.push_back(ui);
	}

}
