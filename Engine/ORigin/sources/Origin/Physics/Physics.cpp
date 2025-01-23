// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Physics.hpp"

#include "PhysX/PhysXApi.hpp"
#include "Jolt/JoltApi.hpp"

namespace origin {

Ref<PhysicsAPIBase> Physics::s_PhysicsAPIContext;
PhysicsAPI Physics::s_PhysicsAPI;

void Physics::Init(PhysicsAPI api)
{
    Physics::s_PhysicsAPI = api;
    switch (Physics::s_PhysicsAPI)
    {
    case PhysicsAPI::PhysX:
    {
        s_PhysicsAPIContext = CreateRef<PhysXAPI>();
        s_PhysicsAPIContext->Init();
        break;
    }

    case PhysicsAPI::Jolt:
    {
        s_PhysicsAPIContext = CreateRef<JoltApi>();
        s_PhysicsAPIContext->Init();
        break;
    }

    default:
        throw std::runtime_error::exception();
        break;
    }
}

void Physics::Shutdown()
{
    if (s_PhysicsAPIContext)
        s_PhysicsAPIContext->Shutdown();
}

void Physics::SetAPI(PhysicsAPI api)
{
    Physics::s_PhysicsAPI = api;
    Physics::Shutdown();

    Physics::Init(api);
}

PhysicsAPI Physics::GetAPI()
{
    return Physics::s_PhysicsAPI;
}

}