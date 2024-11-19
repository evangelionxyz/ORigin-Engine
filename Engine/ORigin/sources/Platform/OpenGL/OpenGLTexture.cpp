// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Origin/Core/Assert.h"
#include "Origin/Profiler/Profiler.h"
#include "OpenGLTexture.h"

#ifdef OGN_PLATFORM_LINUX
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace origin
{
    namespace Utils
    {
        static GLenum ORiginImageFormatToGLDataFormat(const ImageFormat format)
        {
            switch (format)
            {
            case ImageFormat::RGB8: return GL_RGB;
            case ImageFormat::RGBA8: return GL_RGBA;
            }

            OGN_CORE_ASSERT(false, "[Texture2D] Invalid DataFormat");
            return 0;
        }

        static GLenum ORiginImageFormatToGLInternalFormat(const ImageFormat format)
        {
            switch (format)
            {
            case ImageFormat::RGB8: return GL_RGB8;
            case ImageFormat::RGBA8: return GL_RGBA8;
            }

            OGN_CORE_ASSERT(false, "[Texture2D] Invalid InternalFormat");
            return 0;
        }

        static GLenum ORiginImageFilterToGLImageFilter(const ImageFilter filter)
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
        : m_Spec(specification), m_Width(specification.Width), m_Height(specification.Height)
    {
        OGN_PROFILER_RENDERING();

        OGN_CORE_ASSERT(m_Spec.Width == m_Width || m_Spec.Height == m_Height, "[Texture2D] Invalid Specification Size");

        m_DataFormat = Utils::ORiginImageFormatToGLDataFormat(m_Spec.Format);
        m_InternalFormat = Utils::ORiginImageFormatToGLInternalFormat(m_Spec.Format);

        m_MinFilter = Utils::ORiginImageFilterToGLImageFilter(m_Spec.MinFilter);
        m_MagFilter = Utils::ORiginImageFilterToGLImageFilter(m_Spec.MagFilter);

        glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
        glTextureStorage2D(m_TextureID, 1, m_InternalFormat, m_Width, m_Height);
        glTexParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, m_MinFilter);
        glTexParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, m_MagFilter);
        glTexParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        if (data)
        {
            SetData(data);
        }

        m_IsLoaded = true;
    }

    OpenGLTexture2D::OpenGLTexture2D(const std::filesystem::path &filepath, const TextureSpecification &specification)
        : m_Filepath(filepath.generic_string()), m_Spec(specification), m_InternalFormat(0), m_DataFormat(0)
    {
        OGN_PROFILER_RENDERING();

        OGN_CORE_WARN("[Texture2D] Trying to load {0}", filepath.generic_string());

        int width, height, bpp;
        stbi_set_flip_vertically_on_load(1);

        Buffer data;
        data.Data = stbi_load(filepath.string().c_str(), &width, &height, &bpp, 0);

        if (!data)
        {
            OGN_CORE_ERROR("[Texture2D] Failed to load texture! {0}", filepath.generic_string());
            data.Release();
            return;
        }

        m_Width = width;
        m_Height = height;
        m_BPP = bpp;

        data.Size = m_Width * m_Height * m_BPP;
        switch (bpp)
        {
        case 3:
            m_InternalFormat = GL_RGB8;
            m_DataFormat = GL_RGB;
            break;
        case 4:
            m_InternalFormat = GL_RGBA8;
            m_DataFormat = GL_RGBA;
            break;

        }

        m_MinFilter = Utils::ORiginImageFilterToGLImageFilter(m_Spec.MinFilter);
        m_MagFilter = Utils::ORiginImageFilterToGLImageFilter(m_Spec.MagFilter);

        OGN_CORE_ASSERT(m_InternalFormat & m_DataFormat, "[Texture2D] Format not supported!");

        glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
        glTextureStorage2D(GL_TEXTURE_2D, 1, m_InternalFormat, m_Width, m_Height);
        glTexParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, m_MinFilter);
        glTexParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, m_MagFilter);
        glTexParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        if (data)
        {
            SetData(data);
            data.Release();
        }

        m_IsLoaded = true;
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        Unbind();
        glDeleteTextures(1, &m_TextureID);
    }

    void OpenGLTexture2D::SetData(const Buffer data)
    {
        OGN_PROFILER_RENDERING();

        // Verify the actual BPP
        const u32 bpp = m_DataFormat == GL_RGBA ? 4 : 3;
        OGN_CORE_ASSERT(data.Size == m_Width * m_Height * bpp, "[Texture2D] Data must be entire texture!");
        glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data.Data);
    }

    void OpenGLTexture2D::Bind(const u32 index)
    {
        OGN_PROFILER_RENDERING();

        m_Index = index;
        glBindTextureUnit(index, m_TextureID); // bind texture index to renderID
    }

    void OpenGLTexture2D::Bind(const u32 binding_point, const u32 index, const u32 array_count)
    {
        OGN_PROFILER_RENDERING();
        const u32 texture_unit = binding_point * array_count + index;

        m_Index = index;
        glBindTextureUnit(texture_unit, m_TextureID); // bind texture index to renderID
    }

    void OpenGLTexture2D::Unbind()
    {
        OGN_PROFILER_RENDERING();

        glBindTextureUnit(m_Index, 0);
    }

    void OpenGLTexture2D::Delete()
    {
        OGN_PROFILER_RENDERING();

        glDeleteTextures(1, &m_TextureID);
        OGN_CORE_WARN("[Texture2D] \"{}\" at index {} has been deleted", m_Filepath, m_Index);
    }

    void OpenGLTexture2D::ChangeSize(const u64 width, const u64 height)
    {
        OGN_PROFILER_RENDERING();

        u32 newTextureID;
        glCreateTextures(GL_TEXTURE_2D, 1, &newTextureID);
        glTextureStorage2D(newTextureID, 1, m_InternalFormat, width, height);

        glTexParameteri(newTextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(newTextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(newTextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(newTextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

        u32 fboIDs[2] = { 0 };
        glCreateFramebuffers(2, fboIDs);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, fboIDs[0]);
        glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureID, 0);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboIDs[1]);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, newTextureID, 0);

        glBlitFramebuffer(0, 0, m_Width, m_Height,
          0, 0, width, height,
          GL_COLOR_BUFFER_BIT, GL_LINEAR);

        glDeleteTextures(1, &m_TextureID);
        glDeleteFramebuffers(2, fboIDs);

        m_TextureID = newTextureID;
        m_Width = width;
        m_Height = height;
    }

    OpenGLTextureCube::OpenGLTextureCube(const u32 width, const u32 height)
        : m_Width(width), m_Height(height)
    {
    }

    OpenGLTextureCube::OpenGLTextureCube(const std::string &path)
        : m_TextureID(0), m_Filepath(path)
    {
        OGN_PROFILER_RENDERING();

        std::string faces[6] =
        {
          "/right.jpg",
          "/left.jpg",
          "/top.jpg",
          "/bottom.jpg",
          "/back.jpg",
          "/front.jpg",
        };

        int width, height, bpp;
        stbi_uc *data = nullptr;

        stbi_set_flip_vertically_on_load(false);
        GLenum dataFormat = 0, internal_format = 0;

        // set the file path
        for (u32 i = 0; i < 6; i++)
        {
            data = stbi_load(std::string(path + faces[i]).c_str(), &height, &width, &bpp, 0);

            switch (bpp)
            {
            case 3:
                internal_format = GL_RGB8;
                dataFormat = GL_RGB;
                break;
            case 4:
                internal_format = GL_RGBA8;
                dataFormat = GL_RGBA;
                break;
            }

            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, static_cast<i32>(internal_format),
                    width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
                OGN_CORE_TRACE("[Texture2D] \"{0}\" Successfully Loaded", faces[i]);
            }
            else OGN_CORE_ERROR("[Texture2D] Failed to load Texture: {0}", faces[i]);

            stbi_image_free(data);
        }

        // generate texture
        glGenTextures(1, &m_TextureID);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    OpenGLTextureCube::~OpenGLTextureCube()
    {
    }

    void OpenGLTextureCube::Delete()
    {
        OGN_PROFILER_RENDERING();

        glBindTexture(m_Index, 0);
    }

    void OpenGLTextureCube::ChangeSize(u64 width, u64 height)
    {
    }

    void OpenGLTextureCube::SetData(Buffer data)
    {
    }

    void OpenGLTextureCube::LoadFaces(std::string &filepath, const Faces faces)
    {
        OGN_PROFILER_RENDERING();

        m_Filepath = filepath;

        int width, height, bpp;
        stbi_uc *data = nullptr;

        stbi_set_flip_vertically_on_load(false);
        GLenum dataFormat = 0, internal_format = 0;

        // set the file path
        data = stbi_load(filepath.c_str(), &height, &width, &bpp, 0);

        switch (bpp)
        {
        case 3:
            internal_format = GL_RGB8;
            dataFormat = GL_RGB;
            break;
        case 4:
            internal_format = GL_RGBA8;
            dataFormat = GL_RGBA;
            break;
        }

        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + faces, 0, static_cast<i32>(internal_format),
                width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
            OGN_CORE_TRACE("[Texture2D] \"{0}\" Successfully Loaded", filepath);
        }
        else OGN_CORE_ERROR("[Texture2D] Failed to load Texture: {0}", filepath);

        stbi_image_free(data);

        /*OGN_CORE_TRACE("Bits Per Pixel : {0}", bpp);
        OGN_CORE_TRACE("Internal Format: {0}, Data Format: {1}", internalFormat, dataFormat);*/

        // generate texture
        glGenTextures(1, &m_TextureID);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    void OpenGLTextureCube::Bind(u32 slot)
    {
    }

    void OpenGLTextureCube::Bind(u32 bindingPoint, u32 index, u32 arrayCount)
    {
    }

    void OpenGLTextureCube::Unbind()
    {
    }

}