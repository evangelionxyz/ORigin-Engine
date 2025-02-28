// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once

#include "Event.h"

namespace origin {

class WindowResizeEvent : public Event
{
private:
    u32 m_Width, m_Height;
public:
    WindowResizeEvent(u32 width, u32 height)
        : m_Width(width), m_Height(height) {
    }

    inline u32 GetWidth() const { return m_Width; }
    inline u32 GetHeight() const { return m_Height; }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
        return ss.str();
    };

    static EventType GetStaticType() { return EventType::WindowResize; }
    virtual EventType GetEventType() const override { return GetStaticType(); }
    virtual i32 GetCategoryFlags() const override { return EventCategoryApplication; }
};

class WindowCloseEvent : public Event
{
public:
    WindowCloseEvent() {}

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "WindowCloseEvent: Window Closed!";
        return ss.str();
    };

    static EventType GetStaticType() { return EventType::WindowClose; }
    virtual EventType GetEventType() const override { return GetStaticType(); }
    virtual i32 GetCategoryFlags() const override { return EventCategoryApplication; }
};

class FramebufferResizeEvent : public Event
{
private:
    i32 m_Width, m_Height;
public:
    FramebufferResizeEvent(i32 width, i32 height)
        : m_Width(width), m_Height(height) {
    }

    inline i32 GetWidth() const { return m_Width; }
    inline i32 GetHeight() const { return m_Height; }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "FramebufferResizeEvent: " << m_Width << ", " << m_Height;
        return ss.str();
    }

    static EventType GetStaticType() { return EventType::FramebufferResize; }
    virtual EventType GetEventType() const override { return GetStaticType(); }
    virtual i32 GetCategoryFlags() const override { return EventCategoryApplication; }
};

class WindowDropEvent : public Event
{
public:
    WindowDropEvent(const std::vector<std::filesystem::path> &paths)
        : m_Paths(paths) {
    }
    WindowDropEvent(std::vector <std::filesystem::path> &&paths)
        : m_Paths(std::move(paths)) {
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "WindoDropEvent: \nDropping files: ";
        for (auto &path : m_Paths)
        {
            ss << path.generic_string() << "\n";
        }
        return ss.str();
    }

    const std::vector<std::filesystem::path> &GetPaths() const { return m_Paths; }

    static EventType GetStaticType() { return EventType::WindowDrop; }
    virtual EventType GetEventType() const override { return GetStaticType(); }
    virtual i32 GetCategoryFlags() const override { return EventCategoryApplication; }
private:
    std::vector<std::filesystem::path> m_Paths;
};
}
