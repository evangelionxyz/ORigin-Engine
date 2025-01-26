// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "PhysXScene.hpp"
#include "PhysXApi.hpp"

#include "Origin/Scene/Scene.h"
#include "Origin/Scene/Entity.h"

#include <PxPhysicsAPI.h>

#include <PxActor.h>

namespace origin {

static PhysXScene *s_Instance = nullptr;

PhysXScene::PhysXScene(Scene *scene_ctx)
    : PhysicsSceneBase(scene_ctx), m_PhysXScene(nullptr),
    m_Material(nullptr)
{
    s_Instance = this;
}

void PhysXScene::Simulate(float delta_time)
{
    m_PhysXScene->simulate(delta_time);
    m_PhysXScene->fetchResults(true);

    auto view = m_SceneCtx->m_Registry.view<RigidbodyComponent>();
    for (auto id : view)
    {
        Entity entity = { id, m_SceneCtx };

        auto &rb = entity.GetComponent<RigidbodyComponent>();
        auto &tc = entity.GetComponent<TransformComponent>();

        if (!rb.Body) continue;

        physx::PxRigidActor *actor = static_cast<physx::PxRigidActor *>(rb.Body);
        physx::PxTransform pose = actor->getGlobalPose();
        tc.Translation = Math::PhysXToGlmVec3(pose.p);
        tc.Rotation = glm::quat(pose.q.w, pose.q.x, pose.q.y, pose.q.z);
    }
}

void PhysXScene::StartSimulation()
{
    // create new scene
    physx::PxSceneDesc scene_desc(PhysXAPI::GetInstance()->m_Physics->getTolerancesScale());
    scene_desc.gravity = physx::PxVec3(0.0f, -9.8f, 0.0f);
    scene_desc.cpuDispatcher = PhysXAPI::GetInstance()->m_CpuDispatcher;
    scene_desc.filterShader = physx::PxDefaultSimulationFilterShader;

    m_PhysXScene = PhysXAPI::GetInstance()->m_Physics->createScene(scene_desc);

    // create entity's components
    for (const auto id : m_SceneCtx->m_Registry.view<RigidbodyComponent>())
    {
        InstantiateEntity({ id, m_SceneCtx });
    }
}

void PhysXScene::StopSimulation()
{
    if (m_PhysXScene)
    {
        for (const auto id : m_SceneCtx->m_Registry.view<RigidbodyComponent>())
        {
            DestroyEntity({ id, m_SceneCtx });
        }
        m_PhysXScene->release();
        m_PhysXScene = nullptr;
    }
}

void PhysXScene::InstantiateEntity(Entity entity)
{
    if (entity.HasComponent<RigidbodyComponent>())
    {
        if (entity.HasComponent<BoxColliderComponent>())
        {
            CreateBoxCollider(entity);
        }
        else if (entity.HasComponent<CapsuleColliderComponent>())
        {
            CreateCapsueCollider(entity);
        }
        else if (entity.HasComponent<SphereColliderComponent>())
        {
            CreateSphereCollider(entity);
        }
    }
}

void PhysXScene::DestroyEntity(Entity entity)
{
    if (entity.HasComponent<RigidbodyComponent>())
    {
        auto &rb = entity.GetComponent<RigidbodyComponent>();
        physx::PxRigidActor *actor = static_cast<physx::PxRigidActor *>(rb.Body);
        physx::PxShape *shape = nullptr;

        if (entity.HasComponent<BoxColliderComponent>())
        {
            auto &c = entity.GetComponent<BoxColliderComponent>();
            shape = static_cast<physx::PxShape *>(c.Shape);
        }
        else if (entity.HasComponent<CapsuleColliderComponent>())
        {
            auto &c = entity.GetComponent<CapsuleColliderComponent>();
            shape = static_cast<physx::PxShape *>(c.Shape);
        }
        else if (entity.HasComponent<SphereColliderComponent>())
        {
            auto &c = entity.GetComponent<SphereColliderComponent>();
            shape = static_cast<physx::PxShape *>(c.Shape);
        }

        if (actor)
        {
            if (shape)
                actor->detachShape(*shape);
            actor->release();
        }
    }
}

physx::PxRigidActor *PhysXScene::CreateBody(RigidbodyComponent &rb, const glm::vec3 &pos, const glm::quat &rot)
{
    physx::PxPhysics *physics = PhysXAPI::GetInstance()->m_Physics;

    physx::PxTransform pose = physx::PxTransform(Math::GlmToPhysXVec3(pos), Math::GlmToPhysXQuat(rot));
    physx::PxRigidDynamic *actor = physics->createRigidDynamic(pose);

    OGN_CORE_ASSERT(actor, "[PhysX] Failed to create actor");

    actor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, rb.IsStatic);

    if (rb.IsStatic)
    {
        actor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eRETAIN_ACCELERATIONS, rb.RetainAcceleration);
    }

    actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, !rb.RotateX);
    actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, !rb.RotateY);
    actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, !rb.RotateZ);

    actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, !rb.MoveX);
    actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, !rb.MoveY);
    actor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, !rb.MoveZ);

    const physx::PxVec3 center_mass = Math::GlmToPhysXVec3(rb.CenterMass);

    physx::PxRigidBodyExt::updateMassAndInertia(*actor, rb.Mass, &center_mass);
    rb.Body = static_cast<void *>(actor);

    return actor;
}

PhysXScene *PhysXScene::GetInstance()
{
    return s_Instance;
}

void PhysXScene::CreateBoxCollider(Entity entity)
{
    physx::PxPhysics *physics = PhysXAPI::GetInstance()->m_Physics;

    auto &tc = entity.GetComponent<TransformComponent>();
    auto &bc = entity.GetComponent<BoxColliderComponent>();
    auto &rb = entity.GetComponent<RigidbodyComponent>();

    physx::PxRigidActor *actor = CreateBody(rb, tc.WorldTranslation, tc.WorldRotation);
    physx::PxTransform relative_pos(Math::GlmToPhysXVec3(rb.Offset));
    physx::PxMaterial *material = physics->createMaterial(bc.StaticFriction, bc.Friction, bc.Restitution);

    physx::PxShape *box_shape = physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxBoxGeometry(Math::GlmToPhysXVec3(tc.Scale * (bc.Scale / 2.0f))), *material);

    box_shape->setLocalPose(relative_pos);
    bc.Shape = static_cast<void *>(box_shape);

    m_PhysXScene->addActor(*actor);
}

void PhysXScene::CreateSphereCollider(Entity entity)
{
    physx::PxPhysics *physics = PhysXAPI::GetInstance()->m_Physics;

    auto &tc = entity.GetComponent<TransformComponent>();
    auto &sc = entity.GetComponent<SphereColliderComponent>();
    auto &rb = entity.GetComponent<RigidbodyComponent>();

    physx::PxRigidActor *actor = CreateBody(rb, tc.WorldTranslation, tc.WorldRotation);
    physx::PxTransform relative_pos(Math::GlmToPhysXVec3(rb.Offset));
    physx::PxMaterial *material = physics->createMaterial(sc.StaticFriction, sc.Friction, sc.Restitution);

    physx::PxShape *sphere_shape = physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxSphereGeometry(tc.Scale.x * sc.Radius * 2.0f), *material);

    sphere_shape->setLocalPose(relative_pos);
    sc.Shape = static_cast<void *>(sphere_shape);

    m_PhysXScene->addActor(*actor);
}

void PhysXScene::CreateCapsueCollider(Entity entity)
{
    physx::PxPhysics *physics = PhysXAPI::GetInstance()->m_Physics;

    auto &tc = entity.GetComponent<TransformComponent>();
    auto &cc = entity.GetComponent<CapsuleColliderComponent>();
    auto &rb = entity.GetComponent<RigidbodyComponent>();

    physx::PxRigidActor *actor = CreateBody(rb, tc.WorldTranslation, tc.WorldRotation);

    physx::PxTransform relative_pos(Math::GlmToPhysXVec3(rb.Offset), physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0, 0, 1)));

    physx::PxMaterial *material = physics->createMaterial(cc.StaticFriction, cc.Friction, cc.Restitution);

    physx::PxShape *capsule_shape = physx::PxRigidActorExt::createExclusiveShape(
        *actor, 
        physx::PxCapsuleGeometry(cc.Radius, tc.Scale.y * (cc.HalfHeight / 2.0f)), 
        *material);

    capsule_shape->setLocalPose(relative_pos);

    cc.Shape = static_cast<void *>(capsule_shape);

    m_PhysXScene->addActor(*actor);
}

}
