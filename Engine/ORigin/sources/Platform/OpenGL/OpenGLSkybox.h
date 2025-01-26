// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef OPENGL_SKYBOX_H
#define OPENGL_SKYBOX_H

#include "Origin/Renderer/VertexArray.h"
#include "Origin/Renderer/Shader.h"
#include "Origin/Renderer/Buffer.h"
#include "Origin/Renderer/RenderCommand.h"
#include "Origin/Scene/Skybox.h"

#include <glm/glm.hpp>
#include <string>

namespace origin
{
	class OpenGLSkybox : public Skybox
	{
	public:
		OpenGLSkybox(const std::string& filepath, const std::string& format);

	private:
		void LoadTexture();
		std::string m_Filepath;
		std::string m_ImageFormat;
	};
}

#endif