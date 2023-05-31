// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Renderer\Texture.h"
#include <glad\glad.h>

namespace origin {

  class OpenGLTexture2D : public Texture2D
  {
  private:
    uint32_t m_RendererID;
    std::string m_FilePath;
    uint32_t m_Width = 0, m_Height = 0, m_BPP = 0;
    uint32_t m_Index = 0;

    GLenum m_InternalFormat, m_DataFormat;

		bool m_IsLoaded = false;

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
		bool IsLoaded() const override { return m_IsLoaded; }
    bool operator== (const Texture& other) const override
    {
      return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
    }
  };

	class OpenGLTextureCube : public TextureCube
	{
	private:
		uint32_t m_RendererID;
		std::string m_FilePath;
		uint32_t m_Width = 0, m_Height = 0, m_BPP = 0;
		uint32_t m_Index = 0;
		uint32_t m_LoadCount = 0;
		GLenum m_InternalFormat, m_DataFormat;

	public:
		OpenGLTextureCube(uint32_t width, uint32_t height);
		OpenGLTextureCube(const std::string& path);
		~OpenGLTextureCube();

		void SetData(void* data, uint32_t size) override;
		std::string GetFilepath() override { return m_FilePath; }

		void LoadFaces(std::string& filepath, Faces faces);

		void Bind(uint32_t slot = 0) override;
		void Delete() override;

		uint32_t GetRendererID() const override { return m_RendererID; }
		uint32_t GetIndex() const override { return m_Index; }
		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }
		bool IsLoaded() const override { return false; }
		bool operator== (const Texture& other) const override
		{
			return m_RendererID == ((OpenGLTextureCube&)other).m_RendererID;
		}
	};
}
