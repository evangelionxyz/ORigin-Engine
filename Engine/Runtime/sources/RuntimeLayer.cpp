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
        RenderCommand::Clear();
        m_ActiveScene->GetUIRenderer()->RenderFramebuffer();
        RenderCommand::SetViewport(X, Y, static_cast<u32>(Width), static_cast<u32>(Height));

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
        Application::GetInstance().Close();
    }

    if (e.GetKeyCode() == Key::F11)
    {
        Application::GetInstance().GetWindow().ToggleFullScreen();
    }

    return false;
}

void RuntimeLayer::Resize(u32 width, u32 height)
{
    if (m_ActiveScene)
    {
        Entity cam = m_ActiveScene->GetPrimaryCameraEntity();
        if (cam.IsValid())
        {
            auto &cc = cam.GetComponent<CameraComponent>().Camera;
            f32 desiredRatio = cc.GetAspectRatio();
            f32 aspectRatio = (f32)width / (f32)height;

            // max out the height
            if (aspectRatio > desiredRatio)
            {
                Height = (f32)height;
                Width = Height * desiredRatio;
                Y = 0.0f;
                X = (width - Width) / 2.0f;
            }
            else
            {
                // max out the width
                Width = (f32)width;
                Height = Width / desiredRatio;
                X = 0.0f;
                Y = (height - Height) / 2.0f;;
            }
        }

        m_ActiveScene->OnViewportResize(width, height);
    }
}

void RuntimeLayer::OnLoadingScreen(f32 endTime)
{
    AudioListener listener(glm::vec3(0.0f), glm::vec3(0.0f), { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f });
    m_LoadingSound = AudioSource::Create();
    m_LoadingSound->LoadSource("loading_screen", "Resources/Sounds/loading_screen.wav");
    m_LoadingSound->SetVolume(5.0f);
    m_LoadingSound->SetLoop(true);

    m_ScreenTexture = TextureImporter::LoadTexture2D("Resources/UITextures/runtime_loading_screen.png");
    auto &app = Application::GetInstance();
    app.GetWindow().ToggleFullScreen();
    f32 elapsedTime = 0.0f;
    auto start = std::chrono::high_resolution_clock::now();
    f32 lastFrame = 0.0f;

    bool isLoaded = false;
    while (elapsedTime < endTime && app.GetWindow().IsLooping())
    {
        if (!isLoaded)
        {
            const auto &commandLineArgs = Application::GetInstance().GetSpecification().CommandLineArgs;
            if (commandLineArgs.Count > 1)
                isLoaded = OpenProject(commandLineArgs[1]);
            else
                isLoaded = OpenProject("Game/Game.oxproj");
        }

        f32 time = app.GetTime();
        Timestep timestep = time - lastFrame;
        lastFrame = time;

        f32 fadeTime = 0.0f;
        if (elapsedTime <= endTime)
        {
            f32 t = elapsedTime / endTime;
            //fadeTime = t * t * (3.0f - 2.0f * t); // Smoothstep function
            fadeTime = sin(t * 3.14159f); // Using sine function to create an ease-in and ease-out effect
        }

        m_LoadingSound->SetVolume(fadeTime * 5.0f);
        RenderCommand::Clear();
        RenderCommand::ClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
        u32 ww = app.GetWindow().GetWidth();
        u32 wh = app.GetWindow().GetHeight();
        f32 desiredAspectRatio = 16.0f / 9.0f;
        f32 aspectRatio = (f32)ww / (f32)wh;
        Width = (f32)ww;
        Height = (f32)wh;
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
        Renderer2D::Begin();
        Renderer2D::DrawQuad(glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)), m_ScreenTexture, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, fadeTime });
        Renderer2D::End();
        m_LoadingSound->Play();
        app.GetWindow().SwapBuffers();

        elapsedTime += timestep;
    }

    m_LoadingSound->Stop();
    m_ScreenTexture.reset();

    if (isLoaded)
    {
        auto &window = Application::GetInstance().GetWindow();
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

