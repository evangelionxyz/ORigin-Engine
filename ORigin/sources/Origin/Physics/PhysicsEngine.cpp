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
        s_Data.JobSystem = std::make_unique<JPH::JobSystemSingleThreaded>(cMaxPhysicsJobs);
    }

    void PhysicsEngine::Shutdown()
    {
        JPH::UnregisterTypes();
        delete JPH::Factory::sInstance;
        JPH::Factory::sInstance = nullptr;
    }

    void PhysicsEngine::CreateBoxCollider(Entity entity)
    {
        auto &tc = entity.GetComponent<TransformComponent>();

        if (!entity.HasComponent<RigidbodyComponent>())
        {
            OGN_CORE_ERROR("[Jolt] Could not create colldier");
            return;
        }

        auto &bc = entity.GetComponent<BoxColliderComponent>();
        auto &rb = entity.GetComponent<RigidbodyComponent>();

        glm::vec3 halfExtents = tc.Scale * (bc.Size * 2.0f);
        JPH::BoxShapeSettings shapeSettings(GlmToJoltVec3(halfExtents));
        shapeSettings.mUserData = entity.GetUUID();

        JPH::ShapeSettings::ShapeResult shapeResult = shapeSettings.Create();

        OGN_CORE_ASSERT(shapeResult.IsValid(), shapeResult.GetError());
        JPH::ShapeRefC shape = shapeResult.Get();
        bc.Shape = (void *)shape.GetPtr();

        JPH::BodyCreationSettings settings(shape, GlmToJoltVec3(tc.WorldTranslation + bc.Offset), GlmToJoltQuat(tc.WorldRotation),
            rb.IsStatic ? JPH::EMotionType::Static : JPH::EMotionType::Dynamic,
            rb.IsStatic ? Layers::NON_MOVING : Layers::MOVING);
        
        JPH::Body *body = s_Data.BodyInterface->CreateBody(settings);

        if (body)
        {
            JPH::BodyID bodyId = body->GetID();
            s_Data.BodyInterface->AddBody(bodyId, JPH::EActivation::Activate);
            s_Data.BodyInterface->SetFriction(bodyId, bc.Friction);
            s_Data.BodyInterface->SetRestitution(bodyId, bc.Restitution);
            rb.Body = body;
        }
    }

    void PhysicsEngine::CreateSphereCollider(Entity entity)
    {
        auto &tc = entity.GetComponent<TransformComponent>();
        if (!entity.HasComponent<RigidbodyComponent>())
        {
            OGN_CORE_ERROR("[Jolt] Could not create colldier");
            return;
        }

        auto &sc = entity.GetComponent<SphereColliderComponent>();
        auto &rb = entity.GetComponent<RigidbodyComponent>();

        JPH::SphereShapeSettings shapeSettings((sc.Radius * 2.0f) * tc.Scale.x);
        shapeSettings.mUserData = entity.GetUUID();

        JPH::ShapeSettings::ShapeResult shapeResult = shapeSettings.Create();
        OGN_CORE_ASSERT(shapeResult.IsValid(), shapeResult.GetError());
        JPH::ShapeRefC shape = shapeResult.Get();
        sc.Shape = (void *)shape.GetPtr();

        JPH::BodyCreationSettings settings(shape, GlmToJoltVec3(tc.WorldTranslation + sc.Offset), GlmToJoltQuat(tc.WorldRotation),
            rb.IsStatic ? JPH::EMotionType::Static : JPH::EMotionType::Dynamic,
            rb.IsStatic ? Layers::NON_MOVING : Layers::MOVING);

        JPH::Body *body = s_Data.BodyInterface->CreateBody(settings);
        if (body)
        {
            JPH::BodyID bodyId = body->GetID();
            s_Data.BodyInterface->AddBody(bodyId, JPH::EActivation::Activate);
            s_Data.BodyInterface->SetFriction(bodyId, sc.Friction);
            s_Data.BodyInterface->SetRestitution(bodyId, sc.Restitution);
            rb.Body = body;
        }
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
        for (auto id : scene->m_Registry.view<BoxColliderComponent>())
        {
            Entity entity = { id, scene };
            CreateBoxCollider(entity);
        }

        for (auto id : scene->m_Registry.view<SphereColliderComponent>())
        {
            Entity entity = { id, scene };
            CreateSphereCollider(entity);
        }
    }

    void PhysicsEngine::OnSimulateStop(Scene *scene)
    {
        for (auto id : scene->m_Registry.view<RigidbodyComponent>())
        {
            Entity entity = { id, scene };
            RigidbodyComponent &rb = entity.GetComponent<RigidbodyComponent>();

            JPH::Body *body = reinterpret_cast<JPH::Body *>(rb.Body);

            s_Data.BodyInterface->RemoveBody(body->GetID());
            s_Data.BodyInterface->DestroyBody(body->GetID());
        }
    }

    void PhysicsEngine::Simulate(Timestep ts, Scene *scene)
    {
        for (auto id : scene->m_Registry.view<RigidbodyComponent>())
        {
            Entity entity = { id, scene };
            TransformComponent &tc = entity.GetComponent<TransformComponent>();
            RigidbodyComponent &rb = entity.GetComponent<RigidbodyComponent>();

            JPH::Body *body = reinterpret_cast<JPH::Body *>(rb.Body);

            if (body)
            {
                glm::vec3 pos = JoltToGlmVec3(body->GetPosition());
                glm::quat rot = JoltToGlmQuat(body->GetRotation());

                tc.WorldTranslation = pos;
                tc.WorldRotation = glm::eulerAngles(rot);

                tc.Translation = tc.WorldTranslation;
                tc.Rotation = tc.WorldRotation;
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

    void PhysicsEngine::OnInstantiateScriptEntity(Entity entity)
    {
        if (entity.HasComponent<RigidbodyComponent>())
        {
            if(entity.HasComponent<BoxColliderComponent>())
                CreateBoxCollider(entity);

            if (entity.HasComponent<SphereColliderComponent>())
                CreateSphereCollider(entity);
        }
    }

    void PhysicsEngine::OnDestroyScriptEntity(Entity entity)
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