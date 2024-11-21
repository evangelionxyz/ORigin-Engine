// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef JOLT_API_HPP
#define JOLT_API_HPP

#include "../PhysicsApiBase.hpp"
#include "JoltBodyActivationListener.hpp"
#include "JoltContactListener.hpp"

#include <Jolt/Jolt.h>
#include <Jolt/Math/Real.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/JobSystemSingleThreaded.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>

namespace origin {

namespace Layers {
static constexpr JPH::ObjectLayer NON_MOVING = 0;
static constexpr JPH::ObjectLayer MOVING = 1;
static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
}

namespace BroadPhaseLayers {
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

class JoltApi : public PhysicsAPIBase
{
public:
    JoltApi();

    void Init() override;
    void Shutdown() override;

    static JoltApi *GetInstance();

private:
    Scope<JPH::PhysicsSystem> m_Physics;
    Scope<JPH::TempAllocator> m_TempAllocator;
    Scope<JPH::JobSystem> m_JobSystem;
    Scope<JPH::BodyActivationListener> m_BodyActivationListener;
    Scope<JPH::ContactListener> m_ContactListener;

    JPH::BodyInterface *m_BodyInterface{};
    JoltBroadPhaseLayerInterfaceImpl m_BroadPhaseLayer;
    JoltObjectVsBroadPhaseLayerFilterImpl m_ObjectVsBroadPhaseLayerFilter;
    JoltObjectLayerPairFilterImpl m_ObjectLayerPairFilter;
};

}


#endif