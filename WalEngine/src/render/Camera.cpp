#include "Camera.h"

float Camera::Zoom = 45.0f;
float Camera::Near = 0.1f;
float Camera::Far = 100.0f;
const float sensitive = 0.07f;

glm::mat4 Camera::get_view_projection() const
{
	glm::mat4 cameraRotation = get_transform().get_transform_rot().conjugate().to_rotation_mat();
	glm::mat4 cameraTranslation = glm::mat4();

	cameraTranslation = glm::translate(cameraTranslation, get_transform().get_transform_pos() * -1.0f);

	return mProjection * cameraRotation * cameraTranslation;
}

glm::mat4 Camera::get_projection() const 
{
	return mProjection;
}

glm::mat4 Camera::get_view() const 
{
	glm::mat4 cameraRotation = get_transform().get_transform_rot().conjugate().to_rotation_mat();
	glm::mat4 cameraTranslation = glm::mat4();

	cameraTranslation = glm::translate(cameraTranslation, get_transform().get_transform_pos() * -1.0f);
	return cameraRotation * cameraTranslation;
}

void Camera::move(glm::vec3 dir)
{
	get_transform()->add_pos(dir * -sensitive);
}

void Camera::rotate(const glm::vec3& axis, float angle)
{
	get_transform()->rotate(axis, angle);
}