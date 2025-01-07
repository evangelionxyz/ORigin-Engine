// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef OPENGL_TEXTURE_H
#define OPENGL_TEXTURE_H

#include "Origin/Renderer/Texture.h"
#include <glad/glad.h>

namespace origin {

	class OpenGLTexture2D final : public Texture2D
	{
	private:
		u32 m_Width = 0, m_Height = 0, m_BPP = 0;
		u32 m_Index = 0;
		GLenum m_InternalFormat, m_DataFormat, m_MinFilter, m_MagFilter;
		TextureSpecification m_Spec;
		std::string m_Filepath;
		bool m_IsLoaded = false;
	public:
		OpenGLTexture2D(const TextureSpecification &specification, Buffer data);
		OpenGLTexture2D(const std::filesystem::path &filepath, const TextureSpecification &specification);
		OpenGLTexture2D(const aiTexture *embedded_texture);
		~OpenGLTexture2D() override;

		[[nodiscard]] const TextureSpecification &GetSpecification() const override { return m_Spec; }

		void SetData(Buffer data) override;
		std::string GetFilepath() override { return m_Filepath; }
		void Bind(u32 binding_point, u32 index, u32 array_count) override;
		void Bind(u32 slot = 0) override;
		void Unbind() override;
		void Delete() override;

		[[nodiscard]] u64 GetEstimatedSize() const override { return m_Width * m_Height * 4; }

		void ChangeSize(const i32 width, const i32 height) override;

		[[nodiscard]] std::string GetName() const override
		{
			return m_Filepath.substr(m_Filepath.find_last_of('/') + 1, m_Filepath.size());
		}
		[[nodiscard]] u32 GetIndex() const override { return m_Index; }
		[[nodiscard]] u32 GetWidth() const override { return m_Width; }
		[[nodiscard]] u32 GetHeight() const override { return m_Height; }
		[[nodiscard]] bool IsLoaded() const override { return m_IsLoaded; }

		bool operator== (const Texture &other) const override
		{
			return m_TextureID == ((OpenGLTexture2D &)other).m_TextureID;
		}
	};

	class OpenGLTextureCube final : public TextureCube
	{
	private:
		u32 m_Width = 0, m_Height = 0, m_BPP = 0;
		u32 m_Index = 0;
		u32 m_LoadCount = 0;
		GLenum m_InternalFormat, m_DataFormat;
		TextureSpecification m_Spec;
		std::string m_Filepath;

	public:
		OpenGLTextureCube(u32 width, u32 height);
		explicit OpenGLTextureCube(const std::string &path);
		~OpenGLTextureCube() override;

		void SetData(Buffer data) override;
		std::string GetFilepath() override { return m_Filepath; }

		void LoadFaces(std::string &filepath, Faces faces);
		void Bind(u32 bindingPoint, u32 index, u32 arrayCount) override;
		void Bind(u32 slot = 0) override;
		void Unbind() override;
		void Delete() override;

		[[nodiscard]] const TextureSpecification &GetSpecification() const override { return m_Spec; }

		[[nodiscard]]u32 GetIndex() const override { return m_Index; }
		[[nodiscard]]u32 GetWidth() const override { return m_Width; }
		[[nodiscard]]u32 GetHeight() const override { return m_Height; }

		[[nodiscard]] u64 GetEstimatedSize() const override { return m_Width * m_Height * 4; }
		void ChangeSize(const i32 width, const i32 height) override;

		[[nodiscard]] std::string GetName() const override
		{
			return m_Filepath.substr(m_Filepath.find_last_of('/') + 1, m_Filepath.size());
		}

		bool IsLoaded() const override { return false; }
		bool operator== (const Texture &other) const override
		{
			return m_TextureID == ((OpenGLTextureCube &)other).m_TextureID;
		}
	};
}

#endif