#pragma once
#include "Entity.h"
#include "Origin\Core\UUID.h"
#include "entt\entt.hpp"

namespace origin
{
	class EntityManager
	{
	public:
		static Entity CreateEntity(const std::string &name, Scene *scene);
		static Entity CreateAudio(const std::string &name, Scene *scene);
		static Entity CreateSprite(const std::string &name, Scene *scene);
		static Entity CreateCircle(const std::string &name, Scene *scene);
		static Entity CreateCamera(const std::string &name, Scene *scene);
		static Entity CreateLighting(const std::string &name, Scene *scene);
		static Entity CreateEntityWithUUID(UUID uuid, const std::string &name, Scene *scene);
		static Entity CreateMesh(const std::string &name, Scene *scene);
		static Entity DuplicateEntity(Entity entity, Scene *scene);

		static void AddChild(Entity destination, Entity source, Scene *scene);
		static void DeleteChild(Entity parent, Entity child);
		static bool ChildExists(UUID destination, UUID source, Scene *scene);
		static bool ChildOrGrandChildExists(UUID destination, UUID source, Scene *scene);
		static bool ParentOrGrandParentExists(UUID destinationParent, UUID source, Scene *scene);

		static void DestroyEntityFromScene(Entity entity, Scene *scene);

		template <typename... Component>
		static void CopyComponent(entt::registry &dst, entt::registry &src, const std::vector<std::tuple<UUID, entt::entity>> enttStorage)
		{
			([&]()
			 {
				 auto view = src.view<Component>();
				 for (auto srcEntity : view)
				 {
					 for (auto e : enttStorage)
					 {
						 if (std::get<0>(e) == src.get<IDComponent>(srcEntity).ID)
						 {
							 entt::entity dstEntity = std::get<1>(e);
							 dst.emplace_or_replace<Component>(dstEntity, src.get<Component>(srcEntity));
						 }
					 }

				 }
			 }(), ...);
		}

		template <typename... Component>
		static void CopyComponent(ComponentGroup<Component...>, entt::registry &dst, entt::registry &src, const std::vector<std::tuple<UUID, entt::entity>> enttStorage)
		{
			CopyComponent<Component...>(dst, src, enttStorage);
		}

		template <typename... Component>
		static void CopyComponentIfExists(Entity dst, Entity src)
		{
			([&]()
			 {
				 if (src.HasComponent<Component>())
					 dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
			 }(), ...);
		}

		template <typename... Component>
		static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
		{
			CopyComponentIfExists<Component...>(dst, src);
		}

	};
}
