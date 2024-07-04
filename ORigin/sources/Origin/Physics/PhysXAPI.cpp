// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "PhysXAPI.h"
#include "PhysXScene.h"
#include "PxPhysicsAPI.h"

#include <glm/glm.hpp>

namespace origin {

	namespace Utils {

		physx::PxVec3 ToPhysXVec3(const glm::vec3& vec)
		{
			return physx::PxVec3{vec.x, vec.y, vec.z};
		}

		physx::PxQuat ToPhysXQuat(const glm::quat& quat)
		{
			return physx::PxQuat{ quat.x, quat.y, quat.z, quat.w };
		}

		glm::vec3 FromPhysXVec3(const physx::PxVec3& vec)
		{
			return glm::vec3{ vec.x, vec.y, vec.z };
		}

		glm::quat FromPhysXQuat(const physx::PxQuat quat)
		{
			return glm::quat{ quat.w, quat.x, quat.y, quat.z };
		}

		glm::vec3 FromPhysXQuatToVec3(const physx::PxQuat& quat)
		{
			glm::vec3 rotation;

			using namespace glm;
			using T = float;

			mat4 LocalMatrix(toMat4(glm::quat(quat.w, quat.x, quat.y, quat.z)));
			if (
				epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
				epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
				epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
			{
				LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
				LocalMatrix[3][3] = static_cast<T>(1);
			}

			vec3 Row[3];

			for (length_t i = 0; i < 3; ++i)
				for (length_t j = 0; j < 3; ++j)
					Row[i][j] = LocalMatrix[i][j];

			Row[0] = detail::scale(Row[0], static_cast<T>(1));
			Row[1] = detail::scale(Row[1], static_cast<T>(1));
			Row[2] = detail::scale(Row[2], static_cast<T>(1));

			rotation.y = asin(-Row[0][2]);
			if (cos(rotation.y) != 0) 
			{
				rotation.x = atan2(Row[1][2], Row[2][2]);
				rotation.z = atan2(Row[0][1], Row[0][0]);
			}
			else 
			{
				rotation.x = atan2(-Row[2][0], Row[1][1]);
				rotation.z = 0;
			}

			return rotation;
		}

		physx::PxTransform PhysicsTransformfromTransformComponent(const glm::mat4& transform)
		{
			glm::vec3 position, rotation, scale;
			Math::DecomposeTransformEuler(transform, position, rotation, scale);
			return physx::PxTransform(ToPhysXVec3(position), ToPhysXQuat(rotation));
		}

	}

	class PhysXAllocator : public physx::PxAllocatorCallback
	{
	public:
		void* allocate(size_t size, const char*, const char*, int)
		{
			void* ptr = physx::platformAlignedAlloc(size);
			PX_ASSERT((reinterpret_cast<size_t>(ptr) & 15) == 0);
			return ptr;
		}

		void deallocate(void* ptr)
		{
			physx::platformAlignedFree(ptr);
		}
	};

	class PhysXErrorCallback : public physx::PxErrorCallback
	{
	public:
		virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override
		{
			const char* errorMessage = NULL;

			switch (code)
			{
			case physx::PxErrorCode::eNO_ERROR: errorMessage = ""; break;
			case physx::PxErrorCode::eDEBUG_INFO: errorMessage = "Info"; break;
			case physx::PxErrorCode::eDEBUG_WARNING: errorMessage = "Warning"; break;
			case physx::PxErrorCode::eINVALID_PARAMETER: errorMessage = "Invalid Parameter"; break;
			case physx::PxErrorCode::eINVALID_OPERATION: errorMessage = "Invalid Operation"; break;
			case physx::PxErrorCode::eOUT_OF_MEMORY: errorMessage = "Out Of Memory"; break;
			case physx::PxErrorCode::eINTERNAL_ERROR: errorMessage = "Internal Error"; break;
			case physx::PxErrorCode::eABORT: errorMessage = "Abort"; break;
			case physx::PxErrorCode::ePERF_WARNING: errorMessage = "Performance Warning"; break;
			case physx::PxErrorCode::eMASK_ALL: errorMessage = "Mask All"; break;
			}

			message = errorMessage;
		}
	};


	struct PhysXData
	{
		physx::PxFoundation* PhysxFoundation;
		physx::PxDefaultCpuDispatcher* PhysxCPUDispatcher;
		physx::PxPhysics* PhysxPhysics;
		physx::PxPvd* PhysXPvd;

		PhysXAllocator Allocator;
		PhysXErrorCallback ErrorCallback;
		//PhysXAssertHandler AssertHandler;

		std::string LastErrorMessage = "";

	};

	static PhysXData* s_PhysXData = nullptr;

	PhysXAPI* PhysXAPI::s_Instance = nullptr;

	void PhysXAPI::Init()
	{
		OGN_PROFILER_PHYSICS();

		s_Instance = this;

		s_PhysXData = new PhysXData();
		s_PhysXData->PhysxFoundation = PxCreateFoundation(
			PX_PHYSICS_VERSION,
			s_PhysXData->Allocator,
			s_PhysXData->ErrorCallback);

		OGN_CORE_ASSERT(s_PhysXData->PhysxFoundation, "PhysXAPI: Failed To Create Foundation");

		s_PhysXData->PhysxCPUDispatcher = physx::PxDefaultCpuDispatcherCreate(2);

		int workerThreads = 4;
		s_PhysXData->PhysxCPUDispatcher = physx::PxDefaultCpuDispatcherCreate(workerThreads);
		OGN_CORE_ASSERT(s_PhysXData->PhysxCPUDispatcher, "PhysX CPU Dispatch is failed to created");
		
		OGN_CORE_INFO("PhysX API: Foundation Created");
	}

	void PhysXAPI::OnSimulationStart()
	{
		OGN_PROFILER_PHYSICS();

		s_PhysXData->PhysXPvd = PxCreatePvd(*PhysXAPI::GetFoundation());

		int port = 5425;
		int timeoutMS = 10;
		bool recordMemoryAllocations = false;
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("localhost", port, timeoutMS);
		s_PhysXData->PhysXPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

		s_PhysXData->PhysxPhysics = PxCreatePhysics(
			PX_PHYSICS_VERSION,
			*s_PhysXData->PhysxFoundation,
			physx::PxTolerancesScale(),
			recordMemoryAllocations,
			s_PhysXData->PhysXPvd);

		bool pxPvdExtension = PxInitExtensions(*s_PhysXData->PhysxPhysics, s_PhysXData->PhysXPvd);
		OGN_CORE_ASSERT(pxPvdExtension, "PhysX PVD Extension is failed to initilized");
		OGN_CORE_ASSERT(s_PhysXData->PhysxPhysics, "PhysXAPI: Failed To Create Physics");
	}

	void PhysXAPI::OnSimulationStop()
	{
		OGN_PROFILER_PHYSICS();

		PxCloseExtensions();
		s_PhysXData->PhysxPhysics->release();
	}

	void PhysXAPI::Shutdown()
	{
		OGN_PROFILER_PHYSICS();

		s_PhysXData->PhysxCPUDispatcher->release();
		s_PhysXData->PhysxFoundation->release();
		OGN_CORE_WARN("PhysX API: Foundation Released");

		delete s_PhysXData;
		s_PhysXData = nullptr;

		OGN_CORE_WARN("PhysX API: Shutdown");
	}

	std::shared_ptr<PhysicsScene> PhysXAPI::CreateScene(const std::shared_ptr<Scene>& scene) const
	{
		return PhysXScene::Create(scene.get());
	}

	const std::string& PhysXAPI::GetLastErrorMessage() const
	{
		return s_PhysXData->LastErrorMessage;
	}

	physx::PxPhysics* PhysXAPI::GetPhysics()
	{
		return s_PhysXData->PhysxPhysics;
	}

	physx::PxFoundation* PhysXAPI::GetFoundation()
	{
		return s_PhysXData->PhysxFoundation;
	}

	physx::PxDefaultCpuDispatcher* PhysXAPI::GetCPUDispatcher()
	{
		return s_PhysXData->PhysxCPUDispatcher;
	}

}
