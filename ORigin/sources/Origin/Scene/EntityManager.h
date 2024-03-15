#pragma once
#include "Entity.h"
#include "Origin\Core\UUID.h"
#include "entt\entt.hpp"

namespace origin
{
	template <typename... Component>
	static void CopyComponent(entt::registry &dst, entt::registry &src, const std::unordered_map<UUID, entt::entity> &enttMap)
	{
		([&]()
		 {
			 auto view = src.view<Component>();
			 for (auto srcEntity : view)
			 {
				 entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);
				 auto srcComponent = src.get<Component>(srcEntity);
				 dst.emplace_or_replace<Component>(dstEntity, srcComponent);
			 }
		 }(), ...);
	}

	template <typename... Component>
	static void CopyComponent(ComponentGroup<Component...>, entt::registry &dst, entt::registry &src, const std::unordered_map<UUID, entt::entity> enttMap)
	{
		CopyComponent<Component...>(dst, src, enttMap);
	}

	template <typename... Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		([&]()
		 {
			 if (src.HasComponent<Component>())
			 {
				 dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
			 }
		 }(), ...);
	}

	template <typename... Component>
	static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
	{
		CopyComponentIfExists<Component...>(dst, src);
	}

	class EntityManager
	{
	public:
		static Entity CreateEntity(const std::string &name, Scene *scene);
		static Entity CreateAudio(const std::string &name, Scene *scene);
		static Entity CreateSprite(const std::string &name, Scene *scene);
		static Entity CreateCircle(const std::string &name, Scene *scene);
		static Entity CreateCamera(const std::string &name, Scene *scene);
		static Entity CreateEntityWithUUID(UUID uuid, const std::string &name, Scene *scene);
		static Entity CreateMesh(const std::string &name, Scene *scene);
		static Entity DuplicateEntity(Entity entity, Scene *scene);
		static void DestroyEntity(Entity entity, Scene *scene);
	};
}
