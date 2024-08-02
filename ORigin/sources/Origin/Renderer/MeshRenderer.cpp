// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
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
		static const uint32_t MaxCubeTriangles = 1000;
		static const uint32_t MaxCubeVertices = MaxCubeTriangles * 24;
		static const uint32_t MaxCubeIndices = MaxCubeTriangles * 36;

		static const uint32_t MaxSphereTriangles = 1000;
		static const uint32_t MaxSphereVertices = MaxSphereTriangles * 544;
		static const uint32_t MaxSphereIndices = MaxSphereTriangles * 768;

        static const uint32_t MaxCapsuleTriangles = 1000;
        static const uint32_t MaxCapsuleVertices = MaxCapsuleTriangles * 1136;
        static const uint32_t MaxCapsuleIndices = MaxCapsuleTriangles * 1620;

		static const uint32_t MaxAlbedoSlots = 16;
		static const uint32_t MaxSpecularSlots = 16;
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
        std::array<std::shared_ptr<Texture2D>, 16> AlbedoSlots;
        std::array<std::shared_ptr<Texture2D>, 16> SpecularSlots;
        uint32_t AlbedoTextureSlotIndex = 1; // 0 White texture
        uint32_t SpecularTextureSlotIndex = 1; // 0 White texture

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

        // ===========================
        // CAPSULE
        MeshData CapsuleData;
        uint32_t CapsuleIndexCount = 0;
        std::shared_ptr<VertexArray> CapsuleVAO;
        std::shared_ptr<VertexBuffer> CapsuleVBO;
        MeshVertexData *CapsuleVBOPtr = nullptr;
        MeshVertexData *CapsuleVBOBase = nullptr;
	};

	static MeshRendererData s_Data;

	void MeshRenderer::Init()
	{	
		BufferLayout bufferLayout = {
			{ ShaderDataType::Float3, "aPosition"     },
			{ ShaderDataType::Float3, "aNormals"      },
			{ ShaderDataType::Float3, "aColor"        },
			{ ShaderDataType::Float2, "aUV"           },
			{ ShaderDataType::Float2, "aTilingFactor" },
			{ ShaderDataType::Float4, "aBoneIDs"      },
			{ ShaderDataType::Float4, "aBoneWeights"  },
			{ ShaderDataType::Float,  "aAlbedoIndex"  },
			{ ShaderDataType::Float,  "aSpecularIndex"},
			{ ShaderDataType::Int,    "aEntityID"     },
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

        // ======================================
       // ================ Capsule ================
        s_Data.CapsuleData = ModelLoader::LoadModel("Resources/Models/capsule.obj");
        s_Data.CapsuleVAO = VertexArray::Create();
        s_Data.CapsuleVBO = VertexBuffer::Create(MeshRenderData::MaxCapsuleVertices * sizeof(MeshVertexData));
        s_Data.CapsuleVBO->SetLayout(bufferLayout);
        s_Data.CapsuleVAO->AddVertexBuffer(s_Data.CapsuleVBO);
        s_Data.CapsuleVBOBase = new MeshVertexData[MeshRenderData::MaxCapsuleVertices];

        OGN_CORE_INFO("{}", s_Data.CapsuleData.vertices.size());

        uint32_t *capsuleIndices = new uint32_t[MeshRenderData::MaxCapsuleIndices];
        baseIndicesCount = s_Data.CapsuleData.indices.size();
        uint32_t maxCapsules = MeshRenderData::MaxCapsuleIndices / baseIndicesCount;

        for (uint32_t capsuleIndex = 0; capsuleIndex < maxCapsules; ++capsuleIndex)
        {
            uint32_t vertexOffset = capsuleIndex * s_Data.CapsuleData.vertices.size();
            for (uint32_t i = 0; i < baseIndicesCount; ++i)
            {
                capsuleIndices[capsuleIndex * baseIndicesCount + i] = s_Data.CapsuleData.indices[i] + vertexOffset;
            }
        }

        std::shared_ptr<IndexBuffer> capsuleIBO = IndexBuffer::Create(capsuleIndices, MeshRenderData::MaxCapsuleIndices);
        s_Data.SphereVAO->SetIndexBuffer(capsuleIBO);
        delete[] capsuleIndices;
        
        s_Data.Ubo = UniformBuffer::Create(sizeof(CameraBufferData), CAMERA_BINDING);

        s_Data.Shader = Renderer::GetShader("Mesh");
        s_Data.AlbedoSlots[0] = Renderer::WhiteTexture;
		s_Data.SpecularSlots[0] = Renderer::WhiteTexture;
	}

	void MeshRenderer::Begin(const Camera &camera, Shader *shader)
	{
		s_Data.CameraData.ViewProjection = camera.GetViewProjection();
		s_Data.CameraData.Position = camera.GetPosition();

		s_Data.Ubo->Bind();
		s_Data.Ubo->SetData(&s_Data.CameraData, sizeof(CameraBufferData));

		if (!shader)
			s_Data.Shader->Enable();
		else
			shader->Enable();

		StartBatch();
	}

	void MeshRenderer::StartBatch()
	{
		s_Data.AlbedoTextureSlotIndex = 1;
		s_Data.SpecularTextureSlotIndex = 1;

		s_Data.CubeIndexCount = 0;
		s_Data.CubeVBOPtr = s_Data.CubeVBOBase;

		s_Data.SphereIndexCount = 0;
        s_Data.SphereVBOPtr = s_Data.SphereVBOBase;

        s_Data.CapsuleIndexCount = 0;
        s_Data.CapsuleVBOPtr = s_Data.CapsuleVBOBase;
	}

	void MeshRenderer::NextBatch()
	{
		End();
		StartBatch();
	}

	void MeshRenderer::End()
	{
        if (s_Data.CubeIndexCount)
        {
            uint32_t dataSize = (uint8_t *)s_Data.CubeVBOPtr - (uint8_t *)s_Data.CubeVBOBase;
            s_Data.CubeVBO->SetData(s_Data.CubeVBOBase, dataSize);

            for (uint32_t i = 0; i < s_Data.AlbedoTextureSlotIndex; i++)
                s_Data.AlbedoSlots[i]->Bind(0, i, MeshRenderData::MaxAlbedoSlots);

            for (uint32_t i = 0; i < s_Data.SpecularTextureSlotIndex; i++)
                s_Data.SpecularSlots[i]->Bind(1, i, MeshRenderData::MaxSpecularSlots);

            RenderCommand::DrawIndexed(s_Data.CubeVAO, s_Data.CubeIndexCount);

            Renderer::GetStatistics().DrawCalls++;
        }

        if (s_Data.SphereIndexCount)
        {
            uint32_t dataSize = (uint8_t *)s_Data.SphereVBOPtr - (uint8_t *)s_Data.SphereVBOBase;
            s_Data.SphereVBO->SetData(s_Data.SphereVBOBase, dataSize);

            for (uint32_t i = 0; i < s_Data.AlbedoTextureSlotIndex; i++)
                s_Data.AlbedoSlots[i]->Bind(0, i, MeshRenderData::MaxAlbedoSlots);

            for (uint32_t i = 0; i < s_Data.SpecularTextureSlotIndex; i++)
                s_Data.SpecularSlots[i]->Bind(1, i, MeshRenderData::MaxSpecularSlots);

            RenderCommand::DrawIndexed(s_Data.SphereVAO, s_Data.SphereIndexCount);
            Renderer::GetStatistics().DrawCalls++;
        }

        if (s_Data.CapsuleIndexCount)
        {
            uint32_t dataSize = (uint8_t *)s_Data.CapsuleVBOPtr - (uint8_t *)s_Data.CapsuleVBOBase;
            s_Data.CapsuleVBO->SetData(s_Data.CapsuleVBOBase, dataSize);

            for (uint32_t i = 0; i < s_Data.AlbedoTextureSlotIndex; i++)
                s_Data.AlbedoSlots[i]->Bind(0, i, MeshRenderData::MaxAlbedoSlots);

            for (uint32_t i = 0; i < s_Data.SpecularTextureSlotIndex; i++)
                s_Data.SpecularSlots[i]->Bind(1, i, MeshRenderData::MaxSpecularSlots);

            RenderCommand::DrawIndexed(s_Data.CapsuleVAO, s_Data.CapsuleIndexCount);
            Renderer::GetStatistics().DrawCalls++;
        }
	}

	void MeshRenderer::DrawCube(const glm::mat4 &transform, const glm::vec4 &color, int entityID)
	{
		if (s_Data.CubeIndexCount >= MeshRenderData::MaxCubeIndices)
			NextBatch();

		for (size_t i = 0; i < s_Data.CubeData.vertices.size(); i++)
		{
			s_Data.CubeVBOPtr->Position = transform * glm::vec4(s_Data.CubeData.vertices[i].Position, 1.0f);
			glm::mat3 transposeMat = glm::mat3(glm::transpose(glm::inverse(transform)));
			s_Data.CubeVBOPtr->Normals = transposeMat * s_Data.CubeData.vertices[i].Normals;
			s_Data.CubeVBOPtr->Color = color;
			s_Data.CubeVBOPtr->UV = s_Data.CubeData.vertices[i].UV;
			s_Data.CubeVBOPtr->TilingFactor = s_Data.CubeData.vertices[i].TilingFactor;
			s_Data.CubeVBOPtr->BoneIDs = glm::vec4(0.0f);
			s_Data.CubeVBOPtr->BoneWeights = glm::vec4(1.0f);
			s_Data.CubeVBOPtr->AlbedoIndex = 0.0f;
			s_Data.CubeVBOPtr->SpecularIndex = 0.0f;
			s_Data.CubeVBOPtr->EntityID = entityID;
			s_Data.CubeVBOPtr++;
		}

		s_Data.CubeIndexCount += s_Data.CubeData.indices.size();
		Renderer::GetStatistics().CubeCount++;
	}

    void MeshRenderer::DrawCube(const glm::mat4 &transform, Material *material, int entityID)
    {
		if (material)
		{
			DrawCube(transform, 
                material->Albedo.Texture,
                material->Metallic.Texture,
                material->TilingFactor, 
                material->Color, entityID);
		}
		else
		{
			DrawCube(transform, glm::vec4(1.0f), entityID);
		}
    }

	void MeshRenderer::DrawCube(const glm::mat4 &transform,
		const std::shared_ptr<Texture2D> &albedo,
		const std::shared_ptr<Texture2D> &specular,
		const glm::vec2 &tilingFactor, const glm::vec4 &color, int entityID)
	{
		if (s_Data.CubeIndexCount >= MeshRenderData::MaxCubeIndices)
			NextBatch();

		float albedoIndex = 0.0f;
		if (albedo)
		{
            for (uint32_t i = 1; i < s_Data.AlbedoTextureSlotIndex; i++)
            {
                if (*s_Data.AlbedoSlots[i] == *albedo)
                {
					albedoIndex = (float)i;
                    break;
                }
            }

            if (albedoIndex == 0)
            {
                if (s_Data.AlbedoTextureSlotIndex >= MeshRenderData::MaxAlbedoSlots)
                    NextBatch();
				albedoIndex = (float)s_Data.AlbedoTextureSlotIndex;
                s_Data.AlbedoSlots[s_Data.AlbedoTextureSlotIndex] = albedo;
                s_Data.AlbedoTextureSlotIndex++;
            }
		}

        float specularIndex = 0.0f;
        if (specular)
        {
            for (uint32_t i = 1; i < s_Data.SpecularTextureSlotIndex; i++)
            {
                if (*s_Data.SpecularSlots[i] == *specular)
                {
                    specularIndex = (float)i;
                    break;
                }
            }

            if (specularIndex == 0)
            {
                if (s_Data.SpecularTextureSlotIndex >= MeshRenderData::MaxSpecularSlots)
                    NextBatch();
                specularIndex = (float)s_Data.SpecularTextureSlotIndex;
                s_Data.SpecularSlots[s_Data.SpecularTextureSlotIndex] = specular;
                s_Data.SpecularTextureSlotIndex++;
            }
        }

        for (size_t i = 0; i < s_Data.CubeData.vertices.size(); i++)
        {
            s_Data.CubeVBOPtr->Position = transform * glm::vec4(s_Data.CubeData.vertices[i].Position, 1.0f);
            glm::mat3 transposeMat = glm::mat3(glm::transpose(glm::inverse(transform)));
            s_Data.CubeVBOPtr->Normals = transposeMat * s_Data.CubeData.vertices[i].Normals;
            s_Data.CubeVBOPtr->Color = color;
            s_Data.CubeVBOPtr->UV = s_Data.CubeData.vertices[i].UV;
            s_Data.CubeVBOPtr->TilingFactor = tilingFactor;
            s_Data.CubeVBOPtr->BoneIDs = glm::vec4(0.0f);
            s_Data.CubeVBOPtr->BoneWeights = glm::vec4(1.0f);
            s_Data.CubeVBOPtr->AlbedoIndex = albedoIndex;
            s_Data.CubeVBOPtr->SpecularIndex = specularIndex;
            s_Data.CubeVBOPtr->EntityID = entityID;
            s_Data.CubeVBOPtr++;
        }

        s_Data.CubeIndexCount += s_Data.CubeData.indices.size();
        Renderer::GetStatistics().CubeCount++;
    }

    void MeshRenderer::DrawSphere(const glm::mat4 &transform, const glm::vec4 &color, int entityID)
    {
        if (s_Data.SphereIndexCount >= MeshRenderData::MaxSphereIndices)
            NextBatch();

        for (size_t i = 0; i < s_Data.SphereData.vertices.size(); i++)
        {
            s_Data.SphereVBOPtr->Position = transform * glm::vec4(s_Data.SphereData.vertices[i].Position, 1.0f);
            glm::mat3 transposeMat = glm::mat3(glm::transpose(glm::inverse(transform)));
            s_Data.SphereVBOPtr->Normals = transposeMat * s_Data.SphereData.vertices[i].Normals;
            s_Data.SphereVBOPtr->Color = color;
            s_Data.SphereVBOPtr->UV = s_Data.SphereData.vertices[i].UV;
            s_Data.SphereVBOPtr->TilingFactor = s_Data.SphereData.vertices[i].TilingFactor;
            s_Data.SphereVBOPtr->BoneIDs = glm::vec4(0.0f);
            s_Data.SphereVBOPtr->BoneWeights = glm::vec4(1.0f);
            s_Data.SphereVBOPtr->AlbedoIndex = 0.0f;
            s_Data.SphereVBOPtr->SpecularIndex = 0.0f;
            s_Data.SphereVBOPtr->EntityID = entityID;
            s_Data.SphereVBOPtr++;
        }

        s_Data.SphereIndexCount += s_Data.SphereData.indices.size();
		Renderer::GetStatistics().SphereCount++;
    }

    void MeshRenderer::DrawSphere(const glm::mat4 &transform, Material *material, int entityID /*= -1*/)
    {
        if (material)
			DrawSphere(transform, material->Color, entityID);
        else
			DrawSphere(transform, glm::vec4(1.0f), entityID);
    }

    void MeshRenderer::DrawCapsule(const glm::mat4 &transform, const glm::vec4 &color, int entityID)
    {
        if (s_Data.CapsuleIndexCount >= MeshRenderData::MaxCapsuleIndices)
            NextBatch();

        for (size_t i = 0; i < s_Data.CapsuleData.vertices.size(); i++)
        {
            s_Data.CapsuleVBOPtr->Position = transform * glm::vec4(s_Data.CapsuleData.vertices[i].Position, 1.0f);
            glm::mat3 transposeMat = glm::mat3(glm::transpose(glm::inverse(transform)));
            s_Data.CapsuleVBOPtr->Normals = transposeMat * s_Data.CapsuleData.vertices[i].Normals;
            s_Data.CapsuleVBOPtr->Color = color;
            s_Data.CapsuleVBOPtr->UV = s_Data.CapsuleData.vertices[i].UV;
            s_Data.CapsuleVBOPtr->TilingFactor = s_Data.CapsuleData.vertices[i].TilingFactor;
            s_Data.CapsuleVBOPtr->BoneIDs = glm::vec4(0.0f);
            s_Data.CapsuleVBOPtr->BoneWeights = glm::vec4(1.0f);
            s_Data.CapsuleVBOPtr->AlbedoIndex = 0.0f;
            s_Data.CapsuleVBOPtr->SpecularIndex = 0.0f;
            s_Data.CapsuleVBOPtr->EntityID = entityID;
            s_Data.CapsuleVBOPtr++;
        }

        s_Data.CapsuleIndexCount += s_Data.CapsuleData.indices.size();
        Renderer::GetStatistics().SphereCount++;
    }

    void MeshRenderer::DrawCapsule(const glm::mat4 &transform, Material *material, int entityID /*= -1*/)
    {
        if (material)
            DrawCapsule(transform, material->Color, entityID);
        else
            DrawCapsule(transform, glm::vec4(1.0f), entityID);
    }

    Shader *MeshRenderer::GetShader()
    {
		return s_Data.Shader.get();
    }

    void MeshRenderer::Shutdown()
	{
		delete[] s_Data.CubeVBOBase;
		delete[] s_Data.SphereVBOBase;
	}
}

