#pragma once

#include"EntityFactory.h"
namespace MV
{
	class Entity
	{
	public:
		friend class EntityFactory;
	public:
		Entity() = default;
		Entity(entt::entity entityHandle) :m_EntityHandle(entityHandle) {};

		template<typename T,typename... Args>
		T& AddComponent(Args&&... args);

		template<typename T>
		T& GetComponent();

		template<typename T>
		bool HasComponent();


		template<typename T>
		void RemoveComponent();

		operator uint32_t()const  { return (uint32_t)m_EntityHandle; }
		operator bool()const { return m_EntityHandle != entt::null; }
		bool operator==(const Entity& others)
		{
			return m_EntityHandle == others.m_EntityHandle;
		}

		bool operator!=(const Entity& other)
		{
			return !(m_EntityHandle == other.m_EntityHandle);
		}
	private:
		entt::entity m_EntityHandle = entt::null;
	};




	//implement
	template<typename T, typename ...Args>
	inline T& Entity::AddComponent(Args && ...args)
	{
		return EntityFactory::GetRegistry().emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
	}

	template<typename T>
	inline T& Entity::GetComponent()
	{
		IGAME_ASSERT(HasComponent<T>(), "Entity does not has component");
		return EntityFactory::GetRegistry().get<T>(m_EntityHandle);
	}

	template<typename T>
	inline bool Entity::HasComponent()
	{
		return EntityFactory::GetRegistry().all_of<T>(m_EntityHandle);
	}

	template<typename T>
	inline void Entity::RemoveComponent()
	{
		IGAME_ASSERT(HasComponent<T>(), "Entity does not has component");
		EntityFactory::GetRegistry().remove(m_EntityHandle);
	}

	

	
}