// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef JOLT_BODY_ACTIVATION_LISTENER_H
#define JOLT_BODY_ACTIVATION_LISTENER_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

namespace origin
{
    class JoltBodyActivationListener : public JPH::BodyActivationListener
    {
    public:
        virtual void OnBodyActivated(const JPH::BodyID &inBodyID, uint64_t inBodyUserData) override;
        virtual void OnBodyDeactivated(const JPH::BodyID &inBodyID, uint64_t inBodyUserData) override;
    };
}
#endif