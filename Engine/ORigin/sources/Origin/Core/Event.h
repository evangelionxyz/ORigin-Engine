// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once

#include "Origin/Core/Base.h"

namespace origin {

enum class EventType
{
	None = 0,
	WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved, WindowDrop,
	FramebufferResize,
	KeyPressed, KeyReleased, KeyTyped,
	MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
};

enum EventCategory
{
	EventCategoryApplication = BIT(0),
	EventCategoryInput = BIT(1),
	EventCategoryKeyboard = BIT(2),
	EventCategoryMouse = BIT(3),
	EventCategoryMouseButton = BIT(4)
};

class Event
{
public:
	virtual ~Event() = default;

	bool Handled = false;

	virtual EventType GetEventType() const = 0;
	virtual int GetCategoryFlags() const = 0;

	virtual std::string ToString() const = 0;

	bool IsInCategory(EventCategory category)
	{
		return GetCategoryFlags() & category;
	}
};

class EventDispatcher
{
public:
	EventDispatcher(Event &event)
		: m_Event(event)
	{
	}

	// F will be deduced by the compiler
	template<typename T, typename F>
	bool Dispatch(const F &func)
	{
		if (m_Event.GetEventType() == T::GetStaticType())
		{
			m_Event.Handled = func(static_cast<T &>(m_Event));
			return true;
		}
		return false;
	}
private:
	Event &m_Event;
};

}
