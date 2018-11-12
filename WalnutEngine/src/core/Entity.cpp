#include "Entity.h"

std::shared_ptr<Entity> Entity::add_child(std::shared_ptr<Entity> child)
{
	mChildren.push_back(child);
	child->get_transform()->set_parent(mTransform);
	return std::make_shared<Entity>(this);
}

std::shared_ptr<Entity> Entity::add_component(std::shared_ptr<Component> component)
{
	mComponents.push_back(component);
	component->set_parent(std::make_shared<Entity>(this));
	return std::make_shared<Entity>(this);
}

//void Entity::update_all(float delta)
//{
//	update(delta);
//
//	for (unsigned int i = 0; i < mChildren.size(); i++)
//	{
//		mChildren[i]->update_all(delta);
//	}
//}

//void Entity::update(float delta)
//{
//	for (unsigned int i = 0; i < mComponents.size(); i++)
//	{
//		mComponents[i]->update(delta);
//	}
//}

//std::vector<std::shared_ptr<Entity> >  Entity::get_all_attached()
//{
//	std::vector<std::shared_ptr<Entity> > result;
//
//	for (unsigned int i = 0; i < mChildren.size(); i++)
//	{
//		std::vector<std::shared_ptr<Entity> > childObjects = mChildren[i]->get_all_attached();
//		result.insert(result.end(), childObjects.begin(), childObjects.end());
//	}
//
//	result.push_back(std::make_shared<Entity>(this));
//	return result;
//}
