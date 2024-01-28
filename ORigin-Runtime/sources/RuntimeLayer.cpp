#include "RuntimeLayer.h"

#include <Origin\EntryPoint.h>
#include <origin\Scripting\ScriptEngine.h>
#include <origin\Asset\AssetManager.h>

using namespace origin;

void RuntimeLayer::OnAttach()
{
	OpenProject("D:/Dev/ORiginProjects/TestGame/TestGame.oxproj");
	RenderCommand::ClearColor(glm::vec4(0.0f));
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
}

bool RuntimeLayer::OpenProject(const std::filesystem::path& path)
{
	if (Project::Load(path))
	{
		ScriptEngine::Init();

		AssetHandle handle = Project::GetActive()->GetConfig().StartScene;
		OpenScene(handle);

		float width = Application::Get().GetWindow().GetWidth();
		float height = Application::Get().GetWindow().GetHeight();
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
	m_ActiveScene->OnViewportResize(e.GetWidth(), e.GetHeight());
	RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
	return false;
}

