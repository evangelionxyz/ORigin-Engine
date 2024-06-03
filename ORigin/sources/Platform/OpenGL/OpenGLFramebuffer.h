// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin/Renderer/Framebuffer.h"

namespace origin {

	class OpenGL_Framebuffer : public Framebuffer
	{
	public:
		OpenGL_Framebuffer(const FramebufferSpecification& specification);
		virtual ~OpenGL_Framebuffer();

		void Invalidate();

		void Resize(uint32_t width, uint32_t height) override;
		int ReadPixel(uint32_t attachmentIndex, int x, int y) override;
		void ClearAttachment(uint32_t attachmentIndex, int value) override;

		uint32_t GetWidth() const override { return m_Spec.Width; }
		uint32_t GetHeight() const override { return m_Spec.Height; }

		void Bind() override;
		void Unbind() override;

		uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { return m_ColorAttachments[index]; };
		uint32_t GetDepthAttachmentRendererID() const override { return m_DepthAttachment; }
		uint32_t GetDepthCubeAttachmentRendererID() const override { return m_DepthCubeAttachment; }

		const FramebufferSpecification& GetSpecification() const override { return m_Spec; }

		virtual uint32_t GetRendererID() const override { return m_RendererID; }

	private:
		uint32_t m_RendererID = 0, m_Renderbuffer = 0;
		FramebufferSpecification m_Spec;
		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

		// Texture ID
		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
		uint32_t m_DepthCubeAttachment = 0;
	};


}
