// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef JOLT_SCENE_HPP
#define JOLT_SCENE_HPP

#include "../PhysicsSceneBase.hpp"

#include <Jolt/Physics/Body/BodyInterface.h>

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

    static JoltScene *GetInstance();

private:
};
}

#endif // !JOLT_SCENE_HPP
