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

			MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssemblyImage());
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
		OGN_ADD_INTERNAL_CALLS(Rigidbody2DComponent_ApplyLinearImpulse);
		OGN_ADD_INTERNAL_CALLS(Rigidbody2DComponent_ApplyLinearImpulseToCenter);

		OGN_ADD_INTERNAL_CALLS(GetScriptInstance);

		OGN_ADD_INTERNAL_CALLS(Input_IsKeyPressed);
	}
}