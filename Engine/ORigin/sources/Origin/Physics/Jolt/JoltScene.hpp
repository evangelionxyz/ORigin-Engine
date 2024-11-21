// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef JOLT_SCENE_HPP
#define JOLT_SCENE_HPP

#include "../PhysicsSceneBase.hpp"

#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/PhysicsSystem.h>

namespace origin {

class JoltScene : public PhysicsSceneBase
{
public:
    JoltScene(Scene *scene_ctx);

    void Simulate(float delta_time) override;

    void StartSimulation() override;
    void StopSimulation() override;

    void InstantiateEntity(Entity entity) override;
    void DestroyEntity(Entity entity) override;

    JPH::BodyCreationSettings CreateBody(JPH::ShapeRefC shape, RigidbodyComponent &rb, const glm::vec3 &pos, const glm::quat &rot);

    static JoltScene *GetInstance();
    JPH::BodyInterface *GetBodyInterface() const;

private:
    void CreateBoxCollider(Entity entity);
    void CreateCircleCollider(Entity entity);
    void CreateSphereCollider(Entity entity);

    JPH::BodyInterface *m_BodyInterface;
    JPH::PhysicsSystem m_Physics;
};
}

#endif // !JOLT_SCENE_HPP
