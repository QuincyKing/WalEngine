#pragma once

#include <vector>
#include "Transform.h"
#include "Component.h"

class Entity
{
public:
	Entity(const glm::vec3& pos = glm::vec3(0, 0, 0), 
		const Quaternion& rot = Quaternion(0, 0, 0, 1), 
		glm::vec3 scale = glm::vec3(1.0))
		{
			mTransform = std::make_shared<Transform>(pos, rot, scale);
		}

	virtual ~Entity() {};

	std::shared_ptr<Entity> add_child(std::shared_ptr<Entity> child);
	std::shared_ptr<Entity> add_component(std::shared_ptr<Component> component);

	inline std::shared_ptr<Transform> get_transform() { return mTransform; }
	//void update_all(float delta);

	//std::vector<std::shared_ptr<Entity> > get_all_attached();

private:
	std::vector<std::shared_ptr<Entity> >       mChildren;
	std::vector<std::shared_ptr<Component> >    mComponents;
	std::shared_ptr<Transform>                  mTransform;

	//void update(float delta);
};