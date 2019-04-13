#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Transform.h"
#include "Window.h"

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

	inline void set_projection() 
	{ 
		mProjection = glm::perspective(
			glm::radians(Camera::mFOV),
			Window::mInput.get_win_size_y() != 0 ?
			(float)Window::mInput.get_win_size_x() / (float)Window::mInput.get_win_size_y() : 0,
			Camera::mNear,
			Camera::mFar
		);
	}
	inline void set_transform(std::shared_ptr<Transform> transform) { mTransform = transform; }

	void move(glm::vec3 dir);
private:
	glm::mat4   mProjection;
	std::shared_ptr<Transform>	mTransform;

public:
	float mFOV = 45.0f;
	float mNear = 0.1f;
	float mFar = 100.0f;
};
