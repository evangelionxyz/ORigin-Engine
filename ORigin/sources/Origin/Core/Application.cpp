// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Application.h"
#include "Origin\Asset\AssetImporter.h"
#include "Origin\Audio\AudioEngine.h"
#include "Origin\Physics\Physics.h"
#include "Origin\Scripting\ScriptEngine.h"

#include <imgui.h>
#include <stb_image.h>

#pragma warning(disable : OGN_DISABLED_WARNINGS)

namespace origin {

	CommandManager<glm::vec4> Application::Vec4CmdManager;
	CommandManager<glm::vec3> Application::Vec3CmdManager;
	CommandManager<glm::vec2> Application::Vec2CmdManager;
	CommandManager<int> Application::IntegerCmdManager;
	CommandManager<float> Application::FloatCmdManager;

	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& spec)
		: m_Spec(spec)
	{
		OGN_PROFILER_FUNCTION();

		OGN_CORE_ASSERT(!s_Instance, "Application already opened!");
		s_Instance = this;

		if (!m_Spec.WorkingDirectory.empty())
			std::filesystem::current_path(m_Spec.WorkingDirectory);

		Window::GLFWInit();

		m_Window = Window::Create(spec.Name.c_str(),
			spec.Width, spec.Height, spec.Maximize);

		m_Window->SetIcon(spec.IconPath.c_str());
		m_Window->SetEventCallback(OGN_BIND_EVENT_FN(Application::OnEvent));
		m_MainInputHandle = std::make_unique<Input>();

		if (!spec.Runtime)
		{
			m_GuiLayer = new GuiLayer((void*)m_Window->GetNativeWindow());
			PushOverlay(m_GuiLayer);
		}

		Renderer::Init();
		Physics::Init();
		AudioEngine::Init();

		StartThreads();
	}

	Application::~Application()
	{
		Window::GLFWShutdown();
		AudioEngine::Shutdown();
		Physics::Shutdown();
		Renderer::Shutdown();

		s_Instance = nullptr;
	}

	void Application::Run()
	{
		m_Window->SetVSync(false);

		while (m_Window->IsLooping())
		{
			OGN_PROFILER_BEGIN_FRAME("MainThread");

			float time = static_cast<float>(glfwGetTime());
			Timestep timestep = time - m_LastFrame;
			m_LastFrame = time;

			ExecuteMainThreadQueue();

			if (!m_Minimized) 
			{
				for (Layer *layer : m_LayerStack)
				{
					layer->OnUpdate(timestep);
				}
			}

			if (!m_Spec.Runtime)
			{
				m_GuiLayer->Begin();
				for (Layer *layer : m_LayerStack)
				{
					layer->OnGuiRender();
				}
				m_GuiLayer->SetDisplaySize((float)m_Window->GetWidth(), (float)m_Window->GetHeight());
				m_GuiLayer->End();
			}
			
			m_Window->OnUpdate();
		}
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::StartThreads()
	{
		AssetImporter::StartThread();
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

	bool Application::OnWindowClose(WindowCloseEvent& e)
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