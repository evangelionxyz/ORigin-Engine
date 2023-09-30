#include "pch.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Platform\OpenGL\OpenGL_Mesh.h"

namespace origin
{
	std::shared_ptr<Mesh> Mesh::Create(
		const std::vector<MeshVertex>& vertices,
		const std::vector<uint32_t>& indices,
		const std::vector<std::shared_ptr<Texture2D>>& textures,
		const std::string& modelFilepath)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:			return nullptr;
			case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLMesh>(vertices, indices, textures, modelFilepath);
		}

		return nullptr;
	}

}
