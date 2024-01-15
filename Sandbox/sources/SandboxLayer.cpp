// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "Origin\EntryPoint.h"
#include "SandboxLayer.h"

#include <imgui.h>

using namespace origin;

std::shared_ptr<Shader> shader;
std::shared_ptr<VertexArray> va;
std::shared_ptr<VertexBuffer> vb;

struct ModelData
{
	std::shared_ptr<Model> Model;
	std::shared_ptr<Material> Material;

	glm::vec3 Position = glm::vec3(0.0f);
	glm::vec3 Rotation = glm::vec3(0.0f);
	glm::vec3 Scale = glm::vec4(1.0f);
	glm::mat4 Transform = glm::mat4(1.0f);

	void Update(float dt)
	{
		glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
		Transform = glm::translate(glm::mat4(1.0f), Position)
			* rotation * glm::scale(glm::mat4(1.0f), Scale);
	}
};

ModelData cylinder;
ModelData plane;

std::shared_ptr<Texture2D> texture;


SandboxLayer::SandboxLayer() : Layer("Sandbox")
{
	camera.InitPerspective(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
	camera.SetProjectionType(ProjectionType::Perspective);
	camera.SetPosition(glm::vec3(0.0f, 1.0f, 10.0f));

	shader = Shader::Create("Resources/Shaders/SPIR-V/Mesh.glsl", true, true);
	shader->Enable();

	cylinder.Material = Material::Create("cylinder", shader);
	cylinder.Model = Model::Create("D:/Dev/ORiginProjects/TestGame/Assets/Models/cylinder_brick.obj", cylinder.Material);
	cylinder.Position.x = 2.0f;
	cylinder.Transform = glm::translate(glm::mat4(1.0f), cylinder.Position) * glm::scale(glm::mat4(1.0f), cylinder.Scale);

	plane.Material = Material::Create("plane", shader);
	plane.Model = Model::Create("D:/Dev/ORiginProjects/TestGame/Assets/Models/checker_plane.obj", plane.Material);
	plane.Position.x - 2.0f;
	plane.Transform = glm::translate(glm::mat4(1.0f), plane.Position) * glm::scale(glm::mat4(1.0f), plane.Scale);

	texture = Texture2D::Create("D:/Dev/ORiginProjects/TestGame/Assets/Models/brick.jpg");
	texture->Bind();
}

SandboxLayer::~SandboxLayer()
{
}

void SandboxLayer::OnUpdate(Timestep deltaTime)
{
	RenderCommand::Clear();

	Renderer::BeginScene(camera);
	camera.OnUpdate(deltaTime);

	Renderer2D::DrawQuad(glm::mat4(1.0f), texture);

	cylinder.Update(deltaTime);
	cylinder.Model->Draw(cylinder.Transform);

	plane.Update(deltaTime);
	plane.Model->Draw(plane.Transform);

	Renderer::EndScene();
}

void SandboxLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnWindowResize));

	camera.OnEvent(e);
}

bool SandboxLayer::OnWindowResize(WindowResizeEvent& e)
{
	camera.SetViewportSize(e.GetWidth(), e.GetHeight());
	return false;
}

void SandboxLayer::OnGuiRender()
{
	ImGui::Begin("Window");
	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	glm::vec3 pos = camera.GetPosition();
	ImGui::Text("Camera Pos (%.3f, %.3f, %.3f)", pos.x, pos.y, pos.z);

	ImGui::Separator();

	ImGui::PushID("cylinder");
	ImGui::Text("Cylinder");
	ImGui::DragFloat3("Position", glm::value_ptr(cylinder.Position), 0.1f);
	ImGui::DragFloat3("Rotation", glm::value_ptr(cylinder.Rotation), 0.1f);
	ImGui::DragFloat3("Scale", glm::value_ptr(cylinder.Scale), 0.1f);
	ImGui::PopID();

	ImGui::Separator();

	ImGui::PushID("plane");
	ImGui::Text("Plane");
	ImGui::DragFloat3("Position", glm::value_ptr(plane.Position), 0.1f);
	ImGui::DragFloat3("Rotation", glm::value_ptr(plane.Rotation), 0.1f);
	ImGui::DragFloat3("Scale", glm::value_ptr(plane.Scale), 0.1f);
	ImGui::PopID();

	ImGui::End();
}
