#pragma once
#include "Origin.h"

using namespace origin;

class HDRScreen
{
public:
	HDRScreen(uint32_t width, uint32_t height);
	~HDRScreen();

	void OnBeginFramebuffer();
	void OnEndFramebuffer();

	void OnRender();
	void OnResize(uint32_t width, uint32_t height);

	uint32_t colorAttachment;
	std::shared_ptr<VertexArray> screenVertexArray;
	std::shared_ptr<VertexBuffer> screenVertexBuffer;
	std::shared_ptr<Shader> screenShader;
	uint32_t framebuffer = 0, renderbuffer;
	uint32_t m_Width = 1280, m_Height = 720;

	float Exposure = 0.1f;
	float Gamma = 2.2f;

private:
	void Invalidate();
};

