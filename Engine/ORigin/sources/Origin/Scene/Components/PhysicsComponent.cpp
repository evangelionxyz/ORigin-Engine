#include "pch.h"
#include "Origin/Physics/Physics.hpp"
#include "Origin/Physics/Jolt/JoltScene.hpp"
#include "Origin/Physics/PhysX/PhysXScene.hpp"

#include "PhysicsComponents.h"

#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyInterface.h>

namespace origin
{
    void RigidbodyComponent::AddForce(const glm::vec3 &force)
    {
        switch (Physics::GetAPI())
        {
        case PhysicsAPI::Jolt:
        {
            JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
            JoltScene::GetInstance()->GetBodyInterface()->AddForce(body->GetID(), GlmToJoltVec3(force));
            break;
        }
        case PhysicsAPI::PhysX:
        {
            physx::PxRigidDynamic *actor = static_cast<physx::PxRigidDynamic *>(Body);
            actor->addForce(Math::GlmToPhysXVec3(force), physx::PxForceMode::eFORCE);
            break;
        }
        }
    }

    void RigidbodyComponent::AddTorque(const glm::vec3 &torque)
    {
        switch (Physics::GetAPI())
        {
        case PhysicsAPI::Jolt:
        {
            JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
            JoltScene::GetInstance()->GetBodyInterface()->AddTorque(body->GetID(), GlmToJoltVec3(torque));
            break;
        }
        case PhysicsAPI::PhysX:
        {
            physx::PxRigidDynamic *actor = static_cast<physx::PxRigidDynamic *>(Body);
            actor->addTorque(Math::GlmToPhysXVec3(torque), physx::PxForceMode::eFORCE);
            break;
        }
        }
    }

    void RigidbodyComponent::AddForceAndTorque(const glm::vec3 &force, const glm::vec3 &torque)
    {
        switch (Physics::GetAPI())
        {
        case PhysicsAPI::Jolt:
        {
            JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
            JoltScene::GetInstance()->GetBodyInterface()->AddForceAndTorque(body->GetID(), GlmToJoltVec3(force), GlmToJoltVec3(torque));
            break;
        }
        case PhysicsAPI::PhysX:
        {
            AddForce(force);
            AddTorque(torque);
            break;
        }
        }
    }

    void RigidbodyComponent::AddAngularImpulse(const glm::vec3 &impulse)
    {
        switch (Physics::GetAPI())
        {
        case PhysicsAPI::Jolt:
        {
            JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
            JoltScene::GetInstance()->GetBodyInterface()->AddAngularImpulse(body->GetID(), GlmToJoltVec3(impulse));
            break;
        }
        case PhysicsAPI::PhysX:
        {
            physx::PxRigidDynamic *actor = static_cast<physx::PxRigidDynamic *>(Body);
            // actor->angular(Math::GlmToPhysXVec3(force), physx::PxForceMode::eFORCE);
            break;
        }
        }
    }

    void RigidbodyComponent::ActivateBody()
    {
        switch (Physics::GetAPI())
        {
        case PhysicsAPI::Jolt:
        {
            JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
            JoltScene::GetInstance()->GetBodyInterface()->ActivateBody(body->GetID());
            break;
        }
        case PhysicsAPI::PhysX:
        {
            physx::PxRigidDynamic *actor = static_cast<physx::PxRigidDynamic *>(Body);
            break;
        }
        }
        JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
        //PhysicsEngine::GetBodyInterface()->
    }

    void RigidbodyComponent::DeactivateBody()
    {
        JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
        //PhysicsEngine::GetBodyInterface()->DeactivateBody(body->GetID());
    }

    void RigidbodyComponent::DestroyBody()
    {
        JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
        //PhysicsEngine::GetBodyInterface()->DestroyBody(body->GetID());
    }

    bool RigidbodyComponent::IsActive()
    {
        JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
        return false; //PhysicsEngine::GetBodyInterface()->IsActive(body->GetID());
    }

    void RigidbodyComponent::MoveKinematic(const glm::vec3 &targetPosition, const glm::vec3 &targetRotation, float deltaTime)
    {
        JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
        //PhysicsEngine::GetBodyInterface()->MoveKinematic(body->GetID(), GlmToJoltVec3(targetPosition), GlmToJoltQuat(targetRotation), deltaTime);
    }

    void RigidbodyComponent::AddImpulse(const glm::vec3 &impulse)
    {
        JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
        //PhysicsEngine::GetBodyInterface()->AddImpulse(body->GetID(), GlmToJoltVec3(impulse));
    }

    void RigidbodyComponent::AddLinearVelocity(const glm::vec3 &velocity)
    {
        JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
        //PhysicsEngine::GetBodyInterface()->AddLinearVelocity(body->GetID(), GlmToJoltVec3(velocity));
    }

    void RigidbodyComponent::SetPosition(const glm::vec3 &position, bool activate)
    {
        JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
        //PhysicsEngine::GetBodyInterface()->SetPosition(body->GetID(),
            //GlmToJoltVec3(position), activate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
    }

    void RigidbodyComponent::SetEulerAngleRotation(const glm::vec3 &rotation, bool activate)
    {
        JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);

        //PhysicsEngine::GetBodyInterface()->SetRotation(body->GetID(),
            //GlmToJoltQuat(rotation), activate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
    }

    void RigidbodyComponent::SetRotation(const glm::quat &rotation, bool activate)
    {
        JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);

        //PhysicsEngine::GetBodyInterface()->SetRotation(body->GetID(),
            //GlmToJoltQuat(rotation), activate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
    }

    void RigidbodyComponent::SetLinearVelocity(const glm::vec3 &vel)
    {
        JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
        //PhysicsEngine::GetBodyInterface()->SetLinearVelocity(body->GetID(), GlmToJoltVec3(vel));
    }

    void RigidbodyComponent::SetFriction(float value)
    {
        JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
        //PhysicsEngine::GetBodyInterface()->SetFriction(body->GetID(), value);
    }

    void RigidbodyComponent::SetRestitution(float value)
    {
        JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
        //PhysicsEngine::GetBodyInterface()->SetRestitution(body->GetID(), value);
    }

    void RigidbodyComponent::SetGravityFactor(float value)
    {
        JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
        //PhysicsEngine::GetBodyInterface()->SetGravityFactor(body->GetID(), value);
    }

    float RigidbodyComponent::GetRestitution()
    {
        JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
        //const auto &value; //PhysicsEngine::GetBodyInterface()->GetRestitution(body->GetID());
        return 0.0f;
    }

    float RigidbodyComponent::GetFriction()
    {
        JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
        //const auto &value = //PhysicsEngine::GetBodyInterface()->GetFriction(body->GetID());
        return 0.0f;
    }

    float RigidbodyComponent::GetGravityFactor()
    {
        JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
        // const auto &value = //PhysicsEngine::GetBodyInterface()->GetGravityFactor(body->GetID());
        return 0.0f;
    }

    glm::vec3 RigidbodyComponent::GetPosition()
    {
        JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
        // const auto &value = //PhysicsEngine::GetBodyInterface()->GetPosition(body->GetID());
        return glm::vec3(0.0f); //{ value.GetX(), value.GetY(), value.GetZ() };
    }

    glm::vec3 RigidbodyComponent::GetEulerAngles()
    {
        JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
        //const auto &value = //PhysicsEngine::GetBodyInterface()->GetRotation(body->GetID());
        return glm::vec3(0.0f); //glm::eulerAngles(JoltToGlmQuat(value));
    }

    glm::quat RigidbodyComponent::GetRotation()
    {
        JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
        //const auto &value = //PhysicsEngine::GetBodyInterface()->GetRotation(body->GetID());
        return glm::quat(1, 0, 0, 0); //JoltToGlmQuat(value);
    }

    glm::vec3 RigidbodyComponent::GetCenterOfMassPosition()
    {
        JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
        // const auto &value = //PhysicsEngine::GetBodyInterface()->GetCenterOfMassPosition(body->GetID());
        return glm::vec3(0.0f); //{ value.GetX(), value.GetY(), value.GetZ() };
    }

    glm::vec3 RigidbodyComponent::GetLinearVelocity()
    {
        JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
        //const auto &value = //PhysicsEngine::GetBodyInterface()->GetLinearVelocity(body->GetID());
        return glm::vec3(0.0f); //{ value.GetX(), value.GetY(), value.GetZ() };
    }

    void RigidbodyComponent::SetMaxLinearVelocity(float max)
    {
        JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
        body->GetMotionProperties()->SetMaxLinearVelocity(max);
    }

    void RigidbodyComponent::SetMaxAngularVelocity(float max)
    {
        JPH::Body *body = reinterpret_cast<JPH::Body *>(Body);
        body->GetMotionProperties()->SetMaxAngularVelocity(max);
    }

    void RigidbodyComponent::SetMass(float mass)
    {
    }

    void RigidbodyComponent::SetOffset(const glm::vec3 &offset)
    {
    }

    void RigidbodyComponent::SetCenterMass(const glm::vec3 &center)
    {
    }
}