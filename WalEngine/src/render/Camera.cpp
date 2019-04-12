#include "Camera.h"

float Camera::mZoom = 45.0f;
float Camera::mNear = 0.1f;
float Camera::mFar = 100.0f;
const float sensitive = 0.07f;

glm::mat4 Camera::get_view_projection() const
{
	glm::mat4 cameraRotation = get_transform().get_transform_rot().conjugate().to_rotation_mat();
	glm::mat4 cameraTranslation = glm::mat4();

	cameraTranslation = glm::translate(cameraTranslation, get_transform().get_transform_pos() * -1.0f);

	return mProjection * cameraRotation * cameraTranslation;
}

void Camera::move(glm::vec3 dir)
{
	get_transform()->add_pos(dir * -sensitive);
}