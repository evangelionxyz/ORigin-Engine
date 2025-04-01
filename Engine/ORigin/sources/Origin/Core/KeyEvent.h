// Copyright (c) 2025 Evangelion Manuhutu | ORigin Engine

#pragma once

#include "Event.h"
#include "KeyCodes.h"

namespace origin {

class KeyEvent : public Event
{
public:
	KeyCode GetKeyCode() const { return m_KeyCode; }
	KeyModCode GetKeyModCode() const { return m_KeyModCode; }

    virtual i32 GetCategoryFlags() const override { return EventCategoryKeyboard | EventCategoryInput; }

	KeyEvent(const KeyCode keycode, const KeyModCode keymod) 
		: m_KeyCode(keycode), m_KeyModCode(keymod) {}

protected:
	KeyCode m_KeyCode;
	KeyModCode m_KeyModCode;
};

class KeyPressedEvent : public KeyEvent
{
public:
	KeyPressedEvent(const KeyCode keycode, const KeyModCode keymod, const u16 repeatCount)
		: KeyEvent(keycode, keymod), m_RepeatCount(repeatCount) {
	}

	u16 GetRepeatCount() const { return m_RepeatCount; }

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "KeyPressedEvent: " << (char)m_KeyCode << " Mod: " << m_KeyModCode << " (" << m_RepeatCount << " repeats)";
		return ss.str();
	}

    static EventType GetStaticType() { return EventType::KeyPressed; }
    virtual EventType GetEventType() const override { return GetStaticType(); }
private:
	u16 m_RepeatCount;
};

class KeyReleasedEvent : public KeyEvent
{
public:
	KeyReleasedEvent(const KeyCode keycode, const KeyModCode keymod)
		: KeyEvent(keycode, keymod) {
	}

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "KeyReleasedEvent: " << (char)m_KeyCode << " Mod: " << m_KeyModCode;
		return ss.str();
	}

    static EventType GetStaticType() { return EventType::KeyReleased; }
    virtual EventType GetEventType() const override { return GetStaticType(); }
};

class KeyTypedEvent : public KeyEvent
{
public:
	KeyTypedEvent(const KeyCode keycode)
		: KeyEvent(keycode, OGN_KEY_UNKNOWN) {
	}

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "KeyTypedEvent: " << (char)m_KeyCode;
		return ss.str();
	}

    static EventType GetStaticType() { return EventType::KeyTyped; }
    virtual EventType GetEventType() const override { return GetStaticType(); }
};
}