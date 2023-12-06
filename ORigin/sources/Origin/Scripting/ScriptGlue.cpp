// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"

#include "Origin/Asset/AssetManager.h"

#include "ScriptGlue.h"
#include "ScriptEngine.h"

#include "Origin/Scene/Components.h"

#include "Origin/Scene/Scene.h"
#include "Origin/Scene/Entity.h"

#include "Origin/IO/KeyCodes.h"
#include "Origin/IO/Input.h"

#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"

#include "box2d/b2_body.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"

#include "Origin/Audio/Audio.h"

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

	// Entity
	static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		OGN_CORE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end(), "")
		return s_EntityHasComponentFuncs.at(managedType)(entity);
	}

	static uint64_t Entity_FindEntityByName(MonoString* stringName)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
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
	static void NativeLog(MonoString* string, int parameter)
	{
		std::string str = Utils::MonoStringToString(string);
		std::cout << str << ", " << parameter << std::endl;
	}

	static void NativeLog_Vector(glm::vec3* parameter, glm::vec3* outResult)
	{
		*outResult = normalize(*parameter);
	}

	static float NativeLog_VectorDot(glm::vec3* parameter)
	{
		return dot(*parameter, *parameter);
	}

	// Component
	static MonoObject* GetScriptInstance(UUID entityID)
	{
		return ScriptEngine::GetManagedInstance(entityID);
	}

	static void TransformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outTranslation = entity.GetComponent<TransformComponent>().Translation;
	}

	static void TransformComponent_SetTranslation(UUID entityID, glm::vec3* translation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<TransformComponent>().Translation = *translation;
	}

	static void TransformComponent_GetRotation(UUID entityID, glm::vec3* outRotation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outRotation = entity.GetComponent<TransformComponent>().Rotation;
	}

	static void TransformComponent_SetRotation(UUID entityID, glm::vec3* rotation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<TransformComponent>().Rotation = *rotation;
	}

	static void TransformComponent_GetScale(UUID entityID, glm::vec3* outScale)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outScale = entity.GetComponent<TransformComponent>().Scale;
	}

	static void TransformComponent_SetScale(UUID entityID, glm::vec3* scale)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<TransformComponent>().Scale = *scale;
	}

	static bool Rigidbody2DComponent_IsContactWithTag(UUID entityID, MonoString* contactWith)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		std::string contactName = Utils::MonoStringToString(contactWith);

		return rb2d.ContactWith == contactName;
	}

	static MonoString* Rigidbody2DComponent_GetContactWithTag(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		return ScriptEngine::CreateString(rb2d.ContactWith.c_str());
	}

	static void Rigidbody2DComponent_ApplyLinearImpulse(UUID entityID, glm::vec2* impulse, glm::vec2* point, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		auto body = static_cast<b2Body*>(rb2d.RuntimeBody);
		body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);
	}

	static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID entityID, glm::vec2* impulse, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		auto body = static_cast<b2Body*>(rb2d.RuntimeBody);
		body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
	}

	static void Rigidbody2DComponent_ApplyForce(UUID entityID, glm::vec2* force, glm::vec2* point, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		auto body = static_cast<b2Body*>(rb2d.RuntimeBody);
		body->ApplyForce(b2Vec2(force->x, force->y), b2Vec2(point->x, point->y), wake);
	}

	static void Rigidbody2DComponent_ApplyForceToCenter(UUID entityID, glm::vec2* force, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
			Entity entity = scene->GetEntityWithUUID(entityID);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		auto body = static_cast<b2Body*>(rb2d.RuntimeBody);
		body->ApplyForceToCenter(b2Vec2(force->x, force->y), wake);
	}

	static MonoString* AudioComponent_GetName(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)
		OGN_CORE_ASSERT(entity.HasComponent<AudioComponent>())

		auto& ac = entity.GetComponent<AudioComponent>();
		return ScriptEngine::CreateString(ac.Name.c_str());
	}

	static void AudioComponent_SetName(UUID entityID, MonoString* name)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)
		OGN_CORE_ASSERT(entity.HasComponent<AudioComponent>())

		auto& ac = entity.GetComponent<AudioComponent>();
		ac.Name = Utils::MonoStringToString(name);
	}

	static void AudioComponent_Play(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)
		OGN_CORE_ASSERT(entity.HasComponent<AudioComponent>())

		auto& ac = entity.GetComponent<AudioComponent>();
		OGN_CORE_ASSERT(ac.Audio)
		const std::shared_ptr<Audio>& audio = AssetManager::GetAsset<Audio>(ac.Audio);
		audio->Play();
	}

	static void AudioComponent_Stop(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)
		OGN_CORE_ASSERT(entity.HasComponent<AudioComponent>())

		auto& ac = entity.GetComponent<AudioComponent>();
		OGN_CORE_ASSERT(ac.Audio)
		const std::shared_ptr<Audio>& audio = AssetManager::GetAsset<Audio>(ac.Audio);
		audio->Stop();
	}

	static void AudioComponent_SetVolume(UUID entityID, float volume)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)
		OGN_CORE_ASSERT(entity.HasComponent<AudioComponent>())

		entity.GetComponent<AudioComponent>().Volume = volume;
	}

	static float AudioComponent_GetVolume(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)
		OGN_CORE_ASSERT(entity.HasComponent<AudioComponent>())

		return entity.GetComponent<AudioComponent>().Volume;
	}

	static void AudioComponent_SetMinDistance(UUID entityID, float minDistance)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)
		OGN_CORE_ASSERT(entity.HasComponent<AudioComponent>())

		entity.GetComponent<AudioComponent>().MinDistance = minDistance;
	}

	static float AudioComponent_GetMinDistance(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)
		OGN_CORE_ASSERT(entity.HasComponent<AudioComponent>())

		return entity.GetComponent<AudioComponent>().MinDistance;
	}

	static void AudioComponent_SetMaxDistance(UUID entityID, float maxDistance)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)
		OGN_CORE_ASSERT(entity.HasComponent<AudioComponent>())

		entity.GetComponent<AudioComponent>().MaxDistance = maxDistance;
	}

	static float AudioComponent_GetMaxDistance(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)
		OGN_CORE_ASSERT(entity.HasComponent<AudioComponent>())

		return entity.GetComponent<AudioComponent>().MinDistance;
	}

	static void AudioComponent_SetPitch(UUID entityID, float pitch)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)
		OGN_CORE_ASSERT(entity.HasComponent<AudioComponent>())

		entity.GetComponent<AudioComponent>().Pitch = pitch;
	}

	static float AudioComponent_GetPitch(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)
		OGN_CORE_ASSERT(entity.HasComponent<AudioComponent>())

		return entity.GetComponent<AudioComponent>().Pitch;
	}

	static void AudioComponent_SetLooping(UUID entityID, bool looping)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)
		OGN_CORE_ASSERT(entity.HasComponent<AudioComponent>())

		entity.GetComponent<AudioComponent>().Looping = looping;
	}

	static bool AudioComponent_IsLooping(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)
		OGN_CORE_ASSERT(entity.HasComponent<AudioComponent>())

		return entity.GetComponent<AudioComponent>().Looping;
	}

	static void AudioComponent_SetSpatial(UUID entityID, bool spatial)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)
		OGN_CORE_ASSERT(entity.HasComponent<AudioComponent>())

		entity.GetComponent<AudioComponent>().Spatial = spatial;
	}

	static bool AudioComponent_IsSpatial(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)
		OGN_CORE_ASSERT(entity.HasComponent<AudioComponent>())

		return entity.GetComponent<AudioComponent>().Spatial;
	}

	static void AudioComponent_SetPlayAtStart(UUID entityID, bool playAtStart)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)
		OGN_CORE_ASSERT(entity.HasComponent<AudioComponent>())

		entity.GetComponent<AudioComponent>().PlayAtStart = playAtStart;
	}

	static bool AudioComponent_IsPlayAtStart(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)
		OGN_CORE_ASSERT(entity.HasComponent<AudioComponent>())

		return entity.GetComponent<AudioComponent>().PlayAtStart;
	}

	static MonoString* TextComponent_GetText(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)
		OGN_CORE_ASSERT(entity.HasComponent<TextComponent>())

		auto& tc = entity.GetComponent<TextComponent>();
		return ScriptEngine::CreateString(tc.TextString.c_str());
	}

	static void TextComponent_SetText(UUID entityID, MonoString* textString)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)
		OGN_CORE_ASSERT(entity.HasComponent<TextComponent>())

		auto& tc = entity.GetComponent<TextComponent>();
		tc.TextString = Utils::MonoStringToString(textString);
	}

	static void TextComponent_GetColor(UUID entityID, glm::vec4* color)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)
		OGN_CORE_ASSERT(entity.HasComponent<TextComponent>())

		auto& tc = entity.GetComponent<TextComponent>();
		*color = tc.Color;
	}

	static void TextComponent_SetColor(UUID entityID, glm::vec4* color)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)

		auto& tc = entity.GetComponent<TextComponent>();
		tc.Color = *color;
	}

	static float TextComponent_GetKerning(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)

		auto& tc = entity.GetComponent<TextComponent>();
		return tc.Kerning;
	}

	static void TextComponent_SetKerning(UUID entityID, float kerning)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)

		auto& tc = entity.GetComponent<TextComponent>();
		tc.Kerning = kerning;
	}

	static float TextComponent_GetLineSpacing(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)

		auto& tc = entity.GetComponent<TextComponent>();
		return tc.LineSpacing;
	}

	static void TextComponent_SetLineSpacing(UUID entityID, float lineSpacing)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)

		auto& tc = entity.GetComponent<TextComponent>();
		tc.LineSpacing = lineSpacing;
	}

	static void CircleRendererComponent_GetColor(UUID entityID, glm::vec4* outColor)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleRendererComponent>().Color = *outColor;
	}

	static void CircleRendererComponent_SetColor(UUID entityID, glm::vec4* color)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleRendererComponent>().Color = *color;
	}

	static void CircleRendererComponent_GetFade(UUID entityID, float* outFade)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outFade = entity.GetComponent<CircleRendererComponent>().Fade;
	}

	static void CircleRendererComponent_SetFade(UUID entityID, float* fade)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleRendererComponent>().Fade = *fade;
	}

	static void CircleRendererComponent_GetThickness(UUID entityID, float* outThickness)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outThickness = entity.GetComponent<CircleRendererComponent>().Thickness;
	}

	static void CircleRendererComponent_SetThickness(UUID entityID, float* thickness)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleRendererComponent>().Thickness = *thickness;
	}

	static void SpriteRenderer2DComponent_GetColor(UUID entityID, glm::vec4* outColor)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outColor = entity.GetComponent<SpriteRenderer2DComponent>().Color;
	}

	static void SpriteRenderer2DComponent_FlipX(UUID entityID, bool* flip)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");

		Entity entity = scene->GetEntityWithUUID(entityID);
		
		entity.GetComponent<SpriteRenderer2DComponent>().FlipX = *flip;
	}

	static void SpriteRenderer2DComponent_SetColor(UUID entityID, glm::vec4* color)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<SpriteRenderer2DComponent>().Color = *color;
	}

	static void SpriteRenderer2DComponent_GetTilingFactor(UUID entityID, glm::vec2* tilingfactor)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		*tilingfactor = entity.GetComponent<SpriteRenderer2DComponent>().TillingFactor;
	}

	static void SpriteRenderer2DComponent_SetTilingFactor(UUID entityID, glm::vec2* tilingfactor)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<SpriteRenderer2DComponent>().TillingFactor = *tilingfactor;
	}

	static void BoxCollider2DComponent_GetOffset(UUID entityID, glm::vec2* outOffset)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outOffset = entity.GetComponent<BoxCollider2DComponent>().Offset;
	}

	static void BoxCollider2DComponent_SetOffset(UUID entityID, glm::vec2* offset)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<BoxCollider2DComponent>().Offset = *offset;
	}

	static void BoxCollider2DComponent_GetSize(UUID entityID, glm::vec2* outSize)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outSize = entity.GetComponent<BoxCollider2DComponent>().Size;
	}

	static void BoxCollider2DComponent_SetSize(UUID entityID, glm::vec2* size)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<BoxCollider2DComponent>().Size = *size;
	}

	static void BoxCollider2DComponent_GetDensity(UUID entityID, float* outDensity)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outDensity = entity.GetComponent<BoxCollider2DComponent>().Density;
	}

	static void BoxCollider2DComponent_SetDensity(UUID entityID, float* density)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<BoxCollider2DComponent>().Density = *density;
	}

	static void BoxCollider2DComponent_GetFriction(UUID entityID, float* outFriction)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outFriction = entity.GetComponent<BoxCollider2DComponent>().Friction;
	}

	static void BoxCollider2DComponent_SetFriction(UUID entityID, float* friction)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<BoxCollider2DComponent>().Friction = *friction;
	}

	static void BoxCollider2DComponent_GetRestitution(UUID entityID, float* outRestitution)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outRestitution = entity.GetComponent<BoxCollider2DComponent>().Restitution;
	}

	static void BoxCollider2DComponent_SetRestitution(UUID entityID, float* restitution)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<BoxCollider2DComponent>().Restitution = *restitution;
	}

	static void BoxCollider2DComponent_GetRestitutionThreshold(UUID entityID, float* outRestitutionThreshold)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outRestitutionThreshold = entity.GetComponent<BoxCollider2DComponent>().RestitutionThreshold;
	}

	static void BoxCollider2DComponent_SetRestitutionThreshold(UUID entityID, float* restitutionThreshold)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<BoxCollider2DComponent>().RestitutionThreshold = *restitutionThreshold;
	}

	static void CircleCollider2DComponent_GetOffset(UUID entityID, glm::vec2* outOffset)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outOffset = entity.GetComponent<CircleCollider2DComponent>().Offset;
	}

	static void CircleCollider2DComponent_SetOffset(UUID entityID, glm::vec2* offset)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleCollider2DComponent>().Offset = *offset;
	}

	static void CircleCollider2DComponent_GetRadius(UUID entityID, float* outRadius)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outRadius = entity.GetComponent<CircleCollider2DComponent>().Radius;
	}

	static void CircleCollider2DComponent_SetRadius(UUID entityID, float* radius)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleCollider2DComponent>().Radius = *radius;
	}

	static void CircleCollider2DComponent_GetDensity(UUID entityID, float* outDensity)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outDensity = entity.GetComponent<CircleCollider2DComponent>().Density;
	}

	static void CircleCollider2DComponent_SetDensity(UUID entityID, float* density)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleCollider2DComponent>().Density = *density;
	}

	static void CircleCollider2DComponent_GetFriction(UUID entityID, float* outFriction)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outFriction = entity.GetComponent<CircleCollider2DComponent>().Friction;
	}

	static void CircleCollider2DComponent_SetFriction(UUID entityID, float* friction)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleCollider2DComponent>().Friction = *friction;
	}

	static void CircleCollider2DComponent_GetRestitution(UUID entityID, float* outRestituion)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outRestituion = entity.GetComponent<CircleCollider2DComponent>().Restitution;
	}

	static void CircleCollider2DComponent_SetRestitution(UUID entityID, float* restitution)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleCollider2DComponent>().Restitution = *restitution;
	}

	static void CircleCollider2DComponent_GetRestitutionThreshold(UUID entityID, float* outRestitutionThreshold)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outRestitutionThreshold = entity.GetComponent<CircleCollider2DComponent>().RestitutionThreshold;
	}

	static void CircleCollider2DComponent_SetRestitutionThreshold(UUID entityID, float* restitutionThreshold)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene")
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleCollider2DComponent>().RestitutionThreshold = *restitutionThreshold;
	}

	static void AnimationComponent_GetActiveState(UUID entityID, MonoString* state)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)

		OGN_CORE_ASSERT(entity.HasComponent<AnimationComponent>())

		auto& ac = entity.GetComponent<AnimationComponent>();
		if (ac.State.HasAnimation())
		{
			state = ScriptEngine::CreateString(ac.State.GetCurrentState().c_str());
		}
	}

	static void AnimationComponent_SetActiveState(UUID entityID, MonoString* state)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene)
		Entity entity = scene->GetEntityWithUUID(entityID);
		OGN_CORE_ASSERT(entity)

		OGN_CORE_ASSERT(entity.HasComponent<AnimationComponent>())

		auto& ac = entity.GetComponent<AnimationComponent>();
		if (ac.State.HasAnimation())
		{
			ac.State.SetActiveState(Utils::MonoStringToString(state));
		}
	}

	static bool Input_IsKeyPressed(KeyCode keycode)
	{
		return Input::IsKeyPressed(keycode);
	}

	template <typename... Component>
	static void RegisterComponent()
	{
		([]()
		{
			std::string_view typeName = typeid(Component).name();
			size_t pos = typeName.find_last_of(':');
			std::string_view structName = typeName.substr(pos + 1);
			std::string managedTypename = fmt::format("ORiginEngine.{}", structName);

			MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(),
			                                                       ScriptEngine::GetCoreAssemblyImage());
			if (!managedType)
			{
				OGN_CORE_ERROR("SCRIPT GLUE: Could not find component type {}", managedTypename);
				return;
			}

			s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
		}(), ...);
	}

	template <typename... Component>
	static void RegisterComponent(ComponentGroup<Component...>)
	{
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
		OGN_ADD_INTERNAL_CALLS(SpriteRenderer2DComponent_FlipX);

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

		OGN_ADD_INTERNAL_CALLS(AnimationComponent_SetActiveState);
		OGN_ADD_INTERNAL_CALLS(AnimationComponent_GetActiveState);

		OGN_ADD_INTERNAL_CALLS(GetScriptInstance);
		OGN_ADD_INTERNAL_CALLS(Input_IsKeyPressed);
	}
}
