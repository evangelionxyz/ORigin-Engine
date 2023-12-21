#pragma once

#include "Origin\Scene\Scene.h"

namespace origin {

	class Contact2DListener;
	class Physics2D
	{
	public:
		Physics2D(Scene* scene);

		void Simulate(float deltaTime);

		void OnSimulationStart();
		void OnSimulationStop();
		
	private:
		Scene* m_SceneContext;

		b2World* m_World;
		Contact2DListener* m_ContactListener;
	};
}

