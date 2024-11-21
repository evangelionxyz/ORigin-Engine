// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef PHYSX_SCENE_HPP
#define PHYSX_SCENE_HPP

#include "../PhysicsSceneBase.hpp"

#include <PxPhysicsAPI.h>

namespace origin {

class PhysXScene : public PhysicsSceneBase
{
public:
    PhysXScene(Scene *scene_ctx);

    void Simulate(float delta_time) override;
    void StartSimulation() override;
    void StopSimulation() override;

    void InstantiateEntity(Entity entity) override;
    void DestroyEntity(Entity entity) override;

    physx::PxRigidActor *CreateBody(RigidbodyComponent &rb, const glm::vec3 &pos, const glm::quat &rot);

private:
    void CreateBoxCollider(Entity entity);

    physx::PxScene *m_PhysXScene;
    physx::PxMaterial *m_Material;
    physx::PxTolerancesScale m_ToleranceScale;
};

}

#endif

