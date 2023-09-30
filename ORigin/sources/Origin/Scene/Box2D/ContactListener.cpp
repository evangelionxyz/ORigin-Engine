#include "pch.h"
#include "ContactListener.h"

#include "..\Entity.h"

#include <glm/glm.hpp>

namespace origin {

	ContactListener::ContactListener(Scene* scene)
		: m_Scene(scene)
	{
	}

	void ContactListener::BeginContact(b2Contact* contact)
	{
		b2Body* bodyA = contact->GetFixtureA()->GetBody();
		b2Body* bodyB = contact->GetFixtureB()->GetBody();

		Entity entityA{ static_cast<entt::entity>(bodyA->GetUserData().pointer), m_Scene };
		Entity entityB{ static_cast<entt::entity>(bodyB->GetUserData().pointer), m_Scene };

		auto& rb2dA = entityA.GetComponent<Rigidbody2DComponent>();
		auto& rb2dB = entityB.GetComponent<Rigidbody2DComponent>();

		rb2dA.ContactWith = entityB.GetTag();
		rb2dB.ContactWith = entityA.GetTag();
	}

	void ContactListener::EndContact(b2Contact* contact)
	{
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
