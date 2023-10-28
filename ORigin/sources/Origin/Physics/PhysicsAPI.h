#pragma once

#include "PhysicsScene.h"

namespace origin {

	enum class PhysicsAPIType
	{
		None, Jolt, PhysX
	};

	class PhysicsAPI
	{
	public:

		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		virtual std::shared_ptr<PhysicsScene> CreateScene(const std::shared_ptr<Scene>& scene) const = 0;

		virtual const std::string& GetLastErrorMessage() const = 0;

		static PhysicsAPIType Current() { return m_CurrentPhyisicsAPI; }
		static void SetCurrentAPI(PhysicsAPIType api) { m_CurrentPhyisicsAPI = api; }

	private:
		inline static PhysicsAPIType m_CurrentPhyisicsAPI = PhysicsAPIType::PhysX;

	};

}


