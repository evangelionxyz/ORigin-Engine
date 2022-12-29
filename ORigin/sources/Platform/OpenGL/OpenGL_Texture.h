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

		void SetData(void* data, uint32_t size) override;
		std::string GetFilepath() override { return m_FilePath; }
		void Bind(uint32_t slot = 0) override;
		void Delete() override;

		uint32_t GetRendererID() const override { return m_RendererID; }

		uint32_t GetIndex() const override { return m_Index; }

		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }

		bool operator== (const Texture& other) const override
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

		void SetData(void* data, uint32_t size) override;
		std::string GetFilepath() override { return m_FilePath; }
		void Bind(uint32_t slot = 0) override;
		void Delete() override;

		uint32_t GetRendererID() const override { return m_RendererID; }
		uint32_t GetIndex() const override { return m_Index; }
		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }

    bool operator== (const Texture& other) const override
    {
      return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
    }
  };

}
