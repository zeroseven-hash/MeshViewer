#pragma once
#include"../DataStructure/DrawableObject.h"
#include<glm/glm.hpp>

namespace MV
{
	struct TagComponent
	{
		std::string Tag;
		
		TagComponent() = default;
		TagComponent(const TagComponent& other) = default;
		TagComponent(const std::string& name):Tag(name){}
		TagComponent(std::string&& name):Tag(std::move(name)){}
	};

	struct TransformComponent
	{
		glm::mat4 Transform=glm::mat4(1.0f);
		TransformComponent() = default;
		TransformComponent(const TransformComponent& other) = default;
		TransformComponent(const glm::mat4& transform):Transform(transform){}
		TransformComponent(glm::mat4&& transform):Transform(std::move(transform)){}
	};

	struct DrawableComponent
	{
		Ref<DrawableObject> Object;
		bool IsShow = true;
		DrawableComponent() = default;
		DrawableComponent(const Ref<DrawableObject>& other):Object(other){}
	};
}