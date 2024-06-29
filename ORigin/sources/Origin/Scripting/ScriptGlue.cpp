// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"

#include "ScriptGlue.h"
#include "ScriptEngine.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Scene/Components.h"
#include "Origin/Scene/Scene.h"
#include "Origin/Scene/Entity.h"
#include "Origin/Scene/EntityManager.h"
#include "Origin/Core/KeyCodes.h"
#include "Origin/Core/Input.h"
#include "Origin/Physics/Physics2D.h"

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

	// ==============================================
	// Entity

	static bool Entity_HasComponent(UUID entityID, MonoReflectionType *componentType)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
			Entity entity = scene->GetEntityWithUUID(entityID);

		MonoType *managedType = mono_reflection_type_get_type(componentType);
		OGN_CORE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end(), "ScriptGlue: Failed to process Entity_HasComponent")
			return s_EntityHasComponentFuncs.at(managedType)(entity);
	}

	static void Entity_AddComponent(UUID entityID, MonoReflectionType *componentType)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		MonoType *managedType = mono_reflection_type_get_type(componentType);
		OGN_CORE_ASSERT(s_EntityAddComponentFuncs.find(managedType) != s_EntityAddComponentFuncs.end(), "ScriptGlue: Failed to process AddComponent");
		s_EntityAddComponentFuncs.at(managedType)(entity);
	}

	static uint64_t Entity_FindEntityByName(MonoString *stringName)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");

		std::string name = Utils::MonoStringToString(stringName);
		Entity entity = scene->FindEntityByName(name);

		if (!entity.IsValid())
			return 0;

		return entity.GetUUID();
	}

	static uint64_t Entity_Instantiate(UUID entityID, glm::vec3 translation)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");

		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			Entity copyEntity = EntityManager::DuplicateEntity(entity, scene);
			copyEntity.GetComponent<TransformComponent>().WorldTranslation = translation;
			scene->GetPhysics2D()->OnInstantiateScriptEntity(copyEntity);
			return copyEntity.GetUUID();
		}
		
		return 0;
	}

	static void Entity_Destroy(UUID entityID)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");

		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			scene->GetPhysics2D()->OnDestroyScriptEntity(entity);
			scene->DestroyEntity(entity);
		}
	}

	// ==============================================
	// Logging
	static void NativeLog(MonoString *string, int parameter)
	{
		OGN_PROFILER_LOGIC();

		std::string str = Utils::MonoStringToString(string);
		std::cout << str << ", " << parameter << std::endl;
	}

	static void NativeLog_Vector(glm::vec3 *parameter, glm::vec3 *outResult)
	{
		OGN_PROFILER_LOGIC();

		*outResult = normalize(*parameter);
	}

	static float NativeLog_VectorDot(glm::vec3 parameter)
	{
		OGN_PROFILER_LOGIC();

		return dot(parameter, parameter);
	}

	// ==============================================
	// Component
	static MonoObject *GetScriptInstance(UUID entityID)
	{
		OGN_PROFILER_LOGIC();

		return ScriptEngine::GetManagedInstance(entityID);
	}

	static void TransformComponent_GetTranslation(UUID entityID, glm::vec3 *outTranslation)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			*outTranslation = entity.GetComponent<TransformComponent>().Translation;
		}
	}

	static void TransformComponent_SetTranslation(UUID entityID, glm::vec3 translation)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<TransformComponent>().Translation = translation;
		}
	}

	static void TransformComponent_GetRotation(UUID entityID, glm::vec3 *outRotation)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		if (entity.IsValid())
		{
			*outRotation = entity.GetComponent<TransformComponent>().Rotation;
		}
	}

	static void TransformComponent_SetRotation(UUID entityID, glm::vec3 rotation)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		if (entity.IsValid())
		{
			entity.GetComponent<TransformComponent>().Rotation = rotation;
		}
	}

	static void TransformComponent_GetScale(UUID entityID, glm::vec3 *outScale)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		if (entity.IsValid())
		{
			*outScale = entity.GetComponent<TransformComponent>().Scale;
		}
	}

	static void TransformComponent_SetScale(UUID entityID, glm::vec3 scale)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<TransformComponent>().Scale = scale;
		}
	}

	static void RigidbodyComponent_SetVelocity(UUID entityID, glm::vec3 velocity, bool autoWake)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			auto &rb = entity.GetComponent<RigidbodyComponent>();
			rb.ApplyLinearVelocity(velocity, autoWake);
		}
	}

	static void RigidbodyComponent_GetVelocity(UUID entityID, glm::vec3 *velocity)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			auto &rb = entity.GetComponent<RigidbodyComponent>();

			physx::PxRigidActor *actor = (physx::PxRigidActor *)rb.Body;
			physx::PxRigidDynamic *dActor = actor->is<physx::PxRigidDynamic>();
			*velocity = glm::vec3(dActor->getLinearVelocity().x, dActor->getLinearVelocity().y, dActor->getLinearVelocity().z);
		}

	}

	static void RigidbodyComponent_SetVelocityForce(UUID entityID, glm::vec3 force)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			auto &rb = entity.GetComponent<RigidbodyComponent>();
			rb.ApplyVelocityForce(force);
		}
	}

	static void RigidbodyComponent_SetForce(UUID entityID, glm::vec3 force)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			auto &rb = entity.GetComponent<RigidbodyComponent>();
			rb.ApplyForce(force);
		}
	}

	static void RigidbodyComponent_SetImpulseForce(UUID entityID, glm::vec3 force)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			auto &rb = entity.GetComponent<RigidbodyComponent>();
			rb.ApplyImpulseForce(force);
		}
	}

	static void Rigidbody2DComponent_SetVelocity(UUID entityID, glm::vec2 velocity)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			auto &rb2d = entity.GetComponent<Rigidbody2DComponent>();
			auto body = static_cast<b2Body *>(rb2d.RuntimeBody);

			body->SetLinearVelocity(b2Vec2(velocity.x, velocity.y));
		}
	}

	static void Rigidbody2DComponent_GetVelocity(UUID entityID, glm::vec2 *velocity)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			auto &rb2d = entity.GetComponent<Rigidbody2DComponent>();
			auto body = static_cast<b2Body *>(rb2d.RuntimeBody);
			auto v = body->GetLinearVelocity();

			*velocity = glm::vec2(v.x, v.y);
		}
	}

	static bool Rigidbody2DComponent_IsContactWithTag(UUID entityID, MonoString *contactWith)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			auto &rb2d = entity.GetComponent<Rigidbody2DComponent>();
			std::string contactName = Utils::MonoStringToString(contactWith);
			return rb2d.ContactWith == contactName;
		}

		OGN_CORE_ERROR("[ScriptGlue] Invalid entity ID: {}", entityID);
		return false;
	}

	static MonoString *Rigidbody2DComponent_GetContactWithTag(UUID entityID)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			auto &rb2d = entity.GetComponent<Rigidbody2DComponent>();
			return ScriptEngine::CreateString(rb2d.ContactWith.c_str());
		}

		OGN_CORE_ERROR("[ScriptGlue] Invalid entity ID: {}", entityID);
		return nullptr;
	}

	static void Rigidbody2DComponent_ApplyLinearImpulse(UUID entityID, glm::vec2 impulse, glm::vec2 point, bool wake)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			auto &rb2d = entity.GetComponent<Rigidbody2DComponent>();
			auto body = static_cast<b2Body *>(rb2d.RuntimeBody);
			body->ApplyLinearImpulse(b2Vec2(impulse.x, impulse.y), b2Vec2(point.x, point.y), wake);
		}
	}

	static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID entityID, glm::vec2 impulse, bool wake)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		if (entity.IsValid())
		{
			auto &rb2d = entity.GetComponent<Rigidbody2DComponent>();
			auto body = static_cast<b2Body *>(rb2d.RuntimeBody);
			body->ApplyLinearImpulseToCenter(b2Vec2(impulse.x, impulse.y), wake);
		}
	}

	static void Rigidbody2DComponent_ApplyForce(UUID entityID, glm::vec2 force, glm::vec2 point, bool wake)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			auto &rb2d = entity.GetComponent<Rigidbody2DComponent>();
			auto body = static_cast<b2Body *>(rb2d.RuntimeBody);
			body->ApplyForce(b2Vec2(force.x, force.y), b2Vec2(point.x, point.y), wake);
		}
	}

	static void Rigidbody2DComponent_ApplyForceToCenter(UUID entityID, glm::vec2 force, bool wake)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			auto &rb2d = entity.GetComponent<Rigidbody2DComponent>();
			auto body = static_cast<b2Body *>(rb2d.RuntimeBody);
			body->ApplyForceToCenter(b2Vec2(force.x, force.y), wake);
		}
	}

	static MonoString *AudioComponent_GetName(UUID entityID)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			auto &ac = entity.GetComponent<AudioComponent>();
			return ScriptEngine::CreateString(ac.Name.c_str());
		}
		OGN_CORE_ERROR("[ScriptGlue]: Invalid Entity");
		return nullptr;
	}

	static void AudioComponent_SetName(UUID entityID, MonoString *name)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			auto &ac = entity.GetComponent<AudioComponent>();
			ac.Name = Utils::MonoStringToString(name);
		}
	}

	static void AudioComponent_Play(UUID entityID)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			auto &ac = entity.GetComponent<AudioComponent>();
			OGN_CORE_ASSERT(ac.Audio, "ScriptGlue: Invalid Audio");

			const std::shared_ptr<AudioSource> &audio = AssetManager::GetAsset<AudioSource>(ac.Audio);
			audio->Play();
		}
	}

	static void AudioComponent_Stop(UUID entityID)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			auto &ac = entity.GetComponent<AudioComponent>();
			OGN_CORE_ASSERT(ac.Audio, "ScriptGlue: Invalid Audio");
			const std::shared_ptr<AudioSource> &audio = AssetManager::GetAsset<AudioSource>(ac.Audio);
			audio->Stop();
		}
	}

	static void AudioComponent_SetVolume(UUID entityID, float volume)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<AudioComponent>().Volume = volume;
		}
	}

	static float AudioComponent_GetVolume(UUID entityID)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			return entity.GetComponent<AudioComponent>().Volume;
		}

		OGN_CORE_ERROR("[ScriptGlue] Invalid entity ID: {}", entityID);
		return 0.0f;
	}

	static void AudioComponent_SetMinDistance(UUID entityID, float minDistance)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<AudioComponent>().MinDistance = minDistance;
		}
	}

	static float AudioComponent_GetMinDistance(UUID entityID)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			return entity.GetComponent<AudioComponent>().MinDistance;
		}
		OGN_CORE_ERROR("[ScriptGlue]: Invalid Entity");
		return 0.0f;
	}

	static void AudioComponent_SetMaxDistance(UUID entityID, float maxDistance)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<AudioComponent>().MaxDistance = maxDistance;
		}
	}

	static float AudioComponent_GetMaxDistance(UUID entityID)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if(entity.IsValid())
		if (entity.IsValid())
		{
			return entity.GetComponent<AudioComponent>().MaxDistance;
		}
		OGN_CORE_ERROR("[ScriptGlue]: Invalid Entity");
		return 0.0f;
	}

	static void AudioComponent_SetPitch(UUID entityID, float pitch)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<AudioComponent>().Pitch = pitch;
		}
	}

	static float AudioComponent_GetPitch(UUID entityID)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			return entity.GetComponent<AudioComponent>().Pitch;
		}

		OGN_CORE_ERROR("[ScriptGlue]: Invalid Entity");
		return 0.0f;
	}

	static void AudioComponent_SetLooping(UUID entityID, bool looping)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if(entity.IsValid())

		entity.GetComponent<AudioComponent>().Looping = looping;
	}

	static bool AudioComponent_IsLooping(UUID entityID)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			return entity.GetComponent<AudioComponent>().Looping;
		}

		OGN_CORE_ERROR("[ScriptGlue]: Invalid Entity");
		return false;
	}

	static void AudioComponent_SetSpatial(UUID entityID, bool spatial)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		if (entity.IsValid())
		{
			entity.GetComponent<AudioComponent>().Spatializing = spatial;
		}
	}

	static bool AudioComponent_IsSpatial(UUID entityID)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			return entity.GetComponent<AudioComponent>().Spatializing;
		}

		OGN_CORE_ERROR("[ScriptGlue]: Invalid Entity");
		return false;
	}

	static void AudioComponent_SetPlayAtStart(UUID entityID, bool playAtStart)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<AudioComponent>().PlayAtStart = playAtStart;
		}
	}

	static bool AudioComponent_IsPlayAtStart(UUID entityID)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			return entity.GetComponent<AudioComponent>().PlayAtStart;
		}

		OGN_CORE_ERROR("[ScriptGlue]: Invalid Entity");
		return false;
	}

	static MonoString *TextComponent_GetText(UUID entityID)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			auto &tc = entity.GetComponent<TextComponent>();
			return ScriptEngine::CreateString(tc.TextString.c_str());
		}
		OGN_CORE_ERROR("[ScriptGlue]: Invalid Entity");
		return nullptr;
	}

	static void TextComponent_SetText(UUID entityID, MonoString *textString)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			auto &tc = entity.GetComponent<TextComponent>();
			tc.TextString = Utils::MonoStringToString(textString);
		}
	}

	static void TextComponent_GetColor(UUID entityID, glm::vec3 *color)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			*color = glm::vec3(entity.GetComponent<TextComponent>().Color);
		}
	}

	static void TextComponent_SetColor(UUID entityID, glm::vec3 color)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<TextComponent>().Color = { color.r, color.g, color.b, 1.0f };
		}
	}

	static float TextComponent_GetKerning(UUID entityID)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			return entity.GetComponent<TextComponent>().Kerning;
		}
		OGN_CORE_ERROR("[ScriptGlue]: Invalid Entity");
		return 0.0f;
	}

	static void TextComponent_SetKerning(UUID entityID, float kerning)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<TextComponent>().Kerning = kerning;
		}
	}

	static float TextComponent_GetLineSpacing(UUID entityID)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			return entity.GetComponent<TextComponent>().LineSpacing;
		}
		OGN_CORE_ERROR("[ScriptGlue]: Invalid Entity");
		return 0.0f;
	}

	static void TextComponent_SetLineSpacing(UUID entityID, float lineSpacing)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<TextComponent>().LineSpacing;
		}
	}

	static void CircleRendererComponent_GetColor(UUID entityID, glm::vec3 *outColor)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if(entity.IsValid())
		{
			*outColor = glm::vec3(entity.GetComponent<CircleRendererComponent>().Color);
		}
	}

	static void CircleRendererComponent_SetColor(UUID entityID, glm::vec3 color)
	{
		OGN_PROFILER_LOGIC();
		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<CircleRendererComponent>().Color = { color.r, color.g, color.b, 1.0f };
		}
	}

	static void CircleRendererComponent_GetFade(UUID entityID, float *outFade)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if(entity.IsValid())
		{
			*outFade = entity.GetComponent<CircleRendererComponent>().Fade;
		}
	}

	static void CircleRendererComponent_SetFade(UUID entityID, float fade)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<CircleRendererComponent>().Fade = fade;
		}
	}

	static void CircleRendererComponent_GetThickness(UUID entityID, float *outThickness)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			*outThickness = entity.GetComponent<CircleRendererComponent>().Thickness;
		}
	}

	static void CircleRendererComponent_SetThickness(UUID entityID, float thickness)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<CircleRendererComponent>().Thickness = thickness;
		}
	}

	static void SpriteRenderer2DComponent_GetColor(UUID entityID, glm::vec4 *outColor)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			*outColor = entity.GetComponent<SpriteRenderer2DComponent>().Color;
		}
	}

	static void SpriteRenderer2DComponent_SetFlipX(UUID entityID, bool flip)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<SpriteRenderer2DComponent>().FlipX = flip;
		}
	}

	static void SpriteRenderer2DComponent_GetFlipX(UUID entityID, bool *flip)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			*flip = entity.GetComponent<SpriteRenderer2DComponent>().FlipX;
		}
	}

	static void SpriteRenderer2DComponent_SetFlipY(UUID entityID, bool flip)
	{
		OGN_PROFILER_LOGIC();
		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<SpriteRenderer2DComponent>().FlipY = flip;
		}
	}

	static void SpriteRenderer2DComponent_GetFlipY(UUID entityID, bool *flip)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");

		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			*flip = entity.GetComponent<SpriteRenderer2DComponent>().FlipY;
		}
	}

	static void SpriteRenderer2DComponent_SetColor(UUID entityID, glm::vec4 color)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<SpriteRenderer2DComponent>().Color = color;
		}
		
	}

	static void SpriteRenderer2DComponent_GetTilingFactor(UUID entityID, glm::vec2 *tilingfactor)
	{
		OGN_PROFILER_LOGIC();
		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			*tilingfactor = entity.GetComponent<SpriteRenderer2DComponent>().TillingFactor;
		}
	}

	static void SpriteRenderer2DComponent_SetTilingFactor(UUID entityID, glm::vec2 tilingfactor)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<SpriteRenderer2DComponent>().TillingFactor = tilingfactor;
		}
	}

	static void BoxCollider2DComponent_GetOffset(UUID entityID, glm::vec2 *outOffset)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			*outOffset = entity.GetComponent<BoxCollider2DComponent>().Offset;
		}
	}

	static void BoxCollider2DComponent_SetOffset(UUID entityID, glm::vec2 offset)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<BoxCollider2DComponent>().Offset = offset;
		}
	}

	static void BoxCollider2DComponent_GetSize(UUID entityID, glm::vec2 *outSize)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			*outSize = entity.GetComponent<BoxCollider2DComponent>().Size;
		}
	}

	static void BoxCollider2DComponent_SetSize(UUID entityID, glm::vec2 *size)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<BoxCollider2DComponent>().Size = *size;
		}
	}

	static void BoxCollider2DComponent_GetDensity(UUID entityID, float *outDensity)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			*outDensity = entity.GetComponent<BoxCollider2DComponent>().Density;
		}
	}

	static void BoxCollider2DComponent_SetDensity(UUID entityID, float density)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<BoxCollider2DComponent>().Density = density;
		}

		
	}

	static void BoxCollider2DComponent_GetFriction(UUID entityID, float *outFriction)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			*outFriction = entity.GetComponent<BoxCollider2DComponent>().Friction;
		}
	}

	static void BoxCollider2DComponent_SetFriction(UUID entityID, float friction)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<BoxCollider2DComponent>().Friction = friction;
		}
	}

	static void BoxCollider2DComponent_GetRestitution(UUID entityID, float *outRestitution)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			*outRestitution = entity.GetComponent<BoxCollider2DComponent>().Restitution;
		}
	}

	static void BoxCollider2DComponent_SetRestitution(UUID entityID, float restitution)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<BoxCollider2DComponent>().Restitution = restitution;
		}
	}

	static void BoxCollider2DComponent_GetRestitutionThreshold(UUID entityID, float *outRestitutionThreshold)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			*outRestitutionThreshold = entity.GetComponent<BoxCollider2DComponent>().RestitutionThreshold;
		}
	}

	static void BoxCollider2DComponent_SetRestitutionThreshold(UUID entityID, float restitutionThreshold)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<BoxCollider2DComponent>().RestitutionThreshold = restitutionThreshold;
		}
	}

	static void CircleCollider2DComponent_GetOffset(UUID entityID, glm::vec2 *outOffset)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			*outOffset = entity.GetComponent<CircleCollider2DComponent>().Offset;
		}
	}

	static void CircleCollider2DComponent_SetOffset(UUID entityID, glm::vec2 offset)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<CircleCollider2DComponent>().Offset = offset;
		}
	}

	static void CircleCollider2DComponent_GetRadius(UUID entityID, float *outRadius)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			*outRadius = entity.GetComponent<CircleCollider2DComponent>().Radius;
		}
	}

	static void CircleCollider2DComponent_SetRadius(UUID entityID, float radius)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<CircleCollider2DComponent>().Radius = radius;
		}
	}

	static void CircleCollider2DComponent_GetDensity(UUID entityID, float *outDensity)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			*outDensity = entity.GetComponent<CircleCollider2DComponent>().Density;
		}
	}

	static void CircleCollider2DComponent_SetDensity(UUID entityID, float density)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<CircleCollider2DComponent>().Density = density;
		}
	}

	static void CircleCollider2DComponent_GetFriction(UUID entityID, float *outFriction)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			*outFriction = entity.GetComponent<CircleCollider2DComponent>().Friction;
		}
	}

	static void CircleCollider2DComponent_SetFriction(UUID entityID, float friction)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<CircleCollider2DComponent>().Friction = friction;
		}
	}

	static void CircleCollider2DComponent_GetRestitution(UUID entityID, float *outRestituion)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			*outRestituion = entity.GetComponent<CircleCollider2DComponent>().Restitution;
		}
	}

	static void CircleCollider2DComponent_SetRestitution(UUID entityID, float restitution)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<CircleCollider2DComponent>().Restitution = restitution;
		}
	}

	static void CircleCollider2DComponent_GetRestitutionThreshold(UUID entityID, float *outRestitutionThreshold)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			*outRestitutionThreshold = entity.GetComponent<CircleCollider2DComponent>().RestitutionThreshold;
		}
	}

	static void CircleCollider2DComponent_SetRestitutionThreshold(UUID entityID, float restitutionThreshold)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			entity.GetComponent<CircleCollider2DComponent>().RestitutionThreshold = restitutionThreshold;
		}
	}

	static void SpriteAnimationComponent_GetActiveState(UUID entityID, MonoString *state)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			auto &ac = entity.GetComponent<SpriteAnimationComponent>();
			if (ac.State->HasAnimations())
			{
				state = ScriptEngine::CreateString(ac.State->GetCurrentState().c_str());
			}
		}
	}

	static void SpriteAnimationComponent_SetActiveState(UUID entityID, MonoString *state)
	{
		OGN_PROFILER_LOGIC();

		Scene *scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "[ScriptGlue] Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);
		if (entity.IsValid())
		{
			auto &ac = entity.GetComponent<SpriteAnimationComponent>();
			if (ac.State->HasAnimations())
			{
				ac.State->SetActiveState(Utils::MonoStringToString(state));
			}
		}
	}

	static bool Input_IsKeyPressed(KeyCode keycode)
	{
		OGN_PROFILER_LOGIC();

		return Input::IsKeyPressed(keycode);
	}

	static bool Input_IsKeyReleased(KeyCode keycode)
	{
		OGN_PROFILER_LOGIC();

		return Input::IsKeyReleased(keycode);
	}

	template <typename... Component>
	static void RegisterComponent()
	{
		OGN_PROFILER_LOGIC();

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
		OGN_PROFILER_LOGIC();

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
		OGN_ADD_INTERNAL_CALLS(Entity_Instantiate);
		OGN_ADD_INTERNAL_CALLS(Entity_Destroy);

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
