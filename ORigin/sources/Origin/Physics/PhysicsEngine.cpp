// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "PhysicsEngine.h"

#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/JobSystemSingleThreaded.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/StateRecorderImpl.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/TaperedCapsuleShape.h>

#include "Origin/Profiler/Profiler.h"
#include "Origin/Scene/Components/PhysicsComponents.h"
#include "JoltContactListener.h"
#include "JoltBodyActivationListener.h"

namespace origin
{
    struct PhysicsEngineData
    {
        std::unique_ptr<JPH::PhysicsSystem> PhysicsSystem;
        std::unique_ptr<JPH::TempAllocator> TempAllocator;
        std::unique_ptr<JPH::JobSystem> JobSystem;
        std::unique_ptr<JPH::BodyActivationListener> BodyActivationListener;
        std::unique_ptr<JPH::ContactListener> ContactListener;
        JPH::BodyInterface *BodyInterface;
        JoltBroadPhaseLayerInterfaceImpl BroadPhaseLayerInterface;
        JoltObjectVsBroadPhaseLayerFilterImpl ObjectVsBroadPaheLayerFilter;
        JoltObjectLayerPairFilterImpl ObjectLayerPairFilter;
    };

    static PhysicsEngineData s_Data;

    static constexpr unsigned int cNumBodies = 1024;
    static constexpr unsigned int cNumBodyMutexes = 0; // Autodetect
    static constexpr unsigned int cMaxBodyPairs = 1024;
    static constexpr unsigned int cMaxContactConstraints = 2048;
    static const int cMaxPhysicsJobs = 2048;

    void PhysicsEngine::Init()
    {
        JPH::RegisterDefaultAllocator();
        JPH::Factory::sInstance = new JPH::Factory();
        JPH::RegisterTypes();
        s_Data.TempAllocator = std::make_unique<JPH::TempAllocatorImpl>(32 * 1024 * 1024);
        s_Data.JobSystem = std::make_unique<JPH::JobSystemThreadPool>(cMaxPhysicsJobs, 8, std::thread::hardware_concurrency() - 1);
    }

    void PhysicsEngine::Shutdown()
    {
        JPH::UnregisterTypes();
        delete JPH::Factory::sInstance;
        JPH::Factory::sInstance = nullptr;
    }

    void *PhysicsEngine::CreateBoxCollider(Entity entity, const glm::vec3 &size, RigidbodyComponent &rb)
    {
        auto &tc = entity.GetComponent<TransformComponent>();

        glm::vec3 halfExtents = tc.Scale * (size * 2.0f);
        JPH::BoxShapeSettings shapeSettings(GlmToJoltVec3(halfExtents));

        shapeSettings.mUserData = entity.GetUUID();
        JPH::ShapeSettings::ShapeResult shapeResult = shapeSettings.Create();
        OGN_CORE_ASSERT(shapeResult.IsValid(), shapeResult.GetError());
        JPH::ShapeRefC shape = shapeResult.Get();

        JPH::BodyCreationSettings bodySettings(shape, GlmToJoltVec3(tc.WorldTranslation + rb.Offset), GlmToJoltQuat(tc.WorldRotation),
            rb.IsStatic ? JPH::EMotionType::Static : JPH::EMotionType::Dynamic,
            rb.IsStatic ? Layers::NON_MOVING : Layers::MOVING);
        
        bodySettings.mAllowedDOFs = JPH::EAllowedDOFs::None;
        if (rb.RotateX) bodySettings.mAllowedDOFs |= JPH::EAllowedDOFs::RotationX;
        if (rb.RotateY) bodySettings.mAllowedDOFs |= JPH::EAllowedDOFs::RotationY;
        if (rb.RotateZ) bodySettings.mAllowedDOFs |= JPH::EAllowedDOFs::RotationZ;
        if (rb.MoveX) bodySettings.mAllowedDOFs |= JPH::EAllowedDOFs::TranslationX;
        if (rb.MoveY) bodySettings.mAllowedDOFs |= JPH::EAllowedDOFs::TranslationY;
        if (rb.MoveZ) bodySettings.mAllowedDOFs |= JPH::EAllowedDOFs::TranslationZ;

        JPH::Body *body = s_Data.BodyInterface->CreateBody(bodySettings);

        if (body)
        {
            JPH::BodyID bodyId = body->GetID();
            s_Data.BodyInterface->AddBody(bodyId, JPH::EActivation::Activate);
            rb.Body = body;
        }

        return (void *)shape.GetPtr();
    }

    void *PhysicsEngine::CreateSphereCollider(Entity entity, float radius, RigidbodyComponent &rb)
    {
        auto &tc = entity.GetComponent<TransformComponent>();

        JPH::SphereShapeSettings shapeSettings((radius * 2.0f) * tc.Scale.x);
        shapeSettings.mUserData = entity.GetUUID();

        JPH::ShapeSettings::ShapeResult shapeResult = shapeSettings.Create();
        OGN_CORE_ASSERT(shapeResult.IsValid(), shapeResult.GetError());
        JPH::ShapeRefC shape = shapeResult.Get();

        JPH::BodyCreationSettings bodySettings(shape, GlmToJoltVec3(tc.WorldTranslation + rb.Offset), GlmToJoltQuat(tc.WorldRotation),
            rb.IsStatic ? JPH::EMotionType::Static : JPH::EMotionType::Dynamic,
            rb.IsStatic ? Layers::NON_MOVING : Layers::MOVING);

        bodySettings.mAllowedDOFs = JPH::EAllowedDOFs::None;
        if (rb.RotateX) bodySettings.mAllowedDOFs |= JPH::EAllowedDOFs::RotationX;
        if (rb.RotateY) bodySettings.mAllowedDOFs |= JPH::EAllowedDOFs::RotationY;
        if (rb.RotateZ) bodySettings.mAllowedDOFs |= JPH::EAllowedDOFs::RotationZ;
        if (rb.MoveX) bodySettings.mAllowedDOFs |= JPH::EAllowedDOFs::TranslationX;
        if (rb.MoveY) bodySettings.mAllowedDOFs |= JPH::EAllowedDOFs::TranslationY;
        if (rb.MoveZ) bodySettings.mAllowedDOFs |= JPH::EAllowedDOFs::TranslationZ;

        JPH::Body *body = s_Data.BodyInterface->CreateBody(bodySettings);
        if (body)
        {
            JPH::BodyID bodyId = body->GetID();
            s_Data.BodyInterface->AddBody(bodyId, JPH::EActivation::Activate);
            rb.Body = body;
        }

        return (void *)shape.GetPtr();
    }

    void *PhysicsEngine::CreateCapsuleCollider(Entity entity, float halfHeight, float radius, RigidbodyComponent &rb)
    {
        auto &tc = entity.GetComponent<TransformComponent>();

        JPH::CapsuleShapeSettings shapeSettings(halfHeight * tc.Scale.y, (radius * 2.0f) * tc.Scale.x);
        shapeSettings.mUserData = entity.GetUUID();

        JPH::ShapeSettings::ShapeResult shapeResult = shapeSettings.Create();
        OGN_CORE_ASSERT(shapeResult.IsValid(), shapeResult.GetError());
        JPH::ShapeRefC shape = shapeResult.Get();

        JPH::BodyCreationSettings bodySettings(shape, GlmToJoltVec3(tc.WorldTranslation + rb.Offset), GlmToJoltQuat(tc.WorldRotation),
            rb.IsStatic ? JPH::EMotionType::Static : JPH::EMotionType::Dynamic,
            rb.IsStatic ? Layers::NON_MOVING : Layers::MOVING);

        bodySettings.mAllowedDOFs = JPH::EAllowedDOFs::None;
        if (rb.RotateX) bodySettings.mAllowedDOFs |= JPH::EAllowedDOFs::RotationX;
        if (rb.RotateY) bodySettings.mAllowedDOFs |= JPH::EAllowedDOFs::RotationY;
        if (rb.RotateZ) bodySettings.mAllowedDOFs |= JPH::EAllowedDOFs::RotationZ;
        if (rb.MoveX) bodySettings.mAllowedDOFs |= JPH::EAllowedDOFs::TranslationX;
        if (rb.MoveY) bodySettings.mAllowedDOFs |= JPH::EAllowedDOFs::TranslationY;
        if (rb.MoveZ) bodySettings.mAllowedDOFs |= JPH::EAllowedDOFs::TranslationZ;

        JPH::Body *body = s_Data.BodyInterface->CreateBody(bodySettings);
        if (body)
        {
            JPH::BodyID bodyId = body->GetID();
            s_Data.BodyInterface->AddBody(bodyId, JPH::EActivation::Activate);
            rb.Body = body;
        }

        return (void *)shape.GetPtr();
    }

    void PhysicsEngine::OnSimulateStart(Scene *scene)
    {
        s_Data.BodyActivationListener = std::make_unique<JoltBodyActivationListener>();
        s_Data.ContactListener = std::make_unique<JoltContactListener>();

        s_Data.PhysicsSystem = std::make_unique<JPH::PhysicsSystem>();
        s_Data.PhysicsSystem->Init(cNumBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, s_Data.BroadPhaseLayerInterface, s_Data.ObjectVsBroadPaheLayerFilter, s_Data.ObjectLayerPairFilter);
        s_Data.PhysicsSystem->SetBodyActivationListener(s_Data.BodyActivationListener.get());
        s_Data.PhysicsSystem->SetContactListener(s_Data.ContactListener.get());
        s_Data.PhysicsSystem->SetGravity(JPH::Vec3(0.0f, -9.81f, 0.0f));
        s_Data.PhysicsSystem->OptimizeBroadPhase();

        s_Data.BodyInterface = &s_Data.PhysicsSystem->GetBodyInterface();

        // Create Components
        for (auto id : scene->m_Registry.view<RigidbodyComponent>())
        {
            Entity entity = { id, scene };
            InstantiateEntity(entity);
        }
    }

    void PhysicsEngine::OnSimulateStop(Scene *scene)
    {
        for (auto id : scene->m_Registry.view<RigidbodyComponent>())
        {
            Entity entity = { id, scene };
            DestroyEntity(entity);
        }
    }

    void PhysicsEngine::Simulate(Timestep ts, Scene *scene)
    {
        for (auto id : scene->m_Registry.view<RigidbodyComponent>())
        {
            Entity entity = { id, scene };
            TransformComponent &tc = entity.GetComponent<TransformComponent>();
            RigidbodyComponent &rb = entity.GetComponent<RigidbodyComponent>();
            IDComponent &idc = entity.GetComponent<IDComponent>();

            JPH::Body *body = reinterpret_cast<JPH::Body *>(rb.Body);

            if (body)
            {
#if 1
                glm::vec3 worldPosition = JoltToGlmVec3(body->GetPosition());
                glm::quat worldRotation = JoltToGlmQuat(body->GetRotation());

                glm::vec3 originalWorldPosition = worldPosition;
                glm::quat originalWorldRotation = worldRotation;

                Entity currentEntity = entity;
                while (currentEntity.HasParent())
                {
                    UUID parentId = currentEntity.GetComponent<IDComponent>().Parent;
                    Entity parent = scene->GetEntityWithUUID(parentId);
                    TransformComponent &ptc = parent.GetComponent<TransformComponent>();

                    glm::quat invParentRotation = glm::inverse(ptc.WorldRotation);
                    glm::vec3 invParentScale = 1.0f / ptc.WorldScale;

                    worldPosition = invParentRotation * ((worldPosition - ptc.WorldTranslation) * invParentScale);
                    worldRotation = invParentRotation * worldRotation;

                    currentEntity = parent;
                }

                tc.Translation = worldPosition;
                tc.Rotation = worldRotation;

                tc.WorldTranslation = originalWorldPosition;
                tc.WorldRotation = originalWorldRotation;
#else
                tc.WorldTranslation = JoltToGlmVec3(body->GetPosition());
                tc.WorldRotation = JoltToGlmQuat(body->GetRotation());
                tc.Translation = tc.WorldTranslation;
                tc.Rotation = tc.WorldRotation;
#endif
            }

            if (entity.HasComponent<BoxColliderComponent>())
            {
                BoxColliderComponent &cc = entity.GetComponent<BoxColliderComponent>();
                if (cc.Shape)
                {
                    JPH::BoxShape *shape = reinterpret_cast<JPH::BoxShape *>(cc.Shape);
                }
            }
            else if (entity.HasComponent<SphereColliderComponent>())
            {
                SphereColliderComponent &cc = entity.GetComponent<SphereColliderComponent>();
                if (cc.Shape)
                {
                    JPH::SphereShape *shape = reinterpret_cast<JPH::SphereShape *>(cc.Shape);
                }
            }
        }

        s_Data.PhysicsSystem->Update(ts, 1, s_Data.TempAllocator.get(), s_Data.JobSystem.get());
    }

    void PhysicsEngine::InstantiateEntity(Entity entity)
    {
        if (entity.HasComponent<RigidbodyComponent>())
        {
            RigidbodyComponent &rb = entity.GetComponent<RigidbodyComponent>();

            if (entity.HasComponent<BoxColliderComponent>())
            {
                BoxColliderComponent &cc = entity.GetComponent<BoxColliderComponent>();
                cc.Shape = CreateBoxCollider(entity, cc.Size, rb);
                rb.SetFriction(cc.Friction);
                rb.SetRestitution(cc.Restitution);
            }
            else if (entity.HasComponent<CapsuleColliderComponent>())
            {
                CapsuleColliderComponent &cc = entity.GetComponent<CapsuleColliderComponent>();
                cc.Shape = CreateCapsuleCollider(entity, cc.HalfHeight, cc.Radius, rb);
                rb.SetFriction(cc.Friction);
                rb.SetRestitution(cc.Restitution);
            }
            else if (entity.HasComponent<SphereColliderComponent>())
            {
                SphereColliderComponent &cc = entity.GetComponent<SphereColliderComponent>();
                cc.Shape = CreateSphereCollider(entity, cc.Radius, rb);
                rb.SetFriction(cc.Friction);
                rb.SetRestitution(cc.Restitution);
            }
        }
    }

    void PhysicsEngine::DestroyEntity(Entity entity)
    {
        if (entity.HasComponent<RigidbodyComponent>())
        {
            RigidbodyComponent &rb = entity.GetComponent<RigidbodyComponent>();

            JPH::Body *body = reinterpret_cast<JPH::Body *>(rb.Body);
            s_Data.BodyInterface->RemoveBody(body->GetID());
            s_Data.BodyInterface->DestroyBody(body->GetID());
        }
    }

    JPH::BodyInterface *PhysicsEngine::GetBodyInterface()
    {
        return s_Data.BodyInterface;
    }

}