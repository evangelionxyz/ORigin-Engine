// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once

#include "Origin/Animation/AnimationState.h"

#include "Origin/Audio/AudioListener.h"
#include "Origin/Math/Math.h"
#include "SceneCamera.h"
#include "Origin/Core/UUID.h"
#include "Origin/Renderer/Texture.h"
#include "Origin/Renderer/Model.h"
#include "Origin/Renderer/Font.h"
#include "Origin/Renderer/ParticleSystem.h"
#include "Origin/Renderer/Material.h"
#include "Origin/Renderer/Framebuffer.h"
#include "SpriteSheet.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>

namespace origin
{
	class IDComponent
	{
	public:
		UUID ID;
		UUID Parent = 0;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	class TagComponent
	{
	public:
		std::string Tag;
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag) : Tag(tag)
		{
		}
	};

	class SpriteAnimation;
	class SpriteAnimationComponent
	{
	public:
		std::shared_ptr<AnimationState<SpriteAnimation>> State;

		SpriteAnimationComponent()
		{
			State = std::make_shared<AnimationState<SpriteAnimation>>();
		}

		SpriteAnimationComponent(const SpriteAnimationComponent&) = default;
		static const AnimationType Type = AnimationType::Sprite;
	};

	class AudioListenerComponent
	{
	public:
		AudioListener Listener;
		uint32_t Index = 0;
		bool Enable = true;
		AudioListenerComponent() = default;
		AudioListenerComponent(const AudioListenerComponent&) = default;
	};

	class AudioSource;
	class AudioComponent
	{
	public:
		AssetHandle Audio = 0;
		std::string Name;
		float Volume = 1.0f;
		float Panning = 0.0f;
		float Pitch = 1.0f;
		float MinDistance = 10.0f;
		float MaxDistance = 20.0f;
		bool Spatializing = false;
		bool Looping = false;
		bool PlayAtStart = false;

		AudioComponent() = default;
		AudioComponent(const AudioComponent&) = default;
	};

	class ParticleComponent
	{
	public:
		ParticleSystem Particle;

		glm::vec3 Velocity = glm::vec3(0.0f);
		glm::vec3 VelocityVariation = glm::vec3(1.0f);
		glm::vec3 Rotation = glm::vec3(1.0f);

		glm::vec4 ColorBegin = {254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f};
		glm::vec4 ColorEnd = {254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f};
		uint32_t PoolIndex = 1000;

		float SizeBegin = 0.5f;
		float SizeEnd = 0.0f;
		float SizeVariation = 0.3f;
		float ZAxis = 0.0f;
		float LifeTime = 1.0f;

		ParticleComponent() = default;
		ParticleComponent(const ParticleComponent&) = default;
	};

	class StaticMeshComponent
	{
	public:
		AssetHandle Handle = 0;
		AssetHandle MaterialHandle = 0;
		StaticMeshComponent() = default;
		StaticMeshComponent(const StaticMeshComponent&) = default;
	};

	class TextComponent
	{
	public:
		AssetHandle FontHandle = 0;
		std::string TextString;

		glm::vec4 Color = glm::vec4(1.0f);
		float Kerning = 0.0f;
		float LineSpacing = 0.0f;

		bool ScreenSpace = false;
	};

	class TransformComponent
	{
	public:
		glm::vec3 Translation = glm::vec3(0.0f);
		glm::vec3 Rotation = glm::vec3(0.0f);
		glm::vec3 Scale = glm::vec3(1.0f);
		glm::vec3 WorldTranslation = glm::vec3(0.0f);
		glm::vec3 WorldRotation = glm::vec3(0.0f);
		glm::vec3 WorldScale = glm::vec3(1.0f);

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;

		TransformComponent(const glm::vec3& translation)
			: Translation(translation)
		{
		}

		void SetTransform(glm::mat4 transform)
		{
			Math::DecomposeTransformEuler(transform, WorldTranslation, WorldRotation, WorldScale);
		}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(WorldRotation));
			return glm::translate(glm::mat4(1.0f), WorldTranslation)
				* rotation * glm::scale(glm::mat4(1.0f), WorldScale);
		}

		glm::vec3 GetForward() const
		{
			glm::mat4 rotation = toMat4(glm::quat(WorldRotation));
			glm::vec4 forward = rotation * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
			return glm::normalize(glm::vec3(forward));
		}

		glm::vec3 GetUp() const
		{
			glm::mat4 rotation = toMat4(glm::quat(WorldRotation));
			glm::vec4 up = rotation * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
			return glm::normalize(glm::vec3(up));
		}

		glm::vec3 GetRight() const
		{
			glm::mat4 rotation = toMat4(glm::quat(WorldRotation));
			glm::vec4 right = rotation * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
			return glm::normalize(glm::vec3(right));
		}
	};

	class SpriteRenderer2DComponent
	{
	public:
		AssetHandle Texture = 0;

		glm::vec4 Color = glm::vec4(1.0f);
		glm::vec2 Min = glm::vec2(0.0f, 0.0f);
		glm::vec2 Max = glm::vec2(1.0f, 1.0f);
		glm::vec2 TillingFactor = glm::vec2(1.0f);

		bool FlipX = false;
		bool FlipY = false;

		SpriteRenderer2DComponent() = default;
		SpriteRenderer2DComponent(const SpriteRenderer2DComponent&) = default;
		SpriteRenderer2DComponent(const SpriteRenderer2DComponent&, glm::vec4 color)
			: Color(color) {}

		SpriteRenderer2DComponent(float r, float g, float b, float a)
			: Color(r, g, b, a){}
	};

	class Lighting;
	class LightComponent
	{
	public:
		std::shared_ptr<Lighting> Light;
		LightComponent() = default;
		LightComponent(const LightComponent&) = default;
	};

	class CircleRendererComponent
	{
	public:
		glm::vec4 Color = glm::vec4(1.0f);
		float Thickness = 1.0f;
		float Fade = 0.005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
	};

	class CameraComponent
	{
	public:
		SceneCamera Camera;
		bool Primary = true;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	class ScriptComponent
	{
	public:
		std::string ClassName = "None";
		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
	};

	class ScriptableEntity;
	class NativeScriptComponent
	{
	public:
		ScriptableEntity* Instance;
		ScriptableEntity* (*InstantiateScript)();

		void (*DestroyScript)(NativeScriptComponent* nsc);

		template <typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc)
			{
				delete nsc->Instance;
				nsc->Instance = nullptr;
			};
		}
	};

	class Rigidbody2DComponent
	{
	public:
		void* RuntimeBody = nullptr;

		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;

		float Mass = 1.0f;
		float LinearDamping = 0.0f;
		float AngularDamping = 0.01f;
		float RotationalInertia = 0.0f;
		glm::vec2 MassCenter = glm::vec2(0.0f);
		float GravityScale = 1.0f;

		bool FreezePositionX = false;
		bool FreezePositionY = false;
		bool FixedRotation = false;
		bool AllowSleeping = true;
		bool Awake = true;
		bool Bullet = true;
		bool Enabled = true;

		std::string ContactWith;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};

	class BoxCollider2DComponent
	{
	public:
		// All fixtures with the same group index always collide(positive index)
		// or never collide(negative index)
		int Group = 1;

		glm::vec2 Offset = {0.0f, 0.0f};
		glm::vec2 Size = {0.5f, 0.5f};

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		void* RuntimeFixture = nullptr;
		void* RuntimeBoxShape = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};

	class CircleCollider2DComponent
	{
	public:
		// All fixtures with the same group index always collide(positive index)
		// or never collide(negative index)
		int Group = 1;

		glm::vec2 Offset = {0.0f, 0.0f};
		float Radius = 0.5f;

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		void* RuntimeFixture = nullptr;
		void* RuntimeCircleShape = nullptr;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
	};

	class RevoluteJoint2DComponent
	{
	public:
		glm::vec2 AnchorPoint;

		// Angles in degrees 
		// (it's it converted to radians when creating component)
		bool EnableLimit = true;
		float LowerAngle = 90.0f;
		float UpperAngle = 45.0f;
		float MaxMotorTorque = 10.0f;
		bool EnableMotor = true;
		float MotorSpeed = 0.0f;
		void* Joint = nullptr;
		UUID ConnectedBodyID = 0;

		RevoluteJoint2DComponent() = default;
		RevoluteJoint2DComponent(const RevoluteJoint2DComponent&) = default;
	};

	template<typename T>
	struct UIData
	{
		enum class Anchor
		{
			Center,
			Left, Right,
			TopLeft, TopRight,
			BottomLeft, BottomRight
		};

		std::string Name = "UI";
		TransformComponent Transform;
		T Component;
		Anchor AnchorType = Anchor::Center;
	};

	class UIComponent
	{
	public:
		std::vector<UIData<TextComponent>> Texts;
		std::vector<UIData<SpriteRenderer2DComponent>> Sprites;

		std::shared_ptr<Framebuffer> Framebuffer;

		UIComponent() = default;
		UIComponent(const UIComponent &) = default;
	};

	template <typename... Component>
	struct ComponentGroup
	{
	};

	class RigidbodyComponent;
	class BoxColliderComponent;
	class SphereColliderComponent;
	class CapsuleColliderComponent;

	using AllComponents = ComponentGroup<TransformComponent, CameraComponent,
		UIComponent, SpriteAnimationComponent, AudioComponent, AudioListenerComponent, LightComponent,
		SpriteRenderer2DComponent, StaticMeshComponent, TextComponent,
		CircleRendererComponent, ParticleComponent, ScriptComponent, NativeScriptComponent,
		Rigidbody2DComponent, BoxCollider2DComponent, CircleCollider2DComponent, RevoluteJoint2DComponent,
		RigidbodyComponent, BoxColliderComponent, SphereColliderComponent, CapsuleColliderComponent
	>;
}
