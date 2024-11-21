// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "JoltApi.hpp"

namespace origin {

static JoltApi *s_Instance = nullptr;


static constexpr unsigned int cNumBodies = 20480;
static constexpr unsigned int cNumBodyMutexes = 0;
static constexpr unsigned int cMaxBodyPairs = 64000;
static constexpr unsigned int cMaxContactConstraints = 20480;
static constexpr int cMaxPhysicsJobs = 2048;

JoltApi::JoltApi()
{
    s_Instance = this;
}

void JoltApi::Init()
{
    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();
    m_TempAllocator = std::make_unique<JPH::TempAllocatorImpl>(32 * 1024 * 1024);
    m_JobSystem = std::make_unique<JPH::JobSystemThreadPool>(cMaxPhysicsJobs, 8,
        std::thread::hardware_concurrency() - 1);
}

void JoltApi::Shutdown()
{
    JPH::UnregisterTypes();
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}

JoltApi *JoltApi::GetInstance()
{
    return s_Instance;
}

}