// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef OPENGL_TEXTURE_H
#define OPENGL_TEXTURE_H

#include "Origin/Renderer/Texture.h"

namespace origin {

class OpenGLTexture2D final : public Texture2D
{
public:
	OpenGLTexture2D(const TextureSpecification &specification, Buffer data);
	OpenGLTexture2D(const std::filesystem::path &filepath, const TextureSpecification &specification);
	OpenGLTexture2D(const aiTexture *embedded_texture);

	~OpenGLTexture2D() override;

	void SetData(Buffer data) override;
	void Bind(u32 binding_point, u32 index, u32 array_count) override;
	void Bind(u32 slot = 0) override;
	void Unbind() override;
	void Destroy() override;
	void ChangeSize(const i32 width, const i32 height) override;

	bool operator==(const Texture &other) const override
	{
		return m_id == ((OpenGLTexture2D &)other).m_id;
	}

private:
	u32 m_index = 0;
};

}

#endif