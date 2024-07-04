
#include "JoltContactListenerImpl.h"

namespace origin {

	JPH::ValidateResult JoltContactListenerImpl::OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult)
	{
		return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
	}

	void JoltContactListenerImpl::OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
	{

	}

	void JoltContactListenerImpl::OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
	{

	}

	void JoltContactListenerImpl::OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair)
	{

	}

	void JoltContactListenerImpl::SaveState(JPH::StateRecorder& inStream) const
	{

	}

	void JoltContactListenerImpl::RestoreState(JPH::StateRecorder& inStream)
	{

	}

	void JoltContactListenerImpl::DrawState()
	{

	}

}

