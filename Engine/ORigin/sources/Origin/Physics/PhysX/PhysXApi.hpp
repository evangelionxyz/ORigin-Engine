// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine


#include <PxPhysicsAPI.h>
#include <glm/glm.hpp>

#include "../PhysicsApiBase.hpp"

#include "Origin/Math/Math.h"

namespace origin {

class Scene;

class PhysXAllocator : public physx::PxAllocatorCallback
{
public:
    void *allocate(size_t size, const char *, const char *, int)
    {
        void *ptr = physx::platformAlignedAlloc(size);
        PX_ASSERT((reinterpret_cast<size_t>(ptr) & 15) == 0);
        return ptr;
    }

    void deallocate(void *ptr)
    {
        physx::platformAlignedFree(ptr);
    }
};

class PhysXErrorCallback : public physx::PxErrorCallback
{
public:
    virtual void reportError(physx::PxErrorCode::Enum code, const char *message, const char *file, int line) override
    {
        const char *error_message = nullptr;
        switch (code)
        {
        case physx::PxErrorCode::eNO_ERROR: error_message = ""; break;
        case physx::PxErrorCode::eDEBUG_INFO: error_message = "Info"; break;
        case physx::PxErrorCode::eDEBUG_WARNING: error_message = "Warning"; break;
        case physx::PxErrorCode::eINVALID_PARAMETER: error_message = "Invalid Parameter"; break;
        case physx::PxErrorCode::eINVALID_OPERATION: error_message = "Invalid Operation"; break;
        case physx::PxErrorCode::eOUT_OF_MEMORY: error_message = "Out Of Memory"; break;
        case physx::PxErrorCode::eINTERNAL_ERROR: error_message = "Internal Error"; break;
        case physx::PxErrorCode::eABORT: error_message = "Abort"; break;
        case physx::PxErrorCode::ePERF_WARNING: error_message = "Performance Warning"; break;
        case physx::PxErrorCode::eMASK_ALL: error_message = "Mask All"; break;
        }

        message = error_message;
    }
};

class PhysXAPI : public PhysicsAPIBase
{
public:
    PhysXAPI();

    void Init() override;
    void Shutdown() override;

    static PhysXAPI *GetInstance();

private:
    physx::PxPhysics *m_Physics;
    physx::PxFoundation *m_Foundation;
    physx::PxDefaultCpuDispatcher *m_CpuDispatcher;
    physx::PxPvd *m_Pvd;

    PhysXAllocator m_Allocator;
    PhysXErrorCallback m_ErrorCallback;

    friend class PhysXScene;
};

}