// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Origin\EntryPoint.h"
#include "Origin\Asset\AssetImporter.h"
#include "Origin\GUI\UI.h"
#include "SandboxLayer.h"
#include <imgui.h>
#include <glad/glad.h>

#pragma warning(disable : OGN_DISABLED_WARNINGS)

using namespace origin;

std::vector<std::shared_ptr<Font>> Fonts;
Renderer2D::TextParams params;

void SandboxLayer::OnAttach()
{
	camera.InitOrthographic(10.0f, 0.0f, 200.0f);
	camera.SetPosition(glm::vec3(0.0f, 0.0f, 8.0f));
	camera.SetMoveActive(true);

	for (int i = 0; i < 30; i++)
	{
		std::shared_ptr<Font> font;
		Fonts.push_back(font);
	}

	int idx = 0;
	for (auto &f : Fonts)
	{
		if (idx % 2 == 0)
			FontLoader::LoadFontAsync(&f, "Resources/Fonts/Winter Memories.ttf");
		else
			FontLoader::LoadFontAsync(&f, "Resources/Fonts/segoeui.ttf");

		idx++;
	}
}

SandboxLayer::~SandboxLayer()
{
}

void SandboxLayer::OnUpdate(Timestep ts)
{
	camera.OnUpdate(ts);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Renderer2D::Begin(camera);

	Renderer2D::DrawQuad(glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)), glm::vec4(1.0f));

	for (int i = 0; i < Fonts.size(); i++)
	{
		auto f = Fonts[i];
		if(f)
			Renderer2D::DrawString("This is text", f, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, i * 1.1f, 0.0f)), params);
	}
	Renderer2D::End();
}

void SandboxLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnWindowResize));
	dispatcher.Dispatch<KeyPressedEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnKeyPressedEvent));

	camera.OnEvent(e);
}

bool SandboxLayer::OnWindowResize(WindowResizeEvent& e)
{
	glViewport(0, 0, e.GetWidth(), e.GetHeight());
	camera.SetViewportSize(e.GetWidth(), e.GetHeight());
	return false;
}

void SandboxLayer::OnGuiRender()
{
}

bool SandboxLayer::OnKeyPressedEvent(KeyPressedEvent &e)
{
	if (e.GetKeyCode() == Key::P)
		Font::CheckChanges();

	return false;
}
