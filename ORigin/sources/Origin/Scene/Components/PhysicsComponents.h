#pragma once
#include <Jolt/Jolt.h>

#include <glm/glm.hpp>

namespace origin
{
    static JPH::Vec3 GlmToJoltVec3(const glm::vec3 &v)
    {
        return JPH::Vec3(v.x, v.y, v.z);
    }

    static glm::vec3 JoltToGlmVec3(const JPH::Vec3 &v)
    {
        return glm::vec3(v.GetX(), v.GetY(), v.GetZ());
    }

    static JPH::Quat GlmToJoltQuat(const glm::quat &q)
    {
        return JPH::Quat(q.x, q.y, q.z, q.w);
    }

    static glm::quat JoltToGlmQuat(const JPH::Quat &q)
    {
        return glm::quat(q.GetW(), q.GetX(), q.GetY(), q.GetZ());
    }

    class RigidbodyComponent
    {
    public:
        bool UseGravity = true;
        bool RotateX = true, RotateY = true, RotateZ = true;
        bool MoveX = true, MoveY = true, MoveZ = true;
        bool IsStatic = false;
        float Mass = 1.0f;
        glm::vec3 CenterMass = { 0.0f, 0.0f, 0.0f };

        void *Body = nullptr;
        RigidbodyComponent() = default;
        RigidbodyComponent(const RigidbodyComponent &) = default;

        void AddForce(const glm::vec3 &force);
        void AddTorque(const glm::vec3 &torque);
        void AddForceAndTorque(const glm::vec3 &force, const glm::vec3 &torque);
        void AddAngularImpulse(const glm::vec3 &impulse);
        void ActivateBody();
        void DeactivateBody();
        void DestroyBody();
        bool IsActive();
        void MoveKinematic(const glm::vec3 &targetPosition, const glm::vec3 &targetRotation, float deltaTime);
        void AddImpulse(const glm::vec3 &impulse);
        void AddLinearVelocity(const glm::vec3 &velocity);
        void SetRestitution(float value);
        void SetPosition(const glm::vec3 &position, bool activate);
        void SetEulerAngleRotation(const glm::vec3 &rotation, bool activate);
        void SetRotation(const glm::quat &rotation, bool activate);
        void SetLinearVelocity(const glm::vec3 &vel);
        void SetFriction(float value);
        void SetGravityFactor(float value);
        float GetRestitution();
        float GetFriction();
        float GetGravityFactor();
        glm::vec3 GetPosition();
        glm::vec3 GetEulerAngleRotation();
        glm::quat GetRotation();
        glm::vec3 GetCenterOfMassPosition();
        glm::vec3 GetLinearVelocity();
    };

    class BoxColliderComponent
    {
    public:
        glm::vec3 Size = { 0.5f, 0.5f, 0.5f };
        glm::vec3 Offset = { 0.0f, 0.0f, 0.0f };

        float Friction = 0.6f;
        float Restitution = 0.6f;

        void *Shape = nullptr;
        void *Body = nullptr;

        BoxColliderComponent() = default;
        BoxColliderComponent(const BoxColliderComponent &) = default;
    };

    class SphereColliderComponent
    {
    public:
        float Radius = 0.5f;
        glm::vec3 Offset = { 0.0f, 0.0f, 0.0f };

        float Friction = 0.6f;
        float Restitution = 0.6f;

        void *Shape = nullptr;
        void *Body = nullptr;

        SphereColliderComponent() = default;
        SphereColliderComponent(const SphereColliderComponent &) = default;
    };

    class CapsuleColliderComponent
    {
    public:
        float Friction = 0.6f;
        float Restitution = 0.6f;

        void *Shape = nullptr;
        void *Body = nullptr;

        CapsuleColliderComponent() = default;
        CapsuleColliderComponent(const CapsuleColliderComponent &) = default;
    };
}