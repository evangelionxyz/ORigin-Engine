// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "Origin\EntryPoint.h"
#include "Sandbox.h"

#include <glm\glm.hpp>

#include <imgui.h>

using namespace Origin;

Sandbox::Sandbox() : Layer("Sandbox")
{
}

Sandbox::~Sandbox()
{
}

void Sandbox::OnAttach()
{
	camera = EditorCamera(30.0f, 16.0f/9.0f, 0.1f, 1000.0f);
}

float rotation = 0.0f;

void Sandbox::OnUpdate(Timestep ts)
{
	RenderCommand::Clear();

	Renderer2D::BeginScene(camera);

	float speed = 1.3f;
	rotation += speed * ts;

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f))
		* glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(1, 0, 0))
		* glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0, 1, 0))
		* glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0, 0, 1))
		* glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

	Renderer2D::DrawQuad(transform, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));

	Renderer2D::EndScene();
}

void Sandbox::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(OGN_BIND_EVENT_FN(Sandbox::OnWindowResize));
}

bool Sandbox::OnWindowResize(WindowResizeEvent& e)
{
	camera.SetViewportSize((float)e.getWidth(), (float)e.getHeight());
	return false;
}

void Sandbox::OnGuiRender()
{
	ImGui::Begin("Window");

	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::End();
}
