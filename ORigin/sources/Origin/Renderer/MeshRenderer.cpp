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
		static const uint32_t MaxTriangles = 1000;
		static const uint32_t MaxCubeVertices = MaxTriangles * 24;
		static const uint32_t MaxCubeIndices = MaxTriangles * 36;
		static const uint32_t MaxSphereVertices = MaxTriangles * 544;
		static const uint32_t MaxSphereIndices = MaxTriangles * 768;

        static const uint32_t MaxCapsuleTriangles = 800;
        static const uint32_t MaxCapsuleVertices = MaxCapsuleTriangles * 1440;
        static const uint32_t MaxCapsuleIndices = MaxCapsuleTriangles * 2004;
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

        Shader *RenderShader = nullptr;
		std::shared_ptr<Shader> BatchShader;
        std::array<std::shared_ptr<Texture2D>, 32> TextureSlots;
        uint32_t DepthMap = 0;
        // 0 White texture
        // 1 Depth texture
        uint32_t AlbedoTextureSlotIndex = 2;
        uint32_t SpecularTextureSlotIndex = 16;

		// ===========================
		// CUBE
		std::shared_ptr<MeshData> CubeData;
		uint32_t CubeIndexCount = 0;
		std::shared_ptr<VertexArray> CubeVAO;
		std::shared_ptr<VertexBuffer> CubeVBO;
		MeshVertexData *CubeVBOPtr = nullptr;
		MeshVertexData *CubeVBOBase = nullptr;

		// ===========================
		// SPHERE
        std::shared_ptr<MeshData> SphereData;
		uint32_t SphereIndexCount = 0;
		std::shared_ptr<VertexArray> SphereVAO;
		std::shared_ptr<VertexBuffer> SphereVBO;
		MeshVertexData *SphereVBOPtr = nullptr;
		MeshVertexData *SphereVBOBase = nullptr;

        // ===========================
        // CAPSULE
        std::shared_ptr<MeshData> CapsuleData;
        uint32_t CapsuleIndexCount = 0;
        std::shared_ptr<VertexArray> CapsuleVAO;
        std::shared_ptr<VertexBuffer> CapsuleVBO;
        MeshVertexData *CapsuleVBOPtr = nullptr;
        MeshVertexData *CapsuleVBOBase = nullptr;

        std::shared_ptr<Shader> TestShader;
	};

	MeshRendererData s_MeshRenderData;

	void MeshRenderer::Init()
	{	
		BufferLayout bufferLayout = {
			{ ShaderDataType::Float3, "aPosition"     },
			{ ShaderDataType::Float3, "aNormals"      },
			{ ShaderDataType::Float3, "aColor"        },
			{ ShaderDataType::Float2, "aUV"           },
			{ ShaderDataType::Float2, "aTilingFactor" },
			{ ShaderDataType::Float,  "aAlbedoIndex"  },
			{ ShaderDataType::Float,  "aSpecularIndex"},
		};

		// ======================================
		// ================ Cube ================
		s_MeshRenderData.CubeData = ModelLoader::LoadModel("Resources/Models/cube.obj");
        for (auto &v : s_MeshRenderData.CubeData->vertices)
        {
            v.Position *= glm::vec3(0.5f);
        }
		s_MeshRenderData.CubeVAO = VertexArray::Create();
		s_MeshRenderData.CubeVBO = VertexBuffer::Create(MeshRenderData::MaxCubeVertices * sizeof(MeshVertexData));
		s_MeshRenderData.CubeVBO->SetLayout(bufferLayout);
		s_MeshRenderData.CubeVAO->AddVertexBuffer(s_MeshRenderData.CubeVBO);
		s_MeshRenderData.CubeVBOBase = new MeshVertexData[MeshRenderData::MaxCubeVertices];

        uint32_t *cubeIndices = new uint32_t[MeshRenderData::MaxCubeIndices];
        uint32_t baseIndicesCount = s_MeshRenderData.CubeData->indices.size();
        uint32_t maxCubes = MeshRenderData::MaxCubeIndices / baseIndicesCount;

        for (uint32_t cubeIndex = 0; cubeIndex < maxCubes; ++cubeIndex)
        {
            uint32_t vertexOffset = cubeIndex * s_MeshRenderData.CubeData->vertices.size();
            for (uint32_t i = 0; i < baseIndicesCount; ++i)
            {
                cubeIndices[cubeIndex * baseIndicesCount + i] = s_MeshRenderData.CubeData->indices[i] + vertexOffset;
            }
        }

        std::shared_ptr<IndexBuffer> cubeIBO = IndexBuffer::Create(cubeIndices, MeshRenderData::MaxCubeIndices);
		s_MeshRenderData.CubeVAO->SetIndexBuffer(cubeIBO);
        delete[] cubeIndices;

        // ======================================
        // ================ Sphere ==============
        s_MeshRenderData.SphereData = ModelLoader::LoadModel("Resources/Models/sphere.obj");
        s_MeshRenderData.SphereVAO = VertexArray::Create();
        s_MeshRenderData.SphereVBO = VertexBuffer::Create(MeshRenderData::MaxSphereVertices * sizeof(MeshVertexData));
        s_MeshRenderData.SphereVBO->SetLayout(bufferLayout);
        s_MeshRenderData.SphereVAO->AddVertexBuffer(s_MeshRenderData.SphereVBO);
        s_MeshRenderData.SphereVBOBase = new MeshVertexData[MeshRenderData::MaxSphereVertices];

        uint32_t *sphereIndices = new uint32_t[MeshRenderData::MaxSphereIndices];
		baseIndicesCount = s_MeshRenderData.SphereData->indices.size();
		uint32_t maxSpheres = MeshRenderData::MaxSphereIndices / baseIndicesCount;

        for (uint32_t sphereIndex = 0; sphereIndex < maxSpheres; ++sphereIndex)
        {
            uint32_t vertexOffset = sphereIndex * s_MeshRenderData.SphereData->vertices.size();
            for (uint32_t i = 0; i < baseIndicesCount; ++i)
            {
                sphereIndices[sphereIndex * baseIndicesCount + i] = s_MeshRenderData.SphereData->indices[i] + vertexOffset;
            }
        }

        std::shared_ptr<IndexBuffer> sphereIBO = IndexBuffer::Create(sphereIndices, MeshRenderData::MaxSphereIndices);
        s_MeshRenderData.SphereVAO->SetIndexBuffer(sphereIBO);
        delete[] sphereIndices;

        // ======================================
       // ================ Capsule ==============
        s_MeshRenderData.CapsuleData = ModelLoader::LoadModel("Resources/Models/capsule.obj");
        s_MeshRenderData.CapsuleVAO = VertexArray::Create();
        s_MeshRenderData.CapsuleVBO = VertexBuffer::Create(MeshRenderData::MaxCapsuleVertices * sizeof(MeshVertexData));
        s_MeshRenderData.CapsuleVBO->SetLayout(bufferLayout);
        s_MeshRenderData.CapsuleVAO->AddVertexBuffer(s_MeshRenderData.CapsuleVBO);
        s_MeshRenderData.CapsuleVBOBase = new MeshVertexData[MeshRenderData::MaxCapsuleVertices];

        uint32_t *capsuleIndices = new uint32_t[MeshRenderData::MaxCapsuleIndices];
        baseIndicesCount = s_MeshRenderData.CapsuleData->indices.size();
        uint32_t maxCapsules = MeshRenderData::MaxCapsuleIndices / baseIndicesCount;
        for (uint32_t capsuleIndex = 0; capsuleIndex < maxCapsules; ++capsuleIndex)
        {
            uint32_t vertexOffset = capsuleIndex * s_MeshRenderData.CapsuleData->vertices.size();
            for (uint32_t i = 0; i < baseIndicesCount; ++i)
            {
                capsuleIndices[capsuleIndex * baseIndicesCount + i] = s_MeshRenderData.CapsuleData->indices[i] + vertexOffset;
            }
        }

        std::shared_ptr<IndexBuffer> capsuleIBO = IndexBuffer::Create(capsuleIndices, MeshRenderData::MaxCapsuleIndices);
        s_MeshRenderData.CapsuleVAO->SetIndexBuffer(capsuleIBO);
        delete[] capsuleIndices;
        
        s_MeshRenderData.Ubo = UniformBuffer::Create(sizeof(CameraBufferData), CAMERA_BINDING);

        s_MeshRenderData.BatchShader = Renderer::GetShader("BatchMesh");
        s_MeshRenderData.TextureSlots[0] = Renderer::WhiteTexture;
        s_MeshRenderData.TestShader = Renderer::GetShader("TestShader");
	}

	void MeshRenderer::Begin(const Camera &camera, Shader *shader)
	{
		s_MeshRenderData.CameraData.ViewProjection = camera.GetViewProjection();
		s_MeshRenderData.CameraData.Position = camera.GetPosition();

		s_MeshRenderData.Ubo->Bind();
		s_MeshRenderData.Ubo->SetData(&s_MeshRenderData.CameraData, sizeof(CameraBufferData));

        if (shader)
        {
            s_MeshRenderData.RenderShader = shader;
        }
        else
        {
            s_MeshRenderData.RenderShader = s_MeshRenderData.BatchShader.get();
        }

		StartBatch();
	}

    void MeshRenderer::Begin(const glm::mat4 &viewProjection)
    {
        s_MeshRenderData.CameraData.ViewProjection = viewProjection;
        s_MeshRenderData.CameraData.Position = glm::vec3(0.0f);

        s_MeshRenderData.Ubo->Bind();
        s_MeshRenderData.Ubo->SetData(&s_MeshRenderData.CameraData, sizeof(CameraBufferData));

        s_MeshRenderData.RenderShader = s_MeshRenderData.BatchShader.get();

        StartBatch();
    }

    void MeshRenderer::AttachShadow(uint32_t id)
    {
        s_MeshRenderData.DepthMap = id;
    }

    void MeshRenderer::StartBatch()
	{
		s_MeshRenderData.AlbedoTextureSlotIndex = 2;
		s_MeshRenderData.SpecularTextureSlotIndex = 16;

		s_MeshRenderData.CubeIndexCount = 0;
		s_MeshRenderData.CubeVBOPtr = s_MeshRenderData.CubeVBOBase;

		s_MeshRenderData.SphereIndexCount = 0;
        s_MeshRenderData.SphereVBOPtr = s_MeshRenderData.SphereVBOBase;

        s_MeshRenderData.CapsuleIndexCount = 0;
        s_MeshRenderData.CapsuleVBOPtr = s_MeshRenderData.CapsuleVBOBase;
	}

	void MeshRenderer::NextBatch()
	{
		End();
		StartBatch();
	}

	void MeshRenderer::End()
	{
        s_MeshRenderData.RenderShader->Enable();

        if (s_MeshRenderData.CubeIndexCount)
        {
            uint32_t dataSize = (uint8_t *)s_MeshRenderData.CubeVBOPtr - (uint8_t *)s_MeshRenderData.CubeVBOBase;
            s_MeshRenderData.CubeVBO->Bind();
            s_MeshRenderData.CubeVBO->SetData(s_MeshRenderData.CubeVBOBase, dataSize);

            for (uint32_t i = 0; i < s_MeshRenderData.AlbedoTextureSlotIndex; i++)
            {
                if (i == 1)
                {
                    glBindTextureUnit(i, s_MeshRenderData.DepthMap);
                }
                else
                {
                    s_MeshRenderData.TextureSlots[i]->Bind(i);
                }
            }

            RenderCommand::DrawIndexed(s_MeshRenderData.CubeVAO, s_MeshRenderData.CubeIndexCount);
            Renderer::GetStatistics().DrawCalls++;
        }

        if (s_MeshRenderData.SphereIndexCount)
        {
            uint32_t dataSize = (uint8_t *)s_MeshRenderData.SphereVBOPtr - (uint8_t *)s_MeshRenderData.SphereVBOBase;
            s_MeshRenderData.SphereVBO->Bind();
            s_MeshRenderData.SphereVBO->SetData(s_MeshRenderData.SphereVBOBase, dataSize);

            for (uint32_t i = 0; i < s_MeshRenderData.AlbedoTextureSlotIndex; i++)
            {
                if (i == 1)
                {
                    glBindTextureUnit(i, s_MeshRenderData.DepthMap);
                }
                else
                {
                    s_MeshRenderData.TextureSlots[i]->Bind(i);
                }
            }

            RenderCommand::DrawIndexed(s_MeshRenderData.SphereVAO, s_MeshRenderData.SphereIndexCount);
            Renderer::GetStatistics().DrawCalls++;
        }

        if (s_MeshRenderData.CapsuleIndexCount)
        {
            uint32_t dataSize = (uint8_t *)s_MeshRenderData.CapsuleVBOPtr - (uint8_t *)s_MeshRenderData.CapsuleVBOBase;
            s_MeshRenderData.CapsuleVBO->Bind();
            s_MeshRenderData.CapsuleVBO->SetData(s_MeshRenderData.CapsuleVBOBase, dataSize);

            for (uint32_t i = 0; i < s_MeshRenderData.AlbedoTextureSlotIndex; i++)
            {
                if (i == 1)
                {
                    glBindTextureUnit(i, s_MeshRenderData.DepthMap);
                }
                else
                {
                    s_MeshRenderData.TextureSlots[i]->Bind(i);
                }
            }

            RenderCommand::DrawIndexed(s_MeshRenderData.CapsuleVAO, s_MeshRenderData.CapsuleIndexCount);
            Renderer::GetStatistics().DrawCalls++;
        }
	}

    void MeshRenderer::DrawMesh(const glm::mat4 &viewProjection, const glm::mat4 &transform, const std::shared_ptr<VertexArray> &va, Shader *shader)
    {
        if (!va)
            return;

        if (!shader)
        {
            shader = s_MeshRenderData.TestShader.get();
            shader->Enable();
            shader->SetMatrix("viewProjection", viewProjection);
            shader->SetMatrix("model", transform);
        }
        else
        {
            shader->Enable();
            shader->SetMatrix("viewProjection", viewProjection * transform);
        }
        
        RenderCommand::DrawIndexed(va);
        shader->Disable();
    }

    void MeshRenderer::DrawCube(const glm::mat4 &transform, glm::vec4 color)
	{
		if (s_MeshRenderData.CubeIndexCount >= MeshRenderData::MaxCubeIndices)
			NextBatch();

		for (size_t i = 0; i < s_MeshRenderData.CubeData->vertices.size(); i++)
		{
			s_MeshRenderData.CubeVBOPtr->Position = transform * glm::vec4(s_MeshRenderData.CubeData->vertices[i].Position, 1.0f);
			glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(transform)));
			s_MeshRenderData.CubeVBOPtr->Normals = s_MeshRenderData.CubeData->vertices[i].Normals;
			s_MeshRenderData.CubeVBOPtr->Color = color;
			s_MeshRenderData.CubeVBOPtr->UV = s_MeshRenderData.CubeData->vertices[i].UV;
			s_MeshRenderData.CubeVBOPtr->TilingFactor = s_MeshRenderData.CubeData->vertices[i].TilingFactor;
			s_MeshRenderData.CubeVBOPtr->AlbedoIndex = 0.0f;
			s_MeshRenderData.CubeVBOPtr->SpecularIndex = 0.0f;
			s_MeshRenderData.CubeVBOPtr++;
		}

		s_MeshRenderData.CubeIndexCount += (uint32_t)s_MeshRenderData.CubeData->indices.size();
		Renderer::GetStatistics().CubeCount++;
	}

    void MeshRenderer::DrawCube(const glm::mat4 &transform, Material *material)
    {
		if (material)
		{
			DrawCube(transform, 
                material->Albedo.Texture,
                material->Metallic.Texture,
                material->TilingFactor, 
                material->Color);
		}
		else
		{
			DrawCube(transform, glm::vec4(1.0f));
		}
    }

	void MeshRenderer::DrawCube(const glm::mat4 &transform,
		const std::shared_ptr<Texture2D> &albedo,
		const std::shared_ptr<Texture2D> &specular,
		const glm::vec2 &tilingFactor, const glm::vec4 &color)
	{
		if (s_MeshRenderData.CubeIndexCount >= MeshRenderData::MaxCubeIndices)
			NextBatch();

		float albedoIndex = 0.0f;
		if (albedo)
		{
            for (uint32_t i = 2; i < s_MeshRenderData.AlbedoTextureSlotIndex; i++)
            {
                if (*s_MeshRenderData.TextureSlots[i] == *albedo)
                {
					albedoIndex = (float)i;
                    break;
                }
            }

            if (albedoIndex == 0)
            {
                if (s_MeshRenderData.AlbedoTextureSlotIndex >= MeshRenderData::MaxTextureSlots)
                    NextBatch();
				albedoIndex = (float)s_MeshRenderData.AlbedoTextureSlotIndex;
                s_MeshRenderData.TextureSlots[s_MeshRenderData.AlbedoTextureSlotIndex] = albedo;
                s_MeshRenderData.AlbedoTextureSlotIndex++;
            }
		}

        float specularIndex = 16.0f;
        if (specular)
        {
            for (uint32_t i = 16; i < s_MeshRenderData.SpecularTextureSlotIndex; i++)
            {
                if (*s_MeshRenderData.TextureSlots[i] == *specular)
                {
                    specularIndex = (float)i;
                    break;
                }
            }

            if (specularIndex == 16)
            {
                if (s_MeshRenderData.SpecularTextureSlotIndex >= MeshRenderData::MaxTextureSlots)
                    NextBatch();
                specularIndex = (float)s_MeshRenderData.SpecularTextureSlotIndex;
                s_MeshRenderData.TextureSlots[s_MeshRenderData.SpecularTextureSlotIndex] = specular;
                s_MeshRenderData.SpecularTextureSlotIndex++;
            }
        }

        for (size_t i = 0; i < s_MeshRenderData.CubeData->vertices.size(); i++)
        {
            s_MeshRenderData.CubeVBOPtr->Position = transform * glm::vec4(s_MeshRenderData.CubeData->vertices[i].Position, 1.0f);
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));
            s_MeshRenderData.CubeVBOPtr->Normals = normalMatrix * s_MeshRenderData.CubeData->vertices[i].Normals;
            s_MeshRenderData.CubeVBOPtr->Color = color;
            s_MeshRenderData.CubeVBOPtr->UV = s_MeshRenderData.CubeData->vertices[i].UV;
            s_MeshRenderData.CubeVBOPtr->TilingFactor = tilingFactor;
            s_MeshRenderData.CubeVBOPtr->AlbedoIndex = albedoIndex;
            s_MeshRenderData.CubeVBOPtr->SpecularIndex = specularIndex;
            s_MeshRenderData.CubeVBOPtr++;
        }

        s_MeshRenderData.CubeIndexCount += (uint32_t)s_MeshRenderData.CubeData->indices.size();
        Renderer::GetStatistics().CubeCount++;
    }

    void MeshRenderer::DrawSphere(const glm::mat4 &transform, const glm::vec4 &color)
    {
        if (s_MeshRenderData.SphereIndexCount >= MeshRenderData::MaxSphereIndices)
            NextBatch();

        for (size_t i = 0; i < s_MeshRenderData.SphereData->vertices.size(); i++)
        {
            s_MeshRenderData.SphereVBOPtr->Position = transform * glm::vec4(s_MeshRenderData.SphereData->vertices[i].Position, 1.0f);
            glm::mat3 transposeMat = glm::mat3(glm::transpose(glm::inverse(transform)));
            s_MeshRenderData.SphereVBOPtr->Normals = transposeMat * s_MeshRenderData.SphereData->vertices[i].Normals;
            s_MeshRenderData.SphereVBOPtr->Color = color;
            s_MeshRenderData.SphereVBOPtr->UV = s_MeshRenderData.SphereData->vertices[i].UV;
            s_MeshRenderData.SphereVBOPtr->TilingFactor = s_MeshRenderData.SphereData->vertices[i].TilingFactor;
            s_MeshRenderData.SphereVBOPtr->AlbedoIndex = 0.0f;
            s_MeshRenderData.SphereVBOPtr->SpecularIndex = 0.0f;
            s_MeshRenderData.SphereVBOPtr++;
        }

        s_MeshRenderData.SphereIndexCount += (uint32_t)s_MeshRenderData.SphereData->indices.size();
		Renderer::GetStatistics().SphereCount++;
    }

    void MeshRenderer::DrawSphere(const glm::mat4 &transform, Material *material)
    {
        if (material) DrawSphere(transform, material->Color);
        else DrawSphere(transform, glm::vec4(1.0f));
    }

    void MeshRenderer::DrawCapsule(const glm::mat4 &transform, const glm::vec4 &color)
    {
        if (s_MeshRenderData.CapsuleIndexCount >= MeshRenderData::MaxCapsuleIndices)
            NextBatch();

        for (size_t i = 0; i < s_MeshRenderData.CapsuleData->vertices.size(); i++)
        {
            s_MeshRenderData.CapsuleVBOPtr->Position = transform * glm::vec4(s_MeshRenderData.CapsuleData->vertices[i].Position, 1.0f);
            glm::mat3 transposeMat = glm::mat3(glm::transpose(glm::inverse(transform)));
            s_MeshRenderData.CapsuleVBOPtr->Normals = transposeMat * s_MeshRenderData.CapsuleData->vertices[i].Normals;
            s_MeshRenderData.CapsuleVBOPtr->Color = color;
            s_MeshRenderData.CapsuleVBOPtr->UV = s_MeshRenderData.CapsuleData->vertices[i].UV;
            s_MeshRenderData.CapsuleVBOPtr->TilingFactor = s_MeshRenderData.CapsuleData->vertices[i].TilingFactor;
            s_MeshRenderData.CapsuleVBOPtr->AlbedoIndex = 0.0f;
            s_MeshRenderData.CapsuleVBOPtr->SpecularIndex = 0.0f;
            s_MeshRenderData.CapsuleVBOPtr++;
        }

        s_MeshRenderData.CapsuleIndexCount += (uint32_t)s_MeshRenderData.CapsuleData->indices.size();
        Renderer::GetStatistics().SphereCount++;
    }

    void MeshRenderer::DrawCapsule(const glm::mat4 &transform, Material *material)
    {
        if (material) DrawCapsule(transform, material->Color);
        else DrawCapsule(transform, glm::vec4(1.0f));
    }

    Shader *MeshRenderer::GetShader()
    {
		return s_MeshRenderData.BatchShader.get();
    }

    void MeshRenderer::Shutdown()
	{
		delete[] s_MeshRenderData.CubeVBOBase;
		delete[] s_MeshRenderData.SphereVBOBase;
	}
}

