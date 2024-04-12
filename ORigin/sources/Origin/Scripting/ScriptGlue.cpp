// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"

#include "Origin/Asset/AssetManager.h"

#include "ScriptGlue.h"
#include "ScriptEngine.h"

#include "Origin/Scene/Components.h"

#include "Origin/Scene/Scene.h"
#include "Origin/Scene/Entity.h"

#include "Origin/Core/KeyCodes.h"
#include "Origin/Core/Input.h"

#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"

#include "box2d/b2_body.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"

#include "Origin/Audio/AudioSource.h"

namespace origin
{
#define OGN_ADD_INTERNAL_CALLS(Name) mono_add_internal_call("ORiginEngine.InternalCalls::"#Name, Name)

	namespace Utils
	{
		std::string MonoStringToString(MonoString* string)
		{
			char* cStr = mono_string_to_utf8(string);
			auto str = std::string(cStr);
			mono_free(cStr);

			return str;
		}
	}

	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;
	static std::unordered_map<MonoType*, std::function<void(Entity)>> s_EntityAddComponentFuncs;

	// Entity
	static bool Entity_HasComponent(UUID entityID, MonoReflectionType *componentType)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		MonoType *managedType = mono_reflection_type_get_type(componentType);
		OGN_CORE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end(), "ScriptGlue: Failed to process Entity_HasComponent")
			return s_EntityHasComponentFuncs.at(managedType)(entity);
	}

	static void Entity_AddComponent(UUID entityID, MonoReflectionType *componentType)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		MonoType *managedType = mono_reflection_type_get_type(componentType);
		OGN_CORE_ASSERT(s_EntityAddComponentFuncs.find(managedType) != s_EntityAddComponentFuncs.end(), "ScriptGlue: Failed to process AddComponent");
		s_EntityAddComponentFuncs.at(managedType)(entity);
	}

	static uint64_t Entity_FindEntityByName(MonoString *stringName)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")

			std::string name = Utils::MonoStringToString(stringName);
		Entity entity = scene->FindEntityByName(name);

		if (!entity)
		{
			OGN_CORE_WARN("Invalid Entity");
			return 0;
		}

		return entity.GetUUID();
	}

	// Logging
	static void NativeLog(MonoString *string, int parameter)
	{
		PROFILER_LOGIC();

		std::string str = Utils::MonoStringToString(string);
		std::cout << str << ", " << parameter << std::endl;
	}

	static void NativeLog_Vector(glm::vec3 *parameter, glm::vec3 *outResult)
	{
		PROFILER_LOGIC();

		*outResult = normalize(*parameter);
	}

	static float NativeLog_VectorDot(glm::vec3 *parameter)
	{
		PROFILER_LOGIC();

		return dot(*parameter, *parameter);
	}

	// Component
	static MonoObject *GetScriptInstance(UUID entityID)
	{
		PROFILER_LOGIC();

		return ScriptEngine::GetManagedInstance(entityID);
	}

	static void TransformComponent_GetTranslation(UUID entityID, glm::vec3 *outTranslation)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		*outTranslation = entity.GetComponent<TransformComponent>().WorldTranslation;
	}

	static void TransformComponent_SetTranslation(UUID entityID, glm::vec3 translation)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<TransformComponent>().Translation = translation;
	}

	static void TransformComponent_GetRotation(UUID entityID, glm::vec3 *outRotation)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		*outRotation = entity.GetComponent<TransformComponent>().Rotation;
	}

	static void TransformComponent_SetRotation(UUID entityID, glm::vec3 rotation)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);
		entity.GetComponent<TransformComponent>().Rotation = rotation;
	}

	static void TransformComponent_GetScale(UUID entityID, glm::vec3 *outScale)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		*outScale = entity.GetComponent<TransformComponent>().Scale;
	}

	static void TransformComponent_SetScale(UUID entityID, glm::vec3 scale)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<TransformComponent>().Scale = scale;
	}

	static void RigidbodyComponent_SetVelocity(UUID entityID, glm::vec3 velocity, bool autoWake)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		auto &rb = entity.GetComponent<RigidbodyComponent>();
		rb.ApplyLinearVelocity(velocity, autoWake);
	}

	static void RigidbodyComponent_GetVelocity(UUID entityID, glm::vec3 *velocity)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		auto &rb = entity.GetComponent<RigidbodyComponent>();

		physx::PxRigidActor *actor = (physx::PxRigidActor *)rb.Body;
		physx::PxRigidDynamic *dActor = actor->is<physx::PxRigidDynamic>();
		*velocity = glm::vec3(dActor->getLinearVelocity().x, dActor->getLinearVelocity().y, dActor->getLinearVelocity().z);
	}

	static void RigidbodyComponent_SetVelocityForce(UUID entityID, glm::vec3 force)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		auto &rb = entity.GetComponent<RigidbodyComponent>();
		rb.ApplyVelocityForce(force);
	}

	static void RigidbodyComponent_SetForce(UUID entityID, glm::vec3 force)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		auto &rb = entity.GetComponent<RigidbodyComponent>();
		rb.ApplyForce(force);
	}

	static void RigidbodyComponent_SetImpulseForce(UUID entityID, glm::vec3 force)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		auto &rb = entity.GetComponent<RigidbodyComponent>();
		rb.ApplyImpulseForce(force);
	}

	static void Rigidbody2DComponent_SetVelocity(UUID entityID, glm::vec2 velocity)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		auto &rb2d = entity.GetComponent<Rigidbody2DComponent>();
		auto body = static_cast<b2Body *>(rb2d.RuntimeBody);

		body->SetLinearVelocity(b2Vec2(velocity.x, velocity.y));
	}

	static void Rigidbody2DComponent_GetVelocity(UUID entityID, glm::vec2 *velocity)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		auto &rb2d = entity.GetComponent<Rigidbody2DComponent>();
		auto body = static_cast<b2Body *>(rb2d.RuntimeBody);
		auto v = body->GetLinearVelocity();

		*velocity = glm::vec2(v.x, v.y);
	}

	static bool Rigidbody2DComponent_IsContactWithTag(UUID entityID, MonoString *contactWith)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		auto &rb2d = entity.GetComponent<Rigidbody2DComponent>();
		std::string contactName = Utils::MonoStringToString(contactWith);

		return rb2d.ContactWith == contactName;
	}

	static MonoString *Rigidbody2DComponent_GetContactWithTag(UUID entityID)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		auto &rb2d = entity.GetComponent<Rigidbody2DComponent>();
		return ScriptEngine::CreateString(rb2d.ContactWith.c_str());
	}

	static void Rigidbody2DComponent_ApplyLinearImpulse(UUID entityID, glm::vec2 impulse, glm::vec2 point, bool wake)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		auto &rb2d = entity.GetComponent<Rigidbody2DComponent>();
		auto body = static_cast<b2Body *>(rb2d.RuntimeBody);
		body->ApplyLinearImpulse(b2Vec2(impulse.x, impulse.y), b2Vec2(point.x, point.y), wake);
	}

	static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID entityID, glm::vec2 impulse, bool wake)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		auto &rb2d = entity.GetComponent<Rigidbody2DComponent>();
		auto body = static_cast<b2Body *>(rb2d.RuntimeBody);
		body->ApplyLinearImpulseToCenter(b2Vec2(impulse.x, impulse.y), wake);
	}

	static void Rigidbody2DComponent_ApplyForce(UUID entityID, glm::vec2 force, glm::vec2 point, bool wake)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		auto &rb2d = entity.GetComponent<Rigidbody2DComponent>();
		auto body = static_cast<b2Body *>(rb2d.RuntimeBody);
		body->ApplyForce(b2Vec2(force.x, force.y), b2Vec2(point.x, point.y), wake);
	}

	static void Rigidbody2DComponent_ApplyForceToCenter(UUID entityID, glm::vec2 force, bool wake)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		auto &rb2d = entity.GetComponent<Rigidbody2DComponent>();
		auto body = static_cast<b2Body *>(rb2d.RuntimeBody);
		body->ApplyForceToCenter(b2Vec2(force.x, force.y), wake);
	}

	static MonoString *AudioComponent_GetName(UUID entityID)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		auto &ac = entity.GetComponent<AudioComponent>();
		return ScriptEngine::CreateString(ac.Name.c_str());
	}

	static void AudioComponent_SetName(UUID entityID, MonoString *name)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		auto &ac = entity.GetComponent<AudioComponent>();
		ac.Name = Utils::MonoStringToString(name);
	}

	static void AudioComponent_Play(UUID entityID)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		auto &ac = entity.GetComponent<AudioComponent>();
		OGN_CORE_ASSERT(ac.Audio, "ScriptGlue: Invalid Audio");

		const std::shared_ptr<AudioSource> &audio = AssetManager::GetAsset<AudioSource>(ac.Audio);
		audio->Play();
	}

	static void AudioComponent_Stop(UUID entityID)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		auto &ac = entity.GetComponent<AudioComponent>();
		OGN_CORE_ASSERT(ac.Audio, "ScriptGlue: Invalid Audio");

		const std::shared_ptr<AudioSource> &audio = AssetManager::GetAsset<AudioSource>(ac.Audio);
		audio->Stop();
	}

	static void AudioComponent_SetVolume(UUID entityID, float volume)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		entity.GetComponent<AudioComponent>().Volume = volume;
	}

	static float AudioComponent_GetVolume(UUID entityID)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		return entity.GetComponent<AudioComponent>().Volume;
	}

	static void AudioComponent_SetMinDistance(UUID entityID, float minDistance)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		entity.GetComponent<AudioComponent>().MinDistance = minDistance;
	}

	static float AudioComponent_GetMinDistance(UUID entityID)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		return entity.GetComponent<AudioComponent>().MinDistance;
	}

	static void AudioComponent_SetMaxDistance(UUID entityID, float maxDistance)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		entity.GetComponent<AudioComponent>().MaxDistance = maxDistance;
	}

	static float AudioComponent_GetMaxDistance(UUID entityID)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		return entity.GetComponent<AudioComponent>().MinDistance;
	}

	static void AudioComponent_SetPitch(UUID entityID, float pitch)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		entity.GetComponent<AudioComponent>().Pitch = pitch;
	}

	static float AudioComponent_GetPitch(UUID entityID)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		return entity.GetComponent<AudioComponent>().Pitch;
	}

	static void AudioComponent_SetLooping(UUID entityID, bool looping)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		entity.GetComponent<AudioComponent>().Looping = looping;
	}

	static bool AudioComponent_IsLooping(UUID entityID)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		return entity.GetComponent<AudioComponent>().Looping;
	}

	static void AudioComponent_SetSpatial(UUID entityID, bool spatial)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		entity.GetComponent<AudioComponent>().Spatializing = spatial;
	}

	static bool AudioComponent_IsSpatial(UUID entityID)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		return entity.GetComponent<AudioComponent>().Spatializing;
	}

	static void AudioComponent_SetPlayAtStart(UUID entityID, bool playAtStart)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		entity.GetComponent<AudioComponent>().PlayAtStart = playAtStart;
	}

	static bool AudioComponent_IsPlayAtStart(UUID entityID)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		return entity.GetComponent<AudioComponent>().PlayAtStart;
	}

	static MonoString *TextComponent_GetText(UUID entityID)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		auto &tc = entity.GetComponent<TextComponent>();
		return ScriptEngine::CreateString(tc.TextString.c_str());
	}

	static void TextComponent_SetText(UUID entityID, MonoString *textString)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		auto &tc = entity.GetComponent<TextComponent>();
		tc.TextString = Utils::MonoStringToString(textString);
	}

	static void TextComponent_GetColor(UUID entityID, glm::vec4 *color)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		auto &tc = entity.GetComponent<TextComponent>();
		*color = tc.Color;
	}

	static void TextComponent_SetColor(UUID entityID, glm::vec4 *color)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		auto &tc = entity.GetComponent<TextComponent>();
		tc.Color = *color;
	}

	static float TextComponent_GetKerning(UUID entityID)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		auto &tc = entity.GetComponent<TextComponent>();
		return tc.Kerning;
	}

	static void TextComponent_SetKerning(UUID entityID, float kerning)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		auto &tc = entity.GetComponent<TextComponent>();
		tc.Kerning = kerning;
	}

	static float TextComponent_GetLineSpacing(UUID entityID)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		auto &tc = entity.GetComponent<TextComponent>();
		return tc.LineSpacing;
	}

	static void TextComponent_SetLineSpacing(UUID entityID, float lineSpacing)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		auto &tc = entity.GetComponent<TextComponent>();
		tc.LineSpacing = lineSpacing;
	}

	static void CircleRendererComponent_GetColor(UUID entityID, glm::vec4 *outColor)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleRendererComponent>().Color = *outColor;
	}

	static void CircleRendererComponent_SetColor(UUID entityID, glm::vec4 *color)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleRendererComponent>().Color = *color;
	}

	static void CircleRendererComponent_GetFade(UUID entityID, float *outFade)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		*outFade = entity.GetComponent<CircleRendererComponent>().Fade;
	}

	static void CircleRendererComponent_SetFade(UUID entityID, float *fade)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleRendererComponent>().Fade = *fade;
	}

	static void CircleRendererComponent_GetThickness(UUID entityID, float *outThickness)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		*outThickness = entity.GetComponent<CircleRendererComponent>().Thickness;
	}

	static void CircleRendererComponent_SetThickness(UUID entityID, float *thickness)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleRendererComponent>().Thickness = *thickness;
	}

	static void SpriteRenderer2DComponent_GetColor(UUID entityID, glm::vec4 *outColor)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		*outColor = entity.GetComponent<SpriteRenderer2DComponent>().Color;
	}

	static void SpriteRenderer2DComponent_SetFlipX(UUID entityID, bool *flip)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");

		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<SpriteRenderer2DComponent>().FlipX = *flip;
	}

	static void SpriteRenderer2DComponent_GetFlipX(UUID entityID, bool *flip)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");

		Entity entity = scene->GetEntityWithUUID(entityID);

		*flip = entity.GetComponent<SpriteRenderer2DComponent>().FlipX;
	}

	static void SpriteRenderer2DComponent_SetFlipY(UUID entityID, bool *flip)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");

		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<SpriteRenderer2DComponent>().FlipY = *flip;
	}

	static void SpriteRenderer2DComponent_GetFlipY(UUID entityID, bool *flip)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");

		Entity entity = scene->GetEntityWithUUID(entityID);

		*flip = entity.GetComponent<SpriteRenderer2DComponent>().FlipY;
	}

	static void SpriteRenderer2DComponent_SetColor(UUID entityID, glm::vec4 *color)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<SpriteRenderer2DComponent>().Color = *color;
	}

	static void SpriteRenderer2DComponent_GetTilingFactor(UUID entityID, glm::vec2 *tilingfactor)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		*tilingfactor = entity.GetComponent<SpriteRenderer2DComponent>().TillingFactor;
	}

	static void SpriteRenderer2DComponent_SetTilingFactor(UUID entityID, glm::vec2 *tilingfactor)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<SpriteRenderer2DComponent>().TillingFactor = *tilingfactor;
	}

	static void BoxCollider2DComponent_GetOffset(UUID entityID, glm::vec2 *outOffset)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		*outOffset = entity.GetComponent<BoxCollider2DComponent>().Offset;
	}

	static void BoxCollider2DComponent_SetOffset(UUID entityID, glm::vec2 *offset)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<BoxCollider2DComponent>().Offset = *offset;
	}

	static void BoxCollider2DComponent_GetSize(UUID entityID, glm::vec2 *outSize)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		*outSize = entity.GetComponent<BoxCollider2DComponent>().Size;
	}

	static void BoxCollider2DComponent_SetSize(UUID entityID, glm::vec2 *size)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<BoxCollider2DComponent>().Size = *size;
	}

	static void BoxCollider2DComponent_GetDensity(UUID entityID, float *outDensity)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		*outDensity = entity.GetComponent<BoxCollider2DComponent>().Density;
	}

	static void BoxCollider2DComponent_SetDensity(UUID entityID, float *density)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<BoxCollider2DComponent>().Density = *density;
	}

	static void BoxCollider2DComponent_GetFriction(UUID entityID, float *outFriction)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		*outFriction = entity.GetComponent<BoxCollider2DComponent>().Friction;
	}

	static void BoxCollider2DComponent_SetFriction(UUID entityID, float *friction)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<BoxCollider2DComponent>().Friction = *friction;
	}

	static void BoxCollider2DComponent_GetRestitution(UUID entityID, float *outRestitution)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		*outRestitution = entity.GetComponent<BoxCollider2DComponent>().Restitution;
	}

	static void BoxCollider2DComponent_SetRestitution(UUID entityID, float *restitution)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<BoxCollider2DComponent>().Restitution = *restitution;
	}

	static void BoxCollider2DComponent_GetRestitutionThreshold(UUID entityID, float *outRestitutionThreshold)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		*outRestitutionThreshold = entity.GetComponent<BoxCollider2DComponent>().RestitutionThreshold;
	}

	static void BoxCollider2DComponent_SetRestitutionThreshold(UUID entityID, float *restitutionThreshold)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<BoxCollider2DComponent>().RestitutionThreshold = *restitutionThreshold;
	}

	static void CircleCollider2DComponent_GetOffset(UUID entityID, glm::vec2 *outOffset)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		*outOffset = entity.GetComponent<CircleCollider2DComponent>().Offset;
	}

	static void CircleCollider2DComponent_SetOffset(UUID entityID, glm::vec2 *offset)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleCollider2DComponent>().Offset = *offset;
	}

	static void CircleCollider2DComponent_GetRadius(UUID entityID, float *outRadius)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		*outRadius = entity.GetComponent<CircleCollider2DComponent>().Radius;
	}

	static void CircleCollider2DComponent_SetRadius(UUID entityID, float *radius)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleCollider2DComponent>().Radius = *radius;
	}

	static void CircleCollider2DComponent_GetDensity(UUID entityID, float *outDensity)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		*outDensity = entity.GetComponent<CircleCollider2DComponent>().Density;
	}

	static void CircleCollider2DComponent_SetDensity(UUID entityID, float *density)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleCollider2DComponent>().Density = *density;
	}

	static void CircleCollider2DComponent_GetFriction(UUID entityID, float *outFriction)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		*outFriction = entity.GetComponent<CircleCollider2DComponent>().Friction;
	}

	static void CircleCollider2DComponent_SetFriction(UUID entityID, float *friction)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleCollider2DComponent>().Friction = *friction;
	}

	static void CircleCollider2DComponent_GetRestitution(UUID entityID, float *outRestituion)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		*outRestituion = entity.GetComponent<CircleCollider2DComponent>().Restitution;
	}

	static void CircleCollider2DComponent_SetRestitution(UUID entityID, float *restitution)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleCollider2DComponent>().Restitution = *restitution;
	}

	static void CircleCollider2DComponent_GetRestitutionThreshold(UUID entityID, float *outRestitutionThreshold)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		*outRestitutionThreshold = entity.GetComponent<CircleCollider2DComponent>().RestitutionThreshold;
	}

	static void CircleCollider2DComponent_SetRestitutionThreshold(UUID entityID, float *restitutionThreshold)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleCollider2DComponent>().RestitutionThreshold = *restitutionThreshold;
	}

	static void SpriteAnimationComponent_GetActiveState(UUID entityID, MonoString *state)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		auto &ac = entity.GetComponent<SpriteAnimationComponent>();
		if (ac.State->HasAnimations())
		{
			state = ScriptEngine::CreateString(ac.State->GetCurrentState().c_str());
		}
	}

	static void SpriteAnimationComponent_SetActiveState(UUID entityID, MonoString *state)
	{
		PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "ScriptGlue: Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity, "ScriptGlue: Invalid Entity");

		auto &ac = entity.GetComponent<SpriteAnimationComponent>();
		if (ac.State->HasAnimations())
		{
			ac.State->SetActiveState(Utils::MonoStringToString(state));
		}
	}

	static bool Input_IsKeyPressed(KeyCode keycode)
	{
		PROFILER_LOGIC();

		return Input::IsKeyPressed(keycode);
	}

	static bool Input_IsKeyReleased(KeyCode keycode)
	{
		PROFILER_LOGIC();

		return Input::IsKeyReleased(keycode);
	}

	template <typename... Component>
	static void RegisterComponent()
	{
		PROFILER_LOGIC();

		([]()
		 {
			 std::string_view typeName = typeid(Component).name();
			 size_t pos = typeName.find_last_of(':');
			 std::string_view structName = typeName.substr(pos + 1);
			 std::string managedTypename = fmt::format("ORiginEngine.{}", structName);

			 MonoType *managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssemblyImage());
			 if (!managedType)
			 {
				 OGN_CORE_ERROR("SCRIPT GLUE: Could not find component type {}", managedTypename);
				 return;
			 }

			 s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
			 s_EntityAddComponentFuncs[managedType] = [](Entity entity) { entity.AddOrReplaceComponent<Component>(); };

		 }(), ...);
	}

	template <typename... Component>
	static void RegisterComponent(ComponentGroup<Component...>)
	{
		PROFILER_LOGIC();

		RegisterComponent<Component...>();
	}

	void ScriptGlue::RegisterComponents()
	{
		s_EntityHasComponentFuncs.clear();
		RegisterComponent(AllComponents{});
	}

	void ScriptGlue::RegisterFunctions()
	{
		// Entity
		OGN_ADD_INTERNAL_CALLS(Entity_FindEntityByName);
		OGN_ADD_INTERNAL_CALLS(Entity_AddComponent);
		OGN_ADD_INTERNAL_CALLS(Entity_HasComponent);

		// Logging
		OGN_ADD_INTERNAL_CALLS(NativeLog);
		OGN_ADD_INTERNAL_CALLS(NativeLog_Vector);
		OGN_ADD_INTERNAL_CALLS(NativeLog_VectorDot);

		// Components
		OGN_ADD_INTERNAL_CALLS(TransformComponent_GetTranslation);
		OGN_ADD_INTERNAL_CALLS(TransformComponent_SetTranslation);
		OGN_ADD_INTERNAL_CALLS(TransformComponent_GetRotation);
		OGN_ADD_INTERNAL_CALLS(TransformComponent_SetRotation);
		OGN_ADD_INTERNAL_CALLS(TransformComponent_GetScale);
		OGN_ADD_INTERNAL_CALLS(TransformComponent_SetScale);

		OGN_ADD_INTERNAL_CALLS(RigidbodyComponent_SetVelocity);
		OGN_ADD_INTERNAL_CALLS(RigidbodyComponent_SetForce);
		OGN_ADD_INTERNAL_CALLS(RigidbodyComponent_SetVelocityForce);
		OGN_ADD_INTERNAL_CALLS(RigidbodyComponent_SetImpulseForce);
		OGN_ADD_INTERNAL_CALLS(RigidbodyComponent_GetVelocity);

		OGN_ADD_INTERNAL_CALLS(Rigidbody2DComponent_GetVelocity);
		OGN_ADD_INTERNAL_CALLS(Rigidbody2DComponent_SetVelocity);
		OGN_ADD_INTERNAL_CALLS(Rigidbody2DComponent_ApplyLinearImpulse);
		OGN_ADD_INTERNAL_CALLS(Rigidbody2DComponent_ApplyLinearImpulseToCenter);
		OGN_ADD_INTERNAL_CALLS(Rigidbody2DComponent_ApplyForce);
		OGN_ADD_INTERNAL_CALLS(Rigidbody2DComponent_ApplyForceToCenter);
		OGN_ADD_INTERNAL_CALLS(Rigidbody2DComponent_IsContactWithTag);
		OGN_ADD_INTERNAL_CALLS(Rigidbody2DComponent_GetContactWithTag);

		OGN_ADD_INTERNAL_CALLS(AudioComponent_Play);
		OGN_ADD_INTERNAL_CALLS(AudioComponent_Stop);
		OGN_ADD_INTERNAL_CALLS(AudioComponent_SetVolume);
		OGN_ADD_INTERNAL_CALLS(AudioComponent_GetVolume);
		OGN_ADD_INTERNAL_CALLS(AudioComponent_SetName);
		OGN_ADD_INTERNAL_CALLS(AudioComponent_GetName);
		OGN_ADD_INTERNAL_CALLS(AudioComponent_SetPitch);
		OGN_ADD_INTERNAL_CALLS(AudioComponent_GetPitch);
		OGN_ADD_INTERNAL_CALLS(AudioComponent_SetMinDistance);
		OGN_ADD_INTERNAL_CALLS(AudioComponent_SetMaxDistance);
		OGN_ADD_INTERNAL_CALLS(AudioComponent_GetMinDistance);
		OGN_ADD_INTERNAL_CALLS(AudioComponent_GetMaxDistance);
		OGN_ADD_INTERNAL_CALLS(AudioComponent_SetLooping);
		OGN_ADD_INTERNAL_CALLS(AudioComponent_IsLooping);
		OGN_ADD_INTERNAL_CALLS(AudioComponent_SetSpatial);
		OGN_ADD_INTERNAL_CALLS(AudioComponent_IsSpatial);
		OGN_ADD_INTERNAL_CALLS(AudioComponent_SetPlayAtStart);
		OGN_ADD_INTERNAL_CALLS(AudioComponent_IsPlayAtStart);

		OGN_ADD_INTERNAL_CALLS(TextComponent_GetText);
		OGN_ADD_INTERNAL_CALLS(TextComponent_SetText);
		OGN_ADD_INTERNAL_CALLS(TextComponent_GetColor);
		OGN_ADD_INTERNAL_CALLS(TextComponent_SetColor);
		OGN_ADD_INTERNAL_CALLS(TextComponent_GetKerning);
		OGN_ADD_INTERNAL_CALLS(TextComponent_SetKerning);
		OGN_ADD_INTERNAL_CALLS(TextComponent_GetLineSpacing);
		OGN_ADD_INTERNAL_CALLS(TextComponent_SetLineSpacing);

		OGN_ADD_INTERNAL_CALLS(CircleRendererComponent_GetColor);
		OGN_ADD_INTERNAL_CALLS(CircleRendererComponent_SetColor);
		OGN_ADD_INTERNAL_CALLS(CircleRendererComponent_GetFade);
		OGN_ADD_INTERNAL_CALLS(CircleRendererComponent_SetFade);
		OGN_ADD_INTERNAL_CALLS(CircleRendererComponent_GetThickness);
		OGN_ADD_INTERNAL_CALLS(CircleRendererComponent_SetThickness);

		OGN_ADD_INTERNAL_CALLS(SpriteRenderer2DComponent_GetColor);
		OGN_ADD_INTERNAL_CALLS(SpriteRenderer2DComponent_SetColor);
		OGN_ADD_INTERNAL_CALLS(SpriteRenderer2DComponent_GetTilingFactor);
		OGN_ADD_INTERNAL_CALLS(SpriteRenderer2DComponent_SetTilingFactor);
		OGN_ADD_INTERNAL_CALLS(SpriteRenderer2DComponent_GetFlipX);
		OGN_ADD_INTERNAL_CALLS(SpriteRenderer2DComponent_SetFlipX);
		OGN_ADD_INTERNAL_CALLS(SpriteRenderer2DComponent_GetFlipY);
		OGN_ADD_INTERNAL_CALLS(SpriteRenderer2DComponent_SetFlipY);

		OGN_ADD_INTERNAL_CALLS(BoxCollider2DComponent_GetOffset);
		OGN_ADD_INTERNAL_CALLS(BoxCollider2DComponent_SetOffset);
		OGN_ADD_INTERNAL_CALLS(BoxCollider2DComponent_GetSize);
		OGN_ADD_INTERNAL_CALLS(BoxCollider2DComponent_SetSize);
		OGN_ADD_INTERNAL_CALLS(BoxCollider2DComponent_GetDensity);
		OGN_ADD_INTERNAL_CALLS(BoxCollider2DComponent_SetDensity);
		OGN_ADD_INTERNAL_CALLS(BoxCollider2DComponent_GetFriction);
		OGN_ADD_INTERNAL_CALLS(BoxCollider2DComponent_SetFriction);
		OGN_ADD_INTERNAL_CALLS(BoxCollider2DComponent_GetRestitution);
		OGN_ADD_INTERNAL_CALLS(BoxCollider2DComponent_SetRestitution);
		OGN_ADD_INTERNAL_CALLS(BoxCollider2DComponent_GetRestitutionThreshold);
		OGN_ADD_INTERNAL_CALLS(BoxCollider2DComponent_SetRestitutionThreshold);

		OGN_ADD_INTERNAL_CALLS(CircleCollider2DComponent_GetOffset);
		OGN_ADD_INTERNAL_CALLS(CircleCollider2DComponent_SetOffset);
		OGN_ADD_INTERNAL_CALLS(CircleCollider2DComponent_GetRadius);
		OGN_ADD_INTERNAL_CALLS(CircleCollider2DComponent_SetRadius);
		OGN_ADD_INTERNAL_CALLS(CircleCollider2DComponent_GetDensity);
		OGN_ADD_INTERNAL_CALLS(CircleCollider2DComponent_SetDensity);
		OGN_ADD_INTERNAL_CALLS(CircleCollider2DComponent_GetFriction);
		OGN_ADD_INTERNAL_CALLS(CircleCollider2DComponent_SetFriction);
		OGN_ADD_INTERNAL_CALLS(CircleCollider2DComponent_GetRestitution);
		OGN_ADD_INTERNAL_CALLS(CircleCollider2DComponent_SetRestitution);
		OGN_ADD_INTERNAL_CALLS(CircleCollider2DComponent_GetRestitutionThreshold);
		OGN_ADD_INTERNAL_CALLS(CircleCollider2DComponent_SetRestitutionThreshold);

		OGN_ADD_INTERNAL_CALLS(SpriteAnimationComponent_SetActiveState);
		OGN_ADD_INTERNAL_CALLS(SpriteAnimationComponent_GetActiveState);

		OGN_ADD_INTERNAL_CALLS(GetScriptInstance);

		OGN_ADD_INTERNAL_CALLS(Input_IsKeyPressed);
		OGN_ADD_INTERNAL_CALLS(Input_IsKeyReleased);
	}
}
