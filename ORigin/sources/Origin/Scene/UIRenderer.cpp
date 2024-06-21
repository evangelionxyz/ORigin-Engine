#include "pch.h"
#include "UIRenderer.h"
#include "Origin\Renderer\Framebuffer.h"
#include "Origin\Renderer\Renderer2D.h"

namespace origin
{
	void UIRenderer::CreateFramebuffer(uint32_t width, uint32_t height)
	{
		FramebufferSpecification spec;

		spec.Attachments = {
			FramebufferTextureFormat::RGBA8,
			FramebufferTextureFormat::RED_INTEGER,
			FramebufferTextureFormat::DEPTH24STENCIL8
		};

		spec.Width = width;
		spec.Height = height;

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

			glClearColor(0.3f, 0.3f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			float width = static_cast<float>(ui.Framebuffer->GetWidth()) * 2.0f;
			float height = static_cast<float>(ui.Framebuffer->GetHeight()) * 2.0f;

			Renderer2D::Begin(glm::mat4(1.0f));

			Renderer2D::DrawRect(glm::scale(glm::mat4(1.0f), { width, height, 1.0f }), {0.0f, 1.0f, 0.0f, 1.0f });

			for (auto &txt : ui.Texts)
			{
				if (txt.Component.FontHandle != 0)
					Renderer2D::DrawString(txt.Component.TextString, txt.Transform.GetTransform(), txt.Component);
			}

			for (auto &texture : ui.Sprites)
			{
				Renderer2D::DrawSprite(texture.Transform.GetTransform(), texture.Component);
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
