#include "pch.h"
#include "UIRenderer.h"
#include "Origin\Renderer\Framebuffer.h"
#include "Origin\Renderer\Renderer2D.h"

namespace origin
{
	void UIRenderer::CreateFramebuffer(float width, float height)
	{
		FramebufferSpecification spec;

		spec.Attachments = {
			FramebufferTextureFormat::RGBA8,
			FramebufferTextureFormat::RED_INTEGER,
			FramebufferTextureFormat::DEPTH24STENCIL8
		};

		spec.Width = static_cast<uint32_t>(width);
		spec.Height = static_cast<uint32_t>(height);

		for (auto &ui : m_UIs)
			ui.Framebuffer = Framebuffer::Create(spec);

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

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			float width = static_cast<float>(ui.Framebuffer->GetWidth());
			float height = static_cast<float>(ui.Framebuffer->GetHeight());

			glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			glm::mat4 projection = glm::ortho(-width, width, -height, height, 0.0f, 2.0f);
			glm::mat4 viewProjection = projection * view;

			Renderer2D::Begin(viewProjection);

			for (auto &txt : ui.Texts)
			{
				if (txt.Component.FontHandle != 0)
				{
					Renderer2D::DrawString(txt.Component.TextString, txt.Transform.GetTransform(), txt.Component);
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
