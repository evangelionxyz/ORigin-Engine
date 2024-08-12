#include "RuntimeLayer.h"
#include <Origin/EntryPoint.h>
#include <Origin/Asset/AssetManager.h>
#include <Origin/Asset/AssetImporter.h>
#include <Origin/Scripting/ScriptEngine.h>
using namespace origin;

RuntimeLayer::RuntimeLayer() : Layer("Runtime Layer") {}
RuntimeLayer::~RuntimeLayer() { if(m_ActiveScene) ScriptEngine::Shutdown(); }

void RuntimeLayer::OnAttach()
{
	OnLoadingScreen(10.0f);
}

void RuntimeLayer::OnUpdate(Timestep ts)
{
	if (m_ActiveScene)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		m_ActiveScene->GetUIRenderer()->RenderFramebuffer();
		RenderCommand::SetViewport(X, Y, Width, Height);

		m_ActiveScene->OnUpdateRuntime(ts);
		m_ActiveScene->GetUIRenderer()->Render();
	}
}

void RuntimeLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(OGN_BIND_EVENT_FN(RuntimeLayer::OnWindowResized));
	dispatcher.Dispatch<KeyPressedEvent>(OGN_BIND_EVENT_FN(RuntimeLayer::OnKeyPressed));
}

bool RuntimeLayer::OnWindowResized(WindowResizeEvent &e)
{
	Resize(e.GetWidth(), e.GetHeight());
	return false;
}

bool RuntimeLayer::OnKeyPressed(origin::KeyPressedEvent &e)
{
	if (e.GetKeyCode() == Key::Escape)
	{
		Application::Instance().Close();
	}

	if (e.GetKeyCode() == Key::F11)
	{
		Application::Instance().GetWindow().ToggleFullScreen();
	}

	return false;
}

void RuntimeLayer::Resize(uint32_t width, uint32_t height)
{
	if (m_ActiveScene)
	{
		Entity cam = m_ActiveScene->GetPrimaryCameraEntity();
		if (cam.IsValid())
		{
			auto &cc = cam.GetComponent<CameraComponent>().Camera;
			float desiredRatio = cc.GetAspectRatio();
			float aspectRatio = (float)width / (float)height;

			// max out the height
			if (aspectRatio > desiredRatio)
			{
				Height = (float)height;
				Width = Height * desiredRatio;
				Y = 0.0f;
				X = (width - Width) / 2.0f;
			}
			else
			{
				// max out the width
				Width = (float)width;
				Height = Width / desiredRatio;
				X = 0.0f;
				Y = (height - Height) / 2.0f;;
			}
		}

		m_ActiveScene->OnViewportResize(width, height);
	}
}

void RuntimeLayer::OnLoadingScreen(float endTime)
{
	AudioEngine::SetListener(glm::vec3(0.0f), glm::vec3(0.0f), { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f });
	m_LoadingSound = AudioSource::Create();
	m_LoadingSound->LoadSource("loading_screen", "Resources/Sounds/loading_screen.wav");
	m_LoadingSound->SetVolume(5.0f);
	m_LoadingSound->SetLoop(true);

	m_ScreenTexture = TextureImporter::LoadTexture2D("Resources/UITextures/runtime_loading_screen.png");
	auto &app = Application::Instance();
	app.GetWindow().ToggleFullScreen();
	float elapsedTime = 0.0f;
	auto start = std::chrono::high_resolution_clock::now();
	float lastFrame = 0.0f;

	bool isLoaded = false;
	while (elapsedTime < endTime && app.GetWindow().IsLooping())
	{
		if (!isLoaded)
		{
			const auto &commandLineArgs = Application::Instance().GetSpecification().CommandLineArgs;
			if (commandLineArgs.Count > 1)
				isLoaded = OpenProject(commandLineArgs[1]);
			else
				isLoaded = OpenProject("Game/Game.oxproj");
		}

		float time = app.GetTime();
		Timestep timestep = time - lastFrame;
		lastFrame = time;

		float fadeTime = 0.0f;
		if (elapsedTime <= endTime)
		{
			float t = elapsedTime / endTime;
			//fadeTime = t * t * (3.0f - 2.0f * t); // Smoothstep function
			fadeTime = sin(t * 3.14159f); // Using sine function to create an ease-in and ease-out effect
		}

		m_LoadingSound->SetVolume(fadeTime * 5.0f);
		RenderCommand::Clear();
		RenderCommand::ClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		uint32_t ww = app.GetWindow().GetWidth();
		uint32_t wh = app.GetWindow().GetHeight();
		float desiredAspectRatio = 16.0f / 9.0f;
		float aspectRatio = (float)ww / (float)wh;
		Width = (float)ww;
		Height = (float)wh;
		X = 0;
		Y = 0;

		if (aspectRatio > desiredAspectRatio)
		{
			Width = wh * desiredAspectRatio;
			X = (ww - Width) / 2.0f;
		}
		else
		{
			Height = ww / desiredAspectRatio;
			Y = (wh - Height) / 2.0f;
		}

		RenderCommand::SetViewport(X, Y, Width, Height);
		Renderer2D::Begin(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		Renderer2D::DrawQuad(glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)), m_ScreenTexture, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, fadeTime });
		Renderer2D::End();
		m_LoadingSound->Play();
		app.GetWindow().OnUpdate();

		elapsedTime += timestep;
	}

	m_LoadingSound->Stop();
	m_ScreenTexture.reset();

	if (isLoaded)
	{
		auto &window = Application::Instance().GetWindow();
		m_ActiveScene->OnRuntimeStart();
		Resize(window.GetWidth(), window.GetHeight());
	}
}

bool RuntimeLayer::OpenProject(const std::filesystem::path &path)
{
	if (Project::Load(path))
	{
		ScriptEngine::Init();

		AssetHandle handle = Project::GetActive()->GetConfig().StartScene;
		if (!AssetManager::IsAssetHandleValid(handle) || handle == 0)
		{
			OGN_CORE_ERROR("[Runtime OpenProject] Invalid Scene");
			return false;
		}

		m_ActiveScene = AssetManager::GetAsset<Scene>(handle);

		if (!m_ActiveScene)
		{
			OGN_CORE_ERROR("[Runtime OpenProject] Invalid Scene with handle {}", handle);
			return false;
		}

		auto metadata = Project::GetActive()->GetEditorAssetManager()->GetMetadata(handle);
		std::string name = metadata.Filepath.stem().string();
		m_ActiveScene->SetName(name);

		return true;
	}
	return false;
}

