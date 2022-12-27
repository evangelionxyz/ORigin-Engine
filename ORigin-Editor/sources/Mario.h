// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Scene\Scene.h"
#include "Origin\Scene\Component\Component.h"
#include "Origin\Scene\ScriptableEntity.h"
#include "Origin\Scene\Component\UUID.h"

namespace Origin {

	class Mario : public ScriptableEntity
	{
	private:
		Rigidbody2DComponent rb2d = AddComponent<Rigidbody2DComponent>();
		TransformComponent transform = AddComponent<TransformComponent>();
		BoxCollider2DComponent bc2d = AddComponent<BoxCollider2DComponent>();

	public:
		void OnCreate();
		void OnDestroy();
		void OnUpdate(Timestep time);
	};

}

