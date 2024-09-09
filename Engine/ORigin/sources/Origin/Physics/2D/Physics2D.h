// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef PHYSICS_2D_H
#define PHYSICS_2D_H

#include "Origin/Scene/Entity.h"
#include "Origin/Scene/Components/Components.h"

#include <box2d/box2d.h>
#include <box2d/types.h>
#include <box2d/math_functions.h>

class b2World;

namespace origin {

	class Scene;

	class Physics2D
	{
	public:
		Physics2D(Scene* scene);

		void CreateRevoluteJoint(RevoluteJoint2DComponent *joint, b2BodyId body_id);
		void CreateBoxCollider(BoxCollider2DComponent *bc2d, b2BodyId body_id, b2Vec2 size);
		void CreateCircleCollider(CircleCollider2DComponent *cc2d, b2BodyId body_id, float radius);

		void Simulate(float deltaTime);
		void OnSimulationStart();
		void OnInstantiateEntity(Entity entity);
		void OnDestroyEntity(Entity entity);
		void OnSimulationStop();
		
	private:
		Scene* m_Scene = nullptr;
		b2WorldId m_WorldId;
	};
}

#endif