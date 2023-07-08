// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Utils\Layer.h"

#include "Origin\IO\Events\AppEvent.h"
#include "Origin\IO\Events\KeyEvent.h"
#include "Origin\IO\Events\MouseEvent.h"

#include "GLFW/glfw3.h"

#include "imgui.h"

namespace origin
{
	class GuiLayer : public Layer
	{
	public:
		GuiLayer();
		~GuiLayer();

		void SetContext(GLFWwindow* window);
		void Init();
		void Detach();

		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Event& e) override;
		void BlockEvents(bool block) { m_BlockEvents = block; }

		void SetDisplaySize(float width, float height);
		void Begin();
		void End();
		uint32_t GetActiveWidgetID();

	private:
		GLFWwindow* m_WindowContext = nullptr;
		ImVec2 m_DisplaySize;

		bool m_BlockEvents = false;
		float m_Time = 0.0f;
	};

}