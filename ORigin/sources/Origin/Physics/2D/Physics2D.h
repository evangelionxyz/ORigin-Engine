#pragma once
#include "Origin/Scene/Entity.h"
#include "Origin/Scene/Components/Components.h"
#include "box2d/b2_body.h"

class b2World;

namespace origin {

	class Scene;
	class Contact2DListener;

	class Physics2D
	{
	public:
		Physics2D(Scene* scene);

		void CreateRevoluteJoint(RevoluteJoint2DComponent rjc, b2Body* body, b2Vec2 anchorPoint);
		void CreateBoxCollider(BoxCollider2DComponent bc2d, b2Body* body, b2Vec2 boxSize);
		void CreateCircleCollider(CircleCollider2DComponent cc2d, b2Body* body, float radius);

		void Simulate(float deltaTime);
		void OnSimulationStart();
		void OnInstantiateScriptEntity(Entity entity);
		void OnDestroyScriptEntity(Entity entity);
		void OnSimulationStop();
		
	private:
		Scene* m_Scene = nullptr;
		b2World* m_World = nullptr;
		Contact2DListener* m_ContactListener = nullptr;
	};
}

