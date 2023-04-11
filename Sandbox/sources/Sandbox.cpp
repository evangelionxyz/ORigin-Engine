// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "Origin\EntryPoint.h"
#include "Sandbox.h"
#include "Draw.h"

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <imgui.h>

using namespace Origin;

Sandbox::Sandbox()
	: Layer("Sandbox")
{
	cubeTexture = Texture2D::Create("assets/textures/block.png");

	draw.Init();
	camera = FPSCamera(45.0f, 16.0f / 9.0f);
}

Sandbox::~Sandbox()
{
}

void Sandbox::OnUpdate(Timestep ts)
{
	glEnable(GL_DEPTH_TEST);
	RenderCommand::Clear();
	camera.OnUpdate(ts);


	// Cube
	draw.GetShader("cube")->Bind();

	draw.GetShader("cube")->SetMatrix("uViewProjection", camera.GetViewProjection());

	cubeTexture->Bind(0);
	draw.GetShader("cube")->SetInt("uTexure", cubeTexture->GetIndex());

	draw.GetShader("cube")->SetVector("uPos", cubePosition);
	draw.GetShader("cube")->SetVector("uSize", cubeSize);
	draw.GetShader("cube")->SetVector("uLightPos", lightPosition);
	draw.GetShader("cube")->SetVector("uLightColor", lightColor);
	draw.GetShader("cube")->SetFloat("uLightAmbient", Ambient);

	draw.GetShader("cube")->SetVector("uCameraPosition", camera.GetPosition());
	draw.RenderCube();
	draw.GetShader("cube")->Unbind();


	// Lighting
	draw.GetShader("light")->Bind();
	draw.GetShader("light")->SetMatrix("uViewProjection", camera.GetViewProjection());
	draw.GetShader("light")->SetVector("uPos", lightPosition);
	draw.GetShader("light")->SetVector("uSize", lightSize);
	draw.GetShader("light")->SetVector("uLightColor", lightColor);
	draw.RenderLight();
	draw.GetShader("light")->Unbind();
}

void Sandbox::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(OGN_BIND_EVENT_FN(Sandbox::OnWindowResize));

	camera.OnEvent(e);
}

bool Sandbox::OnWindowResize(WindowResizeEvent& e)
{
	camera.OnWindowResize(e.GetWidth(), e.GetHeight());
	return false;
}

void Sandbox::OnGuiRender()
{
	ImGui::Begin("Window");
	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Begin("Control");
	ImGui::PushID("cube");
	ImGui::Text("Cube");
	ImGui::DragFloat3("Position", glm::value_ptr(cubePosition), 0.25);
	ImGui::DragFloat3("Size", glm::value_ptr(cubeSize), 0.25);
	ImGui::ColorEdit4("Color", glm::value_ptr(cubeColor));
	ImGui::Separator();
	ImGui::PopID();

	ImGui::PushID("light");
	ImGui::Text("Light");
	ImGui::DragFloat3("Position", glm::value_ptr(lightPosition), 0.25);
	ImGui::DragFloat3("Size", glm::value_ptr(lightSize), 0.25);
	ImGui::ColorEdit4("Color", glm::value_ptr(lightColor));
	ImGui::DragFloat("Ambient",&Ambient, 0.01f);
	ImGui::Separator();
	ImGui::PopID();

	ImGui::End();
}
