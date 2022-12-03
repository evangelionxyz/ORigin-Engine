// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Renderer\Texture.h"
#include <glad\glad.h>

namespace Origin {

  class OpenGLTexture3D : public Texture3D
  {
	private:
		uint32_t m_RendererID;
		std::string m_FilePath;
		uint32_t m_Width, m_Height, m_BPP;
		uint32_t m_Index;
		GLenum m_InternalFormat, m_DataFormat;

	public:
    OpenGLTexture3D(uint32_t width, uint32_t height);
    OpenGLTexture3D(const std::string& path);
		~OpenGLTexture3D();

		virtual void SetData(void* data, uint32_t size) override;
    virtual std::string GetFilepath() override { return m_FilePath; }
		virtual void Bind(uint32_t slot = 0) override;
    virtual void Delete() override;

		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual uint32_t GetIndex() const override { return m_Index; }

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }

		virtual bool operator== (const Texture& other) const override
		{
			return m_RendererID == ((OpenGLTexture3D&)other).m_RendererID;
		}

  };

  class OpenGLTexture2D : public Texture2D
  {
  private:
    uint32_t m_RendererID;
    std::string m_FilePath;
    uint32_t m_Width, m_Height, m_BPP;
    uint32_t m_Index;
    GLenum m_InternalFormat, m_DataFormat;

  public:
    OpenGLTexture2D(uint32_t width, uint32_t height);
    OpenGLTexture2D(const std::string& path);
    ~OpenGLTexture2D();

    virtual void SetData(void* data, uint32_t size) override;
    virtual std::string GetFilepath() override { return m_FilePath; }
    virtual void Bind(uint32_t slot = 0) override;
    virtual void Delete() override;

    virtual uint32_t GetRendererID() const override { return m_RendererID; }
    virtual uint32_t GetIndex() const override { return m_Index; }
    virtual uint32_t GetWidth() const override { return m_Width; }
    virtual uint32_t GetHeight() const override{ return m_Height; }

    virtual bool operator== (const Texture& other) const override
    {
      return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
    }
  };

}
