#include "pch.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Platform\OpenGL\OpenGL_Mesh.h"

namespace origin
{
	std::shared_ptr<Mesh> Mesh::Create(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:			return nullptr;
		case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLMesh>(vertices, indices);
		}

		return nullptr;
	}

}
