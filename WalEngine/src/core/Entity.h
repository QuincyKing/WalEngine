#pragma once

#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>

#include "Transform.h"
#include "Component.h"
#include "../render/Camera.h"
#include "../component/Material.h"

class RenderEngine;

class Entity
{
public:
	Entity(
		std::string name,
		glm::vec3 pos = glm::vec3(0, 0, 0), 
		Quaternion rot = Quaternion(0, 0, 0, 1), 
		glm::vec3 scale = glm::vec3(1.0))
		{
			mName = name;
			mCount++;
			mIndex = mCount;
			mTransform = std::make_shared<Transform>(pos, rot, scale);
			//TODO memory leak
			mComponents[ComType::Mat] = new Material("default");
		}

	Entity(const Entity &entity)
	{
		mName = entity.mName;
		mChildren = entity.mChildren;
		mTransform = entity.mTransform;
		Root.push_back(this);
		mCount++;
		mIndex = mCount;
	}

	virtual ~Entity() {};

	void add_child(Entity *child);
	virtual void render(Shader *shader = nullptr) {};
	void render_all(Shader *shader = nullptr);
	void set_mat(Material* mat) 
	{ 
		mComponents[ComType::Mat] = mat;
	};

	virtual bool add_component(ComType type, Component* component);

	template<typename T>
	T* get_component(ComType type) const
	{
		if (mComponents.count(type) > 0)
			return dynamic_cast<T *>(mComponents.at(type));
		else
			return nullptr;
	};

	//void update_all(float delta);

	//std::vector<std::shared_ptr<Entity> > get_all_attached();

public:
	std::vector<Entity*>				mChildren;
	std::shared_ptr<Transform>          mTransform;
	std::string							mName;
	int									mIndex;
	static int							mCount;
	static std::vector<Entity*>			Root;
	std::function<void(void)>			mRenderStateEvent;

protected:
	std::map<ComType, Component* >	mComponents;
	//void update(float delta);
};