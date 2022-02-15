#include "pch.h"
#include "EntityFactory.h"


#include"Entity.h"
namespace MV
{
	entt::registry EntityFactory::s_Registry;
	Entity EntityFactory::CreateEntity(const std::string& name,const glm::mat4& transform)
	{
		Entity entity(s_Registry.create());
		entity.AddComponent<TransformComponent>(transform);
		auto& tag = entity.AddComponent<TagComponent>(name);
		return entity;
	}

	Entity EntityFactory::CreateEntity(std::string&& name, glm::mat4&& transform)
	{
		Entity entity(s_Registry.create());
		entity.AddComponent<TransformComponent>(std::move(transform));
		auto& tag = entity.AddComponent<TagComponent>(std::move(name));
		return entity;
	}

	void EntityFactory::DestroyEntity(Entity& entity)
	{
		s_Registry.destroy(entity.m_EntityHandle);
	}

}
