// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once

#include "glad\glad.h"

#include <vector>
#include <memory>

namespace origin
{
	enum class FramebufferTextureFormat
	{
		None = 0,

		// color
		RGBA8,
		RED_INTEGER,

		// depth/stencil
		DEPTH24STENCIL8,
		DEPTH = DEPTH24STENCIL8
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
			: TextureFormat(format) {}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			: TextureAttachments(attachments) {}

		std::vector<FramebufferTextureSpecification> TextureAttachments;
	};

	struct FramebufferSpecification
	{
		FramebufferAttachmentSpecification Attachments;
		uint32_t Width = 0, Height = 0;
		uint32_t Samples = 1;

		GLenum WrapMode = GL_CLAMP_TO_EDGE;
		GLenum FilterMode = GL_NEAREST;

		bool ReadBuffer = true;
		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;
		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
		virtual uint32_t GetDepthAttachmentRendererID() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;

		static std::shared_ptr<Framebuffer> Create(const FramebufferSpecification& spec);
	};

}