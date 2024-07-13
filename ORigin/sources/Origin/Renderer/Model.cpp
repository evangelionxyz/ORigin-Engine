// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Model.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLModel.h"

namespace origin
{
	std::shared_ptr<Model> Model::Create(const char *filepath, Scene *scene)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:		return nullptr;
			case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLModel>(filepath, scene);
		}
		return nullptr;
	}

}

