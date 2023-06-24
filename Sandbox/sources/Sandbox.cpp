// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "Origin\EntryPoint.h"
#include "Sandbox.h"

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <imgui.h>

using namespace origin;

Sandbox::Sandbox()
	: Layer("Sandbox")
{
	tex1 = Texture2D::Create("Sandbox/marioA.png");
	tex2 = Texture2D::Create("Sandbox/marioB.png");
	tex3 = Texture2D::Create("Sandbox/marioC.png");
	tex4 = Texture2D::Create("Sandbox/marioD.png");
	tex5 = Texture2D::Create("Sandbox/marioE.png");

	anim.AddFrame(tex1, frameTime);
	anim.AddFrame(tex2, frameTime);
	anim.AddFrame(tex3, frameTime);
	anim.AddFrame(tex4, frameTime);
	anim.AddFrame(tex5, frameTime);

	camera = EditorCamera(30.0f, 16.0f/9.0f, 0.1f, 1000.0f);
	camera.EnableMovement(true);
}

Sandbox::~Sandbox()
{
}

void Sandbox::OnUpdate(Timestep ts)
{
	RenderCommand::Clear();
	RenderCommand::ClearColor(glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));

	camera.OnUpdate(ts);
	uint32_t width = Application::Get().GetWindow().GetWidth();
	uint32_t height = Application::Get().GetWindow().GetHeight();
	camera.SetViewportSize((float)width, (float)height);

	// Render Here

	Renderer2D::ResetStats();
	Renderer::BeginScene(camera);

	anim.Update(ts);
	anim.Render();

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.7f, 0.0f))
		* glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));

	TextComponent tc;
	tc.FontAsset = Font::GetDefault();
	tc.Color = glm::vec4(0.23f, 0.12f, 1.0f, 1.0f);

	Renderer2D::DrawString("It's Me Mario", transform, tc);

	Renderer::EndScene();
}

void Sandbox::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(OGN_BIND_EVENT_FN(Sandbox::OnWindowResize));

	camera.OnEvent(e);
}

bool Sandbox::OnWindowResize(WindowResizeEvent& e)
{
	return false;
}

void Sandbox::OnGuiRender()
{
	ImGui::Begin("Window");
	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	glm::vec3 pos = camera.GetPosition();
	ImGui::Text("Camera Pos (%.3f, %.3f, %.3f)", pos.x, pos.y, pos.z);
	ImGui::Text("Anim SpriteIndex: %i", anim.GetFrameIndex());
	ImGui::Text("Anim FrameCount : %i", anim.GetFrameCount());

	if (ImGui::Button("Reset")) anim.Reset(); ImGui::SameLine();
	if (ImGui::Checkbox("Loop", &looping)) anim.SetLoop(looping);
	if (ImGui::SliderFloat("Frame Time", &frameTime, 0.0f, 1.0f))
		anim.SetFrameTime(frameTime);

	ImGui::End();
}
