// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef OPENGL_TEXTURE_H
#define OPENGL_TEXTURE_H

#include "Origin/Renderer/Texture.h"
#include <glad/glad.h>

namespace origin {

	class OpenGLTexture2D final : public Texture2D
	{
	private:
		GLenum m_InternalFormat, m_DataFormat, m_MinFilter, m_MagFilter;
		TextureSpecification m_Spec;
		std::string m_Filepath;
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

		[[nodiscard]] u64 GetEstimatedSize() const override { return m_width * m_height * 4; }

		void ChangeSize(const i32 width, const i32 height) override;

		[[nodiscard]] std::string GetName() const override
		{
			return m_Filepath.substr(m_Filepath.find_last_of('/') + 1, m_Filepath.size());
		}

		bool operator== (const Texture &other) const override
		{
			return m_id == ((OpenGLTexture2D &)other).m_id;
		}
	};

	class OpenGLTextureCube final : public TextureCube
	{
	private:
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

		[[nodiscard]] u64 GetEstimatedSize() const override { return m_width * m_height * 4; }
		void ChangeSize(const i32 width, const i32 height) override;

		[[nodiscard]] std::string GetName() const override
		{
			return m_Filepath.substr(m_Filepath.find_last_of('/') + 1, m_Filepath.size());
		}

		bool operator== (const Texture &other) const override
		{
			return m_id == ((OpenGLTextureCube &)other).m_id;
		}
	};
}

#endif