// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "Mario.h"
#include "Origin\IO\Input.h"
#include "Origin\IO\KeyCodes.h"
#include "Origin\IO\MouseCodes.h"
#include "Origin\Scene\Entity.h"

namespace Origin
{
	void Mario::OnCreate()
	{
		rb2d.Type = Rigidbody2DComponent::BodyType::Dynamic;
		bc2d.Restitution = 1.0f;
	}

	void Mario::OnUpdate(Timestep time)
	{
		float speed = 5.0f;

		auto& translation = transform.Translation;
		if (Input::IsKeyPressed(Key::W))
			translation.y += speed * time;
		if (Input::IsKeyPressed(Key::S))
			translation.y -= speed * time;
		if (Input::IsKeyPressed(Key::D))
			translation.x += speed * time;
		if (Input::IsKeyPressed(Key::A))
			translation.x -= speed * time;
	}

	void Mario::OnDestroy()
	{
	}
}