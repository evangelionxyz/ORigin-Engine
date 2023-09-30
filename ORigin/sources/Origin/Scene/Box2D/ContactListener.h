#pragma once

#include "..\Scene.h"
#include "box2d/box2d.h"

namespace origin {

	class ContactListener : public b2ContactListener
	{
	public:
		ContactListener(Scene* scene);
		void BeginContact(b2Contact* contact) override;
		void EndContact(b2Contact* contact) override;

	private:
		Scene* m_Scene = nullptr;
	};

}

