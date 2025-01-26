// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "OpenGLSkybox.h"
#include "Origin/Renderer/Renderer.h"

#include "Origin/Profiler/Profiler.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace origin {
OpenGLSkybox::OpenGLSkybox(const std::string &filepath, const std::string &format)
	: m_Filepath(filepath), m_ImageFormat(format)
{
	OGN_PROFILER_RENDERING();

	f32 vertices[] = {

		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
	};

	vbo = VertexBuffer::Create(vertices, sizeof(vertices));
	vbo->SetLayout({ {ShaderDataType::Float3, "position"} });

	vao = VertexArray::Create();
	vao->AddVertexBuffer(vbo);

	u32 *indices = new u32[36];
	u32 Offset = 0;
	for (u32 i = 0; i < 36; i += 6)
	{
		indices[i + 0] = Offset + 0;
		indices[i + 1] = Offset + 1;
		indices[i + 2] = Offset + 2;

		indices[i + 3] = Offset + 2;
		indices[i + 4] = Offset + 3;
		indices[i + 5] = Offset + 0;

		Offset += 4;
	}

	Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, 36);
	vao->SetIndexBuffer(indexBuffer);
	delete[] indices;

	LoadTexture();
}

void OpenGLSkybox::LoadTexture()
{
	std::vector<std::string> path
	{
		m_Filepath + "/right" + m_ImageFormat,
		m_Filepath + "/left" + m_ImageFormat,
		m_Filepath + "/top" + m_ImageFormat,
		m_Filepath + "/bottom" + m_ImageFormat,
		m_Filepath + "/front" + m_ImageFormat,
		m_Filepath + "/back" + m_ImageFormat
	};

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

	i32 width, height, bpp;
	stbi_uc *data = nullptr;

	stbi_set_flip_vertically_on_load(false);

	for (size_t i = 0; i < path.size(); i++)
	{
		data = stbi_load(path[i].c_str(), &height, &width, &bpp, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(i), 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			stbi_image_free(data);
			OGN_CORE_ASSERT(false, "Failed to load Texture: {}", path[i]);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

}
