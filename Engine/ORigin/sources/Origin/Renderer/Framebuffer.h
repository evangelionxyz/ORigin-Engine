// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "Texture.h"

#include <vector>
#include <memory>

namespace origin {
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
	DEPTH24STENCIL8_ARRAY,

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
	FramebufferAttachmentSpecification attachments;
	ImageWrapMode wrap_mode = ImageWrapMode::CLAMP_TO_EDGE;
	ImageFilter filter_mode = ImageFilter::Linear;
	u32 width = 0, height = 0;
	u32 samples = 1;
	i32 depth_array_count = 0;
	bool read_buffer = false;
};

class Framebuffer
{
public:
	virtual ~Framebuffer() = default;
	virtual void Bind() = 0;
	virtual void Unbind() = 0;

	virtual void Resize(u32 width, u32 height) = 0;
	virtual int ReadPixel(u32 attachmentIndex, int x, int y) = 0;
	virtual void ClearAttachment(u32 attachmentIndex, int value) = 0;

	virtual u32 GetColorAttachment(u32 index = 0) const = 0;
	virtual u32 GetDepthAttachment() const = 0;

	virtual u32 GetWidth() const = 0;
	virtual u32 GetHeight() const = 0;

	virtual u32 GetBufferID() const = 0;

	virtual const FramebufferSpecification& GetSpecification() const = 0;

	static std::shared_ptr<Framebuffer> Create(const FramebufferSpecification& spec);
};
}

#endif