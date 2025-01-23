// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "JoltScene.hpp"

#include "JoltApi.hpp"

namespace origin {

static constexpr unsigned int cNumBodies = 20480;
static constexpr unsigned int cNumBodyMutexes = 0;
static constexpr unsigned int cMaxBodyPairs = 64000;
static constexpr unsigned int cMaxContactConstraints = 20480;

JoltScene::JoltScene(Scene *scene_ctx)
    : PhysicsSceneBase(scene_ctx)
{
}

void JoltScene::Simulate(float delta_time)
{
    JoltApi *jolt = JoltApi::GetInstance();

    for (const auto id : m_SceneCtx->m_Registry.view<RigidbodyComponent>())
    {
        Entity entity = { id, m_SceneCtx };
        const auto &rb = entity.GetComponent<RigidbodyComponent>();
        auto &tc = entity.GetComponent<TransformComponent>();
        auto &idc = entity.GetComponent<IDComponent>();

        auto *body = reinterpret_cast<JPH::Body *>(rb.Body);
        if (!body) continue;

        tc.WorldTranslation = JoltToGlmVec3(body->GetPosition());
        tc.WorldRotation = JoltToGlmQuat(body->GetRotation());
        tc.Translation = tc.WorldTranslation;
        tc.Rotation = tc.WorldRotation;
    }

    m_Physics.Update(delta_time, 1, jolt->m_TempAllocator.get(), jolt->m_JobSystem.get());

}

void JoltScene::StartSimulation()
{
    JoltApi *jolt = JoltApi::GetInstance();

    m_Physics.Init(cNumBodies, cNumBodyMutexes, cMaxBodyPairs,
        cMaxContactConstraints, jolt->m_BroadPhaseLayer,
        jolt->m_ObjectVsBroadPhaseLayerFilter,
        jolt->m_ObjectLayerPairFilter);

    m_Physics.SetBodyActivationListener(jolt->m_BodyActivationListener.get());
    m_Physics.SetContactListener(jolt->m_ContactListener.get());
    m_Physics.SetGravity(JPH::Vec3(0.0f, -9.8f, 0.0f));
    m_Physics.OptimizeBroadPhase();

    m_BodyInterface = &m_Physics.GetBodyInterface();

    for (const auto id : m_SceneCtx->m_Registry.view<RigidbodyComponent>())
    {
        InstantiateEntity({ id, m_SceneCtx });
    }
}

void JoltScene::StopSimulation()
{
    for (const auto id : m_SceneCtx->m_Registry.view<RigidbodyComponent>())
    {
        DestroyEntity({ id, m_SceneCtx });
    }
}

void JoltScene::InstantiateEntity(Entity entity)
{
    if (entity.HasComponent<RigidbodyComponent>())
    {
        auto &rb = entity.GetComponent<RigidbodyComponent>();

        if (entity.HasComponent<BoxColliderComponent>())
        {
            CreateBoxCollider(entity);
        }
        else if (entity.HasComponent<CapsuleColliderComponent>())
        {
        }
        else if (entity.HasComponent<SphereColliderComponent>())
        {
        }
    }

}

void JoltScene::DestroyEntity(Entity entity)
{
    if (entity.HasComponent<RigidbodyComponent>())
    {
        auto &rb = entity.GetComponent<RigidbodyComponent>();
        auto body = reinterpret_cast<JPH::Body *>(rb.Body);
        if (body)
        {
            m_BodyInterface->RemoveBody(body->GetID());
            m_BodyInterface->DestroyBody(body->GetID());
            body = nullptr;
        }
    }
}

JPH::BodyCreationSettings JoltScene::CreateBody(JPH::ShapeRefC shape, RigidbodyComponent &rb, const glm::vec3 &pos, const glm::quat &rot)
{
    JPH::BodyCreationSettings bodySettings(shape, GlmToJoltVec3(pos), GlmToJoltQuat(rot),
        rb.IsStatic ? JPH::EMotionType::Static : JPH::EMotionType::Dynamic,
        rb.IsStatic ? Layers::NON_MOVING : Layers::MOVING);

    bodySettings.mAllowedDOFs = JPH::EAllowedDOFs::None;
    if (rb.RotateX) bodySettings.mAllowedDOFs |= JPH::EAllowedDOFs::RotationX;
    if (rb.RotateY) bodySettings.mAllowedDOFs |= JPH::EAllowedDOFs::RotationY;
    if (rb.RotateZ) bodySettings.mAllowedDOFs |= JPH::EAllowedDOFs::RotationZ;
    if (rb.MoveX) bodySettings.mAllowedDOFs |= JPH::EAllowedDOFs::TranslationX;
    if (rb.MoveY) bodySettings.mAllowedDOFs |= JPH::EAllowedDOFs::TranslationY;
    if (rb.MoveZ) bodySettings.mAllowedDOFs |= JPH::EAllowedDOFs::TranslationZ;

    switch (rb.MotionQuality)
    {
    case RigidbodyComponent::EMotionQuality::Discrete:
        bodySettings.mMotionQuality = JPH::EMotionQuality::Discrete;
        break;
    case RigidbodyComponent::EMotionQuality::LinearCast:
        bodySettings.mMotionQuality = JPH::EMotionQuality::LinearCast;
        break;
    }

    return bodySettings;
}

JoltScene *JoltScene::GetInstance()
{
    return nullptr;
}

JPH::BodyInterface *JoltScene::GetBodyInterface() const
{
    return m_BodyInterface;
}

void JoltScene::CreateBoxCollider(Entity entity)
{
    auto &tc = entity.GetComponent<TransformComponent>();
    auto &rb = entity.GetComponent<RigidbodyComponent>();
    auto &bc = entity.GetComponent<BoxColliderComponent>();

    glm::vec3 halfExtents = bc.Scale / 2.0f;
    JPH::BoxShapeSettings shapeSettings(GlmToJoltVec3(halfExtents));

    JPH::ShapeSettings::ShapeResult shapeResult = shapeSettings.Create();
    JPH::ShapeRefC shape = shapeResult.Get();

    JPH::BodyCreationSettings bodySettings = CreateBody(shape, rb, tc.Translation + rb.Offset, tc.Rotation);

    if (JPH::Body *body = m_BodyInterface->CreateBody(bodySettings))
    {
        JPH::BodyID bodyId = body->GetID();
        m_BodyInterface->AddBody(bodyId, JPH::EActivation::Activate);
        m_BodyInterface->SetFriction(bodyId, bc.Friction);
        m_BodyInterface->SetRestitution(bodyId, bc.Restitution);
        rb.Body = body;
    }

    bc.Shape = (void *)shape.GetPtr();
}

void JoltScene::CreateCapsuleCollider(Entity entity)
{
}

void JoltScene::CreateSphereCollider(Entity entity)
{
}

}