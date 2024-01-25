// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Origin\EntryPoint.h"
#include "Origin\Asset\AssetImporter.h"

#include "SandboxLayer.h"
#include <imgui.h>

#pragma warning(disable : OGN_DISABLED_WARNINGS)

using namespace origin;

std::shared_ptr<Model> boxModel, plane, cube;
std::shared_ptr<Texture2D> boxTexture;
std::shared_ptr<Shader> shader;

struct LightData
{
	glm::vec3 Position = glm::vec3(0.0f);
	glm::vec3 Color = glm::vec3(1.0f);
	float Size = 3.0f;
	float Specular = 1.0f;
	float Ambient = 0.0f;

	LightData(glm::vec3 position, glm::vec3 color)
		: Position(position), Color(color){}
};
std::vector<LightData> lights;

glm::vec3 boxPosition = glm::vec3(0.0f);

uint32_t depthFramebuffer, depthMap, shadowCubeMap;
uint32_t width = 1024, height = 1024;
std::shared_ptr<Shader> depthShader;
float nearPlane = 0.001f;
float farPlane = 50.0f;
glm::mat4 shadowProjection;

static void BindForWriting(GLenum face)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, depthFramebuffer);
	glViewport(0, 0, width, width);  // set the width/height of the shadow map!
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, face, shadowCubeMap, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
}

SandboxLayer::SandboxLayer() : Layer("Sandbox")
{
	screen = new HDRScreen(1280, 720);
	camera.InitPerspective(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
	camera.SetProjectionType(ProjectionType::Perspective);
	camera.SetPosition(glm::vec3(0.0f, 1.0f, 10.0f));

	shader = Shader::Create("Resources/Sandbox/Shader.glsl");
	boxModel = Model::Create("D:/Dev/ORiginProjects/Assets/cube.obj");
	boxTexture = TextureImporter::LoadTexture2D("D:/Dev/ORiginProjects/Assets/crate.jpg");

	std::shared_ptr<Material> material = Material::Create("mat", shader);
	plane = Model::Create("D:/Dev/ORiginProjects/TestGame/Assets/Models/checker_plane.obj", material);

	cube = Model::Create("Resources/Models/cube.obj");
	light = new TestLight(cube);

	lights.push_back(LightData(glm::vec3(0.0f, 4.0f, 10.0f), glm::vec3(1.0f)));


	depthShader = Shader::Create("Resources/Sandbox/PointLightDepthMap.glsl");

	glCreateTextures(GL_TEXTURE_2D, 1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &shadowCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeMap);
	for (uint32_t i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glCreateFramebuffers(1, &depthFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

SandboxLayer::~SandboxLayer()
{
	delete light;
	delete screen;
}

void SandboxLayer::OnUpdate(Timestep deltaTime)
{
	camera.OnUpdate(deltaTime);

	depthShader->Enable();
	shadowProjection = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProjection * glm::lookAt(lights[0].Position, lights[0].Position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProjection * glm::lookAt(lights[0].Position, lights[0].Position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProjection * glm::lookAt(lights[0].Position, lights[0].Position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	shadowTransforms.push_back(shadowProjection * glm::lookAt(lights[0].Position, lights[0].Position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	shadowTransforms.push_back(shadowProjection * glm::lookAt(lights[0].Position, lights[0].Position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProjection * glm::lookAt(lights[0].Position, lights[0].Position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

	glDisable(GL_CULL_FACE);
	for (int i = 0; i < 6; i++)
	{
		BindForWriting(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);

		depthShader->SetMatrix("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);

		depthShader->SetFloat("farPlane", farPlane);
		depthShader->SetVector("lightPosition", lights[0].Position);

		glm::mat4 boxTransform = glm::translate(glm::mat4(1.0f), boxPosition);
		depthShader->SetMatrix("modelTransform", shadowTransforms[i] * boxTransform);
		boxModel->Draw();

		glm::mat4 planeTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
		depthShader->SetMatrix("modelTransform", shadowTransforms[i] * planeTransform);
		plane->Draw();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	depthShader->Disable();

	{
		screen->OnBeginFramebuffer();
		shader->Enable();
		shader->SetMatrix("viewProjection", camera.GetViewProjection());
		shader->SetVector("cameraPosition", camera.GetPosition());

		for (int i = 0; i < lights.size(); i++)
		{
			shader->SetVector("lights[" + std::to_string(i) + "].Position", lights[i].Position);
			shader->SetVector("lights[" + std::to_string(i) + "].Color", lights[i].Color * 100.0f);
			shader->SetFloat("lights[" + std::to_string(i) + "].Size", lights[i].Size);
			shader->SetFloat("lights[" + std::to_string(i) + "].Specular", lights[i].Specular);
			shader->SetFloat("lights[" + std::to_string(i) + "].Ambient", lights[i].Ambient);
		}

		boxTexture->Bind(0);
		shader->SetInt("u_DiffTexture", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);
		shader->SetInt("u_DepthTexture", 1);

		shader->SetFloat("farPlane", farPlane);

		glm::mat4 boxTransform = glm::translate(glm::mat4(1.0f), boxPosition);
		shader->SetMatrix("modelTransform", boxTransform);
		boxModel->Draw();
		shader->Disable();

		plane->GetMaterial()->m_Shader->Enable();
		plane->GetMaterial()->m_Shader->SetInt("u_DiffTexture", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);
		plane->GetMaterial()->m_Shader->SetInt("u_DepthTexture", 1);
		plane->GetMaterial()->m_Shader->SetMatrix("viewProjection", camera.GetViewProjection());
		plane->GetMaterial()->m_Shader->SetVector("cameraPosition", camera.GetPosition());
		plane->GetMaterial()->m_Shader->SetFloat("farPlane", farPlane);

		for (int i = 0; i < lights.size(); i++)
		{
			plane->GetMaterial()->m_Shader->SetVector("lights[" + std::to_string(i) + "].Position", lights[i].Position);
			plane->GetMaterial()->m_Shader->SetVector("lights[" + std::to_string(i) + "].Color", lights[i].Color * 100.0f);
			plane->GetMaterial()->m_Shader->SetFloat("lights[" + std::to_string(i) + "].Size", lights[i].Size);
			plane->GetMaterial()->m_Shader->SetFloat("lights[" + std::to_string(i) + "].Specular", lights[i].Specular);
			plane->GetMaterial()->m_Shader->SetFloat("lights[" + std::to_string(i) + "].Ambient", lights[i].Ambient);
		}
		
		glm::mat4 planeTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
		plane->GetMaterial()->m_Shader->SetMatrix("modelTransform", planeTransform);
		plane->Draw();
		plane->GetMaterial()->m_Shader->Disable();

		for (int i = 0; i < lights.size(); i++)
		{
			light->position = lights[i].Position;
			light->color = lights[i].Color * 100.0f;
			light->OnRender(camera.GetViewProjection());
		}
		screen->OnEndFramebuffer();
	}

	screen->OnRender();
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
	screen->OnResize(e.GetWidth(), e.GetHeight());

	return false;
}

void SandboxLayer::OnGuiRender()
{
	ImGui::Begin("Window");
	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	glm::vec3 pos = camera.GetPosition();

	ImGui::Text("Camera Pos (%.3f, %.3f, %.3f)", pos.x, pos.y, pos.z);
	ImGui::DragFloat3("Box Position", glm::value_ptr(boxPosition), 0.1f);

	if (ImGui::Button("Add Light"))
	{
		lights.push_back(LightData(glm::vec3(0.0f), glm::vec3(1.0f)));
	}

	for (int i = 0; i < lights.size(); i++)
	{
		ImGui::PushID(i);
		ImGui::DragFloat3("Light Pos", glm::value_ptr(lights[i].Position), 0.1f);
		ImGui::ColorEdit3("Light Color", glm::value_ptr(lights[i].Color));
		ImGui::DragFloat("Light Size", &lights[i].Size, 0.01f, 0.1f, 10.0f);
		ImGui::DragFloat("Light Specular", &lights[i].Specular, 0.01f, 0.1f, 10.0f);
		ImGui::DragFloat("Light Ambient", &lights[i].Ambient, 0.01f, 0.1f, 10.0f);
		ImGui::DragFloat("Shadow Near", &nearPlane, 0.1f, 0.1f, 1000.0f);
		ImGui::DragFloat("Shadow Far", &farPlane, 0.1f, 0.1f, 1000.0f);
		ImGui::Separator();
		ImGui::PopID();
	}

	if (ImGui::Button("Refresh Shader"))
	{
		shader.reset();
		shader = Shader::Create("Resources/Sandbox/Shader.glsl");

		plane->GetMaterial()->m_Shader.reset();
		plane->GetMaterial()->m_Shader = Shader::Create("Resources/Sandbox/Shader.glsl");
	}
	
	ImGui::End();

	ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Exposure %.2f", screen->Exposure);
	float w = ImGui::GetContentRegionAvail().x;
	ImVec2 imageSize = ImVec2(w, w / 16.0f * 9.0f);

	ImTextureID screenImage = reinterpret_cast<ImTextureID>(screen->colorAttachment);
	ImGui::Image(screenImage, imageSize, ImVec2(0, 1), ImVec2(1, 0));

	ImGui::DragFloat("Exposure", &screen->Exposure, 0.01f, 0.1f, 1000.0f);
	ImGui::DragFloat("Gamma", &screen->Gamma, 0.01f, 0.0f, 1000.0f);

	if (ImGui::Button("Refresh Screen Shader"))
	{
		screen->screenShader.reset();
		screen->screenShader = Shader::Create("Resources/Sandbox/HDRscreen.glsl");
	}
	ImGui::End();
}
