#include "RuntimeLayer.h"

#include <Origin\EntryPoint.h>
#include <origin\Scripting\ScriptEngine.h>
#include <origin\Asset\AssetManager.h>

using namespace origin;

RuntimeLayer::RuntimeLayer()
{
	const auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
	if (commandLineArgs.Count > 1)
	{
		OpenProject(commandLineArgs[1]);
	}
}

RuntimeLayer::~RuntimeLayer()
{
}

void RuntimeLayer::OnAttach()
{
}

void RuntimeLayer::OnUpdate(Timestep ts)
{
	RenderCommand::Clear();
	RenderCommand::ClearColor(glm::vec4(0.0f));

	Renderer2D::ResetStats();
	Renderer3D::ResetStats();

	uint32_t Wx = Application::Get().GetWindow().GetWidth();
	uint32_t Wy = Application::Get().GetWindow().GetHeight();

	m_ActiveScene->OnUpdateRuntime(ts);

	m_ActiveScene->OnViewportResize(Wx, Wy);
	m_ActiveScene->OnShadowRender();

}

bool RuntimeLayer::OpenProject(const std::filesystem::path& path)
{
	if (Project::Load(path))
	{
		ScriptEngine::Init();

		AssetHandle handle = Project::GetActive()->GetConfig().StartScene;
		OpenScene(handle);

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
