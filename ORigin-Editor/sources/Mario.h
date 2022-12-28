// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Scene\Component\UUID.h"
#include "Origin\Scene\ScriptableEntity.h"
#include "Origin\Scene\Component\Component.h"

namespace Origin {

	class Mario : public ScriptableEntity
	{
	public:
		void OnCreate() override
		{
			auto& rb2d = GetComponent<Rigidbody2DComponent>();
			auto& bc2d = GetComponent<BoxCollider2DComponent>();

			bc2d.Restitution = 0.3f;
			rb2d.Type = Rigidbody2DComponent::BodyType::Dynamic;
		}

		void OnUpdate(Timestep time) override
		{
			auto& transform = GetComponent<TransformComponent>();
			float speed = 10.0f;

			if (Input::IsKeyPressed(Key::W))
				transform.Translation.y += speed * time;

			if (Input::IsKeyPressed(Key::S))
				transform.Translation.y -= speed * time;

			if (Input::IsKeyPressed(Key::A))
				transform.Translation.x -= speed * time;

			if(Input::IsKeyPressed(Key::D))
				transform.Translation.x += speed * time;
		}

		void OnDestroy() override
		{
		}
	};
}

