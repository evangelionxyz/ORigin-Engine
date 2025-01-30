// Copyright (c) Evangelion Manuhutu | ORigin Engine

#ifndef SCRIPTABLE_ENTITY_H
#define SCRIPTABLE_ENTITY_H

#include "Origin/Core/Time.h"
#include "Origin/Scene/Entity.h"

namespace origin {
class ScriptableEntity
{
public:
	ScriptableEntity() = default;
	virtual ~ScriptableEntity() {}

	template<typename T>
	T &GetComponent()
	{

		if (m_Entity.HasComponent<T>())
			return m_Entity.GetComponent<T>();

		return m_Entity.AddComponent<T>();
	}

protected:
	virtual void OnCreate() {}
	virtual void OnUpdate(Timestep time) {}
	virtual void OnDestroy() {}

private:
	Entity m_Entity;
	friend class Scene;
};
}

#endif