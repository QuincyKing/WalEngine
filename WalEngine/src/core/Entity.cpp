#include "Entity.h"
#include <algorithm>

std::vector<Entity*> Entity::mRoot = std::vector<Entity *>();
int Entity::mCount = 0;

void Entity::add_child(Entity* child)
{
	if (std::find(mChildren.begin(), mChildren.end(), child) == mChildren.end())
	{	
		auto iter = std::find(Entity::mRoot.begin(), Entity::mRoot.end(), child);
		if(iter != Entity::mRoot.end())
			mRoot.erase(iter);
		mChildren.push_back(child);
		child->mTransform->set_parent(mTransform);
	}
}

//std::shared_ptr<Entity> Entity::add_component(std::shared_ptr<Component> component)
//{
//	mComponents.push_back(component);
//	component->set_parent(std::make_shared<Entity>(this));
//	return std::make_shared<Entity>(this);
//}

//void Entity::render_all(const Shader& shader, const Renderer& renderingEngine, const Camera& camera) const
//{
//	render(shader, renderingEngine, camera);
//
//	for (unsigned int i = 0; i < mChildren.size(); i++)
//	{
//		mChildren[i]->render_all(shader, renderingEngine, camera);
//	}
//}


//void Entity::render(const Shader& shader, const Renderer& renderingEngine, const Camera& camera) const
//{
//	for (unsigned int i = 0; i < mComponents.size(); i++)
//	{
//		mComponents[i]->render(shader, renderingEngine, camera);
//	}
//}

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
