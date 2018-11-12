#pragma once

#include "Transform.h"
#include "Entity.h"
#include "Input.h"

struct Component
{
public:
	Component() :
		mParent(0) {}
	virtual ~Component() {}

	//virtual void process_input(const Input& input, float delta) {}
	//virtual void update(float delta) {}

	inline std::shared_ptr<Transform> get_transform() { return mParent->get_transform(); }
	inline const Transform& get_transform() const { return *mParent->get_transform(); }

	virtual void set_parent(std::shared_ptr<Entity> parent) { mParent = parent; }

private:
	std::shared_ptr<Entity> mParent;
};
