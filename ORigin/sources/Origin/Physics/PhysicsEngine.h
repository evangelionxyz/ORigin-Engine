#pragma once
#include "Origin/Core/Time.h"
#include "Origin/Scene/Entity.h"
#include <glm/glm.hpp>

#include <Jolt/Jolt.h>
#include <Jolt/Math/Real.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>

namespace origin {

	class Scene;
	class PhysicsEngine
	{
	public:
		static void Init();
		static void Shutdown();

        static void CreateBoxCollider(Entity entity);
        static void CreateSphereCollider(Entity entity);

		static void OnSimulateStart(Scene *scene);
		static void OnSimulateStop(Scene *scene);
		static void Simulate(Timestep ts, Scene *scene);

		static void OnInstantiateScriptEntity(Entity entity);
		static void OnDestroyScriptEntity(Entity entity);

		static JPH::BodyInterface *GetBodyInterface();
	};

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
}

