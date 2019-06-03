#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../core/Transform.h"

class Camera
{
public:
	Camera() 
	{
	}

	Camera(const glm::mat4& projection, std::shared_ptr<Transform> transform) :
		mProjection(projection),
		mTransform(transform)
	{

	}

	glm::mat4 get_view_projection()           const;

	inline std::shared_ptr<Transform> get_transform() { return mTransform; }
	inline const Transform& get_transform() const { return *mTransform; }

	inline void set_projection(const glm::mat4& projection) { mProjection = projection; }
	inline void set_transform(std::shared_ptr<Transform> transform) { mTransform = transform; }

	void move(glm::vec3 dir);
	void rotate(const glm::vec3& axis, float angle);

private:
	glm::mat4   mProjection;
	std::shared_ptr<Transform>	mTransform;

public:
	static float mZoom;
	static float mNear;
	static float mFar;
};
