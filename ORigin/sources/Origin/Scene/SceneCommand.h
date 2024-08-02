#pragma once
#include "Scene.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Origin/Core/CommandManager.h"
#include <functional>

namespace origin
{
    class DestoryEntityCommand : public Command
    {
    public:
        DestoryEntityCommand(std::shared_ptr<Scene> &scene, Entity selectedEntity)
            : m_Scene(scene), m_Entity(selectedEntity)
        {
        }

        void Execute() override
        {
            m_Scene->DestroyEntity(m_Entity);
        }

        void Undo() override
        {
            m_Scene->DeserializeDeletedEntity();

        }
    private:
        std::shared_ptr<Scene> &m_Scene;
        Entity m_Entity;
    };
}