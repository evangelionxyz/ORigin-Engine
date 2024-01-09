// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "Origin\EntryPoint.h"
#include "SandboxLayer.h"

#include <imgui.h>

using namespace origin;

std::shared_ptr<Shader> shader;
std::shared_ptr<VertexArray> va;
std::shared_ptr<VertexBuffer> vb;

struct MyBuffer
{
	glm::mat4 projection;
	glm::mat4 view;
	glm::vec3 color;
};

MyBuffer buffer;
uint32_t ubo;

SandboxLayer::SandboxLayer() : Layer("Sandbox")
{
	shader = Shader::Create("Resources/Sandbox/shader.glsl", true, true);
	shader->Enable();

	float v[] = {
		-0.5f,-0.5f, 1.0f, 1.0f, 1.0f,
		 0.0f, 0.5f, 1.0f, 1.0f, 1.0f,
		 0.5f,-0.5f, 1.0f, 1.0f, 1.0f
	};

	va = VertexArray::Create();
	vb = VertexBuffer::Create(v, sizeof(v));
	vb->SetLayout({
		{ShaderDataType::Float2, "aPos"},
		{ShaderDataType::Float3, "aColor"}
		});
	va->AddVertexBuffer(vb);

	auto w = Application::Get().GetWindow().GetWidth();
	auto h = Application::Get().GetWindow().GetHeight();

	float aspectRatio = w / h;
	buffer.projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);
	buffer.view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 4.0f));
	buffer.view = glm::inverse(buffer.view);

	buffer.color = glm::vec3(1.0f, 0.0f, 0.0f);

	glCreateBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(MyBuffer), NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

SandboxLayer::~SandboxLayer()
{
	glDeleteBuffers(1, &ubo);
}

void SandboxLayer::OnUpdate(Timestep ts)
{
	RenderCommand::Clear();

	shader->Enable();
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(MyBuffer), &buffer);

	RenderCommand::DrawArrays(va, 3);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void SandboxLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnWindowResize));

	camera.OnEvent(e);
}

bool SandboxLayer::OnWindowResize(WindowResizeEvent& e)
{
	RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());

	float aspectRatio = static_cast<float>(e.GetWidth()) / static_cast<float>(e.GetHeight());
	buffer.projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);
	buffer.view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 4.0f));
	buffer.view = glm::inverse(buffer.view);

	return false;
}

void SandboxLayer::OnGuiRender()
{
	ImGui::Begin("Window");
	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	glm::vec3 pos = camera.GetPosition();
	ImGui::Text("Camera Pos (%.3f, %.3f, %.3f)", pos.x, pos.y, pos.z);
	ImGui::ColorEdit4("Color", glm::value_ptr(buffer.color));

	ImGui::End();
}
