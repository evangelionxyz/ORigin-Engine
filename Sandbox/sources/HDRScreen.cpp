#include "HDRScreen.h"

HDRScreen::HDRScreen(uint32_t width, uint32_t height)
	: m_Width(width), m_Height(height)
{
	Invalidate();

	float screenQuadVertices[] = {
		-1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,
			1.0f,  1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f, 0.0f
	};

	screenVertexArray = VertexArray::Create();
	screenVertexBuffer = VertexBuffer::Create(screenQuadVertices, sizeof(screenQuadVertices));
	screenVertexBuffer->SetLayout({
		{ShaderDataType::Float2, "position"},
		{ShaderDataType::Float2, "texCoord"}
		});
	screenVertexArray->AddVertexBuffer(screenVertexBuffer);

	uint32_t screenIndices[] = {
		0, 2, 1,
		2, 0, 3
	};

	std::shared_ptr<IndexBuffer> screenIB = IndexBuffer::Create(screenIndices, 6);
	screenVertexArray->SetIndexBuffer(screenIB);
	screenShader = Shader::Create("Resources/Sandbox/HDRscreen.glsl");
}

HDRScreen::~HDRScreen()
{
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteTextures(1, &colorAttachment);
	glDeleteRenderbuffers(1, &renderbuffer);
}

void HDRScreen::OnBeginFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glViewport(0, 0, m_Width, m_Height);
}

void HDRScreen::OnEndFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void HDRScreen::OnRender()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	screenShader->Enable();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorAttachment);
	screenShader->SetInt("screenTexture", 0);
	screenShader->SetFloat("Exposure", Exposure);
	screenShader->SetFloat("Gamma", Gamma);

	RenderCommand::DrawIndexed(screenVertexArray);
	screenShader->Disable();
}

void HDRScreen::OnResize(uint32_t width, uint32_t height)
{
	if (width == 0 || height == 0)
	{
		OGN_CORE_WARN("Attempted to resize framebuffer to {0}, {1}", width, height);
		return;
	}

	m_Width = width;
	m_Height = height;

	Invalidate();
}

void HDRScreen::Invalidate()
{
	if (framebuffer)
	{
		glDeleteFramebuffers(1, &framebuffer);
		glDeleteTextures(1, &colorAttachment);
		glDeleteRenderbuffers(1, &renderbuffer);
	}

	glCreateFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glCreateTextures(GL_TEXTURE_2D, 1, &colorAttachment);
	glTextureStorage2D(colorAttachment, 1, GL_RGB16F, m_Width, m_Height);
	glTextureParameteri(colorAttachment, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(colorAttachment, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(colorAttachment, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(colorAttachment, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);

	glCreateRenderbuffers(1, &renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);
}
