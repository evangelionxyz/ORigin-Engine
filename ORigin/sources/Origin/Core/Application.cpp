// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Application.h"
#include "Origin\Asset\AssetImporter.h"
#include "Origin\Audio\Audio.h"
#include "Origin\Physics\Physics.h"
#include "Origin\Scripting\ScriptEngine.h"

#include <imgui.h>
#include <stb_image.h>

#pragma warning(disable : OGN_DISABLED_WARNINGS)


namespace origin {

	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& spec)
		: m_Spec(spec)
	{
		OGN_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		if (!m_Spec.WorkingDirectory.empty())
			std::filesystem::current_path(m_Spec.WorkingDirectory);

		Window::GLFWInit();

		if (!spec.Runtime)
		{
			WindowConfig windowConfig;
			windowConfig.Title = spec.Name;
			windowConfig.Decorated = false;
			windowConfig.Width = 800;
			windowConfig.Height = 420;

			m_Window = Window::Create(windowConfig);

			m_GraphicContext = GraphicsContext::Create(m_Window->GetNativeWindow());
			m_GraphicContext->Init();
			m_Window->SetIcon(spec.IconPath);
			m_Window->SetEventCallback(OGN_BIND_EVENT_FN(Application::OnEvent));

			m_GuiLayer = new GuiLayer();
			m_GuiLayer->SetContext(m_Window->GetNativeWindow());
			PushOverlay(m_GuiLayer);

			std::shared_ptr<Texture2D> splashImage = TextureImporter::LoadTexture2D("Resources/UITextures/splashscreen.png");
			m_GuiLayer->Begin();
			ImVec2 windowPos = ImVec2(m_Window->GetPosition().x, m_Window->GetPosition().y);
			ImVec2 windowSize = ImVec2((float)m_Window->GetWidth(), (float)m_Window->GetHeight());
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGuiWindowFlags imageWinFlags = ImGuiWindowFlags_NoTitleBar
				| ImGuiWindowFlags_NoResize
				| ImGuiWindowFlags_NoMove
				| ImGuiWindowFlags_NoScrollbar
				| ImGuiWindowFlags_NoDocking;

			ImGui::SetNextWindowSize(windowSize);
			ImGui::SetNextWindowPos(windowPos);
			ImGui::SetNextWindowBgAlpha(0.0f);
			ImGui::Begin("Splash Screen", nullptr, imageWinFlags);
			ImGui::Image(reinterpret_cast<ImTextureID>(splashImage->GetRendererID()), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::End();

			ImGui::PopStyleVar();
			m_GuiLayer->SetDisplaySize((float)m_Window->GetWidth(), (float)m_Window->GetHeight());
			m_GuiLayer->End();
			m_Window->OnUpdate();
		}
		else
		{
			m_Window = Window::Create(m_Spec.Name, 1280, 640);
			m_GraphicContext = GraphicsContext::Create(m_Window->GetNativeWindow());
			m_GraphicContext->Init();
			m_Window->SetIcon(spec.IconPath);
			m_Window->SetEventCallback(OGN_BIND_EVENT_FN(Application::OnEvent));
		}

		Renderer::Init();
		Physics::Init();
		AudioEngine::Init();

		RenderCommand::Clear();
		RenderCommand::ClearColor(glm::vec4(0.7f));
		m_Window->OnUpdate();
		
		if (!m_Spec.Runtime)
		{
			m_Window->Decorated(true);
			m_Window->SetSize(1280, 640);
		}

		m_MainInputHandle = std::make_unique<Input>();
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

		while (m_Window->Loop())
		{
			double time = glfwGetTime();
			Timestep timestep = time - m_LastFrame;
			m_LastFrame = time;
			timestep.SetDeltaTime(m_LastFrame);

			ExecuteMainThreadQueue();

			if (!m_Minimized) 
			{
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}

			if (!m_Spec.Runtime)
			{
				m_GuiLayer->Begin();
				for (Layer* layer : m_LayerStack)
					layer->OnGuiRender();
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

	void Application::SubmitToMainThread(const std::function<void()>& function)
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadMutex);
		m_MainThreadQueue.emplace_back(function);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		if(m_Window->Loop())
			return false;

		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return true;
		}
		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

	void Application::ExecuteMainThreadQueue()
	{
		for (auto& func : m_MainThreadQueue)
			func();

		m_MainThreadQueue.clear();
	}
}