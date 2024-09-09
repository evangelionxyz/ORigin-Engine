// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Application.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/Audio/AudioEngine.h"
#include "Origin/Scripting/ScriptEngine.h"
#include "Origin/Physics/PhysicsEngine.h"
#include "ConsoleManager.h"

#include <imgui.h>
#include <stb_image.h>

#ifdef OGN_PLATFORM_WINDOWS
    #include "Platform/Win32/Win32Window.h"
    #include "Platform/DX11/DX11Context.h"
#endif

#define OGN_OPENGL_API

namespace origin {

    Application* Application::s_Instance = nullptr;

    Application::Application(ApplicationSpecification& spec)
        : m_Spec(spec)
    {
        OGN_PROFILER_FUNCTION();

        OGN_CORE_ASSERT(!s_Instance, "Application already opened!");
        s_Instance = this;

        if (!m_Spec.WorkingDirectory.empty())
            std::filesystem::current_path(m_Spec.WorkingDirectory);
#ifdef OGN_OPENGL_API
        RendererAPI::SetAPI(RendererAPI::API::OpenGL);
#elif OGN_VULKAN_API
        RendererAPI::SetAPI(RendererAPI::API::Vulkan);
#elif OGN_DX11_API
        RendererAPI::SetAPI(RendererAPI::API::DX11);
#endif
        Window::GLFWInit();
        m_ConsoleManager = std::make_unique<ConsoleManager>();

        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::DX11:
            spec.Name.insert(spec.Name.size(), " <DX11>");
            break;
        case RendererAPI::API::OpenGL:
            spec.Name.insert(spec.Name.size(), " <OpenGL>");
            break;
        case RendererAPI::API::Vulkan:
            spec.Name.insert(spec.Name.size(), " <Vulkan>");
            break;
        }

        m_Window = Window::Create(spec.Name.c_str(), spec.Width, spec.Height, spec.Maximize);
        m_Window->SetIcon(spec.IconPath.c_str());
        m_Window->SetEventCallback(OGN_BIND_EVENT_FN(Application::OnEvent));
        m_InputHandle.Init(m_Window->GetNativeWindow());

        m_GuiLayer = new GuiLayer(m_Window);
        PushOverlay(m_GuiLayer);

        PhysicsEngine::Init();
        AudioEngine::Init();
        Renderer::Init();
    }

    Application::~Application()
    {
        m_Window->DestroyWindow();
        m_LayerStack.Shutdown();

        Renderer::Shutdown();
        AudioEngine::Shutdown();
        PhysicsEngine::Shutdown();

        Window::GLFWShutdown();
    }

    void Application::Run()
    {
        while (m_Window->IsLooping())
        {
            OGN_PROFILER_BEGIN_FRAME("MainThread");
            m_Window->UpdateEvents();
            
            float time = static_cast<float>(glfwGetTime());
            Timestep ts = time - m_LastFrame;
            m_LastFrame = time;

            m_InputHandle.Update();

            AssetImporter::SyncToMainThread(ts);
            ExecuteMainThreadQueue();

            if (!m_Minimized)
            {
                for (Layer *layer : m_LayerStack)
                {
                    layer->OnUpdate(ts);
                }
            }

            if (m_GuiLayer)
            {
                m_GuiLayer->Begin();
                for (Layer *layer : m_LayerStack)
                {
                    layer->OnGuiRender();
                }

                m_GuiLayer->End();
            }

            m_Window->OnUpdate();
        }
    }

    void Application::Close()
    { 
        m_Window->CloseWindow();
    }

    void Application::PushOverlay(Layer* layer)
    {
        m_LayerStack.PushOverlay(layer);
        layer->OnAttach();
    }

    float Application::GetTime()
    {
        return static_cast<float>(glfwGetTime());
    }

    void Application::PushLayer(Layer *layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(OGN_BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(OGN_BIND_EVENT_FN(Application::OnWindowResize));

        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
        {
            if (e.Handled)
                break;
            (*it)->OnEvent(e);
        }
    }

    void Application::SubmitToMainThread(const std::function<void()>& function)
    {
        std::scoped_lock<std::mutex> lock(m_MainThreadMutex);
        m_MainThreadQueue.emplace_back(function);
    }

    bool Application::OnWindowClose(WindowCloseEvent& e) const
    {
        return !m_Window->IsLooping();
    }

    bool Application::OnWindowResize(WindowResizeEvent& e)
    {
        OGN_PROFILER_FUNCTION();

        m_Minimized = e.GetWidth() == 0 || e.GetHeight() == 0;
        return false;
    }

    void Application::ExecuteMainThreadQueue()
    {
        for (auto &func : m_MainThreadQueue)
            func();

        m_MainThreadQueue.clear();
    }

}