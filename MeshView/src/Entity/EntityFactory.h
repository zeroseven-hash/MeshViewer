#pragma once

#include"Component.h"
#include<entt.hpp>
namespace MV
{
	class Entity;
	class EntityFactory
	{
	public:
		~EntityFactory() = default;
	
		static Entity CreateEntity(const std::string& name= std::string("Entity"), const glm::mat4& transform=glm::mat4(1.0f));
		static Entity CreateEntity(std::string&& name=std::string("Entity"),glm::mat4&& transform=glm::mat4(1.0f));

		static void DestroyEntity(Entity& entity);
		static entt::registry& GetRegistry() { return s_Registry; }
	private:
		EntityFactory() = default;
	
	private:
		static entt::registry s_Registry;
	};







	
}