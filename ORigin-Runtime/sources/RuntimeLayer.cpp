#include "RuntimeLayer.h"

#include <Origin\EntryPoint.h>
#include <origin\Asset\AssetManager.h>
#include <Origin\Asset\AssetImporter.h>
#include <origin\Scripting\ScriptEngine.h>
#include <chrono>

using namespace origin;

void RuntimeLayer::OnAttach()
{
	Application::Get().GetWindow().SetFullscreen(true);

	
	OnLoadingScreen(5);

	OpenProject("D:/Dev/ORiginProjects/TestGame/TestGame.oxproj");
	IsLoaded = true;
}

void RuntimeLayer::OnUpdate(Timestep ts)
{
	RenderCommand::Clear();
	m_ActiveScene->OnUpdateRuntime(ts);
}

void RuntimeLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<FramebufferResizeEvent>(OGN_BIND_EVENT_FN(RuntimeLayer::OnResize));
	dispatcher.Dispatch<KeyPressedEvent>(OGN_BIND_EVENT_FN(RuntimeLayer::OnKeyPressed));
}

void RuntimeLayer::OnLoadingScreen(uint32_t waitingTime)
{
	m_LoadingScreenImage = TextureImporter::LoadTexture2D("Resources/UITextures/runtime_loading_screen.png");

	uint32_t elapsedTime = 0;
	auto start = std::chrono::high_resolution_clock::now();
	while (elapsedTime < waitingTime)
	{
		RenderCommand::Clear();
		RenderCommand::ClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		float width = Application::Get().GetWindow().GetWidth();
		float height = Application::Get().GetWindow().GetHeight();
		RenderCommand::SetViewport(0, 0, width, height);
		float orthoSize = 1.0f;
		glm::mat4 projection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, 0.0f, 10.0f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		Renderer2D::Begin(projection * view);
		glm::mat4 modelScale = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f));
		Renderer2D::DrawQuad(modelScale, m_LoadingScreenImage);
		Renderer2D::End();
		Application::Get().GetWindow().OnUpdate();

		auto end = std::chrono::high_resolution_clock::now();
		elapsedTime = (uint32_t)std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
	}

	m_LoadingScreenImage.reset();
}

bool RuntimeLayer::OpenProject(const std::filesystem::path& path)
{
	if (Project::Load(path))
	{
		ScriptEngine::Init();

		AssetHandle handle = Project::GetActive()->GetConfig().StartScene;
		OpenScene(handle);

		float width = static_cast<float>(Application::Get().GetWindow().GetWidth());
		float height = static_cast<float>(Application::Get().GetWindow().GetHeight());
		RenderCommand::SetViewport(0, 0, width, height);

		m_ActiveScene->OnViewportResize(width, height);
		m_ActiveScene->OnRuntimeStart();

		return true;
	}
	
	return false;
}

void RuntimeLayer::OpenScene(AssetHandle handle)
{
	if (!AssetManager::IsAssetHandleValid(handle) || handle == 0)
		return;

	m_ActiveScene = AssetManager::GetAsset<Scene>(handle);
	m_ActiveScene->OnRuntimeStart();
}

bool RuntimeLayer::OnResize(FramebufferResizeEvent& e)
{
	if (!IsLoaded)
		return false;

	m_ActiveScene->OnViewportResize(e.GetWidth(), e.GetHeight());
	RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
	return false;
}

bool RuntimeLayer::OnKeyPressed(origin::KeyPressedEvent &e)
{
	if (e.GetKeyCode() == Key::Escape)
		Application::Get().Close();

	return false;
}

