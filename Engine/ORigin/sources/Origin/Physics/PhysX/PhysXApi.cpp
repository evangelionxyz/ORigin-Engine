// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "PhysXApi.hpp"

namespace origin {

static PhysXAPI *s_Instance = nullptr;

PhysXAPI::PhysXAPI()
{
    s_Instance = this;
}

void PhysXAPI::Init()
{
    const bool record_memory_allocations = true;

    m_Foundation = PxCreateFoundation(
        PX_PHYSICS_VERSION,
        m_Allocator,
        m_ErrorCallback
    );

    OGN_CORE_ASSERT(m_Foundation, "[PhysX] Failed to create physx foundation");

    m_CpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
    m_Pvd = PxCreatePvd(*m_Foundation);

    const int port = { 5425 };
    const int timeout_ms = { 10 };
    physx::PxPvdTransport *transport = physx::PxDefaultPvdSocketTransportCreate("localhost", port, timeout_ms);

    m_Pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

    m_Physics = PxCreatePhysics(
        PX_PHYSICS_VERSION,
        *m_Foundation,
        physx::PxTolerancesScale(),
        record_memory_allocations,
        m_Pvd
    );

    OGN_CORE_ASSERT(m_Physics, "[PhysX] Failed to create physx physics");

    const bool px_extension_initialized = PxInitExtensions(*m_Physics, m_Pvd);
    OGN_CORE_ASSERT(px_extension_initialized, "[PhysX] Failed to initialize extension");

    const i32 worker_threads = 4;
    m_CpuDispatcher = physx::PxDefaultCpuDispatcherCreate(worker_threads);
    OGN_CORE_ASSERT(m_CpuDispatcher, "[PhysX] Failed to create cpu dispatcher");

    OGN_CORE_INFO("[PhysX] Initalized");
}

void PhysXAPI::Shutdown()
{
    m_CpuDispatcher->release();
    m_CpuDispatcher = nullptr;

    PxCloseExtensions();

    m_Physics->release();
    m_Physics = nullptr;

    m_Foundation->release();
    m_Foundation = nullptr;

    OGN_CORE_INFO("[PhysX] Shutdown");
}

PhysXAPI *PhysXAPI::GetInstance()
{
    return s_Instance;
}

}