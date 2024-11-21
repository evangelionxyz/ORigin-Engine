// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef PHYSICS_COMPONENTS_H
#define PHYSICS_COMPONENTS_H

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

        enum EMotionQuality
        {
            Discrete = 0,
            LinearCast = 1
        };

        EMotionQuality MotionQuality = Discrete;

        bool UseGravity = true;
        bool RotateX = true, RotateY = true, RotateZ = true;
        bool MoveX = true, MoveY = true, MoveZ = true;
        bool IsStatic = false;
        float Mass = 1.0f;
        bool AllowSleeping = true;
        bool RetainAcceleration = false;
        float GravityFactor = 1.0f;
        glm::vec3 CenterMass = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Offset = { 0.0f, 0.0f, 0.0f };

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
        void SetMaxLinearVelocity(float max);
        void SetMaxAngularVelocity(float max);
        void SetMass(float mass);
        void SetOffset(const glm::vec3 &offset);
        void SetCenterMass(const glm::vec3 &center);
        void SetSleep(bool sleep);
        float GetRestitution();
        float GetFriction();
        float GetGravityFactor();

        glm::vec3 GetPosition();
        glm::vec3 GetEulerAngles();
        glm::quat GetRotation();
        glm::vec3 GetCenterOfMassPosition();
        glm::vec3 GetLinearVelocity();
    };

    class PhysicsCollider
    {
    public:
        float Friction = 0.6f;
        float StaticFriction = 0.6f;
        float Restitution = 0.6f;
        float Density = 1.0f;

        void *Shape = nullptr;
    };

    class BoxColliderComponent : public PhysicsCollider
    {
    public:
        glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };
        
        BoxColliderComponent() = default;
        BoxColliderComponent(const BoxColliderComponent &) = default;
    };

    class SphereColliderComponent : public PhysicsCollider
    {
    public:
        float Radius = 0.5f;

        SphereColliderComponent() = default;
        SphereColliderComponent(const SphereColliderComponent &) = default;
    };

    class CapsuleColliderComponent : public PhysicsCollider
    {
    public:
        float HalfHeight = 1.0f;
        float Radius = 0.5f;

        CapsuleColliderComponent() = default;
        CapsuleColliderComponent(const CapsuleColliderComponent &) = default;
    };
}

#endif