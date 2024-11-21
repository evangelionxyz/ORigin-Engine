// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef PHYSICS_SCENE_HPP
#define PHYSICS_SCENE_HPP

#include "Origin/Scene/Entity.h"

namespace origin {

class Scene;
class PhysicsSceneBase
{
public:
    PhysicsSceneBase(Scene *scene_ctx) : m_SceneCtx(scene_ctx) {}

    virtual void Simulate(float delta_time) {};
    virtual void StartSimulation() {};
    virtual void StopSimulation() {};

    virtual void InstantiateEntity(Entity entity) {}
    virtual void DestroyEntity(Entity entity) {}

protected:
    Scene *m_SceneCtx;
};

}

#endif

