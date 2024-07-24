// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "PhysicsEngine.h"

#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/JobSystemSingleThreaded.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/StreamWrapper.h>
#include <Jolt/Core/StringTools.h>
#include <Jolt/Geometry/OrientedBox.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/StateRecorderImpl.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/SoftBody/SoftBodyMotionProperties.h>
#include <Jolt/Physics/SoftBody/SoftBodyCreationSettings.h>
#include <Jolt/Physics/PhysicsScene.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/ShapeCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollidePointResult.h>
#include <Jolt/Physics/Collision/AABoxCast.h>
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Collision/Shape/HeightFieldShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/TaperedCapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/TriangleShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/Collision/Shape/MutableCompoundShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>
#include <Jolt/Physics/Collision/NarrowPhaseStats.h>
#include <Jolt/Physics/Constraints/DistanceConstraint.h>
#include <Jolt/Physics/Constraints/PulleyConstraint.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>

#include "Origin/Profiler/Profiler.h"

namespace origin
{

    static void TraceImpl(const char *inFMT, ...)
    {
        // Format the message
        va_list list;
        va_start(list, inFMT);
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), inFMT, list);
        va_end(list);

        // Print to the TTY
        OGN_CORE_TRACE(buffer);
    }

    namespace Layers
    {
        static constexpr JPH::ObjectLayer NON_MOVING = 0;
        static constexpr JPH::ObjectLayer MOVING = 1;
        static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
    }

    namespace BroadPhaseLayers
    {
        static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
        static constexpr JPH::BroadPhaseLayer MOVING(1);
        static constexpr uint32_t NUM_LAYERS(2);
    }

    /// Class that determines if two object layers can collide
    class JoltObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
    {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
        {
            switch (inObject1)
            {
            case Layers::NON_MOVING:
                return inObject2 == Layers::MOVING; // Non moving only collides with moving
            case Layers::MOVING:
                return true; // Moving collides with everything
            default:
                JPH_ASSERT(false);
                return false;
            }
        }
    };

    class JoltBroadPhaseLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
    {
    public:
        JoltBroadPhaseLayerInterfaceImpl()
        {
            m_ObjectToBP[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
            m_ObjectToBP[Layers::MOVING] = BroadPhaseLayers::MOVING;
        }

        virtual uint32_t GetNumBroadPhaseLayers() const override
        {
            return BroadPhaseLayers::NUM_LAYERS;
        }

        virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
        {
            OGN_CORE_ASSERT(inLayer < Layers::NUM_LAYERS, "[Jolt] Invalid Layer");
            return m_ObjectToBP[inLayer];
        }

    private:
        JPH::BroadPhaseLayer m_ObjectToBP[Layers::NUM_LAYERS];
    };

    /// Class that determines if an object layer can collide with a broadphase layer
    class JoltObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
    {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
        {
            switch (inLayer1)
            {
            case Layers::NON_MOVING:
                return inLayer2 == BroadPhaseLayers::MOVING;
            case Layers::MOVING:
                return true;
            default:
                OGN_CORE_ASSERT(false, "");
                return false;
            }
        }
    };

    // An example contact listener
    class JoltContactListener : public JPH::ContactListener
    {
    public:
        virtual JPH::ValidateResult	OnContactValidate(const JPH::Body &inBody1, const JPH::Body &inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult &inCollisionResult) override
        {
            OGN_CORE_TRACE("Contact validate callback");
            return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
        }

        virtual void OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override
        {
            OGN_CORE_TRACE("A contact was added");
        }

        virtual void OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override
        {
            OGN_CORE_TRACE("A contact was presisted");
        }

        virtual void OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) override
        {
            OGN_CORE_TRACE("A contact was removed");
        }
    };

    // An example activation listener
    class JoltBodyActivationListener : public JPH::BodyActivationListener
    {
    public:
        virtual void OnBodyActivated(const JPH::BodyID &inBodyID, uint64_t inBodyUserData) override
        {
            OGN_CORE_TRACE("A body was activated");
        }

        virtual void OnBodyDeactivated(const JPH::BodyID &inBodyID, uint64_t inBodyUserData) override
        {
            OGN_CORE_TRACE("A body went to sleep");
        }
    };

    // Conversion functions
    static JPH::Vec3 GlmToJoltVec3(const glm::vec3 &v)
    {
        return JPH::Vec3(v.x, v.y, v.z);
    }

    static glm::vec3 JoltToGlmVec3(const JPH::Vec3 &v)
    {
        return glm::vec3(v.GetX(), v.GetY(), v.GetZ());
    }

    static JPH::Quat GlmToJoltQuat(const glm::quat &q)
    {
        return JPH::Quat(q.x, q.y, q.z, q.w);
    }

    static glm::quat JoltToGlmQuat(const JPH::Quat &q)
    {
        return glm::quat(q.GetW(), q.GetX(), q.GetY(), q.GetZ());
    }

    static constexpr uint32_t cNumBodies = 10240;
    static constexpr uint32_t cNumBodiesMutexes = 0;
    static constexpr uint32_t cMaxBodyPairs = 65536;
    static constexpr uint32_t cMaxContactConstraints = 2048;

    struct PhysicsEngineData
    {
        std::unique_ptr<JPH::PhysicsSystem> PhysicsSystem;
        JPH::BodyInterface *BodyInterface;
        std::unique_ptr<JPH::TempAllocator> TempAllocator;
        std::unique_ptr<JPH::JobSystem> JobSystem;
        std::unique_ptr<JPH::BodyActivationListener> BodyActivationListener;
        std::unique_ptr<JPH::ContactListener> ContactListener;

        JoltBroadPhaseLayerInterfaceImpl BroadPhaseLayerInterface;
        JoltObjectVsBroadPhaseLayerFilterImpl ObjectVsBroadPaheLayerFilter;
        JoltObjectLayerPairFilterImpl ObjectLayerPairFilter;
    };

    static PhysicsEngineData s_Data;

    void PhysicsEngine::Init()
    {
        JPH::RegisterDefaultAllocator();

        // TODO: Install callbacks

        JPH::Factory::sInstance = new JPH::Factory();

        JPH::RegisterTypes();

        s_Data.TempAllocator = std::make_unique<JPH::TempAllocatorImpl>(32 * 1024 * 1024);
        s_Data.JobSystem = std::make_unique<JPH::JobSystemThreadPool>(2048, 8, std::thread::hardware_concurrency() - 1);

        const uint32_t maxBodies = 1024;
        const uint32_t numBodyMutexes = 0;
        const uint32_t maxBodyPairs = 1024;
        const uint32_t maxContactConstrainsts = 1024;

        

        s_Data.PhysicsSystem = std::make_unique<JPH::PhysicsSystem>();
        s_Data.PhysicsSystem->Init(maxBodies, numBodyMutexes, maxBodyPairs, maxContactConstrainsts,
            s_Data.BroadPhaseLayerInterface, s_Data.ObjectVsBroadPaheLayerFilter, s_Data.ObjectLayerPairFilter);

        s_Data.BodyActivationListener = std::make_unique<JoltBodyActivationListener>();
        s_Data.PhysicsSystem->SetBodyActivationListener(s_Data.BodyActivationListener.get());

        s_Data.ContactListener = std::make_unique<JoltContactListener>();
        s_Data.PhysicsSystem->SetContactListener(s_Data.ContactListener.get());

        s_Data.BodyInterface = &s_Data.PhysicsSystem->GetBodyInterface();
        s_Data.PhysicsSystem->OptimizeBroadPhase();
    }

    void PhysicsEngine::Shutdown()
    {
        JPH::UnregisterTypes();
        delete JPH::Factory::sInstance;
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
        JPH::ShapeSettings::ShapeResult shapeResult = shapeSettings.Create();

        OGN_CORE_ASSERT(shapeResult.IsValid(), shapeResult.GetError());
        JPH::ShapeRefC shape = shapeResult.Get();
        bc.Shape = reinterpret_cast<void *>(&shape);

        JPH::BodyCreationSettings settings(shape, GlmToJoltVec3(tc.WorldTranslation), GlmToJoltQuat(tc.WorldRotation),
            rb.IsStatic ? JPH::EMotionType::Static : JPH::EMotionType::Dynamic,
            rb.IsStatic ? Layers::NON_MOVING : Layers::MOVING);
        
        JPH::Body *body = s_Data.BodyInterface->CreateBody(settings);

        if (body)
        {
            s_Data.BodyInterface->AddBody(body->GetID(), JPH::EActivation::Activate);
            s_Data.BodyInterface->SetLinearVelocity(body->GetID(), JPH::Vec3(0.0f, -5.0f, 0.0f));
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

        JPH::SphereShapeSettings shapeSettings(sc.Radius * 2.0f);
        JPH::ShapeSettings::ShapeResult shapeResult = shapeSettings.Create();

        OGN_CORE_ASSERT(shapeResult.IsValid(), shapeResult.GetError());
        JPH::ShapeRefC shape = shapeResult.Get();

        sc.Shape = reinterpret_cast<void *>(&shape);

        JPH::BodyCreationSettings settings(shape, GlmToJoltVec3(tc.WorldTranslation), GlmToJoltQuat(tc.WorldRotation),
            rb.IsStatic ? JPH::EMotionType::Static : JPH::EMotionType::Dynamic,
            rb.IsStatic ? Layers::NON_MOVING : Layers::MOVING);

        JPH::Body *body = s_Data.BodyInterface->CreateBody(settings);
        if (body)
        {
            s_Data.BodyInterface->AddBody(body->GetID(), JPH::EActivation::Activate);
            s_Data.BodyInterface->SetLinearVelocity(body->GetID(), JPH::Vec3(0.0f, -5.0f, 0.0f));
            rb.Body = body;
        }
    }

    void PhysicsEngine::OnSimulateStart(Scene *scene)
    {
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
        }

        s_Data.PhysicsSystem->Update(ts, 1, s_Data.TempAllocator.get(), s_Data.JobSystem.get());
    }

}