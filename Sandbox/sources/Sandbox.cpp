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
	mario1 = Texture2D::Create("Sandbox/marioA.png");
	mario2 = Texture2D::Create("Sandbox/marioB.png");
	mario3 = Texture2D::Create("Sandbox/marioC.png");
	mario4 = Texture2D::Create("Sandbox/marioD.png");
	mario5 = Texture2D::Create("Sandbox/marioE.png");

	marioAnim.AddFrame(mario1, marioFrameTime);
	marioAnim.AddFrame(mario2, marioFrameTime);
	marioAnim.AddFrame(mario3, marioFrameTime);
	marioAnim.AddFrame(mario4, marioFrameTime);
	marioAnim.AddFrame(mario5, marioFrameTime);


	fireball1 = Texture2D::Create("Sandbox/fireball/fireballA.png");
	fireball2 = Texture2D::Create("Sandbox/fireball/fireballB.png");
	fireball3 = Texture2D::Create("Sandbox/fireball/fireballC.png");
	fireball4 = Texture2D::Create("Sandbox/fireball/fireballD.png");
	fireball5 = Texture2D::Create("Sandbox/fireball/fireballE.png");
	fireball6 = Texture2D::Create("Sandbox/fireball/fireballF.png");
	fireball7 = Texture2D::Create("Sandbox/fireball/fireballG.png");
	fireball8 = Texture2D::Create("Sandbox/fireball/fireballH.png");

	fireballAnim.AddFrame(fireball1, fireballFrameTime);
	fireballAnim.AddFrame(fireball2, fireballFrameTime);
	fireballAnim.AddFrame(fireball3, fireballFrameTime);
	fireballAnim.AddFrame(fireball4, fireballFrameTime);
	fireballAnim.AddFrame(fireball5, fireballFrameTime);
	fireballAnim.AddFrame(fireball6, fireballFrameTime);
	fireballAnim.AddFrame(fireball7, fireballFrameTime);
	fireballAnim.AddFrame(fireball8, fireballFrameTime);

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

	marioAnim.Update(ts);
	fireballAnim.Update(ts);

	glm::mat4 marioTransform = glm::translate(glm::mat4(1.0f), marioPosition);
	marioAnim.Render(marioTransform);

	glm::mat4 fireballTransform = glm::translate(glm::mat4(1.0f), fireballPosition);
	fireballAnim.Render(fireballTransform);

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

	ImGui::Separator();
	ImGui::PushID("mario");
	ImGui::Text("Mario");
	ImGui::Text("Anim SpriteIndex: %i", marioAnim.GetFrameIndex());
	ImGui::Text("Anim FrameCount : %i", marioAnim.GetFrameCount());

	if (ImGui::Button("Reset")) marioAnim.Reset(); ImGui::SameLine();
	if (ImGui::Checkbox("Loop", &looping)) marioAnim.SetLoop(looping);
	ImGui::DragFloat3("Position", glm::value_ptr(marioPosition), 0.5f);
	if (ImGui::SliderFloat("Frame Time", &marioFrameTime, 0.0f, 1.0f))
		marioAnim.SetFrameTime(marioFrameTime);
	ImGui::PopID();
	ImGui::Separator();

	ImGui::PushID("fireball");
	ImGui::Text("Fireball");
	ImGui::Text("Anim SpriteIndex: %i", fireballAnim.GetFrameIndex());
	ImGui::Text("Anim FrameCount : %i", fireballAnim.GetFrameCount());

	if (ImGui::Button("Reset")) fireballAnim.Reset(); ImGui::SameLine();
	if (ImGui::Checkbox("Loop", &looping)) fireballAnim.SetLoop(looping);
	ImGui::DragFloat3("Position", glm::value_ptr(fireballPosition), 0.5f);
	if (ImGui::SliderFloat("Frame Time", &fireballFrameTime, 0.0f, 1.0f))
		fireballAnim.SetFrameTime(fireballFrameTime);
	ImGui::PopID();

	ImGui::End();
}
