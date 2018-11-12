#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include "Quaternion.h"

class Transform
{
public:
	Transform(const glm::vec3& pos = glm::vec3(0), const Quaternion& rot = Quaternion(0, 0, 0, 1), glm::vec3 scale = glm::vec3(1.0f)) :
		mPos(pos),
		mRot(rot),
		mScale(scale),
		mParent(0),
		mParentMat(glm::mat4(1)),
		mInitializedOldStuff(false) 
	{}

	glm::mat4 get_transform() const;
	bool has_changed();
	void update();
	void rotate(const glm::vec3& axis, float angle);
	void rotate(const Quaternion& rotation);
	void lookat(const glm::vec3& point, const glm::vec3& up);

	Quaternion get_lookat_rotation(const glm::vec3& point, const glm::vec3& up)
	{
		return Quaternion(Quaternion::rotation_from_direction(glm::normalize(point - mPos), up));
	}

	inline glm::vec3*		 get_pos()					  { return &mPos; }
	inline const glm::vec3&  get_pos()				const { return mPos; }
	inline Quaternion*		 get_rot()					  { return &mRot; }
	inline const Quaternion& get_rot()				const { return mRot; }
	inline glm::vec3		 get_scale()			const { return mScale; }
	inline glm::vec3		 get_transform_pos()	const { return glm::vec3(get_parent_mat() * glm::vec4(mPos, 0.0)); }
	Quaternion				 get_transform_rot()	const;
	glm::vec3				 get_transform_scale()  const;
	inline void set_pos(const glm::vec3& pos) { mPos = pos; }
	inline void set_rot(const Quaternion& rot) { mRot = rot; }
	inline void set_scale(glm::vec3 scale) { mScale = scale; }
	inline void set_parent(std::shared_ptr<Transform> parent) { mParent = parent; }

	inline void add_pos(const glm::vec3& dir) { mPos = mPos + dir; }
private:
	const glm::mat4& get_parent_mat() const;

	glm::vec3 mPos;
	Quaternion mRot;
	glm::vec3 mScale;
	std::shared_ptr<Transform> mParent;

	mutable glm::mat4 mParentMat;
	mutable glm::vec3 mOldPos;
	mutable Quaternion mOldRot;
	mutable glm::vec3 mOldScale;
	mutable bool mInitializedOldStuff;
};