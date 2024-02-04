// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once
#include "Origin\Utils\Layer.h"
#include "Origin\IO\Events\AppEvent.h"
#include "Origin\IO\Events\KeyEvent.h"
#include "Origin\IO\Events\MouseEvent.h"

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