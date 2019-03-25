#include "Transform.h"

bool Transform::has_changed()
{
	if (mParent != 0 && mParent->has_changed()) return true;
	if (mPos != mOldPos)  return true;
	if (mRot != mOldRot) return true;
	if (mScale != mOldScale) return true;

	return false;
}

void Transform::update()
{
	if (mInitializedOldStuff)
	{
		mOldPos = mPos;
		mOldRot = mRot;
		mOldScale = mScale;
	}
	else
	{
		mOldPos = mPos + glm::vec3(1, 1, 1);
		mOldRot = mRot * 0.5f;
		mOldScale = mScale + glm::vec3(1);
		mInitializedOldStuff = true;
	}
}

void Transform::rotate(const glm::vec3& axis, float angle)
{
	rotate(Quaternion(axis, angle));
}

void Transform::rotate(const Quaternion& rotation)
{
	mRot = Quaternion((rotation * mRot).normalize());
}

void Transform::lookat(const glm::vec3& point, const glm::vec3& up)
{
	mRot = get_lookat_rotation(point, up);
}

//计算得到平移，变大，旋转的矩阵合
glm::mat4 Transform::get_model() const
{
	glm::mat4 translationMatrix;
	glm::mat4 scaleMatrix;

	translationMatrix = glm::translate(translationMatrix, mPos);
	scaleMatrix = glm::scale(scaleMatrix, mScale);
	glm::mat4 result = translationMatrix * (mRot.to_rotation_mat()) * scaleMatrix;

	return get_parent_mat() * result;
}

//更新Parent矩阵
const glm::mat4& Transform::get_parent_mat() const
{
	if (mParent != 0 && mParent->has_changed())
	{
		mParentMat = mParent->get_model();
	}

	return mParentMat;
}    

//计算当前的绝对旋转四元数
Quaternion Transform::get_transform_rot() const
{
	Quaternion parentRot = Quaternion(0, 0, 0, 1);

	if (mParent)
	{
		parentRot = mParent->get_transform_rot();
	}

	return parentRot * mRot;
}

glm::vec3 Transform::get_transform_scale()  const
{
	glm::vec3 parentScale = glm::vec3(1);

	if (mParent)
	{
		parentScale = mParent->get_transform_scale();
	}

	return parentScale * mScale;
}