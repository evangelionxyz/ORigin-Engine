// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Renderer.h"
#include "Renderer3D.h"
#include "Renderer2D.h"
#include "UniformBuffer.h"

#include "Origin/Asset/AssetManager.h"

namespace origin {

	struct CameraBufferData
	{
		glm::mat4 ViewProjection;
		glm::vec3 Position;
	};

	static CameraBufferData s_CameraBufferData;
	static std::shared_ptr<UniformBuffer> s_CameraUniformBuffer;

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;

		int EntityID;
	};

	struct Renderer3DData
	{
		// Cube Data
		std::shared_ptr<Shader> CubeShader;
		std::shared_ptr<VertexArray> CubeVertexArray;
		std::shared_ptr<VertexBuffer> CubeVertexBuffer;

		uint32_t CubeIndexCount = 0;
		Vertex* CubeVertexBufferBase = nullptr;
		Vertex* CubeVertexBufferPtr = nullptr;
		glm::vec4 CubeVertexPosition[24];
		std::array<std::shared_ptr<Texture2D>, 32> TextureSlots;
		uint32_t TextureSlotIndex = 1;
	};

	static Renderer3DData s_Render3DData;

	void Renderer3D::Init()
	{
		OGN_PROFILER_FUNCTION();

		s_CameraUniformBuffer = UniformBuffer::Create(sizeof(CameraBufferData), 0);

		//  ======== Cube ========
		s_Render3DData.CubeVertexArray = VertexArray::Create();
		s_Render3DData.CubeVertexBuffer = VertexBuffer::Create(Renderer::s_RenderData.MaxVertices * sizeof(Vertex));
		s_Render3DData.CubeVertexBuffer->SetLayout({
			{ShaderDataType::Float3,	"a_Pos"			},
			{ShaderDataType::Float4,	"a_Color"		},
			{ShaderDataType::Float2,	"a_TexCoord"    },
			{ShaderDataType::Float,		"a_TexIndex"    },
			{ShaderDataType::Int,		"a_EntityID"    },
		});

		s_Render3DData.CubeVertexArray->AddVertexBuffer(s_Render3DData.CubeVertexBuffer);
		s_Render3DData.CubeVertexBufferBase = new Vertex[Renderer::s_RenderData.MaxVertices];

		auto CubeIndices = new uint32_t[Renderer::s_RenderData.MaxQuadIndices];
		uint32_t Offset = 0;
		for (uint32_t i = 0; i < Renderer::s_RenderData.MaxQuadIndices; i += 6)
		{
			CubeIndices[i + 0] = Offset + 0;
			CubeIndices[i + 1] = Offset + 1;
			CubeIndices[i + 2] = Offset + 2;

			CubeIndices[i + 3] = Offset + 2;
			CubeIndices[i + 4] = Offset + 3;
			CubeIndices[i + 5] = Offset + 0;
			Offset += 4;
		}

		std::shared_ptr<IndexBuffer> CubeIndexBuffer = IndexBuffer::Create(CubeIndices, Renderer::s_RenderData.MaxQuadIndices);
		s_Render3DData.CubeVertexArray->SetIndexBuffer(CubeIndexBuffer);
		delete[] CubeIndices;

		s_Render3DData.TextureSlots[0] = Renderer::WhiteTexture;
		{
			// Front
			s_Render3DData.CubeVertexPosition[0] = glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f);
			s_Render3DData.CubeVertexPosition[1] = glm::vec4(0.5f, -0.5f, 0.5f, 1.0f);
			s_Render3DData.CubeVertexPosition[2] = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
			s_Render3DData.CubeVertexPosition[3] = glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f);

			// Back
			s_Render3DData.CubeVertexPosition[4] = glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f);
			s_Render3DData.CubeVertexPosition[5] = glm::vec4(0.5f, -0.5f, -0.5f, 1.0f);
			s_Render3DData.CubeVertexPosition[6] = glm::vec4(0.5f, 0.5f, -0.5f, 1.0f);
			s_Render3DData.CubeVertexPosition[7] = glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f);

			// Left
			s_Render3DData.CubeVertexPosition[8] = glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f);
			s_Render3DData.CubeVertexPosition[9] = glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f);
			s_Render3DData.CubeVertexPosition[10] = glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f);
			s_Render3DData.CubeVertexPosition[11] = glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f);

			// Right
			s_Render3DData.CubeVertexPosition[12] = glm::vec4(0.5f, -0.5f, 0.5f, 1.0f);
			s_Render3DData.CubeVertexPosition[13] = glm::vec4(0.5f, -0.5f, -0.5f, 1.0f);
			s_Render3DData.CubeVertexPosition[14] = glm::vec4(0.5f, 0.5f, -0.5f, 1.0f);
			s_Render3DData.CubeVertexPosition[15] = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

			// Top
			s_Render3DData.CubeVertexPosition[16] = glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f);
			s_Render3DData.CubeVertexPosition[17] = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
			s_Render3DData.CubeVertexPosition[18] = glm::vec4(0.5f, 0.5f, -0.5f, 1.0f);
			s_Render3DData.CubeVertexPosition[19] = glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f);

			// Bottom
			s_Render3DData.CubeVertexPosition[20] = glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f);
			s_Render3DData.CubeVertexPosition[21] = glm::vec4(0.5f, -0.5f, 0.5f, 1.0f);
			s_Render3DData.CubeVertexPosition[22] = glm::vec4(0.5f, -0.5f, -0.5f, 1.0f);
			s_Render3DData.CubeVertexPosition[23] = glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f);
		}

		s_Render3DData.CubeShader = Renderer::GetShader("Cube");
	}

	void Renderer3D::Begin(const Camera &camera, const glm::mat4& camTransform)
	{
		OGN_PROFILER_RENDERING();

		s_CameraBufferData.ViewProjection = camera.GetProjection() * glm::inverse(camTransform);
		s_CameraBufferData.Position = camera.GetPosition();

		s_CameraUniformBuffer->Bind();
		s_CameraUniformBuffer->SetData(&s_CameraBufferData, sizeof(CameraBufferData));

		StartBatch();
	}

	void Renderer3D::Begin(const Camera &camera)
	{
		OGN_PROFILER_RENDERING();

		s_CameraBufferData.ViewProjection = camera.GetViewProjection();
		s_CameraBufferData.Position = camera.GetPosition();

		s_CameraUniformBuffer->Bind();
		s_CameraUniformBuffer->SetData(&s_CameraBufferData, sizeof(CameraBufferData));

		StartBatch();
	}

	void Renderer3D::StartBatch()
	{
		OGN_PROFILER_RENDERING();

		s_Render3DData.CubeIndexCount = 0;
		s_Render3DData.CubeVertexBufferPtr = s_Render3DData.CubeVertexBufferBase;

		s_Render3DData.TextureSlotIndex = 1;
	}

	void Renderer3D::End()
	{
		Flush();
	}

	void Renderer3D::Flush()
	{
		OGN_PROFILER_RENDERING();

		if(s_Render3DData.CubeIndexCount)
		{
			uint32_t dataSize = (uint8_t*)s_Render3DData.CubeVertexBufferPtr - (uint8_t*)s_Render3DData.CubeVertexBufferBase;
			s_Render3DData.CubeVertexBuffer->SetData(s_Render3DData.CubeVertexBufferBase, dataSize);

			for (uint32_t i = 0; i < s_Render3DData.TextureSlotIndex; i++)
				s_Render3DData.TextureSlots[i]->Bind(i);

			s_Render3DData.CubeShader->Enable();
			RenderCommand::DrawIndexed(s_Render3DData.CubeVertexArray, s_Render3DData.CubeIndexCount);
			Renderer::GetStatistics().DrawCalls++;
		}
	}

	void Renderer3D::DrawCube(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		OGN_PROFILER_RENDERING();

		if (s_Render3DData.CubeIndexCount >= Renderer::s_RenderData.MaxQuadIndices)
			NextBatch();

		constexpr uint8_t verticesSize = 24;
		const float textureIndex = 0.0f; // White Texture
		const float tilingFactor = 1.0f;
		glm::vec2 textureCoords = glm::vec2(0.0f);

		for (size_t i = 0; i < verticesSize; i++)
		{
			s_Render3DData.CubeVertexBufferPtr->Position = transform * s_Render3DData.CubeVertexPosition[i];
			s_Render3DData.CubeVertexBufferPtr->Color = color;
			s_Render3DData.CubeVertexBufferPtr->TexCoord = textureCoords;
			s_Render3DData.CubeVertexBufferPtr->TexIndex = textureIndex;
			s_Render3DData.CubeVertexBufferPtr->EntityID = entityID;
			s_Render3DData.CubeVertexBufferPtr++;
		}
		s_Render3DData.CubeIndexCount += 36;
		Renderer::GetStatistics().CubeCount++;
	}

	void Renderer3D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID)
	{
		glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
		glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

		Renderer2D::DrawLine(p0, p1, color);
		Renderer2D::DrawLine(p1, p2, color);
		Renderer2D::DrawLine(p2, p3, color);
		Renderer2D::DrawLine(p3, p0, color);
	}

	void Renderer3D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		glm::vec3 lineVertices[24];
		for (size_t i = 0; i < 24; i++)
			lineVertices[i] = transform * s_Render3DData.CubeVertexPosition[i];

		Renderer2D::DrawLine(lineVertices[0], lineVertices[1], color);
		Renderer2D::DrawLine(lineVertices[1], lineVertices[2], color);
		Renderer2D::DrawLine(lineVertices[2], lineVertices[3], color);
		Renderer2D::DrawLine(lineVertices[3], lineVertices[0], color);

		Renderer2D::DrawLine(lineVertices[4], lineVertices[5], color);
		Renderer2D::DrawLine(lineVertices[5], lineVertices[6], color);
		Renderer2D::DrawLine(lineVertices[6], lineVertices[7], color);
		Renderer2D::DrawLine(lineVertices[7], lineVertices[4], color);

		Renderer2D::DrawLine(lineVertices[8], lineVertices[9], color);
		Renderer2D::DrawLine(lineVertices[9], lineVertices[10], color);
		Renderer2D::DrawLine(lineVertices[10], lineVertices[11], color);
		Renderer2D::DrawLine(lineVertices[11], lineVertices[8], color);

		Renderer2D::DrawLine(lineVertices[12], lineVertices[13], color);
		Renderer2D::DrawLine(lineVertices[13], lineVertices[14], color);
		Renderer2D::DrawLine(lineVertices[14], lineVertices[15], color);
		Renderer2D::DrawLine(lineVertices[15], lineVertices[12], color);

		Renderer2D::DrawLine(lineVertices[16], lineVertices[17], color);
		Renderer2D::DrawLine(lineVertices[17], lineVertices[18], color);
		Renderer2D::DrawLine(lineVertices[18], lineVertices[19], color);
		Renderer2D::DrawLine(lineVertices[19], lineVertices[16], color);

		Renderer2D::DrawLine(lineVertices[20], lineVertices[21], color);
		Renderer2D::DrawLine(lineVertices[21], lineVertices[22], color);
		Renderer2D::DrawLine(lineVertices[22], lineVertices[23], color);
		Renderer2D::DrawLine(lineVertices[23], lineVertices[20], color);
	}

	void Renderer3D::DrawQuad(const glm::mat4& transform, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec4& color, int entityID)
	{
		OGN_PROFILER_RENDERING();

		for (size_t i = 0; i < 6; ++i)
		{
			s_Render3DData.CubeVertexBufferPtr->Position = transform * glm::vec4((i % 2 == 0 ? p0 : p2), 1.0f);
			s_Render3DData.CubeVertexBufferPtr->Color = color;
			s_Render3DData.CubeVertexBufferPtr->TexCoord = glm::vec2(0.0f);
			s_Render3DData.CubeVertexBufferPtr->TexIndex = 0.0f;
			s_Render3DData.CubeVertexBufferPtr->EntityID = entityID;
			s_Render3DData.CubeVertexBufferPtr++;

			s_Render3DData.CubeVertexBufferPtr->Position = transform * glm::vec4((i % 2 == 0 ? p1 : p3), 1.0f);
			s_Render3DData.CubeVertexBufferPtr->Color = color;
			s_Render3DData.CubeVertexBufferPtr->TexCoord = glm::vec2(0.0f);
			s_Render3DData.CubeVertexBufferPtr->TexIndex = 0.0f;
			s_Render3DData.CubeVertexBufferPtr++;

			s_Render3DData.CubeIndexCount += 6;
		}
	}

	void Renderer3D::DrawSphere(const glm::mat4& transform, const glm::vec4& color, float radius, int entityID, uint8_t segments, uint8_t stacks)
	{
		OGN_PROFILER_RENDERING();

		if (s_Render3DData.CubeIndexCount + (segments * stacks * 6) >= Renderer::s_RenderData.MaxQuadIndices)
			NextBatch();

		const float textureIndex = 0.0f; // White Texture
		const float tilingFactor = 1.0f;
		glm::vec2 textureCoords = glm::vec2(0.0f);

		float phiIncrement = glm::pi<float>() / stacks;
		float thetaIncrement = glm::two_pi<float>() / segments;

		for (uint8_t i = 0; i < stacks; ++i)
		{
			float phi0 = i * phiIncrement;
			float phi1 = (i + 1) * phiIncrement;

			for (uint8_t j = 0; j < segments; ++j)
			{
				float theta0 = j * thetaIncrement;
				float theta1 = (j + 1) * thetaIncrement;

				// Vertices for the sphere
				glm::vec3 p0(radius * sin(phi0) * cos(theta0), radius * cos(phi0), radius * sin(phi0) * sin(theta0));
				glm::vec3 p1(radius * sin(phi0) * cos(theta1), radius * cos(phi0), radius * sin(phi0) * sin(theta1));
				glm::vec3 p2(radius * sin(phi1) * cos(theta1), radius * cos(phi1), radius * sin(phi1) * sin(theta1));
				glm::vec3 p3(radius * sin(phi1) * cos(theta0), radius * cos(phi1), radius * sin(phi1) * sin(theta0));

				// Draw the faces of the sphere
				DrawQuad(transform, p0, p1, p2, p3, color, entityID);
			}
		}
	}

	void Renderer3D::DrawCapsule(const glm::mat4& transform, const glm::vec4& color, float radius, float height, int entityID)
	{
		OGN_PROFILER_RENDERING();

		constexpr uint8_t segments = 20;
		constexpr uint8_t stacks = 10;

		if (s_Render3DData.CubeIndexCount + (segments * 6 * 2) >= Renderer::s_RenderData.MaxQuadIndices)
			NextBatch();

		const float textureIndex = 0.0f; // White Texture
		const float tilingFactor = 1.0f;
		glm::vec2 textureCoords = glm::vec2(0.0f);

		// Draw cylinder
		float angleIncrement = glm::two_pi<float>() / static_cast<float>(segments);
		for (uint8_t i = 0; i < segments; ++i)
		{
			float angle0 = angleIncrement * static_cast<float>(i);
			float angle1 = angleIncrement * static_cast<float>(i + 1);

			// Vertices for the cylinder
			glm::vec3 p0(radius * cos(angle0), -height * 0.5f, radius * sin(angle0));
			glm::vec3 p1(radius * cos(angle1), -height * 0.5f, radius * sin(angle1));
			glm::vec3 p2(radius * cos(angle1), height * 0.5f, radius * sin(angle1));
			glm::vec3 p3(radius * cos(angle0), height * 0.5f, radius * sin(angle0));

			// Draw the faces of the cylinder
			DrawQuad(transform * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)), p0, p1, p2, p3, color, entityID);
		}

		DrawHemisphere(transform * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, height / 2.0f, 0.0f)), radius, segments, stacks, color, entityID);
		DrawHemisphere(transform * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -height / 2.0f, 0.0f))
			* glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)), radius, segments, stacks, color, entityID);

	}

	void Renderer3D::DrawHemisphere(const glm::mat4& transform, float radius, uint8_t segments, uint8_t stacks, const glm::vec4& color, int entityID)
	{
		OGN_PROFILER_RENDERING();

		float stackIncrement = glm::half_pi<float>() / static_cast<float>(stacks);
		float angleIncrement = glm::two_pi<float>() / static_cast<float>(segments);

		for (uint8_t i = 0; i <= stacks; ++i)
		{
			float stackAngle = glm::half_pi<float>() - (static_cast<float>(i) * stackIncrement);

			for (uint8_t j = 0; j < segments; ++j)
			{
				float angle0 = static_cast<float>(j) * angleIncrement;
				float angle1 = angle0 + angleIncrement;

				// Vertices for the hemisphere
				glm::vec3 p0(radius * sin(stackAngle) * cos(angle0), radius * cos(stackAngle), radius * sin(stackAngle) * sin(angle0));
				glm::vec3 p1(radius * sin(stackAngle) * cos(angle1), radius * cos(stackAngle), radius * sin(stackAngle) * sin(angle1));
				glm::vec3 p2(radius * sin(stackAngle - stackIncrement) * cos(angle1), radius * cos(stackAngle - stackIncrement), radius * sin(stackAngle - stackIncrement) * sin(angle1));
				glm::vec3 p3(radius * sin(stackAngle - stackIncrement) * cos(angle0), radius * cos(stackAngle - stackIncrement), radius * sin(stackAngle - stackIncrement) * sin(angle0));

				// Draw the faces of the hemisphere
				DrawQuad(transform, p0, p1, p2, p3, color, entityID);
			}
		}
	}

	void Renderer3D::Shutdown()
	{
		delete[] s_Render3DData.CubeVertexBufferBase;
	}

	void Renderer3D::NextBatch()
	{
		Flush();
		StartBatch();
	}
}