#pragma once
#include"Event/Event.h"
#include<string>
#include"TimeStep.h"
#include<Entity/Entity.h>
namespace MV
{
	
	class Layer
	{
	public:

		Layer(const std::string& name)
			:m_Name(name){}

		virtual ~Layer() = default;

		virtual void OnAttach(){}
		virtual void OnUpdate(TimeStep ts){}
		virtual void OnImGuiRender(){}
		virtual void OnEvent(Event& e){}
		
		virtual Entity& GetSelectedContext() { IGAME_ASSERT(false, " "); return Entity{}; }
		virtual void OnEntityConstruction(Entity& entity){}

		
	
	
		const std::string& GetName()const { return m_Name; }
	private:
		//for debug
		std::string m_Name;
	};
}