// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "JoltScene.hpp"

namespace origin {

JoltScene::JoltScene(Scene *scene_ctx)
    : PhysicsSceneBase(scene_ctx)
{

}

void JoltScene::Simulate(float delta_time)
{

}

void JoltScene::StartSimulation()
{

}

void JoltScene::StopSimulation()
{

}

void JoltScene::InstantiateEntity(Entity entity)
{
    if (entity.HasComponent<RigidbodyComponent>())
    {
        auto &rb = entity.GetComponent<RigidbodyComponent>();

        if (entity.HasComponent<BoxColliderComponent>())
        {
           // auto &cc = entity.GetComponent<BoxColliderComponent>();
           // cc.Shape = CreateBoxCollider(entity, cc.Scale, rb);
           // rb.SetFriction(cc.Friction);
           // rb.SetRestitution(cc.Restitution);
        }
        else if (entity.HasComponent<CapsuleColliderComponent>())
        {
            //auto &cc = entity.GetComponent<CapsuleColliderComponent>();
            //cc.Shape = CreateCapsuleCollider(entity, cc.HalfHeight, cc.Radius, rb);
            //rb.SetFriction(cc.Friction);
            //rb.SetRestitution(cc.Restitution);
        }
        else if (entity.HasComponent<SphereColliderComponent>())
        {
           // auto &cc = entity.GetComponent<SphereColliderComponent>();
           // cc.Shape = CreateSphereCollider(entity, cc.Radius, rb);
           // rb.SetFriction(cc.Friction);
           // rb.SetRestitution(cc.Restitution);
        }
    }
}

void JoltScene::DestroyEntity(Entity entity)
{
    if (entity.HasComponent<RigidbodyComponent>())
    {
        const auto &rb = entity.GetComponent<RigidbodyComponent>();

        const auto body = reinterpret_cast<JPH::Body *>(rb.Body);
        if (body)
        {
            //s_Data.BodyInterface->RemoveBody(body->GetID());
            //s_Data.BodyInterface->DestroyBody(body->GetID());
        }
    }
}

JoltScene *JoltScene::GetInstance()
{
    return nullptr;
}

}