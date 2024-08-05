// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Origin/EntryPoint.h"
#include "Origin/Core/KeyCodes.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/GUI/UI.h"
#include "SandboxLayer.h"
#include <iostream>
#include <imgui.h>

using namespace origin;

SandboxLayer::SandboxLayer() : Layer("Sandbox")
{
}

void SandboxLayer::OnAttach()
{
	m_Camera.InitPerspective(45.0f, 1.778f, 1.0f, 100.0f);
    m_Camera.SetStyle(Pivot);
	m_Camera.SetAllowedMove(true);
}

SandboxLayer::~SandboxLayer()
{
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
	Frustum frustum(m_Camera.GetViewProjection());

	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Renderer2D::Begin(m_Camera);
	for (auto &e : frustum.GetEdges())
	{
		Renderer2D::DrawLine(e.first, e.second, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	}
	glm::vec3 pos;
	pos.x = frustum.GetPlanes()[Frustum::Plane::Near][0];
	pos.y = frustum.GetPlanes()[Frustum::Plane::Near][1];
	pos.z = frustum.GetPlanes()[Frustum::Plane::Near][2];

	Renderer2D::DrawQuad(pos, glm::vec2(1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	Renderer2D::End();

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
