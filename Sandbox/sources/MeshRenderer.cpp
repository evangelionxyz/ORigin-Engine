#include "Origin/Renderer/Renderer.h"
#include "Origin/Renderer/VertexArray.h"
#include "Origin/Renderer/Shader.h"
#include "MeshVertexData.h"
#include "MeshRenderer.h"
#include "ModelLoader.h"

namespace origin
{
	struct MeshRenderData
	{
		static const uint32_t MaxCubeTriangles = 9000;
		static const uint32_t MaxCubeVertices = MaxCubeTriangles * 24;
		static const uint32_t MaxCubeIndices = MaxCubeTriangles * 36;

		static const uint32_t MaxSphereTriangles = 4896;
		static const uint32_t MaxSphereVertices = MaxSphereTriangles * 544;
		static const uint32_t MaxSphereIndices = MaxSphereTriangles * 768;

		static const uint32_t MaxTextureSlots = 32;
	};

	struct CameraBufferData
	{
		glm::mat4 ViewProjection;
		glm::vec3 Position;
	};

	struct MeshRendererData
	{
		CameraBufferData CameraData;
		std::shared_ptr<UniformBuffer> Ubo;
		std::shared_ptr<Shader> Shader;
        std::array<std::shared_ptr<Texture2D>, 32> TextureSlots;
        uint32_t TextureSlotIndex = 1; // 0 White texture

		// ===========================
		// CUBE
		MeshData CubeData;
		uint32_t CubeIndexCount = 0;
		
		std::shared_ptr<VertexArray> CubeVAO;
		std::shared_ptr<VertexBuffer> CubeVBO;
		MeshVertexData *CubeVBOPtr = nullptr;
		MeshVertexData *CubeVBOBase = nullptr;

		// ===========================
		// SPHERE
		MeshData SphereData;
		uint32_t SphereIndexCount = 0;
		std::shared_ptr<VertexArray> SphereVAO;
		std::shared_ptr<VertexBuffer> SphereVBO;
		MeshVertexData *SphereVBOPtr = nullptr;
		MeshVertexData *SphereVBOBase = nullptr;
	};

	static MeshRendererData s_Data;

	void MeshRenderer::Init()
	{
		s_Data.Ubo = UniformBuffer::Create(sizeof(CameraBufferData), 0);
		s_Data.Shader = Shader::Create("Resources/Shaders/MeshTest.glsl", true, true);
		s_Data.TextureSlots[0] = Renderer::WhiteTexture;


		BufferLayout bufferLayout = {
			{ ShaderDataType::Float3, "aPosition"     },
			{ ShaderDataType::Float3, "aNormal"       },
			{ ShaderDataType::Float4, "aColor"        },
			{ ShaderDataType::Float2, "aTexCoord"     },
			{ ShaderDataType::Float,  "aTexIndex"     },
			{ ShaderDataType::Int,    "aEntityID"     }
		};

		// ======================================
		// ================ Cube ================
		s_Data.CubeData = ModelLoader::LoadModel("Resources/Models/cube.obj");

		s_Data.CubeVAO = VertexArray::Create();
		s_Data.CubeVBO = VertexBuffer::Create(MeshRenderData::MaxCubeVertices * sizeof(MeshVertexData));
		s_Data.CubeVBO->SetLayout(bufferLayout);
		s_Data.CubeVAO->AddVertexBuffer(s_Data.CubeVBO);
		s_Data.CubeVBOBase = new MeshVertexData[MeshRenderData::MaxCubeVertices];

        uint32_t *cubeIndices = new uint32_t[MeshRenderData::MaxCubeIndices];
        uint32_t baseIndicesCount = s_Data.CubeData.indices.size();
        uint32_t maxCubes = MeshRenderData::MaxCubeIndices / baseIndicesCount;

        for (uint32_t cubeIndex = 0; cubeIndex < maxCubes; ++cubeIndex)
        {
            uint32_t vertexOffset = cubeIndex * s_Data.CubeData.vertices.size();
            for (uint32_t i = 0; i < baseIndicesCount; ++i)
            {
                cubeIndices[cubeIndex * baseIndicesCount + i] = s_Data.CubeData.indices[i] + vertexOffset;
            }
        }

        std::shared_ptr<IndexBuffer> cubeIBO = IndexBuffer::Create(cubeIndices, MeshRenderData::MaxCubeIndices);
		s_Data.CubeVAO->SetIndexBuffer(cubeIBO);
        delete[] cubeIndices;


        // ======================================
        // ================ Sphere ================
        s_Data.SphereData = ModelLoader::LoadModel("Resources/Models/sphere.obj");
        s_Data.SphereVAO = VertexArray::Create();
        s_Data.SphereVBO = VertexBuffer::Create(MeshRenderData::MaxSphereVertices * sizeof(MeshVertexData));
        s_Data.SphereVBO->SetLayout(bufferLayout);
        s_Data.SphereVAO->AddVertexBuffer(s_Data.SphereVBO);
        s_Data.SphereVBOBase = new MeshVertexData[MeshRenderData::MaxSphereVertices];

        uint32_t *sphereIndices = new uint32_t[MeshRenderData::MaxSphereIndices];
		baseIndicesCount = s_Data.SphereData.indices.size();
		uint32_t maxSpheres = MeshRenderData::MaxSphereIndices / baseIndicesCount;

        for (uint32_t sphereIndex = 0; sphereIndex < maxSpheres; ++sphereIndex)
        {
            uint32_t vertexOffset = sphereIndex * s_Data.SphereData.vertices.size();
            for (uint32_t i = 0; i < baseIndicesCount; ++i)
            {
                sphereIndices[sphereIndex * baseIndicesCount + i] = s_Data.SphereData.indices[i] + vertexOffset;
            }
        }

        std::shared_ptr<IndexBuffer> sphereIBO = IndexBuffer::Create(sphereIndices, MeshRenderData::MaxSphereIndices);
        s_Data.SphereVAO->SetIndexBuffer(sphereIBO);
        delete[] sphereIndices;
        
	}

	void MeshRenderer::Begin(const Camera &camera)
	{
		s_Data.CameraData.ViewProjection = camera.GetViewProjection();
		s_Data.CameraData.Position = camera.GetPosition();

		s_Data.Ubo->Bind();
		s_Data.Ubo->SetData(&s_Data.CameraData, sizeof(CameraBufferData));

		StartBatch();
	}

	void MeshRenderer::StartBatch()
	{
		s_Data.TextureSlotIndex = 1;

		s_Data.CubeIndexCount = 0;
		s_Data.CubeVBOPtr = s_Data.CubeVBOBase;

		s_Data.SphereIndexCount = 0;
        s_Data.SphereVBOPtr = s_Data.SphereVBOBase;
	}

	void MeshRenderer::Flush()
	{
		s_Data.Shader->Enable();

		if (s_Data.CubeIndexCount)
		{
			uint32_t dataSize = (uint8_t *)s_Data.CubeVBOPtr - (uint8_t *)s_Data.CubeVBOBase;
			s_Data.CubeVBO->SetData(s_Data.CubeVBOBase, dataSize);

			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
				s_Data.TextureSlots[i]->Bind(i);
			RenderCommand::DrawIndexed(s_Data.CubeVAO, s_Data.CubeIndexCount);

			Renderer::GetStatistics().DrawCalls++;
		}

        if (s_Data.SphereIndexCount)
        {
            uint32_t dataSize = (uint8_t *)s_Data.SphereVBOPtr - (uint8_t *)s_Data.SphereVBOBase;
            s_Data.SphereVBO->SetData(s_Data.SphereVBOBase, dataSize);

            for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
                s_Data.TextureSlots[i]->Bind(i);
            RenderCommand::DrawIndexed(s_Data.SphereVAO, s_Data.SphereIndexCount);
			Renderer::GetStatistics().DrawCalls++;
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

	void MeshRenderer::DrawCube(const glm::mat4 &transform, const glm::vec4 &color, int entityID)
	{
		if (s_Data.CubeIndexCount >= MeshRenderData::MaxCubeIndices)
			NextBatch();

		for (size_t i = 0; i < 24; i++)
		{
			s_Data.CubeVBOPtr->Position = transform * glm::vec4(s_Data.CubeData.vertices[i].Position, 1.0f);
			s_Data.CubeVBOPtr->Normal = glm::mat3(transform) * s_Data.CubeData.vertices[i].Normal;
			s_Data.CubeVBOPtr->Color = color;
			s_Data.CubeVBOPtr->TexCoord = s_Data.CubeData.vertices[i].TexCoord;
			s_Data.CubeVBOPtr->TexIndex = 0.0f;
			s_Data.CubeVBOPtr->EntityID = entityID;
			s_Data.CubeVBOPtr++;
		}

		s_Data.CubeIndexCount += 36;
		Renderer::GetStatistics().CubeCount++;
	}

    void MeshRenderer::DrawSphere(const glm::mat4 &transform, const glm::vec4 &color, int entityID)
    {
        if (s_Data.SphereIndexCount >= MeshRenderData::MaxSphereIndices)
            NextBatch();

        for (size_t i = 0; i < 544; i++)
        {
            s_Data.SphereVBOPtr->Position = transform * glm::vec4(s_Data.SphereData.vertices[i].Position, 1.0f);
            s_Data.SphereVBOPtr->Normal = glm::mat3(transform) * s_Data.SphereData.vertices[i].Normal;
            s_Data.SphereVBOPtr->Color = color;
            s_Data.SphereVBOPtr->TexCoord = s_Data.SphereData.vertices[i].TexCoord;
            s_Data.SphereVBOPtr->TexIndex = 0.0f;
            s_Data.SphereVBOPtr->EntityID = entityID;
            s_Data.SphereVBOPtr++;
        }

        s_Data.SphereIndexCount += 768;
		Renderer::GetStatistics().SphereCount++;
    }

    void MeshRenderer::Shutdown()
	{
		delete[] s_Data.CubeVBOBase;
		delete[] s_Data.SphereVBOBase;
	}
}

