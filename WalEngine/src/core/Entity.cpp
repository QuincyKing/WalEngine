#include "Entity.h"
#include "../render/RenderEngine.h"
#include <algorithm>

std::vector<Entity*> Entity::Root = std::vector<Entity *>();
int Entity::mCount = 0;

void Entity::add_child(Entity* child)
{
	if (std::find(mChildren.begin(), mChildren.end(), child) == mChildren.end())
	{	
		auto iter = std::find(Entity::Root.begin(), Entity::Root.end(), child);
		if(iter != Entity::Root.end())
			Root.erase(iter);
		mChildren.push_back(child);
		child->mTransform->set_parent(mTransform);
	}
}

void Entity::render_all()
{
	if(mRenderStateEvent)
		mRenderStateEvent();
	render();
	for (unsigned int i = 0; i < mChildren.size(); i++)
	{
		mChildren[i]->render_all();
	}
}

bool Entity::add_component(ComType type, Component* component)
{
	auto ret = mComponents.insert(std::pair<ComType, Component* >(type, component));
	return ret.second;
}

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
