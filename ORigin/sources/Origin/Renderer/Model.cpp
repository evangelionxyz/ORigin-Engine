#include "pch.h"
#include "Model.h"
#include "Renderer.h"

#include "Platform\OpenGL\OpenGL_Model.h"

namespace origin
{
	std::shared_ptr<Model> Model::Create(const std::string& filepath, std::shared_ptr<Material> material)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:			return nullptr;
		case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLModel>(filepath, material);
		}

		return nullptr;
	}

	std::shared_ptr<Model> Model::Create(const std::string& filepath)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:			return nullptr;
		case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLModel>(filepath);
		}

		return nullptr;
	}

}

