// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Core\Layer.h"
#include "Origin\Core\AppEvent.h"
#include "Origin\Core\KeyEvent.h"
#include "Origin\Core\MouseEvent.h"

namespace origin
{
	class GuiLayer : public Layer
	{
	public:
		GuiLayer(void* window);
		~GuiLayer();

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
		void* m_Context = nullptr;
		float m_Width = 0.0f, m_Height = 0.0f;
		bool m_BlockEvents = false;
		float m_Time = 0.0f;
	};

}