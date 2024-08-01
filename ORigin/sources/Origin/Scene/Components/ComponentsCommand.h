#pragma once
#include "Components.h"
#include "Origin/Core/CommandManager.h"
#include "Origin/Core/ConsoleManager.h"

namespace origin
{
    template<typename T>
    class ComponentsCommand : public Command
    {
    public:
        ComponentsCommand(T &comp, const T &oldComp)
            : m_Component(comp), m_OldComponent(oldComp), m_NewComponent(comp)
        {
        }

        void Execute() override
        {
            m_Component = m_NewComponent;
        }
        void Undo() override
        {
            m_Component = m_OldComponent;
        }

        static T &GetTempComponent()
        {
            static T tempComponent;
            return tempComponent;
        }

    private:
        T &m_Component;
        T m_OldComponent;
        T m_NewComponent;
    };
}
