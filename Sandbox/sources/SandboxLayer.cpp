// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Origin/EntryPoint.h"
#include "Origin/Core/KeyCodes.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/GUI/UI.h"
#include "SandboxLayer.h"
#include <iostream>
#include <imgui.h>

using namespace origin;


// Function to compile and link shaders
GLuint compileShaders(const char *vertexSrc, const char *fragmentSrc)
{
    // Compile Vertex Shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, NULL);
    glCompileShader(vertexShader);

    // Check for Vertex Shader compile errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Compile Fragment Shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSrc, NULL);
    glCompileShader(fragmentShader);

    // Check for Fragment Shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Clean up shaders as they are no longer needed
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

struct Data
{
    GLuint depthShaderProgram;
    GLuint fboId, texId;
    std::shared_ptr<UniformBuffer> ubo;
    std::shared_ptr<Shader> shader;

    void CreateFbo()
    {
        glCreateFramebuffers(1, &fboId);

        glCreateTextures(GL_TEXTURE_2D, 1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, fboId);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texId, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Vertex Shader source code
        const char *vertexSrc = R"glsl(
#version 450 core
layout (location = 0) in vec3 aPos;
layout(std140, location = 0) uniform Depth {
    mat4 viewProjection;
};
void main()
{
    gl_Position = viewProjection * vec4(aPos, 1.0);
}
)glsl";

        // Fragment Shader source code
        const char *fragmentSrc = R"glsl(
#version 450 core
void main()
{
}
)glsl";
        depthShaderProgram = compileShaders(vertexSrc, fragmentSrc);
        ubo = UniformBuffer::Create(sizeof(glm::mat4), 0);

        shader = Shader::Create("Resources/Shaders/SPIR-V/DepthMap.glsl");
    }

    void Shutdown()
    {
        glDeleteBuffers(1, &fboId);
    }
};

Data d;

SandboxLayer::SandboxLayer() : Layer("Sandbox")
{
}

void SandboxLayer::OnAttach()
{
	m_Camera.InitPerspective(45.0f, 1.778f, 1.0f, 100.0f);
    m_Camera.SetStyle(Pivot);
	m_Camera.SetAllowedMove(true);

    d.CreateFbo();
}

SandboxLayer::~SandboxLayer()
{
    d.Shutdown();
}

void SandboxLayer::OnUpdate(Timestep ts)
{
	int posX, posY;
	int width, height;
	glfwGetWindowPos((GLFWwindow *)Input::Get().GetWindow(), &posX, &posY);
	glfwGetWindowSize((GLFWwindow *)Input::Get().GetWindow(), &width, &height);
	glm::vec2 screenMin = { posX, posY };
	glm::vec2 screenMax = { width, height };
	m_Camera.OnUpdate(ts, screenMin, screenMax);

    glBindFramebuffer(GL_FRAMEBUFFER, d.fboId);
    glViewport(0, 0, 1024, 1024);
    glClear(GL_DEPTH_BUFFER_BIT);

    d.shader->Enable();
    glm::mat4 viewProjection = m_Camera.GetViewProjection();
    d.ubo->Bind();
    d.ubo->SetData(&viewProjection, sizeof(glm::mat4));

    MeshRenderer::Begin(m_Camera.GetViewProjection());
    MeshRenderer::DrawCube(glm::mat4(1.0f), glm::vec4(1.0f));
    MeshRenderer::DrawCube(glm::translate(glm::mat4(1.0f), { 5.0f, 0.0f, 0.0f }), glm::vec4(1.0f));
    MeshRenderer::End();
    d.shader->Disable();

    glBindBuffer(GL_FRAMEBUFFER, 0);


}

void SandboxLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<FramebufferResizeEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnWindowResize));
	dispatcher.Dispatch<KeyPressedEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnKeyPressedEvent));

	m_Camera.OnEvent(e);
}

void SandboxLayer::OnGuiRender()
{
    ImGui::Begin("Window");
    ImTextureID texId = reinterpret_cast<ImTextureID>(d.texId);
    ImGui::Image(texId, ImVec2(312.0f, 312.0f), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
}

bool SandboxLayer::OnWindowResize(FramebufferResizeEvent& e)
{
	RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
	m_Camera.SetViewportSize(e.GetWidth(), e.GetHeight());
	return false;
}

bool SandboxLayer::OnKeyPressedEvent(KeyPressedEvent &e)
{
	if (e.GetKeyCode() == Key::D1)
	{
		MeshRenderer::GetShader()->Reload();
	}

	return false;
}
