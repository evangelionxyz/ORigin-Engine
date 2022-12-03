// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Entity.h"
#include "Origin\Utils\Time.h"

namespace Origin
{
	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() {}

		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
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

