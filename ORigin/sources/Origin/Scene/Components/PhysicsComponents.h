#pragma once
#include <glm/glm.hpp>

namespace origin
{
    // 2D Components


    // 3D Components
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


        void SetLinearVelocity(const glm::vec3 &);
    };

    class BoxColliderComponent
    {
    public:
        glm::vec3 Size = { 0.5f, 0.5f, 0.5f };

        float Friction = 0.6f;
        float Restitution = 0.0f;

        void *Shape = nullptr;
        void *Rigidbody = nullptr;

        BoxColliderComponent() = default;
        BoxColliderComponent(const BoxColliderComponent &) = default;
    };

    class SphereColliderComponent
    {
    public:
        float Radius = 0.5f;

        float Friction = 0.6f;
        float Restitution = 0.0f;

        void *Shape = nullptr;
        void *Rigidbody = nullptr;

        SphereColliderComponent() = default;
        SphereColliderComponent(const SphereColliderComponent &) = default;
    };

    class CapsuleColliderComponent
    {
    public:
        float Friction = 0.6f;
        float Restitution = 0.0f;

        void *Shape = nullptr;
        void *Rigidbody = nullptr;

        CapsuleColliderComponent() = default;
        CapsuleColliderComponent(const CapsuleColliderComponent &) = default;
    };
}