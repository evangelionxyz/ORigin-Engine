// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "Texture.h"

#include <vector>
#include <memory>

namespace origin
{
	enum class FramebufferTextureFormat
	{
		None = 0,
		// Color
		RGBA16F,
		RGBA8,
		RED_INTEGER,

		// Depth
		DEPTH,
		DEPTH24STENCIL8,

		DEPTH_CUBE
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
		ImageWrapMode WrapMode = ImageWrapMode::CLAMP_TO_EDGE;
		ImageFilter FilterMode = ImageFilter::Linear;
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
		virtual uint32_t GetDepthCubeAttachmentRendererID() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual uint32_t GetBufferID() const = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;

		static std::shared_ptr<Framebuffer> Create(const FramebufferSpecification& spec);
	};

}

#endif