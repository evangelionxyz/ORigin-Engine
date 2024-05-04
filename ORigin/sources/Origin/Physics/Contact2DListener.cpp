// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Contact2DListener.h"
#include "origin/Scene/Entity.h"

#include <glm/glm.hpp>

namespace origin {

	Contact2DListener::Contact2DListener(Scene* scene)
		: m_Scene(scene)
	{
	}

	void Contact2DListener::BeginContact(b2Contact* contact)
	{
		OGN_PROFILER_PHYSICS();

		b2Body* bodyA = contact->GetFixtureA()->GetBody();
		b2Body* bodyB = contact->GetFixtureB()->GetBody();

		Entity entityA{ static_cast<entt::entity>(bodyA->GetUserData().pointer), m_Scene };
		Entity entityB{ static_cast<entt::entity>(bodyB->GetUserData().pointer), m_Scene };

		auto& rb2dA = entityA.GetComponent<Rigidbody2DComponent>();
		auto& rb2dB = entityB.GetComponent<Rigidbody2DComponent>();

		rb2dA.ContactWith = entityB.GetTag();
		rb2dB.ContactWith = entityA.GetTag();
	}

	void Contact2DListener::EndContact(b2Contact* contact)
	{
		OGN_PROFILER_PHYSICS();

		b2Body* bodyA = contact->GetFixtureA()->GetBody();
		b2Body* bodyB = contact->GetFixtureB()->GetBody();

		Entity entityA{ static_cast<entt::entity>(bodyA->GetUserData().pointer), m_Scene };
		Entity entityB{ static_cast<entt::entity>(bodyB->GetUserData().pointer), m_Scene };

		auto& rb2dA = entityA.GetComponent<Rigidbody2DComponent>();
		auto& rb2dB = entityB.GetComponent<Rigidbody2DComponent>();

		rb2dA.ContactWith = "";
		rb2dB.ContactWith = "";
	}
}
