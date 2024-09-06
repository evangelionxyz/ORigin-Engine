// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#ifndef LAYER_H
#define LAYER_H

#include "Event.h"

#include "Time.h"

namespace origin {

	class Layer
	{
	protected:
		std::string m_Name;

	public:
		Layer(const std::string& name = "layer");
		
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnGuiRender() {}
		virtual void OnEvent(Event& e) {}

		inline const std::string& GetName() const { return m_Name; }
	};
}

#endif
