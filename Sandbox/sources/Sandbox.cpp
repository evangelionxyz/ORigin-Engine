// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "Origin\EntryPoint.h"
#include "Sandbox.h"

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <imgui.h>

using namespace origin;

float rectangleVertices[] =
{
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,

	 1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f
};

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Color;
	glm::vec2 TexUV;
};

// Vertices for plane with texture
std::vector<Vertex> vertices =
{
	Vertex{glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
	Vertex{glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
	Vertex{glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
	Vertex{glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)}
};

// Indices for plane with texture
std::vector<GLuint> indices =
{
	0, 1, 2,
	0, 2, 3
};

struct RenderData
{
	uint32_t RectVAO, RectVBO;
	uint32_t FBO, FbTexture, RBO;
	uint32_t PPFBO, PPTexture, PPRBO;

	std::shared_ptr<Shader> defShader;
	std::shared_ptr<Shader> fboShader;
	float Gamma = 2.2f;
};
static RenderData s;

struct PlaneData
{
	std::shared_ptr<VertexArray> VAO;
	std::shared_ptr<VertexBuffer> VBO;

	std::shared_ptr<Texture> diffuseMap;
	std::shared_ptr<Texture> normMap;
	std::shared_ptr<Texture> displMap;
};

static PlaneData plane;

uint32_t width, height;

Sandbox::Sandbox() : Layer("Sandbox")
{
	camera = EditorCamera(30.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
	camera.EnableMovement(true);

	width = Application::Get().GetWindow().GetWidth();
	height = Application::Get().GetWindow().GetHeight();

	s.defShader = Shader::Create("Resources/Shaders/sandbox/default.glsl", false);
	s.fboShader = Shader::Create("Resources/Shaders/sandbox/framebuffer.glsl", false);

	// Take care of all the light related things
	glm::vec4 lightColor = glm::vec4(100.0f, 100.0f, 100.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);

	s.defShader->Enable();
	s.defShader->SetVector("lightColor", lightColor);
	s.defShader->SetVector("lightPos", lightColor);

	s.fboShader->Enable();
	s.fboShader->SetInt("screenTexture", 0);
	s.fboShader->SetFloat("gamma", s.Gamma);

	glGenVertexArrays(1, &s.RectVAO);
	glGenBuffers(1, &s.RectVBO);
	glBindVertexArray(s.RectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, s.RectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glGenFramebuffers(1, &s.FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, s.FBO);

	glGenTextures(1, &s.FbTexture);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, s.FbTexture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGB16F, width, height, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, s.FbTexture, 0);

	glGenRenderbuffers(1, &s.RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, s.RBO);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, s.RBO);

	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer error: " << fboStatus << std::endl;

	glGenFramebuffers(1, &s.PPFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, s.PPFBO);

	glGenTextures(1, &s.PPTexture);
	glBindTexture(GL_TEXTURE_2D, s.PPTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s.PPTexture, 0);

	// Error checking framebuffer
	fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Post-Processing Framebuffer error: " << fboStatus << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	plane.VAO = VertexArray::Create();
	plane.VBO = VertexBuffer::Create(vertices.size() * sizeof(Vertex));

	plane.VBO->SetLayout
	({
		{ ShaderDataType::Float3, "aPosition" },
		{ ShaderDataType::Float3, "aNormal"		},
		{	ShaderDataType::Float3,	"aColor"		},
		{	ShaderDataType::Float2,	"aUVS"			}
	});

	plane.VAO->AddVertexBuffer(plane.VBO);

	std::shared_ptr<IndexBuffer> indexBuffer = IndexBuffer::Create(indices);
	plane.VAO->SetIndexBuffer(indexBuffer);

	plane.diffuseMap = Texture2D::Create("Resources/Sandbox/diffuse.png");
	plane.normMap = Texture2D::Create("Resources/Sandbox/normal.png");
	plane.displMap = Texture2D::Create("Resources/Sandbox/displacement.png");

	plane.diffuseMap->Bind(0);
	s.defShader->Enable();
	s.defShader->SetInt("diffuse0", 0);
}

Sandbox::~Sandbox()
{
}

void Sandbox::OnUpdate(Timestep ts)
{
	width = Application::Get().GetWindow().GetWidth();
	height = Application::Get().GetWindow().GetHeight();
	

	// Bind the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, s.FBO);
	glClearColor(pow(0.07f, s.Gamma), pow(0.13f, s.Gamma), pow(0.17f, s.Gamma), 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	camera.OnUpdate(ts);
	camera.SetViewportSize((float)width, (float)height);

	Renderer2D::ResetStats();
	Renderer::BeginScene(camera);

	s.defShader->Enable();
	plane.normMap->Bind(1);
	plane.displMap->Bind(2);

	s.defShader->SetInt("normal0", 1);
	s.defShader->SetInt("displacement0", 2);

	s.defShader->SetVector("camPos", camera.GetPosition());
	s.defShader->SetMatrix("camMatrix", camera.GetViewMatrix());

	glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f));
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

	s.defShader->SetMatrix("translation", translation);
	s.defShader->SetMatrix("rotation", rotation);
	s.defShader->SetMatrix("scale", scale);
	s.defShader->SetMatrix("model", glm::mat4(1.0f));

	RenderCommand::DrawIndexed(plane.VAO);

	Renderer::EndScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

	ImGui::End();
}
