// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Origin/EntryPoint.h"
#include "Origin/Core/KeyCodes.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/GUI/UI.h"

#include "Platform/Vulkan/VulkanShader.h"

#include "SandboxLayer.h"

#include <iostream>

using namespace origin;

SandboxLayer::SandboxLayer() : Layer("Sandbox")
{
}

void SandboxLayer::OnAttach()
{
	VulkanShader shader = VulkanShader("Resources/Shaders/Vulkan/vert.spv", "Resources/Shaders/Vulkan/frag.spv");
}

SandboxLayer::~SandboxLayer()
{
}

void SandboxLayer::OnUpdate(Timestep ts)
{
}

void SandboxLayer::OnEvent(Event &e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<FramebufferResizeEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnWindowResize));
	dispatcher.Dispatch<KeyPressedEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnKeyPressedEvent));
}

void SandboxLayer::OnGuiRender()
{
}

bool SandboxLayer::OnWindowResize(FramebufferResizeEvent &e)
{
	RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
	return false;
}

bool SandboxLayer::OnKeyPressedEvent(KeyPressedEvent &e)
{
	return false;
}