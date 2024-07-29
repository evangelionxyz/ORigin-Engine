// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin/Renderer/Texture.h"
#include <glad/glad.h>

namespace origin {

  class OpenGLTexture2D : public Texture2D
  {
  private:
		TextureSpecification m_Spec;

		std::string m_MaterialTypeName;

    uint32_t m_RendererID = 0;
    std::string m_Filepath;
    uint32_t m_Width = 0, m_Height = 0, m_BPP = 0;
    uint32_t m_Index = 0;
    GLenum m_InternalFormat, m_DataFormat, m_MinFilter, m_MagFilter;
		bool m_IsLoaded = false;

  public:
    OpenGLTexture2D(const TextureSpecification& specification, Buffer data);
    OpenGLTexture2D(const std::filesystem::path& filepath, const TextureSpecification& specification);
    ~OpenGLTexture2D();

		const TextureSpecification& GetSpecification() const override { return m_Spec; }

		void SetData(Buffer data) override;
		std::string GetFilepath() override { return m_Filepath; }
		void Bind(uint32_t bindingPoint, uint32_t index, uint32_t arrayCount) override;
		void Bind(uint32_t slot = 0) override;
		void Unbind() override;
		void Delete() override;
		uint32_t GetRendererID() const override { return m_RendererID; }

		uint64_t GetEstimatedSize() const override { return m_Width * m_Height * 4; }
		void ChangeSize(uint64_t width, uint64_t height) override;

		std::string GetName() const override
		{
			return m_Filepath.substr(m_Filepath.find_last_of('/') + 1, m_Filepath.size());
		}
		uint32_t GetIndex() const override { return m_Index; }
		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }
		bool IsLoaded() const override { return m_IsLoaded; }
    bool operator== (const Texture& other) const override
    {
      return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
    }

		void SetMaterialTypeName(const std::string& typeName) override { m_MaterialTypeName = typeName; }
		const std::string& GetMaterialTypeName() const override { return m_MaterialTypeName; }
  };

	class OpenGLTextureCube : public TextureCube
	{
	private:
		TextureSpecification m_Spec;

		std::string m_MaterialTypeName;

		uint32_t m_RendererID;
		std::string m_Filepath;
		uint32_t m_Width = 0, m_Height = 0, m_BPP = 0;
		uint32_t m_Index = 0;
		uint32_t m_LoadCount = 0;
		GLenum m_InternalFormat, m_DataFormat;

	public:
		OpenGLTextureCube(uint32_t width, uint32_t height);
		OpenGLTextureCube(const std::string& path);
		~OpenGLTextureCube();

		void SetData(Buffer data) override;
		std::string GetFilepath() override { return m_Filepath; }

		void LoadFaces(std::string& filepath, Faces faces);
		void Bind(uint32_t bindingPoint, uint32_t index, uint32_t arrayCount) override;
		void Bind(uint32_t slot = 0) override;
		void Unbind() override;
		void Delete() override;

		const TextureSpecification& GetSpecification() const override { return m_Spec; }

		uint32_t GetRendererID() const override { return m_RendererID; }
		uint32_t GetIndex() const override { return m_Index; }
		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }

		uint64_t GetEstimatedSize() const override { return m_Width * m_Height * 4; }
		void ChangeSize(uint64_t width, uint64_t height) override;

		std::string GetName() const override 
		{
			return m_Filepath.substr(m_Filepath.find_last_of('/') + 1, m_Filepath.size());
		}

		bool IsLoaded() const override { return false; }
		bool operator== (const Texture& other) const override
		{
			return m_RendererID == ((OpenGLTextureCube&)other).m_RendererID;
		}

		void SetMaterialTypeName(const std::string& typeName) override { m_MaterialTypeName = typeName; }
		const std::string& GetMaterialTypeName() const override { return m_MaterialTypeName; }
	};
}
