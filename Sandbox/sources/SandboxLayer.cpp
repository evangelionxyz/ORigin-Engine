// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "Origin\EntryPoint.h"
#include "SandboxLayer.h"

#include <imgui.h>

using namespace origin;

SandboxLayer::SandboxLayer() : Layer("Sandbox")
{
}

SandboxLayer::~SandboxLayer()
{
}

void SandboxLayer::OnUpdate(Timestep ts)
{
}

void SandboxLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnWindowResize));

	camera.OnEvent(e);
}

bool SandboxLayer::OnWindowResize(WindowResizeEvent& e)
{
	return false;
}

void SandboxLayer::OnGuiRender()
{
	ImGui::Begin("Window");
	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	glm::vec3 pos = camera.GetPosition();
	ImGui::Text("Camera Pos (%.3f, %.3f, %.3f)", pos.x, pos.y, pos.z);

	ImGui::End();
}
