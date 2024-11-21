// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "PhysXScene.hpp"
#include "PhysXApi.hpp"

#include "Origin/Scene/Scene.h"
#include "Origin/Scene/Entity.h"

#include <PxPhysicsAPI.h>

#include <PxActor.h>

namespace origin {

PhysXScene::PhysXScene(Scene *scene_ctx)
    : PhysicsSceneBase(scene_ctx), m_PhysXScene(nullptr),
    m_Material(nullptr)
{
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
        tc.Translation = utils::FromPhysXVec3(pose.p);
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
    auto view = m_SceneCtx->m_Registry.view<RigidbodyComponent>();
    for (auto id : view)
    {
        Entity entity = { id, m_SceneCtx };
        InstantiateEntity(entity);
    }

}

void PhysXScene::StopSimulation()
{
    if (m_PhysXScene)
    {
        auto view = m_SceneCtx->m_Registry.view<RigidbodyComponent>();
        for (auto id : view)
        {
            Entity entity = { id, m_SceneCtx };
            DestroyEntity(entity);
        }

        m_PhysXScene->release();
        m_PhysXScene = nullptr;
    }
}

void PhysXScene::InstantiateEntity(Entity entity)
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
            auto &cc = entity.GetComponent<CapsuleColliderComponent>();
            /*cc.Shape = CreateCapsuleCollider(entity, cc.HalfHeight, cc.Radius, rb);
            rb.SetFriction(cc.Friction);
            rb.SetRestitution(cc.Restitution);*/
        }
        else if (entity.HasComponent<SphereColliderComponent>())
        {
            auto &cc = entity.GetComponent<SphereColliderComponent>();
            /*cc.Shape = CreateSphereCollider(entity, cc.Radius, rb);
            rb.SetFriction(cc.Friction);
            rb.SetRestitution(cc.Restitution);*/
        }
    }
}

void PhysXScene::DestroyEntity(Entity entity)
{

}

physx::PxRigidActor *PhysXScene::CreateBody(RigidbodyComponent &rb, const glm::vec3 &pos, const glm::quat &rot)
{
    physx::PxPhysics *physics = PhysXAPI::GetInstance()->m_Physics;

    physx::PxTransform pose = physx::PxTransform(utils::ToPhysXVec3(pos), utils::ToPhysXQuat(rot));
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

    auto center_mass = utils::ToPhysXVec3(rb.CenterMass);

    physx::PxRigidBodyExt::updateMassAndInertia(*actor, rb.Mass, &center_mass);
    rb.Body = static_cast<void *>(actor);

    return actor;
}

void PhysXScene::CreateBoxCollider(Entity entity)
{
    physx::PxPhysics *physics = PhysXAPI::GetInstance()->m_Physics;

    auto &tc = entity.GetComponent<TransformComponent>();
    auto &bc = entity.GetComponent<BoxColliderComponent>();
    auto &rb = entity.GetComponent<RigidbodyComponent>();

    physx::PxRigidActor *actor = CreateBody(rb, tc.WorldTranslation, tc.WorldRotation);
    physx::PxTransform relative_pos(utils::ToPhysXVec3(rb.Offset));
    physx::PxMaterial *material = physics->createMaterial(bc.StaticFriction, bc.Friction, bc.Restitution);

    physx::PxShape *box_shape = physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxBoxGeometry(utils::ToPhysXVec3(tc.Scale * (bc.Scale / 2.0f))), *material);

    box_shape->setLocalPose(relative_pos);
    bc.Shape = static_cast<void *>(box_shape);

    m_PhysXScene->addActor(*actor);
}

}
