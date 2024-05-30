// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Origin\EntryPoint.h"
#include "Origin\Asset\AssetImporter.h"
#include "Origin\GUI\UI.h"
#include "SandboxLayer.h"
#include <imgui.h>
#include <glad/glad.h>

#pragma warning(disable : OGN_DISABLED_WARNINGS)

using namespace origin;

std::vector<std::shared_ptr<Asset>> Assets;
Renderer2D::TextParams params;

void SandboxLayer::OnAttach()
{
	camera.InitOrthographic(10.0f, 0.0f, 200.0f);
	camera.SetPosition(glm::vec3(0.0f, 0.0f, 8.0f));
	camera.SetViewportSize(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());

	camera.SetMoveActive(true);
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

	static float hue = 0.0f;
	hue += ImGui::GetIO().DeltaTime;
	if (hue >= 360.0f)
			hue -= 360.0f;
	ImVec4 col = ImColor::HSV(hue, 0.5f, 1.0f).Value;
	glm::vec4 color = glm::vec4(col.x, col.y, col.z, col.w);
	Renderer2D::DrawQuad(glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)), color);

	int i = 0;
	for (auto &font : Assets)
	{
		if (font)
		{
			//Renderer2D::DrawString("This is text", font, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, i * 1.2f, 0.0f)), params);
			Renderer2D::DrawString("This is text", std::static_pointer_cast<Font>(font), glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, i * 1.2f, 0.0f)), params);
		}
			
		i++;
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
	ImGui::Begin("Test Window");

	ImGui::Text("%.3f fps %.3f", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);

	if (ImGui::Button("Add"))
	{
		for (int i = 0; i < 3; i++)
		{
			std::shared_ptr<Asset> asset;
			Assets.push_back(asset);
			
		}

		for (auto &asset : Assets)
		{
			FontImporter::LoadAsync(&asset, "Resources/Fonts/segoeui.ttf");
		}
		
	}

	ImGui::End();
}

bool SandboxLayer::OnKeyPressedEvent(KeyPressedEvent &e)
{
	return false;
}
