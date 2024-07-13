// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Origin/EntryPoint.h"
#include "Origin/Core/KeyCodes.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/GUI/UI.h"
#include "SandboxLayer.h"
#include <imgui.h>

#include "MeshRenderer.h"
#include "MeshVertexData.h"
#include "ModelLoader.h"

using namespace origin;


struct MyModel
{
	std::shared_ptr<VertexArray> vao;
	std::shared_ptr<VertexBuffer> vbo;
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	MeshData data;
	glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
	glm::vec3 position = { 0.0f, 0.0f, 0.0f };
	std::shared_ptr<Shader> shader;
};

MyModel model;
int size = 10;
float framerate = 0.0f;
float response = 0.0f;
float deltaTime = 0.0f;
float updateRate = 1.0f;
float timer = 0.0f;

glm::vec3 lightPos = glm::vec3(0.0f, 1.0f, 0.0f);

void reload()
{
	model.shader = Shader::Create("Resources/Shaders/Mesh.glsl", false);
}

void SandboxLayer::OnAttach()
{
	MeshRenderer::Init();

	m_Camera.InitPerspective(45.0f, 1.776f, 0.1f, 800.0f);
	m_Camera.SetPosition({ 0.0f, 5.0f, 10.0f });

	model.data = ModelLoader::LoadModel("Resources/Models/cube.obj");
	ModelLoader::ProcessMesh(model.data, model.vao, model.vbo);

	model.shader = Shader::Create("Resources/Shaders/Mesh.glsl", false);
}

SandboxLayer::~SandboxLayer()
{
}

void SandboxLayer::OnUpdate(Timestep ts)
{
	m_Camera.OnUpdate(ts);

	RenderCommand::ClearColor(0.1f, 0.3f, 0.4f, 1.0f);
	RenderCommand::Clear();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	if (timer <= 0.0f)
	{
		deltaTime = ts;
		framerate = 1.0f / deltaTime;
		response = 1000.0f / framerate;
		timer = updateRate;
	}
	timer -= ts;

	model.shader->Enable();
	model.shader->SetMatrix("uViewProjection", m_Camera.GetViewProjection());
	model.shader->SetVector("uLightingPosition", lightPos);
	model.shader->SetVector("uColor", model.color);
	//MeshRenderer::Begin();
	//glm::mat4 transform = glm::translate(glm::mat4(1.0f), model.position) * glm::scale(glm::mat4(1.0f), model.scale);
	//MeshRenderer::Draw(transform, model.data.vertices, model.data.indices);
	//MeshRenderer::End();

	for (int x = -size; x < size; x++)
	{
		for (int z = -size; z < size; z++)
		{
			model.shader->SetMatrix("uTransform", glm::translate(glm::mat4(1.0f), { x + (x * 1.5), 0.0f, z + (z * 1.5) }));
			RenderCommand::DrawIndexed(model.vao);
		}
	}

	model.shader->Disable();

	DrawGrid();

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
	ImGui::Begin("Window");
	
	ImGui::Text("%.3f fps %0.3f ms", framerate, response);

	ImGui::ColorEdit3("Color", glm::value_ptr(model.color));
	ImGui::DragFloat3("Light pos", glm::value_ptr(lightPos), 0.025f);
	ImGui::DragInt("Count", &size);

	ImGui::End();
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
		reload();
	}

	return false;
}

void SandboxLayer::DrawGrid()
{ 
	Renderer2D::Begin(m_Camera);

	float alphaColor = 0.3f;
	glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, alphaColor);
	glm::vec2 cameraPosition = { m_Camera.GetPosition().x, m_Camera.GetPosition().z };
	int size = 30;

	for (int s = -size; s <= size; s++)
	{
		if (s == 0)
		{
			Renderer2D::DrawLine({ -size, 0.0f, s }, { size, 0.0f, s }, { 0.0f, 0.0f, 1.0f, 1.0f });
			Renderer2D::DrawLine({ s, 0.0f, -size }, { s, 0.0f, size }, { 1.0f, 0.0f, 0.0f, 1.0f });
		}
		else
		{
			Renderer2D::DrawLine({ -size, 0.0f, s }, { size, 0.0f, s }, color);
			Renderer2D::DrawLine({ s, 0.0f, -size }, { s, 0.0f, size }, color);
		}
	}

	Renderer2D::End();
}
