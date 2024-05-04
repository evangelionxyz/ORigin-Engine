// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Origin\EntryPoint.h"
#include "Origin\Asset\AssetImporter.h"
#include "Origin\GUI\UI.h"
#include "SandboxLayer.h"
#include <imgui.h>
#include <glad/glad.h>

#pragma warning(disable : OGN_DISABLED_WARNINGS)

using namespace origin;

struct UIRenderer
{
	uint32_t vao, vbo;
	std::shared_ptr<Texture2D> m_Texture;
	std::shared_ptr<Shader> m_Shader;
	
	UIRenderer()
	{
		float vertices[] = {
			-1.0f, 1.0f,
			 1.0f, 1.0f,
			-1.0f,-1.0f,
			 1.0f,-1.0f
		};
		
		glCreateVertexArrays(1, &vao);
		glBindVertexArray(vao);
		
		glCreateBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(float), nullptr);

		m_Shader = Shader::Create("Resources/Shaders/SPIR-V/Screen.glsl", false);
		m_Texture = Texture2D::Create("Resources/UITextures/runtime_loading_screen.png");
	}

	~UIRenderer()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}

	void Draw()
	{
		m_Shader->Enable();
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glActiveTexture(GL_TEXTURE0);
		m_Texture->Bind(0);
		m_Shader->SetInt("uTexture", m_Texture->GetIndex());

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.9f, 0.9f, 0.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 1.0f));
		m_Shader->SetMatrix("uProjection", transform);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		m_Shader->Disable();
	}
};

UIRenderer *ui = nullptr;

void SandboxLayer::OnAttach()
{
	ui = new UIRenderer();

	camera.InitPerspective(45.0f, 16.0f / 9.0f, 0.1f, 500.0f);
	camera.SetMoveActive(true);
}

SandboxLayer::~SandboxLayer()
{
	delete ui;
}

void SandboxLayer::OnUpdate(Timestep ts)
{
	camera.OnUpdate(ts);

	glClearColor(0.5f,0.5f, 0.5f,0.5f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//ui->Draw();
}

void SandboxLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnWindowResize));
}

bool SandboxLayer::OnWindowResize(WindowResizeEvent& e)
{
	glViewport(0, 0, e.GetWidth(), e.GetHeight());
	camera.SetViewportSize(e.GetWidth(), e.GetHeight());
	return false;
}

void SandboxLayer::OnGuiRender()
{
	ImGui::Begin("Test");
	static bool test = false;
	static glm::vec2 vec2Test = glm::vec2(0.0f);
	static glm::vec3 vec3Test = glm::vec3(0.0f);

	UI::DrawCheckbox("Hello World", &test);
	UI::DrawVec3Control("Position", vec3Test);
	UI::DrawVec2Control("Size", vec2Test);

	ImGui::End();
}
