// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once

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

