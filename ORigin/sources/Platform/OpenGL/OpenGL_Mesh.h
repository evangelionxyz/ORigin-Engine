// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine
#pragma once
#include "Origin\Renderer\Mesh.h"
#include "Origin\Renderer\VertexArray.h"
#include "Origin\Renderer\Buffer.h"
#include <vector>

namespace origin
{
	class OpenGLMesh : public Mesh
	{
	public:
		OpenGLMesh() = default;

		OpenGLMesh(
			const std::vector<MeshVertex>& vertices, 
			const std::vector<uint32_t>& indices, 
			const std::vector<std::shared_ptr<Texture2D>>& textures, 
			const std::string& modelFilepath = "");

		~OpenGLMesh() override;

		void Draw(const std::shared_ptr<Shader>& shader) override;
		bool IsLoaded() const override { return m_Loaded; }

	private:
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::vector<std::shared_ptr<Texture2D>> m_Textures;

		bool m_Loaded = false;
	};
}