// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef OPENGK_FRAMEBUFFER_H
#define OPENGK_FRAMEBUFFER_H

#include "Origin/Renderer/Framebuffer.h"

namespace origin {
class OpenGLFramebuffer : public Framebuffer
{
public:
	OpenGLFramebuffer(const FramebufferSpecification& specification);
	virtual ~OpenGLFramebuffer();

	void Invalidate();

	void Resize(u32 width, u32 height) override;
	int ReadPixel(u32 attachment_index, int x, int y) override;
	void ClearAttachment(u32 attachment_index, int value) override;

	u32 GetWidth() const override { return m_Spec.width; }
	u32 GetHeight() const override { return m_Spec.height; }

	void Bind() override;
	void Unbind() override;

	u32 GetColorAttachment(u32 index = 0) const override { return m_ColorAttachments[index]; };
	u32 GetDepthAttachment() const override { return m_DepthAttachment; }

	const FramebufferSpecification& GetSpecification() const override { return m_Spec; }

	u32 GetBufferID() const override { return m_BufferID; }

private:
	u32 m_BufferID = 0, m_Renderbuffer = 0;
	FramebufferSpecification m_Spec;
	std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
	FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

	// Texture ID
	std::vector<u32> m_ColorAttachments;
	u32 m_DepthAttachment = 0;
};
}

#endif