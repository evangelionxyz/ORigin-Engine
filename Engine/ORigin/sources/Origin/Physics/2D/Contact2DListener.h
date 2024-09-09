// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef CONTACT_2D_LISTENER_H
#define CONTACT_2D_LISTENER_H

#include "box2d/box2d.h"

namespace origin {

	class Scene;
	class Contact2DListener : public b2ContactListener
	{
	public:
		Contact2DListener(Scene* scene);
		void BeginContact(b2Contact* contact) override;
		void EndContact(b2Contact* contact) override;

	private:
		Scene* m_Scene = nullptr;
	};

}

#endif