#pragma once

#include "PxPhysicsAPI.h"
#include "PhysicsAPI.h"

#include "PhysicsScene.h"

#include <glm\glm.hpp>

namespace origin {

	namespace Utils {
		physx::PxVec3 ToPhysXVec3(const glm::vec3& vec);
		physx::PxQuat ToPhysXQuat(const glm::quat& quat);
		glm::vec3 FromPhysXVec3(const physx::PxVec3& vec);
		glm::quat FromPhysXQuat(const physx::PxQuat quat);
		glm::vec3 FromPhysXQuatToVec3(const physx::PxQuat& quat);
		physx::PxTransform PhysicsTransformfromTransformComponent(const glm::mat4& transform);
	}
	
	class PhysXAPI : public PhysicsAPI
	{
	public:
		virtual void Init() override;
		void OnSimulationStart();
		void OnSimulationStop();
		virtual void Shutdown() override;
		virtual std::shared_ptr<PhysicsScene> CreateScene(const std::shared_ptr<Scene>& scene) const override;
		virtual const std::string& GetLastErrorMessage() const override;

		static PhysXAPI Get() { return *s_Instance; }

		static physx::PxPhysics* GetPhysics();
		static physx::PxFoundation* GetFoundation();
		static physx::PxDefaultCpuDispatcher* GetCPUDispatcher();

	private:
		static PhysXAPI* s_Instance;
	};
}

