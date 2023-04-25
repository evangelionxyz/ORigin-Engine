// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "pch.h"

#include "Origin\Scene\SceneCamera.h"
#include "Origin\Renderer\Texture.h"

#include "Origin\Scene\Component\UUID.h"
#include "Origin\Scene\Component\Lighting.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm\gtx\quaternion.hpp>
#include <unordered_map>
#include <vector>

namespace Origin
{
	struct IDComponent
	{
		UUID ID;
		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent
	{
		std::string Tag;
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::vec3 Translation = glm::vec3(0.0f);
		glm::vec3 Rotation = glm::vec3(0.0f);
		glm::vec3 Scale = glm::vec3(1.0f);

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Translation)
				* rotation * glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color = glm::vec4(1.0f);
		std::shared_ptr<Texture2D> Texture;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color) : Color(color) {}
		SpriteRendererComponent(float r, float g, float b, float, float a) : Color(r, g, b, a) {}
	};

	struct SpriteRenderer2DComponent
	{
		glm::vec4 Color = glm::vec4(1.0f);
		std::shared_ptr<Texture2D> Texture;
		float TillingFactor = 1.0f;

		SpriteRenderer2DComponent() = default;
		SpriteRenderer2DComponent(const SpriteRenderer2DComponent&) = default;
		SpriteRenderer2DComponent(const SpriteRenderer2DComponent&, glm::vec4 color) : Color(color) {}
		SpriteRenderer2DComponent(float r, float g, float b, float a) : Color(r, g, b, a) {}
	};

	struct LightingComponent
	{
		glm::vec4 Color = glm::vec4(1.0f);
		float Ambient = 0.8f;
		LightingType type = LightingType::Direct;

		LightingComponent() = default;
		LightingComponent(const LightingComponent&) = default;
		LightingComponent(const LightingComponent&, glm::vec4 color) : Color(color) {}
		LightingComponent(float r, float g, float b, float a) : Color(r, g, b, a) {}
	};

	struct CircleRendererComponent
	{
		glm::vec4 Color = glm::vec4(1.0f);
		float Thickness = 1.0f;
		float Fade = 0.005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct ScriptComponent
	{
		std::string ClassName = "None";

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
	};

	class ScriptableEntity;
	struct NativeScriptComponent
	{
		ScriptableEntity* Instance;
		ScriptableEntity* (*InstantiateScript)();

		void (*DestroyScript)(NativeScriptComponent* nsc);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};

	struct Rigidbody2DComponent
	{
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;

		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};

	struct BoxCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		glm::vec2 Size = { 0.5f, 0.5f };

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};

	struct CircleCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		float Radius = 0.5f;

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		void* RuntimeFixture = nullptr;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
	};

	template<typename... Component>
	struct ComponentGroup { };

	using AllComponents =
		ComponentGroup<TransformComponent, LightingComponent,
		SpriteRendererComponent, SpriteRenderer2DComponent,
		CircleRendererComponent, CameraComponent,
		ScriptComponent, NativeScriptComponent,
		Rigidbody2DComponent, BoxCollider2DComponent, CircleCollider2DComponent>;
}
