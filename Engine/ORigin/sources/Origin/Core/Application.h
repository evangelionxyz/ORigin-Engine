// Copyright (c) Evangelion Manuhutu | ORigin Engine

#ifndef APPLICATION_H
#define APPLICATION_H

#include "Window.h"
#include "Input.h"
#include "AppEvent.h"
#include "Time.h"
#include "Layer.h"
#include "LayerStack.h"
#include "Origin/GUI/GuiLayer.h"
#include "Origin/Renderer/Renderer.h"
#include "Origin/Renderer/RendererAPI.h"
#include "Origin/Renderer/GraphicsContext.h"
#include "ConsoleManager.h"

#include "ProcessMonitor.h"

#include <mutex>

namespace origin {

struct ApplicationCommandLineArgs
{
    int Count = 0;
    char **Args = nullptr;

    const char *operator[](int index) const
    {
        OGN_CORE_ASSERT(index < Count, "Invalid index");
        return Args[index];
    }
};

struct ApplicationSpecification
{
    ApplicationCommandLineArgs CommandLineArgs;
    std::string Name = "ORigin Application";
    std::string IconPath = "Resources/icon.png";
    std::string WorkingDirectory;
    RendererAPI::API RenderAPI = RendererAPI::API::OpenGL;

    uint32_t Width = 1280, Height = 640;
    bool Maximize = false;
};

class Application
{
public:
    Application(ApplicationSpecification &spec);
    virtual ~Application();
    void OnEvent(Event &e);
    void Run();
    void Close();
    void PushLayer(Layer *layer);
    void PopLayer(Layer *layer);
    void PushOverlay(Layer *layer);
    void PopOverlay(Layer *layer);
    float GetTime();

    void SubmitToMainThread(const std::function<void()> &function);
    static Application &GetInstance() { return *s_Instance; }
    bool GetMinimized() const { return m_Minimized; }
    Window &GetWindow() const { return *m_Window.get(); }
    const ApplicationSpecification &GetSpecification() const { return m_Spec; }
    GuiLayer *GetGuiLayer() const { return m_GuiLayer; }
    bool SetVSync = false;

    ProcessMonitor &GetProcessMonitor() { return m_ProcessMonitor; }

private:
    ProcessMonitor m_ProcessMonitor;
    Scope<ConsoleManager> m_ConsoleManager;
    ApplicationSpecification m_Spec;
    LayerStack m_LayerStack;
    GuiLayer *m_GuiLayer = nullptr;
    Ref<Window> m_Window;
    static Application *s_Instance;
    bool OnWindowClose(WindowCloseEvent &e) const;
    bool OnWindowResize(WindowResizeEvent &e);
    void ExecuteMainThreadQueue();

    bool m_Minimized = false;
    float m_LastFrame = 0.0f;

    std::vector<std::function<void()>> m_MainThreadQueue;
    std::mutex m_MainThreadMutex;
};

Application *CreateApplication(ApplicationCommandLineArgs args);
}

#endif