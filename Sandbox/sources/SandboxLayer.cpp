// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Origin/EntryPoint.h"
#include "Origin/Core/KeyCodes.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/GUI/UI.h"
#include "SandboxLayer.h"
#include "MeshRenderer.h"
#include "MeshVertexData.h"
#include "ModelLoader.h"
#include <imgui.h>


using namespace origin;

struct Light
{
	glm::vec4 Direction = { -0.5f, -1.0f, -0.5f, 1.0f };
	glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	glm::vec4 AmbientColor = { 0.6f, 0.6f, 0.6f, 1.0f };
};

Light light;
std::shared_ptr<UniformBuffer> lightBuffer;

SandboxLayer::SandboxLayer()
	: Layer("Sandbox"), rng(std::random_device {}()), dist(0.0f, 1.0f)
{
	randomColor.resize(10, glm::vec3(1.0f));
}

void SandboxLayer::OnAttach()
{
	MeshRenderer::Init();

	m_Camera.InitPerspective(45.0f, 1.776f, 1.0f, 300.0f);
	m_Camera.SetPosition({ 0.0f, 5.0f, 10.0f });

    m_SceneCamera.InitPerspective(45.0f, 1.776f, 1.0f, 100.0f);
	m_CamTC.WorldTranslation = { 0.0f, 20.0f, 0.0f };
	nPlane = m_SceneCamera.GetNear();
	fPlane = m_SceneCamera.GetFar();
	FOV = m_SceneCamera.GetFOV();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	lightBuffer = UniformBuffer::Create(sizeof(Light), 1);
}

SandboxLayer::~SandboxLayer()
{
}

void SandboxLayer::OnUpdate(Timestep ts)
{
	Renderer::GetStatistics().Reset();

	m_Camera.OnUpdate(ts);
	m_SceneCamera.OnUpdate(ts);

	m_Frustum.Update(m_SceneCamera.GetProjection() * glm::inverse(m_CamTC.GetTransform()));
	
	RenderCommand::ClearColor(0.1f, 0.3f, 0.4f, 1.0f);
	RenderCommand::Clear();

	glPolygonMode(GL_FRONT_AND_BACK, polygonMode ? GL_LINE : GL_FILL);
		
	if (timer <= 0.0f)
	{
		deltaTime = ts;
		framerate = 1.0f / deltaTime;
		response = 1000.0f / framerate;
		timer = updateRate;
	}
	timer -= ts;

	lightBuffer->Bind();
	lightBuffer->SetData(&light, sizeof(Light));
	MeshRenderer::Begin(m_Camera);

	if (timeColorChange >= 2.0f)
	{
		for (auto &color : randomColor)
		{
			color = GetRandomColor();
		}
		timeColorChange = 0.0f;
	}

	timeColorChange += ts;

    for (int i = -size; i < size; ++i)
    {
        for (int z = -size; z < size; ++z)
        {
            float x = i + i * 1.5f;
            float z_pos = z + z * 2.0f;
            float time = glfwGetTime();
            float y = sin(x * 0.5f + z_pos * 2.5f + time) * 1.5f; // Adjust multipliers for wave frequency and amplitude

			glm::vec3 center = { x, y, z_pos };

			if (m_Frustum.IsSphereVisible(center, 1.0f))
			{
                glm::mat4 transform = glm::translate(glm::mat4(1.0f), center);
                MeshRenderer::DrawSphere(transform, glm::vec4(randomColor[(i + size) % 10], 1.0f));
			}
        }
    }

	MeshRenderer::End();

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
	ImGui::Checkbox("Polygon Mode", &polygonMode);
	ImGui::SliderInt("Size", &size, 0, 1000);
	ImGui::Separator();

	ImGui::DragFloat3("Direction", glm::value_ptr(light.Direction), 0.025f);
	ImGui::ColorEdit3("LightColor", glm::value_ptr(light.Color));
	ImGui::ColorEdit3("AmbientColor", glm::value_ptr(light.AmbientColor));

	ImGui::Separator();
	ImGui::Text("Line Count %d", Renderer::GetStatistics().LineCount);

	ImGui::Text("Cube Count %d", Renderer::GetStatistics().CubeCount);
	ImGui::Text("Cube Vertices %d", Renderer::GetStatistics().GetTotalCubeVertexCount());
	ImGui::Text("Cube Indices %d", Renderer::GetStatistics().GetTotalCubeIndexCount());

	ImGui::Text("Sphere Count %d", Renderer::GetStatistics().SphereCount);
    ImGui::Text("Sphere Vertices %d", Renderer::GetStatistics().GetTotalSphereVertexCount());
    ImGui::Text("Sphere Indices %d", Renderer::GetStatistics().GetTotalSphereIndexCount());

	ImGui::Text("Draw Calls %d", Renderer::GetStatistics().DrawCalls);

	ImGui::End();


	ImGui::Begin("Camera");
    UI::DrawVec3Control("Translation", m_CamTC.WorldTranslation);
    glm::vec3 rotation = glm::degrees(m_CamTC.WorldRotation);
    UI::DrawVec3Control("Rotation", rotation, 1.0f);
	m_CamTC.WorldRotation = glm::radians(rotation);

	UI::DrawFloatControl("FOV", &FOV, 1.0f, 20.0f, 90.0f);
	m_SceneCamera.SetFov(glm::radians(FOV));
	UI::DrawFloatControl("Near", &nPlane, 0.025f, 0.0f, 20.0f);
	m_SceneCamera.SetNear(nPlane);
	UI::DrawFloatControl("Far", &fPlane, 0.025f, 100.0f, 500.0f);
	m_SceneCamera.SetFar(fPlane);

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

	for (const auto &edge : m_Frustum.GetEdges())
	{
		Renderer2D::DrawLine(edge.first, edge.second, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	Renderer2D::End();
}

glm::vec3 SandboxLayer::GetRandomColor()
{
    return glm::vec3(dist(rng), dist(rng), dist(rng));
}
