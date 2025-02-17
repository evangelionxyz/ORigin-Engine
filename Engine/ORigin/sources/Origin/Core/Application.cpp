// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Application.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/Audio/AudioEngine.h"
#include "Origin/Audio/FmodAudio.h"
#include "Origin/Scripting/ScriptEngine.h"

#include "ConsoleManager.h"
#include "EmbeddedImages.h"
#include <stb_image.h>

namespace origin {

    Application* Application::s_Instance = nullptr;
    Application::Application(ApplicationSpecification& spec)
        : m_Spec(spec)
    {
        OGN_PROFILER_FUNCTION();

        OGN_CORE_ASSERT(!s_Instance, "Application already opened!");
        s_Instance = this;

        if (!m_Spec.WorkingDirectory.empty())
        {
            std::filesystem::current_path(m_Spec.WorkingDirectory);
        }

        RendererAPI::SetAPI(m_Spec.RenderAPI);

        m_ConsoleManager = CreateScope<ConsoleManager>();

        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::OpenGL:
            spec.Name.insert(spec.Name.size(), " - OpenGL");
            break;
        case RendererAPI::API::Vulkan:
            spec.Name.insert(spec.Name.size(), " - Vulkan");
            break;
        }

        m_Window = CreateRef<Window>(spec.Name.c_str(), spec.Width, spec.Height, spec.Maximize);
        m_Window->SetIcon(logo_black_data, logo_black_width, logo_black_height);
        
        m_Window->SetEventCallback(OGN_BIND_EVENT_FN(Application::OnEvent));
        Input::Init(m_Window->GetNativeWindow());

        m_GuiLayer = new GuiLayer(m_Window);
        m_GuiLayer->OnAttach();

        AudioEngine::Init();
        FmodAudio::Init();
        Renderer::Init();

        m_Window->Show();
    }

    Application::~Application()
    {
        m_LayerStack.Shutdown();

        Renderer::Shutdown();
        FmodAudio::Shutdown();
        AudioEngine::Shutdown();
    }

    void Application::Run()
    {
        while (m_Window->IsLooping())
        {
            OGN_PROFILER_BEGIN_FRAME("MainThread");
            m_Window->UpdateEvents();
            
            f32 time = static_cast<float>(glfwGetTime());
            Timestep ts = time - m_LastFrame;
            m_LastFrame = time;

            Input::Update();

            AssetImporter::SyncToMainThread(ts);
            ExecuteMainThreadQueue();

            if (!m_Minimized)
            {
                for (Layer *layer : m_LayerStack)
                    layer->OnUpdate(ts);
            }

            if (m_GuiLayer)
            {
                m_GuiLayer->Begin();
                for (Layer *layer : m_LayerStack)
                    layer->OnGuiRender();
                m_GuiLayer->End();
            }

            m_Window->SwapBuffers();
        }

        if (m_GuiLayer)
        {
            m_GuiLayer->OnDetach();
            delete m_GuiLayer;
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

    void Application::PopLayer(Layer* layer)
    {
        layer->OnDetach();
        m_LayerStack.PopLayer(layer);
    }

    void Application::PopOverlay(Layer* layer)
    {
        layer->OnDetach();
        m_LayerStack.PopOverlay(layer);
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