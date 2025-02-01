// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"

#include "OpenGLFramebuffer.h"
#include "Origin/Core/Assert.h"
#include "Origin/Profiler/Profiler.h"

#include <glad/glad.h>

namespace origin {
static const u32 s_MaxFramebufferSize = 8192;

static i32 GetGLImageWrapMode(ImageWrapMode mode)
{
	switch (mode)
	{
	case ImageWrapMode::CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
	default:
	case ImageWrapMode::REPEAT: return GL_REPEAT;
	}
}

static i32 GetGLImageFilter(ImageFilter filter)
{
	switch (filter)
	{
	default:
	case ImageFilter::Linear: return GL_LINEAR;
	case ImageFilter::Nearest: return GL_NEAREST;
	case ImageFilter::NearestMipmapNearest: return GL_NEAREST_MIPMAP_NEAREST;
	case ImageFilter::NearestMipmapLinear: return GL_NEAREST_MIPMAP_LINEAR;
	case ImageFilter::LinearMipmapLinear: return GL_LINEAR_MIPMAP_LINEAR;
	case ImageFilter::LinearMipmapNearest: return GL_LINEAR_MIPMAP_NEAREST;
	}
}

static u32 GetGLImageFormat(ImageFormat format)
{
	switch (format)
	{
	default:
	case ImageFormat::RGB8: return GL_RGB8;
	case ImageFormat::RGBA8: return GL_RGBA;
	case ImageFormat::RGBA32F: return GL_RGBA32F;
	case ImageFormat::R8: return GL_R8;
	}
}

namespace Utils	{

// ================ Texture 2D ================
static GLenum Texture2DTarget(bool multisampled) 
{ 
	return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D; 
}

static void AttachColorTexture2D(u32 textureID, GLenum internalFormat, GLenum format, FramebufferSpecification spec, int index)
{
	OGN_PROFILER_RENDERING();

	bool multisampled = spec.samples > 1;
	if (multisampled)
	{
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, spec.samples, internalFormat, spec.width, spec.height, GL_FALSE);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, spec.width, spec.height, 0, format, GL_UNSIGNED_BYTE, nullptr);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetGLImageFilter(spec.filter_mode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetGLImageFilter(spec.filter_mode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetGLImageWrapMode(spec.wrap_mode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetGLImageWrapMode(spec.wrap_mode));

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, Texture2DTarget(multisampled), textureID, 0);
}

static void AttachDepthTexture2D(u32 textureID, GLenum textureFormat, GLenum attachmentType, FramebufferSpecification spec)
{
	OGN_PROFILER_RENDERING();

	bool multisampled = spec.samples > 1;
	if (multisampled)
	{
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, spec.samples, textureFormat, spec.width, spec.height, GL_FALSE);
	}
	else
	{
		if(attachmentType == GL_DEPTH_STENCIL_ATTACHMENT) glTexStorage2D(GL_TEXTURE_2D, 1, textureFormat, spec.width, spec.height);
		else if(attachmentType == GL_DEPTH_ATTACHMENT) glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, spec.width, spec.height, 0, textureFormat, GL_FLOAT, nullptr);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetGLImageFilter(spec.filter_mode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetGLImageFilter(spec.filter_mode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetGLImageWrapMode(spec.wrap_mode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetGLImageWrapMode(spec.wrap_mode));
	f32 borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, Texture2DTarget(multisampled), textureID, 0);
}

static bool IsDepthFormat(FramebufferTextureFormat format)
{
	switch (format)
	{
	case FramebufferTextureFormat::DEPTH:
	case FramebufferTextureFormat::DEPTH24STENCIL8:
	case FramebufferTextureFormat::DEPTH24STENCIL8_ARRAY:
	case FramebufferTextureFormat::DEPTH_CUBE: return true;
	}

	return false;
}

static GLenum ORiginFBTextureFormatToGL(FramebufferTextureFormat format)
{
	switch (format)
	{
	case FramebufferTextureFormat::RGBA16F: return GL_RGBA16F;
	case FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
	default:
	case FramebufferTextureFormat::RGBA8: return GL_RGBA8;
	}
}
}

OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& specification)
	: m_Spec(specification)
{
	OGN_CORE_ASSERT(m_Spec.width > 0 && m_Spec.height > 0, "[OpenGLFramebuffer] Invalid size");

	for (auto spec : m_Spec.attachments.TextureAttachments)
	{
		if (!Utils::IsDepthFormat(spec.TextureFormat))
			m_ColorAttachmentSpecifications.emplace_back(spec);
		else
			m_DepthAttachmentSpecification = spec;
	}

	Invalidate();
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
	OGN_PROFILER_RENDERING();

	glDeleteFramebuffers(1, &m_BufferID);
	glDeleteRenderbuffers(1, &m_Renderbuffer);
	glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
	glDeleteTextures(1, &m_DepthAttachment);
}

void OpenGLFramebuffer::Invalidate()
{
	OGN_PROFILER_RENDERING();

	if (m_BufferID)
	{
		glDeleteFramebuffers(1, &m_BufferID);
		glDeleteRenderbuffers(1, &m_Renderbuffer);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);

		m_ColorAttachments.clear();
		m_DepthAttachment = 0;
	}

	glCreateFramebuffers(1, &m_BufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);

	bool multisample = m_Spec.samples > 1;

	if (m_ColorAttachmentSpecifications.size())
	{
		m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
		glCreateTextures(Utils::Texture2DTarget(multisample), m_ColorAttachments.size(), m_ColorAttachments.data());
		
		for (size_t i = 0; i < m_ColorAttachments.size(); i++)
		{
			glBindTexture(Utils::Texture2DTarget(multisample), m_ColorAttachments[i]);
			switch (m_ColorAttachmentSpecifications[i].TextureFormat)
			{
			case FramebufferTextureFormat::RGBA16F:
				Utils::AttachColorTexture2D(m_ColorAttachments[i], GL_RGBA16F, GL_RGBA, m_Spec, i);
				break;
			case FramebufferTextureFormat::RGBA8:
				Utils::AttachColorTexture2D(m_ColorAttachments[i], GL_RGBA8, GL_RGBA, m_Spec, i);
				break;
			case FramebufferTextureFormat::RED_INTEGER:
				Utils::AttachColorTexture2D(m_ColorAttachments[i], GL_R32I, GL_RED_INTEGER, m_Spec, i);
				break;
			}
		}
	}

	if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
	{
		switch (m_DepthAttachmentSpecification.TextureFormat)
		{
		case FramebufferTextureFormat::DEPTH:
		{
			glCreateTextures(Utils::Texture2DTarget(multisample), 1, &m_DepthAttachment);
			glBindTexture(Utils::Texture2DTarget(multisample), m_DepthAttachment);
			Utils::AttachDepthTexture2D(m_DepthAttachment, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, m_Spec);
			break;
		}
		case FramebufferTextureFormat::DEPTH24STENCIL8:
		{
			glCreateTextures(Utils::Texture2DTarget(multisample), 1, &m_DepthAttachment);
			glBindTexture(Utils::Texture2DTarget(multisample), m_DepthAttachment);
			Utils::AttachDepthTexture2D(m_DepthAttachment, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Spec);
			break;
		}
		case FramebufferTextureFormat::DEPTH24STENCIL8_ARRAY:
		{
			glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_DepthAttachment);
			glBindTexture(GL_TEXTURE_2D_ARRAY, m_DepthAttachment);
			glTexStorage3D(GL_TEXTURE_2D_ARRAY, 
				1,                             // Mip Levels         
				GL_DEPTH_COMPONENT32F,         // Internal format
				m_Spec.width, m_Spec.height,   // Width/Height (fixed per layer)
				m_Spec.depth_array_count       // Number of cascades
			);

            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;
		}
		case FramebufferTextureFormat::DEPTH_CUBE:
		{
			glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_DepthAttachment);
			glBindTexture(Utils::Texture2DTarget(multisample), m_DepthAttachment);
			for (i32 i = 0; i < 6; i++)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, m_Spec.width, m_Spec.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetGLImageFilter(m_Spec.filter_mode));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetGLImageFilter(m_Spec.filter_mode));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GetGLImageWrapMode(m_Spec.wrap_mode));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetGLImageWrapMode(m_Spec.wrap_mode));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetGLImageWrapMode(m_Spec.wrap_mode));
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, m_DepthAttachment, 0);
			break;
		}
		}
	}

	if (m_ColorAttachments.size() > 1)
	{
		GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(m_ColorAttachments.size(), buffers);
	}

	if(m_ColorAttachments.empty()) glDrawBuffer(GL_NONE);
	if (m_Spec.read_buffer == false) glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);
	glViewport(0, 0, m_Spec.width, m_Spec.height);
}

void OpenGLFramebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebuffer::Resize(u32 width, u32 height)
{
	bool invalid = width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize;
	if (invalid)
	{
		OGN_CORE_WARN("Attempted to resize framebuffer to {0}, {1}", width, height);
		return;
	}

	m_Spec.width = width;
	m_Spec.height = height;
	Invalidate();
}

int OpenGLFramebuffer::ReadPixel(u32 attachment_idx, int x, int y)
{
	bool valid = attachment_idx < m_ColorAttachments.size();
	OGN_CORE_ASSERT(valid, "[OpenGLFramebuffer::ReadPixel] Trying to read wrong attachments {}", attachment_idx);
	if (!valid)
		return -1;

	int pixelData;
	glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment_idx);
	glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
	return pixelData;
}

void OpenGLFramebuffer::ClearAttachment(u32 attachment_idx, int value)
{
	if (attachment_idx < m_ColorAttachmentSpecifications.size())
	{
		auto &spec = m_ColorAttachmentSpecifications[attachment_idx];
		glClearTexImage(m_ColorAttachments[attachment_idx], 0, Utils::ORiginFBTextureFormatToGL(spec.TextureFormat), GL_INT, &value);
	}
}
}
