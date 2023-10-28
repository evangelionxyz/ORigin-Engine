#include "pch.h"

#include "PhysXAPI.h"
#include "PhysXScene.h"
#include "PxPhysicsAPI.h"

namespace origin {

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
		virtual void PhysXErrorCallback::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override
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

		PhysXAllocator Allocator;
		PhysXErrorCallback ErrorCallback;
		//PhysXAssertHandler AssertHandler;

		std::string LastErrorMessage = "";

	};

	static PhysXData* s_PhysXData = nullptr;

	void PhysXAPI::Init()
	{
		s_PhysXData = new PhysXData();

		s_PhysXData->PhysxFoundation = PxCreateFoundation(
			PX_PHYSICS_VERSION,
			s_PhysXData->Allocator,
			s_PhysXData->ErrorCallback);

		OGN_CORE_ASSERT(s_PhysXData->PhysxFoundation, "PhysXAPI: Failed To Create Foundation");

		s_PhysXData->PhysxCPUDispatcher = physx::PxDefaultCpuDispatcherCreate(2);

		s_PhysXData->PhysxPhysics = PxCreatePhysics(
			PX_PHYSICS_VERSION,
			*s_PhysXData->PhysxFoundation,
			physx::PxTolerancesScale());

		OGN_CORE_ASSERT(s_PhysXData->PhysxPhysics, "PhysXAPI: Failed To Create Physics")

	}

	void PhysXAPI::Shutdown()
	{
		s_PhysXData->PhysxPhysics->release();
		s_PhysXData->PhysxPhysics = nullptr;

		s_PhysXData->PhysxFoundation->release();
		s_PhysXData->PhysxFoundation = nullptr;

		delete s_PhysXData;
		s_PhysXData = nullptr;
	}

	std::shared_ptr<PhysicsScene> PhysXAPI::CreateScene(const std::shared_ptr<Scene>& scene) const
	{
		return PhysXScene::Create(scene);
	}

	const std::string& PhysXAPI::GetLastErrorMessage() const
	{
		return s_PhysXData->LastErrorMessage;
	}

	physx::PxPhysics* PhysXAPI::GetPhysics()
	{
		return s_PhysXData->PhysxPhysics;
	}

	physx::PxDefaultCpuDispatcher* PhysXAPI::GetCPUDispatcher()
	{
		return s_PhysXData->PhysxCPUDispatcher;
	}

	

}
