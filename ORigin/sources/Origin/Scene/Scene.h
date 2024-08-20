// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef SCENE_H
#define SCENE_H

#include "UIRenderer.h"
#include "EditorCamera.h"
#include "Origin/Asset/Asset.h"
#include "Origin/Scene/Skybox.h"
#include "Origin/Core/Time.h"

#include <entt/entt.hpp>

class b2World;

namespace origin
{
    class Entity;
    class Physics2D;

    class Scene : public Asset
    {
    public:
        Scene();
        static std::shared_ptr<Scene> Copy(const std::shared_ptr<Scene> &other);

        Entity GetPrimaryCameraEntity();
        Entity GetEntityWithUUID(UUID uuid);
        Entity FindEntityByName(std::string_view name);
        Entity DuplicateEntityRecursive(Entity entity, Entity newParent);
        Entity DuplicateEntity(Entity entity);

        void DestroyEntityRecursive(UUID entityId);
        void DestroyEntity(Entity entity);

        void PreRender(const Camera &camera, Timestep ts);
        void PostRender(const Camera &camera, Timestep ts);
        
        void OnRuntimeStart();
        void OnRuntimeStop();
        void OnUpdateRuntime(Timestep time);
        void OnUpdateSimulation(const Camera &camera, Timestep time, entt::entity selectedID);

        void OnUpdateEditor(const Camera &camera, Timestep time, entt::entity selectedID);

        void OnViewportResize(const uint32_t width, const uint32_t height);
        void OnRenderShadow();

        void SetFocus(bool focus);
        void LockMouse();
        void UnlockMouse();
        bool IsFocusing() { return m_IsFocus; }

        template <typename... Components>
        auto GetAllEntitiesWith() { return m_Registry.view<Components...>(); }
        std::vector<std::pair<UUID, entt::entity>> &GetEntityMap() { return m_EntityStorage; }
        const std::string &GetName() const { return m_Name; }
        void SetName(const std::string &name) { m_Name = name; }
        std::shared_ptr<UIRenderer> GetUIRenderer() { return m_UIRenderer; }
        const std::shared_ptr<Physics2D> &GetPhysics2D() const { return m_Physics2D; }
        
        uint32_t GetWidth() { return m_ViewportWidth; }
        uint32_t GetHeight() { return m_ViewportHeight; }

        static AssetType GetStaticType() { return AssetType::Scene; }
        AssetType GetType() const override { return GetStaticType(); }
    private:

        void Update(Timestep ts);
        void UpdateScripts(Timestep ts);
        void UpdatePhysics(Timestep ts);

        void RenderScene(const Camera &camera);
        void RenderStencilScene(const Camera &camera, entt::entity selectedId);

        bool IsRunning() const { return m_Running; }
        bool IsPaused() const { return m_Paused; }
        void SetPaused(bool paused) { m_Paused = paused; }
        void Step(int frames);

        std::vector<UUID> GetChildrenUUIDs(UUID parentId);
        template <typename T>
        void OnComponentAdded(Entity entity, T &component);
        entt::registry m_Registry {};
        std::string m_Name = "untitled";
        std::shared_ptr<UIRenderer> m_UIRenderer;
        std::shared_ptr<Physics2D> m_Physics2D;
        glm::vec4 m_GridColor = glm::vec4(1.0f);
        std::vector<std::pair<UUID, entt::entity>> m_EntityStorage;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        uint32_t m_GameViewportWidth = 0, m_GameViewportHeight = 0;

        bool m_Running = false;
        bool m_Paused = false;
        bool m_IsFocus = false;
        int m_StepFrames = 0;

        friend class Entity;
        friend class EntityManager;
        friend class EditorLayer;
        friend class Gizmos;
        friend class SceneSerializer;
        friend class SceneHierarchyPanel;
        friend class Physics2D;
        friend class PhysicsEngine;
    };
}

#endif