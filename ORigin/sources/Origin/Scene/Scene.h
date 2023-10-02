// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "EditorCamera.h"
#include "Origin/Utils/Time.h"
#include "Origin/Scene/Skybox.h"
#include "Origin/Scene/Component.h"
#include "Origin/Renderer/Framebuffer.h"

#include "Origin\Renderer\ParticleSystem.h"

#include "Origin/Renderer/Texture.h"
#include "entt/entt.hpp"

class b2World;

namespace origin
{
    class Entity;
    class Contact2DListener;

    class Scene
    {
    public:
        Scene();
        ~Scene();

        static std::shared_ptr<Scene> Copy(std::shared_ptr<Scene> other);

        Entity CreateEntity(const std::string& name = std::string());
        Entity CreateCircle(const std::string& name = std::string());
        Entity CreateSpriteEntity(const std::string& name = std::string());

        Entity CreateCube(const std::string& name = std::string());
        Entity CreateCamera(const std::string& name = std::string());
        Entity CreatePointlight();
        Entity CreateSpotLight();
        Entity CreateDirectionalLight();
        Entity CreateMesh(const std::string& name = std::string());

        Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
        Entity GetPrimaryCameraEntity();
        Entity DuplicateEntity(Entity entity);

        Entity GetEntityWithUUID(UUID uuid);
        Entity FindEntityByName(std::string_view name);

        void DestroyEntity(Entity entity);

        void OnRuntimeStart();
        void OnRuntimeStop();
        void OnUpdateRuntime(Timestep time);

        void OnSimulationStart();
        void OnSimulationStop();
        void OnUpdateSimulation(Timestep time, EditorCamera& camera);

        void OnUpdateEditor(Timestep time, EditorCamera& camera);

        void OnViewportResize(const uint32_t width, const uint32_t height);

        void DrawGrid(int size = 5, glm::vec4 color = glm::vec4(1.0f));
        void SetGrid(int size, glm::vec4 color);

        template <typename... Components>
        auto GetAllEntitiesWith()
        {
            return m_Registry.view<Components...>();
        }

        void OnShadowRender();

    private:
        // 2D Scene
        void OnPhysics2DStart();
        void OnPhysics2DStop();

        Contact2DListener* m_Box2DContactListener;

        void RenderScene(const EditorCamera& camera);
        void RenderScene(Camera* camera, const TransformComponent& transform);
        
        template <typename T>
        void OnComponentAdded(Entity entity, T& component);
        
        void DrawIcon(const EditorCamera& camera, int entity, const std::shared_ptr<Texture2D>& texture, const TransformComponent& tc, bool rotate = true);
        bool IsRunning() const { return m_Running; }
        bool IsPaused() const { return m_Paused; }

        void SetPaused(bool paused) { m_Paused = paused; }
        void Step(int frames);

        glm::vec4 m_GridColor = glm::vec4(1.0f);
        int m_GridSize = 5;

        std::unordered_map<UUID, entt::entity> m_EntityMap;
        std::shared_ptr<Texture2D> m_CameraIcon, m_LightingIcon, m_AudioIcon;

        entt::registry m_Registry;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        uint32_t m_GameViewportWidth = 0, m_GameViewportHeight = 0;
        
        b2World* m_Box2DWorld;

        bool m_Running = false;
        bool m_Paused = false;
        int m_StepFrames = 0;

        friend class Entity;
        friend class Editor;
        friend class SceneSerializer;
        friend class SceneHierarchyPanel;
    };
}
