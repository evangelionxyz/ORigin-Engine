// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include "PhysicsApiBase.hpp"

namespace origin {

enum class PhysicsAPI
{
    None = -1, Jolt = 0, PhysX
};

static std::string PhysicsApiTostring(PhysicsAPI api)
{
    switch (api)
    {
    case origin::PhysicsAPI::None: return "None";
    case origin::PhysicsAPI::Jolt: return "Jolt";
    case origin::PhysicsAPI::PhysX: return "PhysX";
    }
    return "Invalid";
}

static PhysicsAPI PhysicsApiFromString(const std::string &api_str)
{
    if (api_str == "Jolt") return PhysicsAPI::Jolt;
    else if (api_str == "PhysX") return PhysicsAPI::PhysX;
    return PhysicsAPI::None;
}

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