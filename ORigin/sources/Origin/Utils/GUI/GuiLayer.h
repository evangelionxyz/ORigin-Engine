// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Core\OriginCore.h"
#include "Origin\Utils\Layer.h"

#include "Origin/IO/Events/AppEvent.h"
#include "Origin/IO/Events/KeyEvent.h"
#include "Origin/IO/Events/MouseEvent.h"


namespace Origin {
	
	class GuiLayer : public Layer
	{
	public:
		GuiLayer() : Layer("Gui Layer") {}
		~GuiLayer();

		virtual void OnUpdate(Timestep ts) override;
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void Begin();
		void End();

	private:
		float m_Time = 0.0f;
	};

}