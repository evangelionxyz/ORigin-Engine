#pragma once

#include "PhysicsAPI.h"

namespace origin {

	class Physics
	{
	public:
		static void Init();
		static void Shutdown();

	private:
		static std::unique_ptr<PhysicsAPI> m_PhysicsAPI;
	};

}

