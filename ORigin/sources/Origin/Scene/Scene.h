// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "EditorCamera.h"
#include "Origin\Asset\Asset.h"
#include "Origin\Renderer\Framebuffer.h"
#include "Origin\Renderer\ParticleSystem.h"
#include "Origin\Scene\Components.h"
#include "Origin\Scene\Skybox.h"

#include "entt\entt.hpp"
#include "Origin\Renderer\Texture.h"

#include "Origin\Utils\Time.h"

class b2World;

namespace origin
{
  class Entity;
  class PhysicsScene;
  class Physics2D;

  class Scene : public Asset
  {
  public:
    Scene();
    ~Scene();
    static std::shared_ptr<Scene> Copy(std::shared_ptr<Scene> other);
    virtual AssetType GetType() const override { return AssetType::Scene; }

    Entity CreateEntity(const std::string& name);
    Entity CreateCamera(const std::string& name);
    Entity CreateMesh(const std::string& name);
    Entity CreateEntityWithUUID(UUID uuid, const std::string& name);
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
    void OnShadowRender();

    void UpdateTransform();

    template <typename... Components>
    auto GetAllEntitiesWith() { return m_Registry.view<Components...>(); }

    std::unordered_map<UUID, entt::entity> GetEntityMap() { return m_EntityMap; }

  private:
    std::unique_ptr<PhysicsScene> m_PhysicsScene;
    Physics2D* m_Physics2D;

    void RenderScene(const EditorCamera& camera);
    void RenderScene(const SceneCamera& camera, const TransformComponent& cameraTransform);

    template <typename T>
    void OnComponentAdded(Entity entity, T& component);

    bool IsRunning() const { return m_Running; }
    bool IsPaused() const { return m_Paused; }

    void SetPaused(bool paused) { m_Paused = paused; }
    void Step(int frames);

    glm::vec4 m_GridColor = glm::vec4(1.0f);
    int m_GridSize = 5;

    std::unordered_map<UUID, entt::entity> m_EntityMap;

    entt::registry m_Registry;
    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
    uint32_t m_GameViewportWidth = 0, m_GameViewportHeight = 0;

    bool m_Running = false;
    bool m_Paused = false;
    int m_StepFrames = 0;

    friend class Entity;
    friend class EditorLayer;
    friend class Gizmos;
    friend class SceneSerializer;
    friend class SceneHierarchyPanel;
    friend class PhysXScene;
    friend class JoltScene;
    friend class Physics2D;
  };
}
