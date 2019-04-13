#pragma once

#include <vector>
#include "Transform.h"
//#include "Component.h"
//#include "../render/Renderer.h"
#include "../render/Shader.h"
#include "../render/Camera.h"
#include <memory>
#include <iostream>
#include <string>

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
			mRoot.push_back(this);
			mTransform = std::make_shared<Transform>(pos, rot, scale);
		}

	Entity(const Entity &entity)
	{
		mName = entity.mName;
		mChildren = entity.mChildren;
		mTransform = entity.mTransform;
		mRoot.push_back(this);
		mCount++;
		mIndex = mCount;
	}

	virtual ~Entity() {};

	void add_child(Entity *child);


	virtual void render(std::shared_ptr<Shader> &shader) = 0;

	//std::shared_ptr<Entity> add_component(std::shared_ptr<Component> component);

	//void render_all(const Shader& shader, const Renderer& renderingEngine, const Camera& camera) const;

	//void update_all(float delta);

	//std::vector<std::shared_ptr<Entity> > get_all_attached();

public:
	std::vector<Entity*>				mChildren;
	std::shared_ptr<Transform>          mTransform;
	std::string							mName;
	static std::vector<Entity*>			mRoot;
	int									mIndex;
	static int							mCount;
	//Entity*								mParent;
	//std::vector<std::shared_ptr<Component> >    mComponents;
	//void render(const Shader& shader, const Renderer& renderingEngine, const Camera& camera) const;
	//void update(float delta);

//protected:

};