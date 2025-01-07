// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Origin/Renderer/Renderer.h"
#include "Origin/Renderer/VertexArray.h"
#include "Origin/Renderer/Shader.h"
#include "Model.h"
#include "Mesh.h"
#include "MeshRenderer.h"

#include <glad/glad.h>

namespace origin {

namespace {
static void ProcessStaticMeshNode(aiNode *node, const aiScene *scene, Mesh *data)
{
    for (u32 meshIndex = 0; meshIndex < node->mNumMeshes; ++meshIndex)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[meshIndex]];

        // Vertices
        for (u32 i = 0; i < mesh->mNumVertices; ++i)
        {
            MeshVertexData vertex;
            vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
            vertex.Normals = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
            if (mesh->mTextureCoords[0])
            {
                vertex.UV = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
            }
            else
            {
                vertex.UV = { 1.0f, 1.0f };
            }
            vertex.TilingFactor = glm::vec2(1.0f);
            data->vertices.push_back(vertex);
        }

        // Faces
        for (u32 i = 0; i < mesh->mNumFaces; ++i)
        {
            aiFace face = mesh->mFaces[i];
            for (u32 in = 0; in < face.mNumIndices; in++)
            {
                data->indices.push_back(face.mIndices[in]);
            }
        }
    }

    for (u32 i = 0; i < node->mNumChildren; ++i)
    {
        ProcessStaticMeshNode(node->mChildren[i], scene, data);
    }
}
}

Ref<Mesh> MeshRenderer::LoadMeshVertexData(const std::filesystem::path &filepath)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filepath.generic_string().c_str(),
        aiProcess_Triangulate | aiProcess_GenSmoothNormals
        | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices
    );

    aiNode *node = scene->mRootNode;
    Ref<Mesh> mesh = CreateRef<Mesh>();
    ProcessStaticMeshNode(node, scene, mesh.get());
    Model::CreateVertex(mesh);
    return mesh;
}
struct MeshRenderData
{
    static constexpr u32 MaxTriangles = 1000;
    static constexpr u32 MaxCubeVertices = MaxTriangles * 24;
    static constexpr u32 MaxCubeIndices = MaxTriangles * 36;
    static constexpr u32 MaxSphereVertices = MaxTriangles * 544;
    static constexpr u32 MaxSphereIndices = MaxTriangles * 768;

    static constexpr u32 MaxCapsuleTriangles = 800;
    static constexpr u32 MaxCapsuleVertices = MaxCapsuleTriangles * 1440;
    static constexpr u32 MaxCapsuleIndices = MaxCapsuleTriangles * 2004;
    static constexpr u32 MaxTextureSlots = 32;
};

struct CameraBufferData
{
    glm::mat4 ViewProjection;
    glm::vec3 Position;
};

struct MeshRendererData
{
    CameraBufferData CameraData;
    Ref<UniformBuffer> Ubo;

    Shader *RenderShader = nullptr;
    Ref<Shader> BatchShader;
    std::array<Ref<Texture2D>, 32> TextureSlots;
    u32 DepthMap = 0;
    // 0 White texture
    // 1 Depth texture
    u32 AlbedoTextureSlotIndex = 2;
    u32 SpecularTextureSlotIndex = 16;

    // ===========================
    // CUBE
    Ref<Mesh> CubeData;
    u32 CubeIndexCount = 0;
    Ref<VertexArray> CubeVAO;
    Ref<VertexBuffer> CubeVBO;
    MeshVertexData *CubeVBOPtr = nullptr;
    MeshVertexData *CubeVBOBase = nullptr;

    // ===========================
    // SPHERE
    Ref<Mesh> SphereData;
    u32 SphereIndexCount = 0;
    Ref<VertexArray> SphereVAO;
    Ref<VertexBuffer> SphereVBO;
    MeshVertexData *SphereVBOPtr = nullptr;
    MeshVertexData *SphereVBOBase = nullptr;

    // ===========================
    // CAPSULE
    Ref<Mesh> CapsuleData;
    u32 CapsuleIndexCount = 0;
    Ref<VertexArray> CapsuleVAO;
    Ref<VertexBuffer> CapsuleVBO;
    MeshVertexData *CapsuleVBOPtr = nullptr;
    MeshVertexData *CapsuleVBOBase = nullptr;

    Ref<Shader> TestShader;
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
    s_MeshRenderData.CubeData = LoadMeshVertexData("Resources/Models/cube.obj");
    for (auto &v : s_MeshRenderData.CubeData->vertices)
    {
        v.Position *= glm::vec3(0.5f);
    }
    s_MeshRenderData.CubeVAO = VertexArray::Create();
    s_MeshRenderData.CubeVBO = VertexBuffer::Create(MeshRenderData::MaxCubeVertices * sizeof(MeshVertexData));
    s_MeshRenderData.CubeVBO->SetLayout(bufferLayout);
    s_MeshRenderData.CubeVAO->AddVertexBuffer(s_MeshRenderData.CubeVBO);
    s_MeshRenderData.CubeVBOBase = new MeshVertexData[MeshRenderData::MaxCubeVertices];

    u32 *cubeIndices = new u32[MeshRenderData::MaxCubeIndices];
    u32 baseIndicesCount = static_cast<u32>(s_MeshRenderData.CubeData->indices.size());
    u32 maxCubes = MeshRenderData::MaxCubeIndices / baseIndicesCount;

    for (u32 cubeIndex = 0; cubeIndex < maxCubes; ++cubeIndex)
    {
        u32 vertexOffset = cubeIndex * static_cast<u32>(s_MeshRenderData.CubeData->vertices.size());
        for (u32 i = 0; i < baseIndicesCount; ++i)
        {
            cubeIndices[cubeIndex * baseIndicesCount + i] = s_MeshRenderData.CubeData->indices[i] + vertexOffset;
        }
    }

    Ref<IndexBuffer> cubeIBO = IndexBuffer::Create(cubeIndices, MeshRenderData::MaxCubeIndices);
    s_MeshRenderData.CubeVAO->SetIndexBuffer(cubeIBO);
    delete[] cubeIndices;

    // ======================================
    // ================ Sphere ==============
    s_MeshRenderData.SphereData = LoadMeshVertexData("Resources/Models/sphere.obj");
    s_MeshRenderData.SphereVAO = VertexArray::Create();
    s_MeshRenderData.SphereVBO = VertexBuffer::Create(MeshRenderData::MaxSphereVertices * sizeof(MeshVertexData));
    s_MeshRenderData.SphereVBO->SetLayout(bufferLayout);
    s_MeshRenderData.SphereVAO->AddVertexBuffer(s_MeshRenderData.SphereVBO);
    s_MeshRenderData.SphereVBOBase = new MeshVertexData[MeshRenderData::MaxSphereVertices];

    u32 *sphereIndices = new u32[MeshRenderData::MaxSphereIndices];
    baseIndicesCount = static_cast<u32>(s_MeshRenderData.SphereData->indices.size());
    u32 maxSpheres = MeshRenderData::MaxSphereIndices / baseIndicesCount;

    for (u32 sphereIndex = 0; sphereIndex < maxSpheres; ++sphereIndex)
    {
        u32 vertexOffset = sphereIndex * static_cast<u32>(s_MeshRenderData.SphereData->vertices.size());
        for (u32 i = 0; i < baseIndicesCount; ++i)
        {
            sphereIndices[sphereIndex * baseIndicesCount + i] = s_MeshRenderData.SphereData->indices[i] + vertexOffset;
        }
    }

    Ref<IndexBuffer> sphereIBO = IndexBuffer::Create(sphereIndices, MeshRenderData::MaxSphereIndices);
    s_MeshRenderData.SphereVAO->SetIndexBuffer(sphereIBO);
    delete[] sphereIndices;

    // ======================================
   // ================ Capsule ==============
    s_MeshRenderData.CapsuleData = LoadMeshVertexData("Resources/Models/capsule.obj");
    s_MeshRenderData.CapsuleVAO = VertexArray::Create();
    s_MeshRenderData.CapsuleVBO = VertexBuffer::Create(MeshRenderData::MaxCapsuleVertices * sizeof(MeshVertexData));
    s_MeshRenderData.CapsuleVBO->SetLayout(bufferLayout);
    s_MeshRenderData.CapsuleVAO->AddVertexBuffer(s_MeshRenderData.CapsuleVBO);
    s_MeshRenderData.CapsuleVBOBase = new MeshVertexData[MeshRenderData::MaxCapsuleVertices];

    u32 *capsuleIndices = new u32[MeshRenderData::MaxCapsuleIndices];
    baseIndicesCount = static_cast<u32>(s_MeshRenderData.CapsuleData->indices.size());
    u32 maxCapsules = MeshRenderData::MaxCapsuleIndices / baseIndicesCount;
    for (u32 capsuleIndex = 0; capsuleIndex < maxCapsules; ++capsuleIndex)
    {
        u32 vertexOffset = capsuleIndex * static_cast<u32>(s_MeshRenderData.CapsuleData->vertices.size());
        for (u32 i = 0; i < baseIndicesCount; ++i)
        {
            capsuleIndices[capsuleIndex * baseIndicesCount + i] = s_MeshRenderData.CapsuleData->indices[i] + vertexOffset;
        }
    }

    Ref<IndexBuffer> capsuleIBO = IndexBuffer::Create(capsuleIndices, MeshRenderData::MaxCapsuleIndices);
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

void MeshRenderer::AttachShadow(u32 id)
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
        u32 dataSize = reinterpret_cast<u8 *>(s_MeshRenderData.CubeVBOPtr) - reinterpret_cast<u8 *>(s_MeshRenderData.CubeVBOBase);
        s_MeshRenderData.CubeVBO->Bind();
        s_MeshRenderData.CubeVBO->SetData(s_MeshRenderData.CubeVBOBase, dataSize);

        for (u32 i = 0; i < s_MeshRenderData.AlbedoTextureSlotIndex; i++)
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
        const u32 data_size = reinterpret_cast<u8 *>(s_MeshRenderData.SphereVBOPtr) - reinterpret_cast<u8 *>(s_MeshRenderData.SphereVBOBase);
        s_MeshRenderData.SphereVBO->Bind();
        s_MeshRenderData.SphereVBO->SetData(s_MeshRenderData.SphereVBOBase, data_size);

        for (u32 i = 0; i < s_MeshRenderData.AlbedoTextureSlotIndex; i++)
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
        u32 dataSize = reinterpret_cast<u8 *>(s_MeshRenderData.CapsuleVBOPtr) - reinterpret_cast<u8 *>(s_MeshRenderData.CapsuleVBOBase);
        s_MeshRenderData.CapsuleVBO->Bind();
        s_MeshRenderData.CapsuleVBO->SetData(s_MeshRenderData.CapsuleVBOBase, dataSize);

        for (u32 i = 0; i < s_MeshRenderData.AlbedoTextureSlotIndex; i++)
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

void MeshRenderer::DrawMesh(const glm::mat4 &viewProjection, const glm::mat4 &transform, const Ref<VertexArray> &va, Shader *shader)
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
        //glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(transform)));
        s_MeshRenderData.CubeVBOPtr->Normals = s_MeshRenderData.CubeData->vertices[i].Normals;
        s_MeshRenderData.CubeVBOPtr->Color = color;
        s_MeshRenderData.CubeVBOPtr->UV = s_MeshRenderData.CubeData->vertices[i].UV;
        s_MeshRenderData.CubeVBOPtr->TilingFactor = s_MeshRenderData.CubeData->vertices[i].TilingFactor;
        s_MeshRenderData.CubeVBOPtr->AlbedoIndex = 0.0f;
        s_MeshRenderData.CubeVBOPtr->SpecularIndex = 0.0f;
        s_MeshRenderData.CubeVBOPtr++;
    }

    s_MeshRenderData.CubeIndexCount += static_cast<u32>(s_MeshRenderData.CubeData->indices.size());
    Renderer::GetStatistics().CubeCount++;
}

void MeshRenderer::DrawCube(const glm::mat4 &transform, const Material *material)
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
    const Ref<Texture2D> &albedo,
    const Ref<Texture2D> &specular,
    const glm::vec2 &tilingFactor, const glm::vec4 &color)
{
    if (s_MeshRenderData.CubeIndexCount >= MeshRenderData::MaxCubeIndices)
        NextBatch();

    float albedoIndex = 0.0f;
    if (albedo)
    {
        for (u32 i = 2; i < s_MeshRenderData.AlbedoTextureSlotIndex; i++)
        {
            if (*s_MeshRenderData.TextureSlots[i] == *albedo)
            {
                albedoIndex = static_cast<float>(i);
                break;
            }
        }

        if (albedoIndex == 0)
        {
            if (s_MeshRenderData.AlbedoTextureSlotIndex >= MeshRenderData::MaxTextureSlots)
                NextBatch();
            albedoIndex = static_cast<float>(s_MeshRenderData.AlbedoTextureSlotIndex);
            s_MeshRenderData.TextureSlots[s_MeshRenderData.AlbedoTextureSlotIndex] = albedo;
            s_MeshRenderData.AlbedoTextureSlotIndex++;
        }
    }

    float specularIndex = 16.0f;
    if (specular)
    {
        for (u32 i = 16; i < s_MeshRenderData.SpecularTextureSlotIndex; i++)
        {
            if (*s_MeshRenderData.TextureSlots[i] == *specular)
            {
                specularIndex = static_cast<float>(i);
                break;
            }
        }

        if (specularIndex == 16)
        {
            if (s_MeshRenderData.SpecularTextureSlotIndex >= MeshRenderData::MaxTextureSlots)
                NextBatch();
            specularIndex = static_cast<float>(s_MeshRenderData.SpecularTextureSlotIndex);
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

    s_MeshRenderData.CubeIndexCount += static_cast<u32>(s_MeshRenderData.CubeData->indices.size());
    Renderer::GetStatistics().CubeCount++;
}

void MeshRenderer::DrawSphere(const glm::mat4 &transform, const glm::vec4 &color)
{
    if (s_MeshRenderData.SphereIndexCount >= MeshRenderData::MaxSphereIndices)
        NextBatch();

    for (size_t i = 0; i < s_MeshRenderData.SphereData->vertices.size(); i++)
    {
        s_MeshRenderData.SphereVBOPtr->Position = transform * glm::vec4(s_MeshRenderData.SphereData->vertices[i].Position, 1.0f);
        glm::mat3 transpose_mat = glm::mat3(glm::transpose(glm::inverse(transform)));
        s_MeshRenderData.SphereVBOPtr->Normals = transpose_mat * s_MeshRenderData.SphereData->vertices[i].Normals;
        s_MeshRenderData.SphereVBOPtr->Color = color;
        s_MeshRenderData.SphereVBOPtr->UV = s_MeshRenderData.SphereData->vertices[i].UV;
        s_MeshRenderData.SphereVBOPtr->TilingFactor = s_MeshRenderData.SphereData->vertices[i].TilingFactor;
        s_MeshRenderData.SphereVBOPtr->AlbedoIndex = 0.0f;
        s_MeshRenderData.SphereVBOPtr->SpecularIndex = 0.0f;
        s_MeshRenderData.SphereVBOPtr++;
    }

    s_MeshRenderData.SphereIndexCount += static_cast<u32>(s_MeshRenderData.SphereData->indices.size());
    Renderer::GetStatistics().SphereCount++;
}

void MeshRenderer::DrawSphere(const glm::mat4 &transform, const Material *material)
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

    s_MeshRenderData.CapsuleIndexCount += static_cast<u32>(s_MeshRenderData.CapsuleData->indices.size());
    Renderer::GetStatistics().SphereCount++;
}

void MeshRenderer::DrawCapsule(const glm::mat4 &transform, const Material *material)
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

