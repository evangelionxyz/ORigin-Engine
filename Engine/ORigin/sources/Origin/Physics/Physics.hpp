// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include "PhysicsApiBase.hpp"

namespace origin {

enum class PhysicsAPI
{
    None = -1, Jolt = 0, PhysX
};

class Physics
{
public:
    static void Init(PhysicsAPI api);
    static void Shutdown();

    static void SetAPI(PhysicsAPI api);
    static PhysicsAPI GetAPI();

    static Ref<PhysicsAPIBase> GetPhysicsContext();

private:
    static Ref<PhysicsAPIBase> s_PhysicsAPIContext;
    static PhysicsAPI s_PhysicsAPI;
};

}

#endif