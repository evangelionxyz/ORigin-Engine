// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Entity.h"

namespace origin
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}
}