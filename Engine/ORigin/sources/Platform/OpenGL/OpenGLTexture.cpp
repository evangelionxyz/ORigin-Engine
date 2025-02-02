// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Origin/Core/Assert.h"
#include "Origin/Profiler/Profiler.h"
#include "OpenGLTexture.h"

#include <stb_image.h>
#include <ktx.h>
#include <glad/glad.h>

namespace origin {

namespace Utils {

static void GLFormat(const ImageFormat format, GLenum *out_format, GLenum *out_internal_format)
{
    switch (format)
    {
    case ImageFormat::R8:
        if (out_format) *out_format = GL_RED;
        if (out_internal_format) *out_internal_format = GL_R8;
        return;
    case ImageFormat::RG8:
        if (out_format) *out_format = GL_RG;
        if (out_internal_format) *out_internal_format = GL_RG8;
        return;
    case ImageFormat::RGB8: 
        if (out_format) *out_format = GL_RGB;
        if (out_internal_format) *out_internal_format = GL_RGB8;
        return;
    case ImageFormat::RGBA8: 
        if (out_format) *out_format = GL_RGBA;
        if (out_internal_format) *out_internal_format = GL_RGBA8;
        return;
    case ImageFormat::RGBA32F:
        if (out_format) *out_format = GL_RGBA;
        if (out_internal_format) *out_internal_format = GL_RGBA32F;
        return;
    }

    OGN_CORE_ASSERT(false, "[Texture2D] Invalid format");
}

static GLenum GLFilter(const ImageFilter filter)
{
    switch (filter)
    {
    case ImageFilter::Linear: return GL_LINEAR;
    case ImageFilter::Nearest: return GL_NEAREST;
    case ImageFilter::LinearMipmapLinear: return GL_LINEAR_MIPMAP_LINEAR;
    case ImageFilter::LinearMipmapNearest: return GL_LINEAR_MIPMAP_NEAREST;
    case ImageFilter::NearestMipmapLinear: return GL_LINEAR_MIPMAP_LINEAR;
    case ImageFilter::NearestMipmapNearest: return GL_LINEAR_MIPMAP_NEAREST;
    default: return GL_NONE;
    }
}
}

OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification &specification, Buffer data)
{
    m_spec = specification;

    m_width = specification.width;
    m_height = specification.height;

    OGN_CORE_ASSERT(m_spec.width == m_width || m_spec.height == m_height, "[Texture2D] Invalid Specification Size");

    GLenum format, internal_format;
    Utils::GLFormat(m_spec.format, &format, &internal_format);

    GLenum min_filter = Utils::GLFilter(m_spec.min_filter);
    GLenum mag_filter = Utils::GLFilter(m_spec.mag_filter);

    glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
    glTextureStorage2D(m_id, 1, internal_format, m_width, m_height);
    glTexParameteri(m_id, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(m_id, GL_TEXTURE_MAG_FILTER, mag_filter);
    glTexParameteri(m_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (data)
    {
        SetData(data);
    }

    m_is_loaded = true;
}

OpenGLTexture2D::OpenGLTexture2D(const std::filesystem::path &filepath, const TextureSpecification &specification)
{
    m_filepath = filepath;
    m_spec = specification;

    OGN_CORE_WARN("[Texture2D] Trying to load {}", filepath.generic_string());

    if (filepath.extension() == ".ktx")
    {
        ktxTexture *ktexture;
        KTX_error_code ktxerror = ktxTexture_CreateFromNamedFile(filepath.string().c_str(), KTX_TEXTURE_CREATE_NO_FLAGS, &ktexture);

        if (ktxerror != KTX_SUCCESS)
        {
            OGN_CORE_ERROR("Failed to load KTX Texture");
            return;
        }

        GLenum gl_target, gl_error;
        ktxerror = ktxTexture_GLUpload(ktexture, &m_id, &gl_target, &gl_error);
        if (ktxerror != KTX_SUCCESS)
        {
            OGN_CORE_ERROR("Failed to load KTX Texture");
            return;
        }

        if (ktexture->numLevels == 1)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE, GL_TEXTURE_MAX_ANISOTROPY, 4.0f);

        i32 alpha = 0;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_SIZE, &alpha);
        m_is_transparent = alpha > 0;

        ktxTexture_Destroy(ktexture);

        return;
    }

    i32 width, height, channels;
    stbi_set_flip_vertically_on_load(1);

    Buffer data;
    data.Data = stbi_load(filepath.string().c_str(), &width, &height, &channels, 0);

    if (!data)
    {
        OGN_CORE_ERROR("[Texture2D] Failed to load texture! {}", filepath.generic_string());
        data.Release();
        return;
    }

    m_width = width;
    m_height = height;
    m_channels = channels;

    data.Size = m_width * m_height * m_channels;

    m_spec.format = GetImageFormat(channels);

    GLenum format, internal_format;
    Utils::GLFormat(m_spec.format, &format, &internal_format);

    GLenum min_filter = Utils::GLFilter(m_spec.min_filter);
    GLenum mag_filter = Utils::GLFilter(m_spec.mag_filter);

    glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
    glTextureStorage2D(m_id, 1, internal_format, m_width, m_height);
    glTexParameteri(m_id, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(m_id, GL_TEXTURE_MAG_FILTER, mag_filter);
    glTexParameteri(m_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    i32 alpha = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_SIZE, &alpha);
    m_is_transparent = alpha > 0;

    if (data)
    {
        SetData(data);
        data.Release();
    }

    m_is_loaded = true;
}

OpenGLTexture2D::OpenGLTexture2D(const aiTexture *embedded_texture)
{
    glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    if (embedded_texture->mHeight == 0)
    {
        // compressed texture (e.g., PNG, JPG)
        i32 width, height, channels;

        stbi_uc *data = stbi_load_from_memory(reinterpret_cast<const stbi_uc *>(embedded_texture->pcData),
            embedded_texture->mWidth, &width, &height, &channels, 0);

        OGN_CORE_ASSERT(data, "[Texture2D] Failed to load texture");

        m_width = static_cast<u32>(width);
        m_height = static_cast<u32>(height);
        m_channels = static_cast<u32>(channels);

        m_spec.format = GetImageFormat(channels);
        GLenum format, internal_format;
        Utils::GLFormat(m_spec.format, &format, &internal_format);

        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else
    {
        // Uncompressed texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, embedded_texture->mWidth, embedded_texture->mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, embedded_texture->pcData);
    }

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    i32 alpha = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_SIZE, &alpha);
    m_is_transparent = alpha > 0;
}

OpenGLTexture2D::~OpenGLTexture2D()
{
    Unbind();
    glDeleteTextures(1, &m_id);
}

void OpenGLTexture2D::SetData(const Buffer data)
{
    // Verify the actual BPP
    GLenum format, internal_format;
    Utils::GLFormat(m_spec.format, &format, &internal_format);

    m_channels = format == GL_RGBA ? 4 : 3;

    OGN_CORE_ASSERT(data.Size == m_width * m_height * m_channels, "[Texture2D] Data must be entire texture!");
    glTextureSubImage2D(m_id, 0, 0, 0, m_width, m_height, format, GL_UNSIGNED_BYTE, data.Data);
}

void OpenGLTexture2D::Bind(const u32 index)
{
    m_index = index;
    glBindTextureUnit(index, m_id); // bind texture index to renderID
}

void OpenGLTexture2D::Bind(const u32 binding_point, const u32 index, const u32 array_count)
{
    m_index = index;
    const u32 texture_unit = binding_point * array_count + index;
    glBindTextureUnit(texture_unit, m_id);
}

void OpenGLTexture2D::Unbind()
{
    glBindTextureUnit(m_index, 0);
}

void OpenGLTexture2D::Destroy()
{
    glDeleteTextures(1, &m_id);
    OGN_CORE_WARN("[Texture2D] \"{}\" at index {} has been deleted", m_filepath.generic_string(), m_index);
}

void OpenGLTexture2D::ChangeSize(const i32 width, const i32 height)
{

    u32 tex_id;

    GLenum format, internal_format;
    Utils::GLFormat(m_spec.format, &format, &internal_format);

    glCreateTextures(GL_TEXTURE_2D, 1, &tex_id);
    glTextureStorage2D(tex_id, 1, format, width, height);

    glTexParameteri(tex_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(tex_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(tex_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(tex_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

    u32 fboIDs[2] = { 0 };
    glCreateFramebuffers(2, fboIDs);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, fboIDs[0]);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_id, 0);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboIDs[1]);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_id, 0);

    glBlitFramebuffer(0, 0, m_width, m_height,
        0, 0, width, height,
        GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glDeleteTextures(1, &m_id);
    glDeleteFramebuffers(2, fboIDs);

    m_id = tex_id;
    m_width = width;
    m_height = height;
}

}