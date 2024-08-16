// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Origin/EntryPoint.h"
#include "Origin/Core/KeyCodes.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/GUI/UI.h"
#include "SandboxLayer.h"

#include "Model.h"
#include <iostream>
#include <imgui.h>

using namespace origin;

const char *vertexShaderSource = R"(
	#version 450 core
	layout (location = 0) in vec3 position; 
	layout (location = 1) in vec3 normal;
	layout (location = 2) in vec2 uv;
	layout (location = 3) in vec4 boneIds;
	layout (location = 4) in vec4 boneWeights;

	out vec2 tex_cord;
	out vec3 v_normal;
	out vec3 v_pos;

	uniform mat4 boneTransforms[50];
	uniform mat4 viewProjection;
	uniform mat4 transform;

	void main()
	{
		mat4 boneTransform  =  mat4(0.0);
		boneTransform  +=    boneTransforms[int(boneIds.x)] * boneWeights.x;
		boneTransform  +=    boneTransforms[int(boneIds.y)] * boneWeights.y;
		boneTransform  +=    boneTransforms[int(boneIds.z)] * boneWeights.z;
		boneTransform  +=    boneTransforms[int(boneIds.w)] * boneWeights.w;
		vec4 pos = boneTransform * vec4(position, 1.0);
		gl_Position = viewProjection * transform * pos;
		v_pos = vec3(transform * boneTransform * pos);
		tex_cord = uv;
		v_normal = mat3(transpose(inverse(transform * boneTransform))) * normal;
		v_normal = normalize(v_normal);
	}

	)";
const char *fragmentShaderSource = R"(
	#version 450 core

	in vec2 tex_cord;
	in vec3 v_normal;
	in vec3 v_pos;
	out vec4 color;

	uniform sampler2D diff_texture;

	vec3 lightPos = vec3(0.2, 1.0, -3.0);
	
	void main()
	{
		vec3 lightDir = normalize(lightPos - v_pos);
		float diff = max(dot(v_normal, lightDir), 0.2);
		vec3 dCol = diff * texture(diff_texture, tex_cord).rgb; 
		color = vec4(dCol, 1);
	}
	)";


uint32_t createVertexArray(std::vector<Vertex> &vertices, std::vector<uint32_t> indices)
{
	uint32_t
		vao = 0,
		vbo = 0,
		ebo = 0;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, uv));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, boneIds));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, boneWeights));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
	return vao;
}

std::shared_ptr<Texture2D> texture;
TestAnimation animation;
TestBone skeleton;
uint32_t vao;
uint32_t shader = 0;
uint32_t boneCount = 0;
uint32_t viewProjectionLoc;
uint32_t modelTransformLoc;
uint32_t boneTransformsLoc;
glm::mat4 globalInverseTransform(1.0f);
glm::mat4 identity(1.0f);
std::vector<glm::mat4> currentPose = {};
std::vector<Vertex> vertices = {};
std::vector<uint32_t> indices = {};
double elapsedTime = 0.0;

SandboxLayer::SandboxLayer() : Layer("Sandbox")
{
}

void SandboxLayer::OnAttach()
{

	MeshRenderer::Init();

	m_Camera.InitPerspective(45.0f, 1.776f, 1.0f, 300.0f);
	m_Camera.SetPosition({ 0.0f, 5.0f, 10.0f });

	m_SceneCamera.InitPerspective(45.0f, 1.776f, 1.0f, 100.0f);
	m_CamTC.WorldTranslation = { 0.0f, 1.0f, 0.0f };
	nPlane = m_SceneCamera.GetNear();
	fPlane = m_SceneCamera.GetFar();
	FOV = m_SceneCamera.GetFOV();

	m_Camera.SetAllowedMove(true);

	//load model file
	Assimp::Importer importer;
	const char *filePath = "SandboxGame/Assets/Models/character.dae";
	const aiScene *scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		OGN_CORE_ASSERT(false, importer.GetErrorString());
		exit(EXIT_FAILURE);
	}
	aiMesh *mesh = scene->mMeshes[0];

	texture = Texture2D::Create("SandboxGame/Assets/Models/diffuse.png");

	globalInverseTransform = AssimpToGlmMatrix(scene->mRootNode->mTransformation);
	//globalInverseTransform = glm::inverse(globalInverseTransform);

	loadModel(scene, mesh, vertices, indices, skeleton, boneCount);
	LoadAnimation(scene, animation);
	currentPose.resize(boneCount, identity);
	vao = createVertexArray(vertices, indices);

	shader = createShader(vertexShaderSource, fragmentShaderSource);
	viewProjectionLoc = glGetUniformLocation(shader, "viewProjection");
	modelTransformLoc = glGetUniformLocation(shader, "transform");
	boneTransformsLoc = glGetUniformLocation(shader, "boneTransforms");



	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.3f, 0.4f, 1.0f);
	
}

SandboxLayer::~SandboxLayer()
{
}

void SandboxLayer::OnUpdate(Timestep ts)
{
	int posX, posY;
	int width, height;
	glfwGetWindowPos((GLFWwindow *)Input::Get().GetWindow(), &posX, &posY);
	glfwGetWindowSize((GLFWwindow *)Input::Get().GetWindow(), &width, &height);
	glm::vec2 screenMin = { posX, posY };
	glm::vec2 screenMax = { width, height };
	m_Camera.OnUpdate(ts, screenMin, screenMax);
	Frustum frustum(m_Camera.GetViewProjection());

	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (timer <= 0.0f)
	{
		deltaTime = ts;
		framerate = 1.0f / deltaTime;
		response = 1000.0f / framerate;
		timer = updateRate;
	}
	timer -= ts;

	elapsedTime = glfwGetTime(); // Get time in seconds

	// RENDER HERE
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode ? GL_LINE : GL_FILL);
	glCullFace(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shader);
	glActiveTexture(GL_TEXTURE0);
	texture->Bind(0);

	GetPose(animation, skeleton, (float)elapsedTime, currentPose, identity, globalInverseTransform);
	for (int x = -5; x < 5; ++x)
	{
		for (int z = -5; z < 5; ++z)
		{
			glUniformMatrix4fv(viewProjectionLoc, 1, GL_FALSE, glm::value_ptr(m_Camera.GetViewProjection()));
			glUniformMatrix4fv(boneTransformsLoc, boneCount, GL_FALSE, glm::value_ptr(currentPose[0]));

			glm::mat4 modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(x + x * 1.5f, 0.0f, z + z * 1.5f))
				* glm::scale(glm::mat4(1.0f), { 0.5f, 0.5f, 0.5f });
			glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(modelTransform));

			glBindVertexArray(vao);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		}
	}

	DrawGrid();

}

void SandboxLayer::OnEvent(Event &e)
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
	ImGui::Separator();

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

	if (ImGui::Begin("Debug Info"))
	{
		ImGui::Text("Shader Program: %u", shader);
		ImGui::Text("Bone Count: %d", boneCount);

		// Display currentPose matrices
		for (size_t i = 0; i < currentPose.size(); ++i)
		{
			ImGui::Text("Bone %zu Transform", i);
			ImGui::InputFloat4("Transform", glm::value_ptr(currentPose[i]), "%.2f");
		}

		ImGui::Text("Elapsed Time: %.2f", elapsedTime);
		ImGui::Text("Ticks Per Second: %.2f", animation.ticksPerSecond);
		ImGui::Text("Animation Duration: %.2f", animation.duration);

		ImGui::End();
	}

}

bool SandboxLayer::OnWindowResize(FramebufferResizeEvent &e)
{
	RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
	m_Camera.SetViewportSize(e.GetWidth(), e.GetHeight());
	return false;
}

bool SandboxLayer::OnKeyPressedEvent(KeyPressedEvent &e)
{
	if (e.GetKeyCode() == Key::D1)
	{
		MeshRenderer::GetShader()->Reload();
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

	for (const auto &edge : m_Frustum.GetEdges())
	{
		Renderer2D::DrawLine(edge.first, edge.second, { 1.0f, 0.0f, 0.0f, 1.0f });
	}

	Renderer2D::End();
}