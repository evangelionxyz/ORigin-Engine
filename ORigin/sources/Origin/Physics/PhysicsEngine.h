#pragma once
#include "Origin/Core/Time.h"
#include "Origin/Scene/Entity.h"
#include <glm/glm.hpp>

#include <Jolt/Jolt.h>
#include <Jolt/Math/Real.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>

namespace origin {

	class Scene;
	class PhysicsEngine
	{
	public:
		static void Init();
		static void Shutdown();

        static void *CreateBoxCollider(Entity entity, const glm::vec3 &scale, RigidbodyComponent &rb);
        static void *CreateSphereCollider(Entity entity, float radius, RigidbodyComponent &rb);
        static void *CreateCapsuleCollider(Entity entity, float halfHeight, float radius, RigidbodyComponent &rb);

		static void OnSimulateStart(Scene *scene);
		static void OnSimulateStop(Scene *scene);
		static void Simulate(Timestep ts, Scene *scene);

		static void InstantiateEntity(Entity entity);
		static void DestroyEntity(Entity entity);

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
}

