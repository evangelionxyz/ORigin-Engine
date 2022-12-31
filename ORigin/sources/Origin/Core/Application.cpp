// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Application.h"

#include "Origin\Scripting\ScriptEngine.h"

namespace Origin {

	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string title, ApplicationCommandLineArgs args)
		: m_CommandLineArgs(args)
	{
		s_Instance = this;
		m_Window = Window::Create(title);
		m_Window->SetEventCallback(OGN_BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();
		ScriptEngine::Init();

		m_GuiLayer = new GuiLayer();
		PushOverlay(m_GuiLayer);
	}

	Application::~Application()
	{
		ScriptEngine::Shutdown();
		Renderer::Shutdown();
	}

	void Application::Run()
	{
		m_Window->SetVSync(false);

		while (m_Window->Loop()) {


			auto time = (float)glfwGetTime();
			Timestep timestep = time - m_LastFrame;
			m_LastFrame = time;
			timestep.SetDeltaTime(m_LastFrame);

			if (!m_Minimized) {
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}

			m_GuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnGuiRender();
			m_GuiLayer->End();
			m_Window->OnUpdate();
		}
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::PushLayer(Layer* layer)
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

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		if(m_Window->Loop())
			return false;

		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.getWidth() == 0 || e.getHeight() == 0)
		{
			m_Minimized = true;
			return true;
		}
		m_Minimized = false;
		Renderer::OnWindowResize(e.getWidth(), e.getHeight());

		return false;
	}
}