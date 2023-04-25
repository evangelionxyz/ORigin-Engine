// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"

#include "ScriptGlue.h"
#include "ScriptEngine.h"

#include "Origin\Scene\Component\UUID.h"
#include "Origin\Scene\Component\Component.h"

#include "Origin\Scene\Scene.h"
#include "Origin\Scene\Entity.h"

#include "Origin\IO\KeyCodes.h"
#include "Origin\IO\Input.h"

#include "mono\metadata\object.h"
#include "mono\metadata\reflection.h"

#include "box2d\b2_body.h"

namespace Origin
{
#define OGN_ADD_INTERNAL_CALLS(Name) mono_add_internal_call("ORiginEngine.InternalCalls::"#Name, Name)

	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

	// Entity
	static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		OGN_CORE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end(), "");
		return s_EntityHasComponentFuncs.at(managedType)(entity);
	}

	static uint64_t Entity_FindEntityByName(MonoString* name)
	{
		char* nameCStr = mono_string_to_utf8(name);

		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");

		Entity entity = scene->FindEntityByName(nameCStr);
		mono_free(nameCStr);

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
		char* cStr = mono_string_to_utf8(string);
		std::string str(cStr);
		mono_free(cStr);
		std::cout << str << ", " << parameter << std::endl;
	}

	static void NativeLog_Vector(glm::vec3* parameter, glm::vec3* outResult)
	{
		*outResult = glm::normalize(*parameter);
	}

	static float NativeLog_VectorDot(glm::vec3* parameter)
	{
		return glm::dot(*parameter, *parameter);
	}

	// Component
	static MonoObject* GetScriptInstance(UUID entityID)
	{
		return ScriptEngine::GetManagedInstance(entityID);
	}

	static void TransformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outTranslation = entity.GetComponent<TransformComponent>().Translation;
	}

	static void TransformComponent_SetTranslation(UUID entityID, glm::vec3* translation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<TransformComponent>().Translation = *translation;
	}

	static void TransformComponent_GetRotation(UUID entityID, glm::vec3* outRotation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outRotation = entity.GetComponent<TransformComponent>().Rotation;
	}

	static void TransformComponent_SetRotation(UUID entityID, glm::vec3* rotation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<TransformComponent>().Rotation = *rotation;
	}

	static void TransformComponent_GetScale(UUID entityID, glm::vec3* outScale)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outScale = entity.GetComponent<TransformComponent>().Scale;
	}

	static void TransformComponent_SetScale(UUID entityID, glm::vec3* scale)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<TransformComponent>().Scale = *scale;
	}

	static void Rigidbody2DComponent_ApplyLinearImpulse(UUID entityID, glm::vec2* impulse, glm::vec2* point, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);
	}

	static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID entityID, glm::vec2* impulse, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
	}

	static void CircleRendererComponent_GetColor(UUID entityID, glm::vec4* outColor)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleRendererComponent>().Color = *outColor;
	}

	static void CircleRendererComponent_SetColor(UUID entityID, glm::vec4* color)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleRendererComponent>().Color = *color;
	}

	static void CircleRendererComponent_GetFade(UUID entityID, float* outFade)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outFade = entity.GetComponent<CircleRendererComponent>().Fade;
	}

	static void CircleRendererComponent_SetFade(UUID entityID, float* fade)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleRendererComponent>().Fade = *fade;
	}

	static void CircleRendererComponent_GetThickness(UUID entityID, float* outThickness)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outThickness = entity.GetComponent<CircleRendererComponent>().Thickness;
	}

	static void CircleRendererComponent_SetThickness(UUID entityID, float* thickness)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleRendererComponent>().Thickness = *thickness;
	}

	static void SpriteRenderer2DComponent_GetColor(UUID entityID, glm::vec4* outColor)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outColor = entity.GetComponent<SpriteRenderer2DComponent>().Color;
	}

	static void SpriteRenderer2DComponent_SetColor(UUID entityID, glm::vec4* color)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<SpriteRenderer2DComponent>().Color= *color;
	}

	static void SpriteRenderer2DComponent_GetTillingFactor(UUID entityID, float* tilingfactor)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		*tilingfactor = entity.GetComponent<SpriteRenderer2DComponent>().TillingFactor;
	}

	static void SpriteRenderer2DComponent_SetTilingFactor(UUID entityID, float* tilingfactor)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<SpriteRenderer2DComponent>().TillingFactor = *tilingfactor;
	}

	static void BoxCollider2DComponent_GetOffset(UUID entityID, glm::vec2* outOffset)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outOffset = entity.GetComponent<BoxCollider2DComponent>().Offset;
	}

	static void BoxCollider2DComponent_SetOffset(UUID entityID, glm::vec2* offset)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<BoxCollider2DComponent>().Offset = *offset;
	}

	static void BoxCollider2DComponent_GetSize(UUID entityID, glm::vec2* outSize)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outSize = entity.GetComponent<BoxCollider2DComponent>().Size;
	}

	static void BoxCollider2DComponent_SetSize(UUID entityID, glm::vec2* size)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<BoxCollider2DComponent>().Size = *size;
	}

	static void BoxCollider2DComponent_GetDensity(UUID entityID, float* outDensity)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outDensity = entity.GetComponent<BoxCollider2DComponent>().Density;
	}

	static void BoxCollider2DComponent_SetDensity(UUID entityID, float* density)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<BoxCollider2DComponent>().Density = *density;
	}

	static void BoxCollider2DComponent_GetFriction(UUID entityID, float* outFriction)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outFriction = entity.GetComponent<BoxCollider2DComponent>().Friction;
	}

	static void BoxCollider2DComponent_SetFriction(UUID entityID, float* friction)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<BoxCollider2DComponent>().Friction = *friction;
	}

	static void BoxCollider2DComponent_GetRestitution(UUID entityID, float* outRestitution)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outRestitution = entity.GetComponent<BoxCollider2DComponent>().Restitution;
	}

	static void BoxCollider2DComponent_SetRestitution(UUID entityID, float* restitution)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<BoxCollider2DComponent>().Restitution = *restitution;
	}

	static void BoxCollider2DComponent_GetRestitutionThreshold(UUID entityID, float* outRestitutionThreshold)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outRestitutionThreshold = entity.GetComponent<BoxCollider2DComponent>().RestitutionThreshold;
	}

	static void BoxCollider2DComponent_SetRestitutionThreshold(UUID entityID, float* restitutionThreshold)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<BoxCollider2DComponent>().RestitutionThreshold = *restitutionThreshold;
	}

	static void CircleCollider2DComponent_GetOffset(UUID entityID, glm::vec2* outOffset)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outOffset = entity.GetComponent<CircleCollider2DComponent>().Offset;
	}

	static void CircleCollider2DComponent_SetOffset(UUID entityID, glm::vec2* offset)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleCollider2DComponent>().Offset = *offset;
	}

	static void CircleCollider2DComponent_GetRadius(UUID entityID, float* outRadius)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outRadius = entity.GetComponent<CircleCollider2DComponent>().Radius;
	}

	static void CircleCollider2DComponent_SetRadius(UUID entityID, float* radius)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleCollider2DComponent>().Radius = *radius;
	}

	static void CircleCollider2DComponent_GetDensity(UUID entityID, float* outDensity)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outDensity = entity.GetComponent<CircleCollider2DComponent>().Density;
	}

	static void CircleCollider2DComponent_SetDensity(UUID entityID, float* density)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleCollider2DComponent>().Density = *density;
	}

	static void CircleCollider2DComponent_GetFriction(UUID entityID, float* outFriction)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outFriction = entity.GetComponent<CircleCollider2DComponent>().Friction;
	}

	static void CircleCollider2DComponent_SetFriction(UUID entityID, float* friction)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleCollider2DComponent>().Friction = *friction;
	}

	static void CircleCollider2DComponent_GetRestitution(UUID entityID, float* outRestituion)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outRestituion = entity.GetComponent<CircleCollider2DComponent>().Restitution;
	}

	static void CircleCollider2DComponent_SetRestitution(UUID entityID, float* restitution)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleCollider2DComponent>().Restitution = *restitution;
	}

	static void CircleCollider2DComponent_GetRestitutionThreshold(UUID entityID, float* outRestitutionThreshold)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		*outRestitutionThreshold = entity.GetComponent<CircleCollider2DComponent>().RestitutionThreshold;
	}

	static void CircleCollider2DComponent_SetRestitutionThreshold(UUID entityID, float* restitutionThreshold)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OGN_CORE_ASSERT(scene, "Invalid Scene");
		Entity entity = scene->GetEntityWithUUID(entityID);

		entity.GetComponent<CircleCollider2DComponent>().RestitutionThreshold = *restitutionThreshold;
	}

	static bool Input_IsKeyPressed(KeyCode keycode)
	{
		return Input::IsKeyPressed(keycode);
	}

	template<typename... Component>
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
				OGN_CORE_ERROR("Could not find component type {}", managedTypename);
				return;
			}

			s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };

		}(), ...);
	}

	template<typename... Component>
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

		OGN_ADD_INTERNAL_CALLS(CircleRendererComponent_GetColor);
		OGN_ADD_INTERNAL_CALLS(CircleRendererComponent_SetColor);
		OGN_ADD_INTERNAL_CALLS(CircleRendererComponent_GetFade);
		OGN_ADD_INTERNAL_CALLS(CircleRendererComponent_SetFade);
		OGN_ADD_INTERNAL_CALLS(CircleRendererComponent_GetThickness);
		OGN_ADD_INTERNAL_CALLS(CircleRendererComponent_SetThickness);

		OGN_ADD_INTERNAL_CALLS(SpriteRenderer2DComponent_GetColor);
		OGN_ADD_INTERNAL_CALLS(SpriteRenderer2DComponent_SetColor);
		OGN_ADD_INTERNAL_CALLS(SpriteRenderer2DComponent_GetTillingFactor);
		OGN_ADD_INTERNAL_CALLS(SpriteRenderer2DComponent_SetTilingFactor);

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

		OGN_ADD_INTERNAL_CALLS(GetScriptInstance);
		OGN_ADD_INTERNAL_CALLS(Input_IsKeyPressed);
	}
}