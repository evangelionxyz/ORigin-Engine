// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef JOLT_CONTACT_LISTENER_H
#define JOLT_CONTACT_LISTENER_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/Collision/CollideShape.h>
namespace origin
{
    class JoltContactListener : public JPH::ContactListener
    {
    public:
        virtual JPH::ValidateResult	OnContactValidate(const JPH::Body &inBody1, const JPH::Body &inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult &inCollisionResult) override;
        virtual void OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override;
        virtual void OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override;
        virtual void OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) override;
    };
}

#endif