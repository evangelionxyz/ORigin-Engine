#include "pch.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Platform\OpenGL\OpenGL_Mesh.h"

namespace Origin
{
	std::shared_ptr<Mesh> Mesh::Create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:			return nullptr;
			case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLMesh>(vertices, indices);
		}

		return nullptr;
	}

}
