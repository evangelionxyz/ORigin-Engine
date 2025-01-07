// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine


#include <PxPhysicsAPI.h>
#include <glm/glm.hpp>

#include "../PhysicsApiBase.hpp"

#include "Origin/Math/Math.h"

namespace origin {

namespace utils {
static physx::PxVec3 ToPhysXVec3(const glm::vec3 &vec)
{
    return physx::PxVec3{ vec.x, vec.y, vec.z };
}

static physx::PxQuat ToPhysXQuat(const glm::quat &quat)
{
    return physx::PxQuat{ quat.x, quat.y, quat.z, quat.w };
}

static glm::vec3 FromPhysXVec3(const physx::PxVec3 &vec)
{
    return glm::vec3{ vec.x, vec.y, vec.z };
}

static glm::quat FromPhysXQuat(const physx::PxQuat quat)
{
    return glm::quat{ quat.w, quat.x, quat.y, quat.z };
}

static glm::vec3 FromPhysXQuatToVec3(const physx::PxQuat &quat)
{
    glm::vec3 rotation;

    using namespace glm;
    using T = float;

    mat4 LocalMatrix(toMat4(glm::quat(quat.w, quat.x, quat.y, quat.z)));
    if (
        epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
        epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
        epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
    {
        LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
        LocalMatrix[3][3] = static_cast<T>(1);
    }

    vec3 Row[3];

    for (length_t i = 0; i < 3; ++i)
        for (length_t j = 0; j < 3; ++j)
            Row[i][j] = LocalMatrix[i][j];

    Row[0] = detail::scale(Row[0], static_cast<T>(1));
    Row[1] = detail::scale(Row[1], static_cast<T>(1));
    Row[2] = detail::scale(Row[2], static_cast<T>(1));

    rotation.y = asin(-Row[0][2]);
    if (cos(rotation.y) != 0)
    {
        rotation.x = atan2(Row[1][2], Row[2][2]);
        rotation.z = atan2(Row[0][1], Row[0][0]);
    }
    else
    {
        rotation.x = atan2(-Row[2][0], Row[1][1]);
        rotation.z = 0;
    }

    return rotation;
}

static physx::PxTransform PhysicsTransformfromTransformComponent(const glm::mat4 &transform)
{
    glm::vec3 position, rotation, scale;
    Math::DecomposeTransformEuler(transform, position, rotation, scale);
    return physx::PxTransform(ToPhysXVec3(position), ToPhysXQuat(rotation));
}

}

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