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

		OGN_CORE_TRACE("{} and {}", entityA.GetName(), entityB.GetName());
		
	}

	void ContactListener::EndContact(b2Contact* contact)
	{
		b2Body* body = contact->GetFixtureA()->GetBody();
	}
}
