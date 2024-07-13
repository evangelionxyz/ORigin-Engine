#include "Origin/Renderer/Renderer.h"
#include "Origin/Renderer/VertexArray.h"
#include "Origin/Renderer/Shader.h"
#include "MeshVertexData.h"
#include "MeshRenderer.h"

namespace origin
{
	struct MeshRenderData
	{
		static const uint32_t MaxTriangles = 10000;
		static const uint32_t MaxVertices = MaxTriangles * 28;
		static const uint32_t MaxQuadIndices = MaxTriangles * 20;
		static const uint32_t MaxTextureSlots = 32;
	};

	struct MeshRendererData
	{
		std::shared_ptr<VertexArray> VertexArray;
		std::shared_ptr<VertexBuffer> VertexBuffer;
		std::shared_ptr<Shader> Shader;
		std::array<std::shared_ptr<Texture2D>, 32> TextureSlots;
		MeshVertexData *VertexBufferBase = nullptr;
		MeshVertexData *VertexBufferPtr = nullptr;
		uint32_t IndexCount = 0;
		uint32_t TextureSlotIndex = 1; // 0 White texture
	};

	static MeshRendererData s_Data;
	static MeshRenderData s_RData;

	void MeshRenderer::Init()
	{
		s_Data.VertexArray = VertexArray::Create();
		s_Data.VertexBuffer = VertexBuffer::Create(s_RData.MaxVertices * sizeof(MeshVertexData));
		s_Data.VertexBuffer->SetLayout({
			
			{ ShaderDataType::Float3, "aPosition"     },
			{ ShaderDataType::Float4, "aColor"        },
			{ ShaderDataType::Float2, "aTexCoord"     },
			{ ShaderDataType::Float,  "aTexIndex"     },
			{ ShaderDataType::Int,    "aEntityID"     }
		});

		s_Data.VertexArray->AddVertexBuffer(s_Data.VertexBuffer);
		s_Data.VertexBufferBase = new MeshVertexData[s_RData.MaxVertices];
		// Initialize the index buffer with a dummy size
		std::shared_ptr<IndexBuffer> indexBuffer = IndexBuffer::Create(nullptr, s_RData.MaxQuadIndices);
		s_Data.VertexArray->SetIndexBuffer(indexBuffer);

		s_Data.TextureSlots[0] = Renderer::WhiteTexture;
	}

	void MeshRenderer::Begin()
	{
		MeshRenderer::StartBatch();
	}

	void MeshRenderer::StartBatch()
	{
		s_Data.IndexCount = 0;
		s_Data.VertexBufferPtr = s_Data.VertexBufferBase;
		s_Data.TextureSlotIndex = 1;
	}

	void MeshRenderer::Flush()
	{
		if (s_Data.IndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t *)s_Data.VertexBufferPtr - (uint8_t *)s_Data.VertexBufferBase);
			s_Data.VertexBuffer->SetData(s_Data.VertexBufferBase, dataSize);
			RenderCommand::DrawIndexed(s_Data.VertexArray, s_Data.IndexCount);
		}
	}

	void MeshRenderer::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void MeshRenderer::End()
	{
		Flush();
	}

	void MeshRenderer::Draw(const glm::mat4 &transform, std::span<MeshVertexData> vertices, std::span<uint32_t> indices)
	{
		if (s_Data.IndexCount + indices.size() >= s_RData.MaxQuadIndices)
			NextBatch();

		for (const auto &vertex : vertices)
		{
			s_Data.VertexBufferPtr->Position = transform * glm::vec4(vertex.Position, 1.0f);
			s_Data.VertexBufferPtr->Color = vertex.Color;
			s_Data.VertexBufferPtr->TexCoord = vertex.TexCoord;
			s_Data.VertexBufferPtr->TexIndex = 1;
			s_Data.VertexBufferPtr->EntityID = -1;
			s_Data.VertexBufferPtr++;
		}

		s_Data.IndexCount += indices.size();
	}

	void MeshRenderer::Shutdown()
	{
		delete[] s_Data.VertexBufferBase;
	}
}

